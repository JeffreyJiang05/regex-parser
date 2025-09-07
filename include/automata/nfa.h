#ifndef NFA_H
#define NFA_H

#include <stdlib.h>

#include "common.h"
#include "macro.h"

typedef int SYMBOL;

typedef struct nondeterministic_state * NSTATE;

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
 * @return zero on success; nonzero on failure
 */
int nstate_free(NSTATE state);

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
 * @param to the state the transition sends in
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
 * @return zero on success; nonzero otherwise
 */
int nstate_clear_transition_symbol(NSTATE from, SYMBOL sym);

/**
 * removes all transitions from a state
 * 
 * @param from the state whose transitions will be removed
 * @return zero on success; nonzero otherwise
 */
int nstate_clear_all_transitions(NSTATE from);

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
NSTATE *nstate_get_transition_states(NSTATE state, SYMBOL sym);

/**
 * counts the number of transitions with a given symbol starting
 * from a provided state. 
 * 
 * @param state the starting state
 * @param sym the symbol 
 * @return the number of states reached via `sym` starting at `state`
 */
size_t nstate_count_transition_states(NSTATE state, SYMBOL sym);

/**
 * returns if there is a transition from one state to another on a symbol
 * 
 * @param from the state the transition beings
 * @param sym the symbol the transition accepts
 * @param to the state to transition end
 * @return nonzero if the transition exists; zero otherwise
 */
int nstate_has_transition(NSTATE from, SYMBOL sym, NSTATE to);

/**
 * prints the NSTATE in a debug friendly way
 * 
 * @param state the state to display
 * @param indent the number of indents for the display
 */
void nstate_debug_display(NSTATE state, size_t indent);

// -------------------------------------------------------------------------------------- //

typedef struct nondeterministic_finite_automaton * NFA;

/**
 * creates a new nondeterministc finite automata (NFA)
 * 
 * upon passing a state machine to NFA, all states connected to the starting state
 * via transitions are under the control of the NFA. This means when the NFA is
 * destroyed, the these states are also destroyed.  additionally, the state machine
 * should not be modified using the primitive NSTATE functions. 
 * 
 * it is an error to pass in an accepting state in which there does not exist a path
 * from the starting state to said accepting state.
 * 
 * @param starting_state the starting state for the NFA
 * @param accepting_states a list of accepting states
 * @param num_accepting_states the number of accepting states
 * @return the newly created NFA if valid, null on any error
 */
NFA nfa_new(NSTATE starting_state, NSTATE *accepting_states, size_t num_accepting_states);

/**
 * destroys a nondeterministic finite automata (NFA)
 * 
 * @param automaton the NFA to destroy
 */
void nfa_free(NFA automaton);

/**
 * retrieves the starting state for the NFA
 * 
 * @param automaton the NFA to get the start state of
 * @return the start state of `automaton`
 */
NSTATE nfa_get_starting_state(NFA automaton);

/**
 * retrieves a list of the accepting states for the NFA
 * 
 * @param automaton the NFA to get the accepting states of
 * @return the dynamically allocated list of accepting states
 * @warning this function returns dynamically allocated memory
 */
NSTATE *nfa_get_accepting_states(NFA automaton);

/**
 * retrieves the number of accepting states in the NFA
 * 
 * @param automaton the NFA to count the number of accepting states
 * @return the number of accepting states
 */
size_t nfa_count_accepting_states(NFA automaton);

/**
 * retrieves a list of all the states in the NFA
 * 
 * @param automaton the NFA to get states of
 * @return the dynamically allocated list of all the states in the automaton
 * @warning this function returns dynamically allocated memory
 */
NSTATE *nfa_get_states(NFA automaton);

/**
 * retrieves the total number of states in the automaton
 * 
 * @param automaton the NFA to count all the states of
 * @return the number of states in the NFA
 */
size_t nfa_count_states(NFA automaton);

// the following functions are used to determine if a string is accepted by an NFA

/**
 * determines if the automaton accepts the following string
 * 
 * @param automaton the NFA
 * @param string the zero terminated string to check for acceptance
 * @return true if the automaton accepts the string; false otherwise
 */
int nfa_accept(NFA automaton, SYMBOL *string);

/**
 * determines if the automaton accepts the following c-style string
 * 
 * @param automaton the NFA
 * @param string the null terminated char string to check for acceptance
 * @return true if the automaton accepts the string; false otherwise
 */
int nfa_accept_cstr(NFA automaton, char *string);

/**
 * prints the NFA in a debug friendly way
 * 
 * @param automaton the NFA to display
 */
void nfa_debug_display(NFA automaton);

// -------------------------------------------------------------------------------------- //

typedef struct NFA_simulator * NFA_SIM;

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
 */
void nfa_sim_step(NFA_SIM sim, SYMBOL input_sym);

/**
 * destroys the NFA simulator 
 * 
 * @param sim the simulator to destroy
 * @return the final simulator status before destruction
 */
SIM_STATUS nfa_sim_fini(NFA_SIM sim);

// ------------------------------------------------------------------------ //

typedef struct nfa_component * NFA_COMPONENT;

NFA nfa_construct(NFA_COMPONENT component);

NFA_COMPONENT nfa_symbol(SYMBOL sym);

NFA_COMPONENT nfa_union(NFA_COMPONENT a, NFA_COMPONENT b);

NFA_COMPONENT nfa_concat(NFA_COMPONENT a, NFA_COMPONENT b);

NFA_COMPONENT nfa_repeat(NFA_COMPONENT a);

NFA_COMPONENT nfa_repeat_exact(NFA_COMPONENT a, size_t count);

NFA_COMPONENT nfa_repeat_min(NFA_COMPONENT a, size_t min);

NFA_COMPONENT nfa_repeat_min_max(NFA_COMPONENT a, size_t min, size_t max);

NFA_COMPONENT nfa_concat_va(size_t count, ...);

// macro for concat many components together
#define nfa_concat_many(...) nfa_concat_va(NARGS(__VA_ARGS__), __VA_ARGS__)

NFA_COMPONENT nfa_union_va(size_t count, ...);

#define nfa_union_many(...) nfa_union_va(NARGS(__VA_ARGS__), __VA_ARGS__)

inline NFA_COMPONENT __sym_conv_address(NFA_COMPONENT component) { return component; };
#define SYM_CONV(type) _Generic((type), \
                        int: nfa_symbol, \
                        default: __sym_conv_address \
)(type)
#define SYM_CONV_LIST_INDIRECTION() SYM_CONV_LIST_HELPER
#define SYM_CONV_LIST1(arg0) SYM_CONV(arg0)
#define SYM_CONV_LIST0(arg0, ...) SYM_CONV(arg0), SYM_CONV_LIST_INDIRECTION EMPTY()()(__VA_ARGS__)
#define SYM_CONV_LIST_HELPER(...) CAT(SYM_CONV_LIST, EQUAL(NARGS(__VA_ARGS__), 1))(__VA_ARGS__)
#define SYM_CONV_LIST(...) EVAL4(SYM_CONV_LIST_HELPER(__VA_ARGS__))

#define NFA_SYMBOL(sym) nfa_symbol(sym)
#define NFA_UNION(arg0, arg1) nfa_union(SYM_CONV(arg0), SYM_CONV(arg1))
#define NFA_CONCAT(arg0, arg1) nfa_concat(SYM_CONV(arg0), SYM_CONV(arg1))
#define NFA_REPEAT(arg0) nfa_repeat(SYM_CONV(arg0))
#define NFA_REPEAT_EXACT(arg0, count) nfa_repeat_exact(SYM_CONV(arg0), count)
#define NFA_REPEAT_MIN(arg0, min) nfa_repeat_min(SYM_CONV(arg0), min)
#define NFA_REPEAT_MIN_MAX(arg0, min, max) nfa_repeat_min_max(SYM_CONV(arg0), min, max)
#define NFA_CONCAT_MANY(...) nfa_concat_many(SYM_CONV_LIST(__VA_ARGS__))
#define NFA_UNION_MANY(...) nfa_union_many(SYM_CONV_LIST(__VA_ARGS__)) 

#endif