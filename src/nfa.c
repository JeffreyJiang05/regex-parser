#include "nfa.h"

#include <stdio.h>
#include <ctype.h>

#include "map.h"
#include "set.h"

const int EPSILON = -1;

#define LOCK_STATE(state) do {      \
    (state)->flags |= 0x1;          \
} while(0)

#define UNLOCK_STATE(state) do {    \
    (state)->flags &= ~0x1;         \
} while(0)

#define IS_STATE_LOCKED(state) ((state)->flags & 0x1)

struct nondeterministic_state
{
    const char *debug_tag;
    // MAP_KEY = symbol
    // MAP_VALUE = SET<NSTATE>
    MAP transitions;
    int flags;
};

NSTATE nstate_new()
{
    NSTATE state = malloc(sizeof(struct nondeterministic_state));
    state->debug_tag = NULL;
    state->transitions = map_init();
    state->flags = 0;
    return state;
}

int nstate_free(NSTATE state)
{
    if (IS_STATE_LOCKED(state)) return -1;

    MAP_ITERATOR iter = map_iterator_init(state->transitions);
    void *data;
    while (map_iterator_has_next(iter))
    {
        map_iterator_next(iter, NULL, &data);
        set_fini(data);
    }
    map_iterator_fini(iter);

    map_fini(state->transitions);
    free(state);
    return 0;
}

/**
 * force frees the object regardless of lock
 */
static void __nstate_force_free(NSTATE state)
{
    map_fini(state->transitions);
    free(state);
}

NSTATE nstate_debug_new(const char *debug_tag)
{
    NSTATE state = malloc(sizeof(struct nondeterministic_state));
    state->debug_tag = debug_tag;
    state->transitions = map_init();
    state->flags = 0;
    return state;
}

const char *nstate_tag(NSTATE state)
{
    return state->debug_tag;   
}

int nstate_add_transition(NSTATE from, SYMBOL sym, NSTATE to)
{
    if (IS_STATE_LOCKED(from)) return -1;

    SET sym_set = map_get(from->transitions, sym);
    if (!sym_set)
    {
        sym_set = set_init();
        map_add(from->transitions, sym, sym_set);
    }
    return set_add(sym_set, to);
}

int nstate_remove_transition(NSTATE from, SYMBOL sym, NSTATE to)
{
    if (IS_STATE_LOCKED(from)) return -1;

    SET sym_set = map_get(from->transitions, sym);
    if (!sym_set) return -1;
    int ret = set_remove(sym_set, to);
    if (!set_size(sym_set))
    {
        set_fini(sym_set);
        map_remove(from->transitions, sym);
    }
    return ret;
}

int nstate_clear_transition_symbol(NSTATE from, SYMBOL sym)
{
    if (IS_STATE_LOCKED(from)) return -1;

    SET sym_set = map_get(from->transitions, sym);
    if (sym_set) set_fini(sym_set);
    map_remove(from->transitions, sym);
    return 0;
}

int nstate_clear_all_transitions(NSTATE from)
{
    if (IS_STATE_LOCKED(from)) return -1;

    SYMBOL sym;
    void *set;
    MAP_ITERATOR iter = map_iterator_init(from->transitions);
    while (map_iterator_has_next(iter))
    {
        map_iterator_next(iter, &sym, &set);
        set_fini(set);
    }
    map_iterator_fini(iter);
    map_clear(from->transitions);
    return 0;
}

SYMBOL *nstate_get_transition_symbols(NSTATE state)
{
    return map_keys(state->transitions);
}

size_t nstate_count_transition_symbols(NSTATE state)
{
    return map_size(state->transitions);
}

NSTATE *nstate_get_transition_states(NSTATE state, SYMBOL sym)
{
    SET sym_set = map_get(state->transitions, sym);
    if (sym_set) return (NSTATE*) set_values(sym_set); 
    return NULL;
}

size_t nstate_count_transition_states(NSTATE state, SYMBOL sym)
{
    SET sym_set = map_get(state->transitions, sym);
    if (sym_set) return set_size(sym_set); 
    return 0;
}

int nstate_has_transition(NSTATE from, SYMBOL sym, NSTATE to)
{
    SET sym_set = map_get(from->transitions, sym);
    if (!sym_set) return 0;
    return set_contains(sym_set, to);
}

void nstate_debug_display(NSTATE state, size_t indent)
{
    for (size_t i = 0; i < indent; ++i) printf("\t");
    if (state->debug_tag) printf("STATE[%s|%p]\n", state->debug_tag, state);
    else printf("STATE[%p]\n", state);
    
    int transition_key;
    NSTATE to;
    void *data;
    SET transition_states;
    SET_ITERATOR state_iter;

    MAP_ITERATOR transition_iter = map_iterator_init(state->transitions);
    while (map_iterator_has_next(transition_iter))
    {
        map_iterator_next(transition_iter, &transition_key, &data);
        transition_states = data;

        state_iter = set_iterator_init(transition_states);
        while (set_iterator_has_next(state_iter))
        {
            to = set_iterator_next(state_iter);
            for (size_t i = 0; i < indent + 1; ++i) printf("\t");

            if (isprint(transition_key)) printf("----[%c]--> ", transition_key);
            else printf("----[%d]--> ", transition_key);
            
            if (to->debug_tag) printf("STATE[%s|%p]\n", to->debug_tag, to);
            else printf("STATE[%p]\n", to);
        }
        set_iterator_fini(state_iter);
    }
    map_iterator_fini(transition_iter);
}

// -------------------------------------------------------------------------------------- //

struct nondeterministic_finite_automaton
{
    NSTATE starting_state;
    SET accepting_states;
    SET all_states;
};

// O(n + m)
static void aggregate_states(NSTATE state, SET state_set)
{
    set_add(state_set, state);

    void *data;
    SET transition_set;
    SET_ITERATOR state_iter;
    NSTATE next_state;

    MAP_ITERATOR transition_iter = map_iterator_init(state->transitions);
    while (map_iterator_has_next(transition_iter))
    {
        map_iterator_next(transition_iter, NULL, &data);
        transition_set = data;
        state_iter = set_iterator_init(transition_set);
        while (set_iterator_has_next(state_iter))
        {
            next_state = set_iterator_next(state_iter);
            if (!set_contains(state_set, next_state))
                aggregate_states(next_state, state_set);
        }
        set_iterator_fini(state_iter);
    }
    map_iterator_fini(transition_iter);
}

// O(n + m)
NFA nfa_new(NSTATE starting_state, NSTATE *accepting_states, size_t num_accepting_states)
{
    if (!starting_state || !accepting_states || !num_accepting_states) return NULL;

    SET accepting = set_init();
    for (size_t i = 0; i < num_accepting_states; ++i) 
        set_add(accepting, accepting_states[i]);

    SET all = set_init();
    aggregate_states(starting_state, all);

    // invariant violated, error
    if (!is_subset(accepting, all))
    {
        set_fini(accepting);
        set_fini(all);
        return NULL;
    }

    // we have a valid NFA, we lock all the states
    NSTATE state;
    SET_ITERATOR iter = set_iterator_init(all);
    while (set_iterator_has_next(iter))
    {
        state = set_iterator_next(iter);
        LOCK_STATE(state);
    }
    set_iterator_fini(iter);

    // allocate now
    NFA nfa = malloc(sizeof(struct nondeterministic_finite_automaton));
    nfa->starting_state = starting_state;
    nfa->accepting_states = accepting;
    nfa->all_states = all;

    return nfa;
}

void nfa_free(NFA automaton)
{
    NSTATE state;
    SET_ITERATOR iter = set_iterator_init(automaton->all_states);
    while (set_iterator_has_next(iter))
    {
        state = set_iterator_next(iter);
        __nstate_force_free(state);
    }
    set_iterator_fini(iter);

    set_fini(automaton->all_states);
    set_fini(automaton->accepting_states);
    free(automaton);
}

/**
 * releases ownership of the states referenced by this automaton
 * and destroys the NFA.
 */
static void __nfa_release(NFA automaton)
{
    NSTATE state;
    SET_ITERATOR iter = set_iterator_init(automaton->all_states);
    while (set_iterator_has_next(iter))
    {
        state = set_iterator_next(iter);
        UNLOCK_STATE(state);
    }
    set_iterator_fini(iter);

    set_fini(automaton->all_states);
    set_fini(automaton->accepting_states);
    free(automaton);
}

void nfa_debug_display(NFA automaton)
{
    SET_ITERATOR iter;
    NSTATE state;

    NSTATE starting_state = automaton->starting_state;
    SET accepting_state = automaton->accepting_states;
    SET all_states = automaton->all_states;

    printf("NFA[%p]\n", automaton);
    printf("\tSTARTING STATE: ");

    if (starting_state->debug_tag) printf("STATE[%s|%p]\n", starting_state->debug_tag, starting_state);
    else printf("STATE[%p]\n", starting_state);

    printf("\tACCEPTING STATES:\n");
    iter = set_iterator_init(accepting_state);
    while (set_iterator_has_next(iter))
    {
        state = set_iterator_next(iter);
        if (state->debug_tag) printf("\t\tSTATE[%s|%p]\n", state->debug_tag, state);
        else printf("\t\tSTATE[%p]\n", state);
    }
    set_iterator_fini(iter);

    printf("\tALL STATES:\n");
    iter = set_iterator_init(all_states);
    while (set_iterator_has_next(iter))
    {
        state = set_iterator_next(iter);
        nstate_debug_display(state, 2);
    }
    set_iterator_fini(iter);
}