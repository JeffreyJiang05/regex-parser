#include <stdio.h>

#include "debug.h"

#include "nfa.h"

int main()
{
    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();
    NSTATE D = nstate_new();
    NSTATE E = nstate_new();
    NSTATE F = nstate_new();
    NSTATE G = nstate_new();
    NSTATE H = nstate_new();
    NSTATE I = nstate_new();
    NSTATE J = nstate_new();

    nstate_add_transition(I, EPSILON, J);
    nstate_add_transition(I, EPSILON, G);
    nstate_add_transition(G, EPSILON, A);
    nstate_add_transition(G, EPSILON, D);
    nstate_add_transition(A, 'a', B);
    nstate_add_transition(D, 'b', E);
    nstate_add_transition(B, 'd', C);
    nstate_add_transition(E, 'c', F);
    nstate_add_transition(C, EPSILON, H);
    nstate_add_transition(F, EPSILON, H);
    nstate_add_transition(H, EPSILON, G);
    nstate_add_transition(H, EPSILON, J);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { F, H, I };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'd');
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'c');
    SIM_STATUS status = nfa_sim_fini(sim);

    printf("%d\n", status);
    
    nfa_free(nfa);
    return 0;
}