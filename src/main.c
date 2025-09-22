#include <stdio.h>

#include "debug.h"

#include "automata/nfa.h"
#include "automata/dot.h"
#include "automata/algorithm.h"

int main()
{
    // char *nfa_output_fn = "NFA.png";
    // char *dfa_output_fn = "DFA.png";

    // (ab|cd){2,}dcb
    NFA_COMPONENT builder = NFA_CONCAT_MANY(
        NFA_REPEAT(
            NFA_UNION('a', 'b')
        ),
        'a',
        NFA_REPEAT_EXACT(
            NFA_UNION('a', 'b'),
            2
        )
    );

    NFA nfa = nfa_construct(builder);
    nfa_display(nfa);
    // printf("Generated image for NFA[%p] in %s.\n", nfa);

    DFA dfa = subset_construction(nfa);
    printf("Completed subset construction of NFA[%p].\n", nfa);

    dfa_display(dfa);
    // printf("Generated image for DFA[%p] in %s.\n", dfa);

    nfa_free(nfa);
    dfa_free(dfa);

    return 0;
}