/**
 * This component generate a dot file to visualize a DFA
 */

#ifndef DOT_H
#define DOT_H

#include <stdio.h>

#include "nfa.h"
#include "dfa.h"

int nfa_gen_dot(NFA nfa, char *filename);

int nfa_gen_img(NFA nfa, char *filename);

int dfa_gen_dot(DFA dfa, char *filename);

int dfa_gen_dot(DFA dfa, char *filename);

#endif