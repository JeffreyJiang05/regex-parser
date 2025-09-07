#include "automata/algorithm.h"

#include <stdint.h>

#include "debug.h"

#include "utility/set.h"
#include "utility/stack.h"
#include "utility/setmap.h"

typedef union
{
    uint64_t val;
    void *ptr;
} CVT;

#define PTR(value) ((CVT){ value }.ptr)
#define SYM(addr) ((SYMBOL) (CVT){.ptr=addr}.val)

typedef SET_MAP DSTATE_MAP;

static DSTATE_MAP dstates_init()
{
    return setmap_init();
}

// free all key sets, does not free the values which wll be DSTATES used to build
// the DFA
static void dstates_fini(DSTATE_MAP map)
{
    SET key = NULL;
    SET_MAP_ITERATOR iter = setmap_iterator_init(map);
    while (setmap_iterator_has_next(iter))
    {
        setmap_iterator_next(iter, &key, NULL);
        set_fini(key);
    }
    setmap_iterator_fini(iter);
    setmap_fini(map);
}

static int dstates_contains(DSTATE_MAP map, SET nstates)
{
    return setmap_contains(map, nstates);
}

static void dstates_add(DSTATE_MAP map, SET nstates)
{
    DSTATE dstate = dstate_new();
    setmap_set(map, nstates, dstate);
}

static DSTATE dstates_get(DSTATE_MAP map, SET nstates)
{
    return setmap_get(map, nstates);
}



static SET make_set(NSTATE *states, size_t sz)
{
    SET set = set_init();
    for (size_t i = 0; i < sz; ++i)
        set_add(set, states[i]);
    return set;
}

static SET set_clone(SET set)
{
    void *data;
    SET copy = set_init();
    SET_ITERATOR iter = set_iterator_init(set);
    while (set_iterator_has_next(iter))
    {
        data = set_iterator_next(iter);
        set_add(copy, data);
    }
    set_iterator_fini(iter);    

    return copy;
}

static SET epsilon_closure_set(SET states)
{
    SET epsilon_closure = set_clone(states);
    STACK stack = stack_init();

    NSTATE start;
    SET_ITERATOR iter = set_iterator_init(states);

    // push all elements in the set of states onto the stack
    while (set_iterator_has_next(iter))
    {
        start = set_iterator_next(iter);
        stack_push(stack, start);
    }
    set_iterator_fini(iter);

    void *state = NULL;
    while (stack_size(stack) != 0)
    {
        stack_pop(stack, &state);
        NSTATE *transition_states = nstate_get_transition_states(state, EPSILON);
        size_t transition_states_sz = nstate_count_transition_states(state, EPSILON);
        
        for (size_t i = 0; i < transition_states_sz; ++i)
        {
            NSTATE to = transition_states[i];
            if (!set_contains(epsilon_closure, to))
            {
                set_add(epsilon_closure, to);
                stack_push(stack, to);
            }
        }
        free(transition_states);
    }
    stack_fini(stack);
    
    return epsilon_closure;
}

static SET epsilon_closure_state(NSTATE state)
{
    SET self = set_init();
    set_add(self, state);
    SET epsilon_closure = epsilon_closure_set(self);
    set_fini(self);

    return epsilon_closure;
}

static SET move(SET from_states, SYMBOL sym)
{
    SET target = set_init();

    NSTATE from;
    SET_ITERATOR iter = set_iterator_init(from_states);
    while (set_iterator_has_next(iter))
    {
        from = set_iterator_next(iter);

        NSTATE *to_states = nstate_get_transition_states(from, sym);
        size_t to_states_sz = nstate_count_transition_states(from, sym);
        if (to_states)
        {
            for (size_t i = 0; i < to_states_sz; ++i)
            {
                NSTATE to = to_states[i];
                set_add(target, to);
            }
            free(to_states);
        }
    }
    set_iterator_fini(iter);

    return target;
}

static SET nonepsilon_transition_symbols(SET states)
{
    SET transition_symbols = set_init();

    SET_ITERATOR iter = set_iterator_init(states);
    while (set_iterator_has_next(iter))
    {
        NSTATE state = set_iterator_next(iter);
        SYMBOL *symbols = nstate_get_transition_symbols(state);
        size_t symbol_sz = nstate_count_transition_symbols(state);

        if (symbols)
        {
            for (size_t i = 0; i < symbol_sz; ++i)
            {
                if (symbols[i] != EPSILON)
                    set_add(transition_symbols, PTR(symbols[i]));
            }
            free(symbols);
        }
    }
    set_iterator_fini(iter);

    return transition_symbols;
}

// returns if there is an intersection
static int has_set_intersection(SET a, SET b)
{
    void *data;
    SET_ITERATOR iter = set_iterator_init(a);
    while (set_iterator_has_next(iter))
    {
        data = set_iterator_next(iter);
        if (set_contains(b, data))
        {
            set_iterator_fini(iter);
            return 1;
        }
    }
    set_iterator_fini(iter);
    return 0;
}

DFA subset_construction(NFA nfa)
{
    DSTATE_MAP dstates = dstates_init();
    SET dfa_accepting_states = set_init();

    NSTATE nfa_starting_state = nfa_get_starting_state(nfa);
    void *nfa_accepting_states_list = nfa_get_accepting_states(nfa);
    SET nfa_accepting_states = make_set(nfa_accepting_states_list, nfa_count_accepting_states(nfa));
    free(nfa_accepting_states_list);

    // transfer ownership of the allocated memory to dstates
    SET initial_states = epsilon_closure_state(nfa_starting_state);
    dstates_add(dstates, initial_states);
    
    // stack of unmarked states that need to be processed
    // every set pushed onto the stack should be in dstates
    STACK unmarked_sets = stack_init();
    stack_push(unmarked_sets, initial_states);
    
    SET T, U;
    void *data = NULL;
    SET_ITERATOR iter; 
    while (stack_size(unmarked_sets) != 0)
    {
        // popping off the stack marks the set
        stack_pop(unmarked_sets, &data);
        T = data;

        SET symbols = nonepsilon_transition_symbols(T);
        
        iter = set_iterator_init(symbols);
        while (set_iterator_has_next(iter))
        {
            // extract the symbol
            SYMBOL a = SYM(set_iterator_next(iter));

            SET move_states = move(T, a);
            U = epsilon_closure_set(move_states);

            int dstates_has = dstates_contains(dstates, U);
            if (!dstates_has)
            {
                // transfer the ownership to dstates, we do not need to free
                dstates_add(dstates, U);
                // mark the set of states
                stack_push(unmarked_sets, U);

                // if U has an accepting state in it, it is an accepting state in the DFA
                if (has_set_intersection(U, nfa_accepting_states))
                    set_add(dfa_accepting_states, dstates_get(dstates, U));
            } 

            // add transition
            DSTATE from = dstates_get(dstates, T);
            DSTATE to = dstates_get(dstates, U);
            dstate_add_transition(from, a, to);

            if (dstates_has) set_fini(U); // we create a set that can not be pushed onto DSTATES, we must free manually
            set_fini(move_states);
        }
        set_iterator_fini(iter);
        set_fini(symbols);
    }

    set_fini(nfa_accepting_states); // free the accepting states
    stack_fini(unmarked_sets); // free stack of unmarked sets

    void **dfa_accepting_states_list = set_values(dfa_accepting_states);
    DFA dfa = dfa_new( dstates_get(dstates, initial_states), (DSTATE*) dfa_accepting_states_list, set_size(dfa_accepting_states) );
    free(dfa_accepting_states_list);

    set_fini(dfa_accepting_states);
    dstates_fini(dstates);

    return dfa;
}