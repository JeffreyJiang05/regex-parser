#include <stdio.h>

#include "debug.h"

#include "automata/nfa.h"
#include "automata/dot.h"
#include "automata/algorithm.h"

int main()
{
    char *nfa_output_fn = "NFA.png";
    char *dfa_output_fn = "DFA.png";

    // REGEX FOR (a|b)*abb
    /* NFA_COMPONENT builder = nfa_concat(
        nfa_repeat_min(
            nfa_union(
                nfa_concat(
                    nfa_symbol('a'),
                    nfa_symbol('b')
                ),
                nfa_concat(
                    nfa_symbol('c'),
                    nfa_symbol('d')
                )
            ),
        2),
        nfa_concat(
            nfa_symbol('d'),
            nfa_concat(
                nfa_symbol('c'),
                nfa_symbol('b')
            )
        )
    ); */

    NFA_COMPONENT builder = NFA_CONCAT_MANY(
        NFA_REPEAT_MIN(
            NFA_UNION(
                NFA_CONCAT('a', 'b'),
                NFA_CONCAT('c', 'd')
            ), 2
        ),
        'd', 'c', 'b'
    );

    NFA nfa = nfa_construct(builder);
    nfa_gen_img(nfa, nfa_output_fn);
    printf("Generated image for NFA[%p] in %s.\n", nfa, nfa_output_fn);

    DFA dfa = subset_construction(nfa);
    printf("Completed subset construction of NFA[%p].\n", nfa);

    dfa_gen_img(dfa, dfa_output_fn);
    printf("Generated image for DFA[%p] in %s.\n", dfa, dfa_output_fn);

    nfa_free(nfa);
    dfa_free(dfa);

    return 0;
}