#include <stdio.h>

#include "debug.h"

#include "automata/nfa.h"
#include "automata/dot.h"

int main()
{
    /* NFA_COMPONENT component = nfa_repeat(
        nfa_union(
            nfa_concat(
                nfa_symbol('a'),
                nfa_symbol('b')
            ),
            nfa_concat(
                nfa_symbol('c'),
                nfa_symbol('d')
            )
        )
    );

    NFA nfa = nfa_construct(component);
    nfa_gen_img(nfa, "graph.png");
    nfa_free(nfa); */

    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'c', A);

    DFA dfa = dfa_new(A, &C, 1);

    dfa_gen_img(dfa, "output.png");

    dfa_free(dfa);

    return 0;
}