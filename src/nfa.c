#include "nfa.h"

#include "debug.h"

#include <stdio.h>
#include <ctype.h>

#include "map.h"
#include "set.h"
#include "stack.h"
#include "ptrmap.h"

const int EPSILON = -1;

/**
 * NFA_STATE_LOCKING toggles if the states owned by an NFA should be locked so that they are immutable
 * this is highly recommended for code that requires debugging
 * 
 * Note state locking can have performance costs with certain functions
 */
#ifdef NFA_STATE_LOCKING

#define LOCK_STATE(state) do {      \
    (state)->flags |= 0x1;          \
} while(0)

#define UNLOCK_STATE(state) do {    \
    (state)->flags &= ~0x1;         \
} while(0)

#define IS_STATE_LOCKED(state) ((state)->flags & 0x1)

#define UNLOCK_STATES(nfa) __nfa_unlock_all(nfa)

#define DESTROY_NFA(nfa) __nfa_destroy(nfa)

#else

#define LOCK_STATE(state) do {} while(0)

#define UNLOCK_STATE(state) do {} while (0)

#define IS_STATE_LOCKED(state) (0)

#define UNLOCK_STATES(nfa) do {} while(0)

#define DESTROY_NFA(nfa) do {} while(0)

#endif

struct nondeterministic_state
{
    const char *debug_tag;
    // MAP_KEY = symbol
    // MAP_VALUE = SET<NSTATE>
    MAP transitions;
    int flags;
    int nfa_id;
};

NSTATE nstate_new()
{
    NSTATE state = malloc(sizeof(struct nondeterministic_state));
    state->debug_tag = NULL;
    state->transitions = map_init();
    state->flags = 0;
    state->nfa_id = -1;
    info("Initialized NSTATE[%p:%s].", state, state->debug_tag ? state->debug_tag : "");
    return state;
}

int nstate_free(NSTATE state)
{
    if (IS_STATE_LOCKED(state)) 
    {
        info("Attempting to free locked NSTATE[%p:%s].", state, state->debug_tag ? state->debug_tag : "");
        return -1;
    }

    info("Destroying NSTATE[%p:%s].", state, state->debug_tag ? state->debug_tag : "");
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
}

NSTATE nstate_debug_new(const char *debug_tag)
{
    NSTATE state = malloc(sizeof(struct nondeterministic_state));
    state->debug_tag = debug_tag;
    state->transitions = map_init();
    state->flags = 0;
    info("Initialized NSTATE[%p:%s].", state, state->debug_tag ? state->debug_tag : "");
    return state;
}

const char *nstate_tag(NSTATE state)
{
    return state->debug_tag;   
}

int nstate_add_transition(NSTATE from, SYMBOL sym, NSTATE to)
{
    if (IS_STATE_LOCKED(from)) 
    {
        info("Attempting to modify locked NSTATE[%p:%s].", from, from->debug_tag ? from->debug_tag : "");
        return -1;
    }

    info("Attempting to add transition on symbol %d from NSTATE[%p:%s] to NSTATE[%p:%s].", sym,
        from, from->debug_tag ? from->debug_tag : "",
        to, to->debug_tag ? to->debug_tag : "");

    SET sym_set = map_get(from->transitions, sym);
    if (!sym_set)
    {
        sym_set = set_init();
        map_add(from->transitions, sym, sym_set);
        info("No prior transitions on symbol %d from NSTATE[%p:%s]. Created new Set[%p].", sym,
            from, from->debug_tag ? from->debug_tag : "", sym_set);
    }

    int ret = set_add(sym_set, to);
    if (ret) 
    {
        info("Failed to add transition on symbol %d from NSTATE[%p:%s] to NSTATE[%p:%s].", sym,
            from, from->debug_tag ? from->debug_tag : "",
            to, to->debug_tag ? to->debug_tag : "");
    }
    else 
    {
        info("Successfully added transition on symbol %d from NSTATE[%p:%s] to NSTATE[%p:%s].", sym,
            from, from->debug_tag ? from->debug_tag : "",
            to, to->debug_tag ? to->debug_tag : "");
    }
    return ret;
}

int nstate_remove_transition(NSTATE from, SYMBOL sym, NSTATE to)
{
    if (IS_STATE_LOCKED(from)) 
    {
        info("Attempting to modify locked NSTATE[%p:%s].", from, from->debug_tag ? from->debug_tag : "");
        return -1;
    }

    info("Attempting to remove transition on symbol %d from NSTATE[%p:%s] to NSTATE[%p:%s].", sym,
        from, from->debug_tag ? from->debug_tag : "",
        to, to->debug_tag ? to->debug_tag : "");

    SET sym_set = map_get(from->transitions, sym);
    if (!sym_set) 
    {
        info("Failed to remove transition on symbol %d from NSTATE[%p:%s] to NSTATE[%p:%s]. Transition did not exist.", sym,
            from, from->debug_tag ? from->debug_tag : "",
            to, to->debug_tag ? to->debug_tag : "");
        return -1;
    }
    int ret = set_remove(sym_set, to);
    if (!set_size(sym_set))
    {
        set_fini(sym_set);
        map_remove(from->transitions, sym);
        info("No more transitions on symbol %d from NSTATE[%p:%s] exist after deletion. Freed Set[%p].", sym,
            from, from->debug_tag ? from->debug_tag : "", sym_set);
    }

    info("Successfully removed transition on symbol %d from NSTATE[%p:%s] to NSTATE[%p:%s].", sym,
        from, from->debug_tag ? from->debug_tag : "",
        to, to->debug_tag ? to->debug_tag : "");

    return ret;
}

int nstate_clear_transition_symbol(NSTATE from, SYMBOL sym)
{
    if (IS_STATE_LOCKED(from)) 
    {
        info("Attempting to modify locked NSTATE[%p:%s].", from, from->debug_tag ? from->debug_tag : "");
        return -1;
    }

    SET sym_set = map_get(from->transitions, sym);
    if (sym_set) 
    {
        set_fini(sym_set);
        info("No more transitions on symbol %d from NSTATE[%p:%s] exist after deletion. Freed Set[%p].", sym,
            from, from->debug_tag ? from->debug_tag : "", sym_set);
    }
    map_remove(from->transitions, sym);

    info("Successfully cleared all transitions on symbol %d from NSTATE[%p:%s].", sym,
        from, from->debug_tag ? from->debug_tag : "");

    return 0;
}

int nstate_clear_all_transitions(NSTATE from)
{
    if (IS_STATE_LOCKED(from)) 
    {
        info("Attempting to modify locked NSTATE[%p:%s].", from, from->debug_tag ? from->debug_tag : "");
        return -1;
    }

    SYMBOL sym;
    void *set;
    MAP_ITERATOR iter = map_iterator_init(from->transitions);
    while (map_iterator_has_next(iter))
    {
        map_iterator_next(iter, &sym, &set);
        set_fini(set);
        info("No more transitions on symbol %d from NSTATE[%p:%s] exist after deletion. Freed Set[%p].", sym,
            from, from->debug_tag ? from->debug_tag : "", set);
    }
    map_iterator_fini(iter);
    map_clear(from->transitions);

    info("Successfully cleared all transitions on symbol %d from NSTATE[%p:%s].", sym,
        from, from->debug_tag ? from->debug_tag : "");

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
static void aggregate_states(NSTATE state, SET state_set, int *uid_counter)
{
    set_add(state_set, state);
    state->nfa_id = (*uid_counter)++;

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
                aggregate_states(next_state, state_set, uid_counter);
        }
        set_iterator_fini(state_iter);
    }
    map_iterator_fini(transition_iter);
}

// O(n + m)
NFA nfa_new(NSTATE starting_state, NSTATE *accepting_states, size_t num_accepting_states)
{
    if (!starting_state || !accepting_states || !num_accepting_states) 
    {
        info("Invalid arguments passed to nfa_new.");
        return NULL;
    }

    int nfa_id = 0;

    SET accepting = set_init();
    for (size_t i = 0; i < num_accepting_states; ++i) 
        set_add(accepting, accepting_states[i]);

    SET all = set_init();
    aggregate_states(starting_state, all, &nfa_id);

    // invariant violated, error
    if (!is_subset(accepting, all))
    {
        set_fini(accepting);
        set_fini(all);
        info("Failed to initialize new NFA. Not all accepting states are reachable from the starting state.");
        return NULL;
    }

#ifdef NFA_STATE_LOCKING
    // we have a valid NFA, we lock all the states
    NSTATE state;
    SET_ITERATOR iter = set_iterator_init(all);
    while (set_iterator_has_next(iter))
    {
        state = set_iterator_next(iter);
        LOCK_STATE(state);
    }
    set_iterator_fini(iter);
#endif

    // allocate now
    NFA nfa = malloc(sizeof(struct nondeterministic_finite_automaton));
    nfa->starting_state = starting_state;
    nfa->accepting_states = accepting;
    nfa->all_states = all;

    info("Initializing NFA[%p].", nfa);

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

    info("Destroying NFA[%p].", automaton);

    free(automaton);
}

/**
 * releases ownership of the states referenced by this automaton
 */
static void __nfa_unlock_all(NFA automaton)
{
#ifdef NFA_STATE_LOCKING
    NSTATE state;
    SET_ITERATOR iter = set_iterator_init(automaton->all_states);
    while (set_iterator_has_next(iter))
    {
        state = set_iterator_next(iter);
        UNLOCK_STATE(state);
    }
    set_iterator_fini(iter);
#endif
}

/**
 * destroys the NFA. it does not release ownership of the states
 */
static void __nfa_destroy(NFA automaton)
{
    set_fini(automaton->all_states);
    set_fini(automaton->accepting_states);
    free(automaton);
}

NSTATE nfa_get_starting_state(NFA automaton)
{
    return automaton->starting_state;
}

NSTATE *nfa_get_accepting_states(NFA automaton)
{
    return (NSTATE*) set_values(automaton->accepting_states);
}

size_t nfa_count_accepting_states(NFA automaton)
{
    return set_size(automaton->accepting_states);
}

NSTATE *nfa_get_states(NFA automaton)
{
    return (NSTATE*) set_values(automaton->all_states);
}

size_t nfa_count_states(NFA automaton)
{
    return set_size(automaton->all_states);
}

int nfa_accept(NFA automaton, SYMBOL *string)
{
    NFA_SIM sim = nfa_sim_init(automaton);
    SYMBOL sym;
    while ((sym = *string++))
        nfa_sim_step(sim, sym);
    SIM_STATUS status = nfa_sim_fini(sim);
    return status == SIM_SUCCESS;
}

int nfa_accept_cstr(NFA automaton, char *string)
{
    NFA_SIM sim = nfa_sim_init(automaton);
    SYMBOL sym;
    while ((sym = *string++))
        nfa_sim_step(sim, sym);
    SIM_STATUS status = nfa_sim_fini(sim);
    return status == SIM_SUCCESS;
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

// -------------------------------------------------------------------------------------- //

struct NFA_simulator
{
    NFA nfa;
    STACK old_states;
    STACK new_states;
    unsigned char *already_on;
};

static void merge_sets(SET A, SET B)
{
    SET_ITERATOR iter = set_iterator_init(B);
    void *state;
    while (set_iterator_has_next(iter))
    {   
        state = set_iterator_next(iter);
        set_add(A, state);
    }
    set_iterator_fini(iter);
}

static void add_state(NFA_SIM sim, NSTATE state)
{
    stack_push(sim->new_states, state);
    sim->already_on[state->nfa_id] = 1;

    SET epsilon_transitions = map_get(state->transitions, EPSILON);
    if (epsilon_transitions)
    {
        SET_ITERATOR iter = set_iterator_init(epsilon_transitions);
        NSTATE t;
        while (set_iterator_has_next(iter))
        {
            t = set_iterator_next(iter);
            if (!sim->already_on[t->nfa_id])
                add_state(sim, t);
        }
        set_iterator_fini(iter);
    }
}

static void transfer_states(NFA_SIM sim)
{
    NSTATE state;
    void *data;
    while (stack_size(sim->new_states) != 0)
    {
        stack_pop(sim->new_states, &data);
        state = data;
        stack_push(sim->old_states, state);
        sim->already_on[state->nfa_id] = 0;
    }
}

NFA_SIM nfa_sim_init(NFA automaton)
{
    NFA_SIM sim = malloc(sizeof(struct NFA_simulator));
    sim->nfa = automaton;
    sim->old_states = stack_init();
    sim->new_states = stack_init();
    size_t sz = set_size(automaton->all_states);
    sim->already_on = calloc(sz, sizeof(unsigned char));

    add_state(sim, automaton->starting_state);
    transfer_states(sim);

    return sim;
}

void nfa_sim_step(NFA_SIM sim, SYMBOL input_sym)
{
    void *data;
    NSTATE state;
    SET set;
    SET_ITERATOR iter;

    while (stack_size(sim->old_states) != 0)
    {
        stack_pop(sim->old_states, &data);
        state = data;

        set = map_get(state->transitions, input_sym);
        if (set)
        {
            iter = set_iterator_init(set);
            while (set_iterator_has_next(iter))
            {
                state = set_iterator_next(iter);
                if (!sim->already_on[state->nfa_id])
                    add_state(sim, state);
            }
            set_iterator_fini(iter);
        }
    }

    transfer_states(sim);
}

SIM_STATUS nfa_sim_fini(NFA_SIM sim)
{
    SIM_STATUS status = SIM_FAILURE;

    void *data;
    while (stack_size(sim->old_states) != 0)
    {
        stack_pop(sim->old_states, &data);
        info("Popped NSTATE[%p:%s] from the old states.", data, ((NSTATE) data)->debug_tag ? ((NSTATE) data)->debug_tag : "");
        if (set_contains(sim->nfa->accepting_states, data))
        {
            status = SIM_SUCCESS;
            break;
        }
    }

    stack_fini(sim->old_states);
    stack_fini(sim->new_states);
    free(sim->already_on);
    free(sim);

    return status;
}

static NSTATE nstate_clone(NSTATE state, PTR_MAP ptrmap)
{
    NSTATE copy = nstate_new();
    copy->debug_tag = state->debug_tag;
    copy->flags = state->flags;
    copy->nfa_id = state->nfa_id;

    ptrmap_set(ptrmap, state, copy);

    MAP_ITERATOR map_iter = map_iterator_init(state->transitions);
    SET_ITERATOR set_iter;
    SYMBOL sym;
    void *data;
    while (map_iterator_has_next(map_iter))
    {
        map_iterator_next(map_iter, &sym, &data);

        set_iter = set_iterator_init(data);
        while (set_iterator_has_next(set_iter))
        {
            NSTATE to = set_iterator_next(set_iter);

            NSTATE to_copy = ptrmap_get(ptrmap, to);            
            if (!to_copy) to_copy = nstate_clone(to, ptrmap);

            nstate_add_transition(copy, sym, to_copy);
        }
        set_iterator_fini(set_iter);
    }
    map_iterator_fini(map_iter);
    return copy;
}

struct nfa_component
{
    NSTATE starting_state;
    NSTATE accepting_state;
};

static NFA_COMPONENT component_new(NSTATE start, NSTATE accepting)
{
    NFA_COMPONENT component = malloc(sizeof(struct nfa_component));
    component->starting_state = start;
    component->accepting_state = accepting;
    return component;
}

static void component_free(NFA_COMPONENT component)
{
    free(component);
}

static void component_free_internals(NFA_COMPONENT component)
{
    NFA nfa = nfa_construct(component);
    nfa_free(nfa);
}

static NFA_COMPONENT component_clone(NFA_COMPONENT component)
{
    PTR_MAP map = ptrmap_init();
    NSTATE clone_start = nstate_clone(component->starting_state, map);

    NSTATE clone_accept = ptrmap_get(map, component->accepting_state);
    ptrmap_fini(map);

    NFA_COMPONENT copy = component_new(clone_start, clone_accept);
    return copy;
}

NFA nfa_construct(NFA_COMPONENT component)
{
    info("Constructing NFA from Component[%p].", component);

    NSTATE starting_state = component->starting_state;
    NSTATE accepting_state = component->accepting_state;
    NFA nfa = nfa_new(starting_state, &accepting_state, 1);
    component_free(component);
    return nfa;
}

NFA_COMPONENT nfa_symbol(SYMBOL sym)
{
    NSTATE start = nstate_new();
    NSTATE end = nstate_new();
    nstate_add_transition(start, sym, end);

    NFA_COMPONENT component = component_new(start, end);
    info("Creating symbol %d construct in Component[%p].", sym, component);

    return component;
}

NFA_COMPONENT nfa_union(NFA_COMPONENT a, NFA_COMPONENT b)
{
    // if either is null, just return the other
    if (!a) return b;
    else if (!b) return a;

    info("Attempting to form the union of Component[%p] and Component[%p].", a, b);

    NSTATE start = nstate_new();
    NSTATE end = nstate_new();
    
    nstate_add_transition(start, EPSILON, a->starting_state);
    nstate_add_transition(start, EPSILON, b->starting_state);

    nstate_add_transition(a->accepting_state, EPSILON, end);
    nstate_add_transition(b->accepting_state, EPSILON, end);

    NFA_COMPONENT component = component_new(start, end);

    info("Successfully formed the union of Component[%p] and Component[%p] into Component[%p].", a, b, component);

    component_free(a);
    component_free(b);
    return component;
}

NFA_COMPONENT nfa_concat(NFA_COMPONENT a, NFA_COMPONENT b)
{
    // if either is null, just return the other
    if (!a) return b;
    else if (!b) return a;

    info("Attempting to form the concatenation of Component[%p] and Component[%p].", a, b);

    NSTATE start = a->starting_state;
    NSTATE end = b->accepting_state;
    
    nstate_add_transition(a->accepting_state, EPSILON, b->starting_state);

    NFA_COMPONENT component = component_new(start, end);

    info("Successfully formed the concatenation of Component[%p] and Component[%p] into Component[%p].", a, b, component);
    
    component_free(a);
    component_free(b);

    return component;
}

NFA_COMPONENT nfa_repeat(NFA_COMPONENT a)
{
    NSTATE start = nstate_new();
    NSTATE end = nstate_new();

    nstate_add_transition(start, EPSILON, end);
    nstate_add_transition(start, EPSILON, a->starting_state);
    nstate_add_transition(a->accepting_state, EPSILON, end);
    nstate_add_transition(a->accepting_state, EPSILON, a->starting_state);

    NFA_COMPONENT component = component_new(start, end);
    component_free(a);

    return component;
}

NFA_COMPONENT nfa_repeat_exact(NFA_COMPONENT a, size_t count)
{
    // trivial state
    if (count == 0)
    {
        NSTATE state = nstate_new();
        component_free_internals(a);
        return component_new(state, state);
    }
    else if (count == 1) return a;

    NFA_COMPONENT aggregate = component_clone(a);
    while (--count)
    {
        NFA_COMPONENT clone = component_clone(a);
        aggregate = nfa_concat(aggregate, clone);
    }
    component_free_internals(a);

    return aggregate;
}

NFA_COMPONENT nfa_repeat_min(NFA_COMPONENT a, size_t min)
{
    if (min == 0) return nfa_repeat(a);
    
    NFA_COMPONENT head = component_clone(a);
    while (--min)
    {
        NFA_COMPONENT clone = component_clone(a);
        head = nfa_concat(head, clone);
    }
    
    NFA_COMPONENT tail = nfa_repeat(component_clone(a));
    NFA_COMPONENT final = nfa_concat(head, tail);

    component_free_internals(a);
    return final;
}

// inclusive-inclusive
NFA_COMPONENT nfa_repeat_min_max(NFA_COMPONENT a, size_t min, size_t max)
{
    NFA_COMPONENT aggregate = NULL;

    while (min <= max)
    {
        NFA_COMPONENT clone = component_clone(a);
        NFA_COMPONENT repeat = nfa_repeat_exact(clone, min);
        aggregate = nfa_union(aggregate, repeat);
        min++;
    }

    component_free_internals(a);
    return aggregate;
}