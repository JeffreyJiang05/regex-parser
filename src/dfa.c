#include "dfa.h"

#include "debug.h"

#include <stdio.h>
#include <ctype.h>

#include "map.h"
#include "set.h"

/**
 * DFA_STATE_LOCKING toggles if the states owned by an DFA should be locked so that they are immutable
 * this is highly recommended for code that requires debugging
 * 
 * Note state locking can have performance costs with certain functions
 */
#ifdef DFA_STATE_LOCKING

#define LOCK_STATE(state) do {      \
    (state)->flags |= 0x1;          \
} while(0)

#define UNLOCK_STATE(state) do {    \
    (state)->flags &= ~0x1;         \
} while(0)

#define IS_STATE_LOCKED(state) ((state)->flags & 0x1)

#define UNLOCK_STATES(dfa) __dfa_unlock_all(dfa)

#define DESTROY_NFA(dfa) __dfa_destroy(dfa)

#else

#define LOCK_STATE(state) do {} while(0)

#define UNLOCK_STATE(state) do {} while (0)

#define IS_STATE_LOCKED(state) (0)

#define UNLOCK_STATES(nfa) do {} while(0)

#define DESTROY_NFA(nfa) do {} while(0)

#endif

#define GET_TAG(state) ((state)->debug_tag ? (state)->debug_tag : "")

struct deterministic_state
{
    const char *debug_tag;
    // MAP_KEY = symbols
    // MAP_VALUE = NSTATE
    MAP transitions;
    int flags;
    int dfa_id;
};   

DSTATE dstate_new()
{
    DSTATE state = malloc(sizeof(struct deterministic_state));
    state->debug_tag = NULL;
    state->transitions = map_init();
    state->flags = 0;
    state->dfa_id = 0;
    info("Initialized DSTATE[%p:%s].", state, GET_TAG(state));
    return state;
}

/**
 * force frees the object regardless of lock
 */
static void __dstate_force_free(DSTATE state)
{
    map_fini(state->transitions);
    free(state);
}

int dstate_free(DSTATE state)
{
    if (IS_STATE_LOCKED(state))
    {
        info("Attempting to free locked DSTATE[%p:%s].", state, GET_TAG(state));
        return -1;
    }

    info("Destroying DSTATE[%p:%s].", state, GET_TAG(state));
    map_fini(state->transitions);
    free(state);
    return 0;
}

DSTATE dstate_debug_new(const char *debug_tag)
{
    DSTATE state = malloc(sizeof(struct deterministic_state));
    state->debug_tag = debug_tag;
    state->transitions = map_init();
    state->flags = 0;
    state->dfa_id = 0;
    info("Initialized DSTATE[%p:%s].", state, GET_TAG(state));
    return state;
}

const char *dstate_tag(DSTATE state)
{
    return state->debug_tag;
}

int dstate_add_transition(DSTATE from, SYMBOL sym, DSTATE to)
{
    if (IS_STATE_LOCKED(from)) 
    {
        info("Attempting to modify locked DSTATE[%p:%s].", from, GET_TAG(from));
        return -1;
    }

    if (sym == EPSILON)
    {
        info("Can not add epsilon transition to DSTATE[%p:%s].", from, GET_TAG(from));
        return -1;
    }

    info("Attempting to add transition on symbol %d from DSTATE[%p:%s] to DSTATE[%p:%s].", sym,
        from, GET_TAG(from), to, GET_TAG(to));

    if (map_contains_key(from->transitions, sym))
    {
        info("Failed to add transition on symbol %d from DSTATE[%p:%s] to DSTATE[%p:%s].", sym,
            from, GET_TAG(from), to, GET_TAG(to));
        return -1;
    }

    map_set(from->transitions, sym, to);
    info("Successfully added transition on symbol %d from DSTATE[%p:%s] to DSTATE[%p:%s].", sym,
        from, GET_TAG(from), to, GET_TAG(to));

    return 0;
}

int dstate_remove_transition(DSTATE from, SYMBOL sym, DSTATE to)
{
    if (IS_STATE_LOCKED(from)) 
    {
        info("Attempting to modify locked DSTATE[%p:%s].", from, GET_TAG(from));
        return -1;
    }

    info("Attempting to remove transition on symbol %d from DSTATE[%p:%s] to DSTATE[%p:%s].", sym,
        from, GET_TAG(from), to, GET_TAG(to));

    DSTATE other = map_get(from->transitions, sym);
    if (!other || other != to)
    {
        info("Failed to remove transition on symbol %d from NSTATE[%p:%s] to NSTATE[%p:%s].", sym,
            from, GET_TAG(from), to, GET_TAG(to));
        return -1;
    }

    map_remove(from->transitions, sym);
    return 0;   
}

int dstate_clear_all_transitions(DSTATE from)
{
    if (IS_STATE_LOCKED(from)) 
    {
        info("Attempting to modify locked DSTATE[%p:%s].", from, GET_TAG(from));
        return -1;
    }

    map_clear(from->transitions);
    return 0;
}

SYMBOL *dstate_get_transition_symbols(DSTATE state)
{
    return map_keys(state->transitions);   
}

size_t dstate_count_transition_symbols(DSTATE state)
{
    return map_size(state->transitions);
}

DSTATE dstate_get_transition_state(DSTATE state, SYMBOL sym)
{
    if (!state) return NULL;
    return map_get(state->transitions, sym);
}

int dstate_has_transition(DSTATE from, SYMBOL sym, DSTATE to)
{
    return map_get(from->transitions, sym) == to;
}

void dstate_debug_display(DSTATE state, size_t indent)
{
    for (size_t i = 0; i < indent; ++i) printf("\t");
    if (state->debug_tag) printf("DSTATE[%s|%p]\n", state->debug_tag, state);
    else printf("DSTATE[%p]\n", state);
    
    SYMBOL sym;    
    void *data;
    DSTATE to;
    MAP_ITERATOR iter = map_iterator_init(state->transitions);
    while (map_iterator_has_next(iter))
    {
        map_iterator_next(iter, &sym, &data);
        to = data;
        for (size_t i = 0; i < indent + 1; ++i) printf("\t");

        if (isprint(sym)) printf("----[%c]--> ", sym);
        else printf("----[%d]--> ", sym);

        if (to->debug_tag) printf("DSTATE[%s|%p] ", to->debug_tag, to);
        else printf("DSTATE[%p] ", to);
    }
    map_iterator_fini(iter);
}

// -------------------------------------------------------------------------------------- //

struct deterministic_finite_automaton
{
    DSTATE starting_state;
    SET accepting_states;
    SET all_states;  
};

static void aggregate_states(DSTATE state, SET state_set, int *uid_counter)
{
    set_add(state_set, state);
    state->dfa_id = (*uid_counter)++;

    void *data;
    MAP_ITERATOR iter = map_iterator_init(state->transitions);
    while (map_iterator_has_next(iter))
    {
        map_iterator_next(iter, NULL, &data);
        if (!set_contains(state_set, data)) aggregate_states(data, state_set, uid_counter);
    }
    map_iterator_fini(iter);
}

DFA dfa_new(DSTATE starting_state, DSTATE *accepting_states, size_t num_accepting_states)
{
    if (!starting_state || !accepting_states || !num_accepting_states) 
    {
        info("Invalid arguments passed to dfa_new.");
        return NULL;
    }

    int dfa_id = 0;
    SET accepting = set_init();
    for (size_t i = 0; i < num_accepting_states; ++i)
        set_add(accepting, accepting_states[i]);
    
    SET all = set_init();
    aggregate_states(starting_state, all, &dfa_id);

    if (!is_subset(accepting, all))
    {
        set_fini(accepting);
        set_fini(all);
        info("Failed to initialize new DFA. Not all accepting states are reachable from the starting state.");
        return NULL;
    }

#ifdef DFA_STATE_LOCKING
    // we have a valid DFA, we lock all the states
    DSTATE state;
    SET_ITERATOR iter = set_iterator_init(all);
    while (set_iterator_has_next(iter))
    {
        state = set_iterator_next(iter);
        LOCK_STATE(state);
    }
    set_iterator_fini(iter);
#endif

    // allocate new
    DFA dfa = malloc(sizeof(struct deterministic_finite_automaton));
    dfa->starting_state = starting_state;
    dfa->accepting_states = accepting;
    dfa->all_states = all;

    info("Initializing DFA[%p].", dfa);

    return dfa;
}

void dfa_free(DFA automaton)
{
    DSTATE state; 
    SET_ITERATOR iter = set_iterator_init(automaton->all_states);
    while (set_iterator_has_next(iter))
    {
        state = set_iterator_next(iter);
        __dstate_force_free(state);
    }
    set_iterator_fini(iter);

    set_fini(automaton->all_states);
    set_fini(automaton->accepting_states);
    
    info("Destroying DFA[%p].", automaton);
    free(automaton);
}

DSTATE dfa_get_starting_state(DFA automaton)
{
    return automaton->starting_state;
}

DSTATE *dfa_get_accepting_states(DFA automaton)
{
    return (DSTATE*) set_values(automaton->accepting_states);
}

size_t dfa_count_accepting_states(DFA automaton)
{
    return set_size(automaton->accepting_states);
}

DSTATE *dfa_get_states(DFA automaton)
{
    return (DSTATE*) set_values(automaton->all_states);
}

size_t dfa_count_states(DFA automaton)
{
    return set_size(automaton->all_states);
}

int dfa_accept(DFA automaton, SYMBOL *string);

int dfa_accept_cstr(DFA automaton, char *string);

void dfa_debug_display(DFA automaton)
{
    SET_ITERATOR iter;
    DSTATE state;

    DSTATE starting_state = automaton->starting_state;
    SET accepting_states = automaton->accepting_states;
    SET all_states = automaton->all_states;

    printf("DFA[%p]\n", automaton);
    printf("\tSTARTING STATE: ");

    if (starting_state->debug_tag) printf("STATE[%s|%p]\n", starting_state->debug_tag, starting_state);
    else printf("STATE[%p]\n", starting_state);

    printf("\tACCEPTING STATES:\n");
    iter = set_iterator_init(accepting_states);
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
        dstate_debug_display(state, 2);
    }
    set_iterator_fini(iter);
}
