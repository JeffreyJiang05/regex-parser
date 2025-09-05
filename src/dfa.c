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

