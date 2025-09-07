#ifndef AUTOMATA_ALGORITHM_H
#define AUTOMATA_ALGORITHM_H

#include "nfa.h"
#include "dfa.h"

DFA subset_construction(NFA nfa);

DFA minimize_dstates(DFA dfa);

#endif