#include "nfa.h"

#include "map.h"
#include "set.h"

struct nondeterministic_state
{
    const char *debug_tag;
    // MAP_KEY = symbol
    // MAP_VALUE = SET<NSTATE>
    MAP transitions;
};

NSTATE nstate_new()
{
    NSTATE state = malloc(sizeof(struct nondeterministic_state));
    state->debug_tag = NULL;
    state->transitions = map_init();
}

void nstate_free(NSTATE state)
{
    map_fini(state->transitions);
    free(state);
}

NSTATE nstate_debug_new(const char *debug_tag)
{
    NSTATE state = malloc(sizeof(struct nondeterministic_state));
    state->debug_tag = debug_tag;
    state->transitions = map_init();
}

const char *nstate_tag(NSTATE state)
{
    return state->debug_tag;   
}

int nstate_add_transition(NSTATE from, SYMBOL sym, NSTATE to)
{
    SET sym_set = map_get(from->transitions, sym);
    if (!sym_set)
    {
        sym_set = set_init();
        map_add(map_get, sym, sym_set);
    }
    return set_add(sym_set, to);
}

int nstate_remove_transition(NSTATE from, SYMBOL sym, NSTATE to)
{
    SET sym_set = map_get(from->transitions, sym);
    int ret = set_remove(sym_set, to);
    if (!set_size(sym_set))
    {
        set_fini(sym_set);
        map_remove(from->transitions, sym);
    }
    return ret;
}

void nstate_clear_transition_symbol(NSTATE from, SYMBOL sym)
{
    SET sym_set = map_get(from->transitions, sym);
    if (sym_set) set_fini(sym_set);
    map_remove(from->transitions, sym);
}

void nstate_clear_all_transitions(NSTATE from)
{
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
}

SYMBOL *nstate_get_transition_symbols(NSTATE state)
{
    return map_keys(state->transitions);
}

size_t nstate_count_transition_symbols(NSTATE state)
{
    return map_size(state->transitions);
}

NSTATE *nstate_get_transitions(NSTATE state, SYMBOL sym)
{
    SET sym_set = map_get(state->transitions, sym);
    if (sym_set) return set_values(sym_set); 
    return NULL;
}

size_t nstate_count_transitions(NSTATE state, SYMBOL sym)
{
    SET sym_set = map_get(state->transitions, sym);
    if (sym_set) return set_size(sym_set); 
    return 0;
}

