#ifndef NFA_H
#define NFA_H

#include <stdlib.h>

typedef int SYMBOL;

typedef struct nondeterministic_state * NSTATE;

extern const int EPSILON;

/**
 * creates a new nondeterministic state
 * 
 * @return a newly allocated empty nondeterministic state
 */
NSTATE nstate_new();

/**
 * releases and destrpys an nondeterministc state
 * 
 * @param state the state to destroy
 */
void nstate_free(NSTATE state);

/**
 * creates a new deterministic state with a tag for debugging
 * 
 * @param debug_tag the tag for debugging
 * @return a newly allocated empty nondeterministic state
 */
NSTATE nstate_debug_new(const char *debug_tag);

/**
 * retrieves the debug tag associated with the nondeterministic state
 * if there is one
 * 
 * @param state the state whose debug tag to retrieve
 * @return the tag associated with `state`
 */
const char *nstate_tag(NSTATE state);

/**
 * adds a new transition to the nondeterministic state
 * 
 * @param from the state the transition starts from
 * @param sym the symbol that causes the transition
 * @param to the state the transitionsends in
 * @return zero on success; nonzero otherwise 
 */
int nstate_add_transition(NSTATE from, SYMBOL sym, NSTATE to);

/**
 * removes a transition from a state to another state on a symbol
 * 
 * @param from the state the transition starts from
 * @param sym the symbol that causes the transition to be removed
 * @param to the state that transition ends in
 * @return zero on success; nonzero otherwise
 */
int nstate_remove_transition(NSTATE from, SYMBOL sym, NSTATE to);

/**
 * removes all transitions from a state on a symbol
 * 
 * @param from the state the transitions start from
 * @param sym the symbol that causes the transitions to be removed
 */
void nstate_clear_transition_symbol(NSTATE from, SYMBOL sym);

/**
 * removes all transitions from a state
 * 
 * @param from the state whose transitions will be removed
 */
void nstate_clear_all_transitions(NSTATE from);

/**
 * retrieves a list of symbols which there is a non-trivial 
 * transition starting from a state
 * 
 * @param state the state to get the list of symbols
 * @return the dynamically allocated list of symbols which has a transition
 * @warning the list returned by this function is dynamically
 * allocated and should be freed
 */
SYMBOL *nstate_get_transition_symbols(NSTATE state);

/**
 * counts the number of symbols which there is a non-trivial
 * transition starting from a state
 * 
 * @param state the state to count the transition symbols
 * @return the number of transition symbols from state
 */
size_t nstate_count_transition_symbols(NSTATE state);

/**
 * retrieves a list of states which is reached via a symbol on a
 * provided state.
 * 
 * @param state the starting state
 * @param sym the symbol 
 * @return the list of states reached via `sym` starting at `state`, NULL if none
 */
NSTATE *nstate_get_transitions(NSTATE state, SYMBOL sym);

/**
 * counts the number of transitions with a given symbol starting
 * from a provided state. 
 * 
 * @param state the starting state
 * @param sym the symbol 
 * @return the number of states reached via `sym` starting at `state`
 */
size_t nstate_count_transitions(NSTATE state, SYMBOL sym);

// -------------------------------------------------------------------------------------- //

typedef struct nondeterministic_finite_automaton * NFA;

/**
 * creates a new nondeterministc finite automata (NFA)
 * 
 * @param starting_state the starting state for the NFA
 * @param accepting_states a list of accepting states
 * @param num_accepting_states the number of accepting states
 * @return the newly created NFA
 */
NFA nfa_new(NSTATE starting_state, NSTATE *accepting_states, size_t num_accepting_states);

/**
 * destroys a nondeterministic finite automata (NFA)
 * 
 * @param automaton the NFA to destroy
 */
void nfa_free(NFA automaton);

/**
 * prints the NFA in a debug friendly way
 * 
 * @param automaton the NFA to display
 */
void NFA_debug_display(NFA automaton);

// -------------------------------------------------------------------------------------- //

typedef struct NFA_simulator * NFA_SIM;

extern int SIM_SUCCESS;
extern int SIM_FAILURE;
extern int SIM_PENDING;

typedef enum sim_status
{
    SIM_FAILURE = -1,
    SIM_SUCCESS = 0,
    SIM_PENDING = 1
} SIM_STATUS;

/**
 * creates a new object for simulating the NFA
 * 
 * @param automaton the automaton to simulate
 * @return the object used to simulate the NFA
 */
NFA_SIM nfa_sim_init(NFA automaton);

/**
 * steps through the NFA simulator with an input symbol
 * 
 * @param sim the simulator 
 * @param input_sym the input sym to progress the simulator
 * @return a simulation status
 */
SIM_STATUS nfa_sim_step(NFA_SIM sim, SYMBOL input_sym);

/**
 * destroys the NFA simulator 
 * 
 * @param sim the simulator to destroy
 * @return the final simulator status before destruction
 */
SIM_STATUS nfa_sim_fini(NFA_SIM sim);

#endif