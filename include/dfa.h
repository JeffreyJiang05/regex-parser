#ifndef DFA_H
#define DFA_H

#include <stdlib.h>

#include "common.h"

typedef int SYMBOL;

typedef struct deterministic_state * DSTATE;

/**
 * creates a new deterministc state
 * 
 * @return a newly allocated empty deterministic state 
 */
DSTATE dstate_new();

/**
 * releases and destroys a deterministic state
 * 
 * @param state the state to destroy
 * @return zero on success; nonzero on failure
 */
int dstate_free();

/**
 * creates a deterministic state with a tag for debugging
 * 
 * @param debug_tag the tag for debugging
 * @return a newly allocated empty deterministic state
 */
DSTATE dstate_debug_new(const char *debug_tag);

/**
 * retrieves the debug tag associated with the deterministic state
 * if there is one
 * 
 * @param state the state whose debug tag to retrieve
 * @return the tag associated with `state`
 */
const char *dstate_tag(DSTATE state);

/**
 * adds a new transition to the deterministic state
 * 
 * @param from the state the transition starts from
 * @param sym the symbol that causes the transition
 * @param to the state the transition sends in
 * @return zero on success; nonzero otherwise 
 */
int dstate_add_transition(DSTATE from, SYMBOL sym, DSTATE to);

/**
 * removes a transition from a state to another state on a symbol
 * 
 * @param from the state the transition starts from
 * @param sym the symbol that causes the transition to be removed
 * @param to the state that transition ends in
 * @return zero on success; nonzero otherwise
 */
int dstate_remove_transition(DSTATE from, SYMBOL sym, DSTATE to);

/**
 * removes all transitions from a state
 * 
 * @param from the state whose transitions will be removed
 * @return zero on success; nonzero otherwise
 */
int dstate_clear_all_transitions(DSTATE from);

/**
 * retrieves a list of symbols which there is a non-trivial 
 * transition starting from a state
 * 
 * @param state the state to get the list of symbols
 * @return the dynamically allocated list of symbols which has a transition
 * @warning the list returned by this function is dynamically
 * allocated and should be freed
 */
SYMBOL *dstate_get_transition_symbols(DSTATE state);

/**
 * counts the number of symbols which there is a non-trivial
 * transition starting from a state
 * 
 * @param state the state to count the transition symbols
 * @return the number of transition symbols from state
 */
size_t dstate_count_transition_symbols(DSTATE state);

/**
 * retrieves the state which is reached via a symbol on a
 * provided state.
 * 
 * @param state the starting state
 * @param sym the symbol 
 * @return the states reached via `sym` starting at `state`, NULL if none
 */
DSTATE dstate_get_transition_state(DSTATE state, SYMBOL sym);

/**
 * returns if there is a transition from one state to another on a symbol
 * 
 * @param from the state the transition beings
 * @param sym the symbol the transition accepts
 * @param to the state to transition end
 * @return nonzero if the transition exists; zero otherwise
 */
int dstate_has_transition(DSTATE from, SYMBOL sym, DSTATE to);

/**
 * prints the DSTATE in a debug friendly way
 * 
 * @param state the state to display
 * @param indent the number of indents for the display
 */
void dstate_debug_display(DSTATE state, size_t indent);

// -------------------------------------------------------------------------------------- //

typedef struct nondeterministic_finite_automaton * DFA;

/**
 * creates a new deterministc finite automata (DFA)
 * 
 * upon passing a state machine to DFA, all states connected to the starting state
 * via transitions are under the control of the DFA. This means when the DFA is
 * destroyed, the these states are also destroyed.  additionally, the state machine
 * should not be modified using the primitive DSTATE functions. 
 * 
 * it is an error to pass in an accepting state in which there does not exist a path
 * from the starting state to said accepting state.
 * 
 * @param starting_state the starting state for the DFA
 * @param accepting_states a list of accepting states
 * @param num_accepting_states the number of accepting states
 * @return the newly created DFA if valid, null on any error
 */
DFA dfa_new(DSTATE starting_state, DSTATE *accepting_states, size_t num_accepting_states);

/**
 * destroys a nondeterministic finite automata (DFA)
 * 
 * @param automaton the DFA to destroy
 */
void dfa_free(DFA automaton);

/**
 * retrieves the starting state for the DFA
 * 
 * @param automaton the DFA to get the start state of
 * @return the start state of `automaton`
 */
DSTATE dfa_get_starting_state(DFA automaton);

/**
 * retrieves a list of the accepting states for the DFA
 * 
 * @param automaton the DFA to get the accepting states of
 * @return the dynamically allocated list of accepting states
 * @warning this function returns dynamically allocated memory
 */
DSTATE *dfa_get_accepting_states(DFA automaton);

/**
 * retrieves the number of accepting states in the DFA
 * 
 * @param automaton the DFA to count the number of accepting states
 * @return the number of accepting states
 */
size_t dfa_count_accepting_states(DFA automaton);

/**
 * retrieves a list of all the states in the DFA
 * 
 * @param automaton the DFA to get states of
 * @return the dynamically allocated list of all the states in the automaton
 * @warning this function returns dynamically allocated memory
 */
DSTATE *dfa_get_states(DFA automaton);

/**
 * retrieves the total number of states in the automaton
 * 
 * @param automaton the DFA to count all the states of
 * @return the number of states in the DFA
 */
size_t dfa_count_states(DFA automaton);

// the following functions are used to determine if a string is accepted by an DFA

/**
 * determines if the automaton accepts the following string
 * 
 * @param automaton the DFA
 * @param string the zero terminated string to check for acceptance
 * @return true if the automaton accepts the string; false otherwise
 */
int dfa_accept(DFA automaton, SYMBOL *string);

/**
 * determines if the automaton accepts the following c-style string
 * 
 * @param automaton the DFA
 * @param string the null terminated char string to check for acceptance
 * @return true if the automaton accepts the string; false otherwise
 */
int dfa_accept_cstr(DFA automaton, char *string);

/**
 * prints the DFA in a debug friendly way
 * 
 * @param automaton the DFA to display
 */
void dfa_debug_display(DFA automaton);

// -------------------------------------------------------------------------------------- //

#if 0
typedef struct DFA_simulator * DFA_SIM;

/**
 * creates a new object for simulating the DFA
 * 
 * @param automaton the automaton to simulate
 * @return the object used to simulate the DFA
 */
DFA_SIM dfa_sim_init(DFA automaton);

/**
 * steps through the DFA simulator with an input symbol
 * 
 * @param sim the simulator 
 * @param input_sym the input sym to progress the simulator
 */
void dfa_sim_step(DFA_SIM sim, SYMBOL input_sym);

/**
 * destroys the DFA simulator 
 * 
 * @param sim the simulator to destroy
 * @return the final simulator status before destruction
 */
SIM_STATUS dfa_sim_fini(DFA_SIM sim);

#endif

#endif