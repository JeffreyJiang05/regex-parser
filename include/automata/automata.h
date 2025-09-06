/**
 * interface of module for describing finite automata.
 * provides primitive operations for creating, manipulating, and destroying automatas
 * 
 * the interface provides two main automata:
 * 1. Nondeterministic finite automata (NFA)
 * 2. Deterministic finite automata (DFA)
 * 
 * the alphabet used is the set of integers. this is ideal as integers are wider than
 * the normal char. additionally, enumerations are stored as integers which allows easy
 * conversion between these integers to values of an not yet specified enumeration.
 */

#ifndef REGEX_AUTOMATA_H
#define REGEX_AUTOMATA_H

#include "nfa.h"
#include "dfa.h"
#include "dot.h"

#endif