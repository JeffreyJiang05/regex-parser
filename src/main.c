#include <stdio.h>

#include "debug.h"

#include "nfa.h"
#include "dot.h"

int main()
{
    NFA_COMPONENT component = nfa_repeat(
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
    nfa_free(nfa);
    return 0;
}