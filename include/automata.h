/**
 * interface of module for describing finite automata.
 * provides primitive operations for creating, manipulating, and destroying automatas
 * 
 * the interface provides two main automata:
 * 1. Nondeterministic finite automata (NFA)
 * 2. Deterministic finite automata (DFA)
 */

#ifndef REGEX_AUTOMATA_H
#define REGEX_AUTOMATA_H

/**
 * Handler for a single state within the automaton
 */
typedef struct state * STATE;

/**
 * Handler for a transition between two states
 */
typedef struct transition * TRANSITION;

/**
 * Handler for a nondeterministic finite automaton (NFA)
 */
typedef struct nondeterministic_finite_automaton * NFA;

/**
 * Handler for a deterministic nonfinite automaton (DFA)
 */
typedef struct deterministic_finite_automaton * DFA;


#endif