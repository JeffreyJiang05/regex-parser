#include <stdio.h>

#include "debug.h"

#include "nfa.h"

int main()
{
    NFA_COMPONENT component = nfa_repeat_min_max(
        nfa_concat(
            nfa_symbol('a'),
            nfa_concat(
                nfa_symbol('b'),
                nfa_concat(
                    nfa_symbol('c'),
                    nfa_symbol('d')
                )
            )
        ),
    0, 3);

    NFA nfa = nfa_construct(component);
    // cr_assert(nfa != NULL, "Expected nfa_construct to return nonnull. Got %p", nfa);
    nfa_debug_display(nfa);

    nfa_free(nfa);
    return 0;
}