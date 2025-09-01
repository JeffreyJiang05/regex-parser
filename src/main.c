#include <stdio.h>

#include "debug.h"

#include "nfa.h"

int main()
{

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', B);
    nstate_add_transition(B, 'b', C);
    nstate_add_transition(C, 'c', A);

    nstate_remove_transition(B, 'a', B);
    nstate_remove_transition(B, 'a', B);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
    return 0;
}