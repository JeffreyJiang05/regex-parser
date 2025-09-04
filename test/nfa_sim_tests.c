#include <criterion/criterion.h>

#include "nfa.h"

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 * [A] --'a'--> [B] --'a'--> [C] --'b'--> [G] --'b'--> [[H]]
 *  |                         |
 * 'b'                        |----'c'--> [[I]]
 * \|/
 * [D] --'a'--> [E] --'b'--> [[F]]
 */
Test(nfa_sim_tests, nfa_sim_trivial, .timeout = 5)
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

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', G);
    nstate_add_transition(G, 'b', H);
    nstate_add_transition(C, 'c', I);
    nstate_add_transition(A, 'b', D);
    nstate_add_transition(D, 'a', E);
    nstate_add_transition(E, 'b', F);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { F, H, I };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);

    status = nfa_accept_cstr(nfa, "");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);

    nfa_free(nfa);
}

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 * [A] --'a'--> [B] --'a'--> [C] --'b'--> [G] --'b'--> [[H]]
 *  |                         |
 * 'b'                        |----'c'--> [[I]]
 * \|/
 * [D] --'a'--> [E] --'b'--> [[F]]
 */
Test(nfa_sim_tests, nfa_sim_simple_0a, .timeout = 5)
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

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', G);
    nstate_add_transition(G, 'b', H);
    nstate_add_transition(C, 'c', I);
    nstate_add_transition(A, 'b', D);
    nstate_add_transition(D, 'a', E);
    nstate_add_transition(E, 'b', F);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { F, H, I };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'b');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = nfa_accept_cstr(nfa, "aabb");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    nfa_free(nfa);
}

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 * [A] --'a'--> [B] --'a'--> [C] --'b'--> [G] --'b'--> [[H]]
 *  |                         |
 * 'b'                        |----'c'--> [[I]]
 * \|/
 * [D] --'a'--> [E] --'b'--> [[F]]
 */
Test(nfa_sim_tests, nfa_sim_simple_0b, .timeout = 5)
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

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', G);
    nstate_add_transition(G, 'b', H);
    nstate_add_transition(C, 'c', I);
    nstate_add_transition(A, 'b', D);
    nstate_add_transition(D, 'a', E);
    nstate_add_transition(E, 'b', F);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { F, H, I };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'b');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);
    
    status = nfa_accept_cstr(nfa, "aab");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);

    nfa_free(nfa);
}

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 * [A] --'a'--> [B] --'a'--> [C] --'b'--> [G] --'b'--> [[H]]
 *  |                         |
 * 'b'                        |----'c'--> [[I]]
 * \|/
 * [D] --'a'--> [E] --'b'--> [[F]]
 */
Test(nfa_sim_tests, nfa_sim_simple_0c, .timeout = 5)
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

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', G);
    nstate_add_transition(G, 'b', H);
    nstate_add_transition(C, 'c', I);
    nstate_add_transition(A, 'b', D);
    nstate_add_transition(D, 'a', E);
    nstate_add_transition(E, 'b', F);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { F, H, I };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'c');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);
    
    status = nfa_accept_cstr(nfa, "aac");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", SIM_SUCCESS, status);

    nfa_free(nfa);
}

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 * [A] --'a'--> [B] --'a'--> [C] --'b'--> [G] --'b'--> [[H]]
 *  |                         |
 * 'b'                        |----'c'--> [[I]]
 * \|/
 * [D] --'a'--> [E] --'b'--> [[F]]
 */
Test(nfa_sim_tests, nfa_sim_simple_0d, .timeout = 5)
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

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', G);
    nstate_add_transition(G, 'b', H);
    nstate_add_transition(C, 'c', I);
    nstate_add_transition(A, 'b', D);
    nstate_add_transition(D, 'a', E);
    nstate_add_transition(E, 'b', F);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { F, H, I };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'b');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);
    
    status = nfa_accept_cstr(nfa, "bab");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);


    nfa_free(nfa);
}

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 * [A] --'a'--> [B] --'a'--> [C] --'b'--> [G] --'b'--> [[H]]
 *  |                         |
 * 'b'                        |----'c'--> [[I]]
 * \|/
 * [D] --'a'--> [E] --'b'--> [[F]]
 */
Test(nfa_sim_tests, nfa_sim_simple_0e, .timeout = 5)
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

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', G);
    nstate_add_transition(G, 'b', H);
    nstate_add_transition(C, 'c', I);
    nstate_add_transition(A, 'b', D);
    nstate_add_transition(D, 'a', E);
    nstate_add_transition(E, 'b', F);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { F, H, I };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'k');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);
    
    status = nfa_accept_cstr(nfa, "babk");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);

    nfa_free(nfa);
}

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 *       [B] --'a'--> [C] --'b'--> [D] --'b'--> [[J]]
 *      /  
 *    /'a'
 * [A] --'a'--> [E] --'a'--> [F] --'c'--> [[G]]
 *   \'b'
 *    \                
 *     [H] --'a'--> [I] --'b'--> [[K]]
 */   
Test(nfa_sim_tests, nfa_sim_simple_1a, .timeout = 5)
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
    NSTATE K = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', D);
    nstate_add_transition(D, 'b', J);
    nstate_add_transition(A, 'a', E);
    nstate_add_transition(E, 'a', F);
    nstate_add_transition(F, 'c', G);
    nstate_add_transition(H, 'a', I);
    nstate_add_transition(A, 'b', H);
    nstate_add_transition(I, 'b', K);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { G, J, K };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'b');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);
    
    status = nfa_accept_cstr(nfa, "aab");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);
        
    nfa_free(nfa);
}

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 *       [B] --'a'--> [C] --'b'--> [D] --'b'--> [[J]]
 *      /  
 *    /'a'
 * [A] --'a'--> [E] --'a'--> [F] --'c'--> [[G]]
 *   \'b'
 *    \                
 *     [H] --'a'--> [I] --'b'--> [[K]]
 */   
Test(nfa_sim_tests, nfa_sim_simple_1b, .timeout = 5)
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
    NSTATE K = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', D);
    nstate_add_transition(D, 'b', J);
    nstate_add_transition(A, 'a', E);
    nstate_add_transition(E, 'a', F);
    nstate_add_transition(F, 'c', G);
    nstate_add_transition(H, 'a', I);
    nstate_add_transition(A, 'b', H);
    nstate_add_transition(I, 'b', K);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { G, J, K };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'b');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);
    
    status = nfa_accept_cstr(nfa, "aabb");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    nfa_free(nfa);
}

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 *       [B] --'a'--> [C] --'b'--> [D] --'b'--> [[J]]
 *      /  
 *    /'a'
 * [A] --'a'--> [E] --'a'--> [F] --'c'--> [[G]]
 *   \'b'
 *    \                
 *     [H] --'a'--> [I] --'b'--> [[K]]
 */   
Test(nfa_sim_tests, nfa_sim_simple_1c, .timeout = 5)
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
    NSTATE K = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', D);
    nstate_add_transition(D, 'b', J);
    nstate_add_transition(A, 'a', E);
    nstate_add_transition(E, 'a', F);
    nstate_add_transition(F, 'c', G);
    nstate_add_transition(H, 'a', I);
    nstate_add_transition(A, 'b', H);
    nstate_add_transition(I, 'b', K);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { G, J, K };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'c');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);
    
    status = nfa_accept_cstr(nfa, "aac");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);
   
    nfa_free(nfa);
}

/**
 * The following NFA can match (Starting A)
 *   aabb
 *   aac
 *   bab
 * 
 *       [B] --'a'--> [C] --'b'--> [D] --'b'--> [[J]]
 *      /  
 *    /'a'
 * [A] --'a'--> [E] --'a'--> [F] --'c'--> [[G]]
 *   \'b'
 *    \                
 *     [H] --'a'--> [I] --'b'--> [[K]]
 */   
Test(nfa_sim_tests, nfa_sim_simple_1d, .timeout = 5)
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
    NSTATE K = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', C);
    nstate_add_transition(C, 'b', D);
    nstate_add_transition(D, 'b', J);
    nstate_add_transition(A, 'a', E);
    nstate_add_transition(E, 'a', F);
    nstate_add_transition(F, 'c', G);
    nstate_add_transition(H, 'a', I);
    nstate_add_transition(A, 'b', H);
    nstate_add_transition(I, 'b', K);

    NSTATE starting_state = A;
    NSTATE accepting_states[] = { G, J, K };

    NFA nfa = nfa_new(starting_state, accepting_states, 3);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'b');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);
    
    status = nfa_accept_cstr(nfa, "bab");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);
    
    nfa_free(nfa);
}

/**
 * The following NFA can match
 *   (ad|bc)*
 */
Test(nfa_sim_tests, nfa_sim_simple_2a, .timeout = 5)
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

    NSTATE starting_state = I;
    NSTATE accepting_states[] = { J };

    NFA nfa = nfa_new(starting_state, accepting_states, 1);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'd');
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'c');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = nfa_accept_cstr(nfa, "adbc");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    nfa_free(nfa);
}

/**
 * The following NFA can match
 *   (ad|bc)*
 */
Test(nfa_sim_tests, nfa_sim_simple_2b, .timeout = 5)
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

    NSTATE starting_state = I;
    NSTATE accepting_states[] = { J };

    NFA nfa = nfa_new(starting_state, accepting_states, 1);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'd');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'd');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'd');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = nfa_accept_cstr(nfa, "adadad");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    nfa_free(nfa);
}

/**
 * The following NFA can match 
 *   (ad|bc)*
 */
Test(nfa_sim_tests, nfa_sim_simple_2c, .timeout = 5)
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

    NSTATE starting_state = I;
    NSTATE accepting_states[] = { J };

    NFA nfa = nfa_new(starting_state, accepting_states, 1);

    NFA_SIM sim = nfa_sim_init(nfa);
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = nfa_accept_cstr(nfa, "");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    nfa_free(nfa);
}

/**
 * The following NFA can match
 *   (ad|bc)*
 */
Test(nfa_sim_tests, nfa_sim_simple_2d, .timeout = 5)
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

    NSTATE starting_state = I;
    NSTATE accepting_states[] = { J };

    NFA nfa = nfa_new(starting_state, accepting_states, 1);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'c');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'd');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'd');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'd');
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'c');
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'c');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = nfa_accept_cstr(nfa, "bcadadadbcbc");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    nfa_free(nfa);
}

/**
 * The following NFA can match
 *   (ad|bc)*
 */
Test(nfa_sim_tests, nfa_sim_simple_2e, .timeout = 5)
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

    NSTATE starting_state = I;
    NSTATE accepting_states[] = { J };

    NFA nfa = nfa_new(starting_state, accepting_states, 1);

    NFA_SIM sim = nfa_sim_init(nfa);
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'b');
    nfa_sim_step(sim, 'c');
    nfa_sim_step(sim, 'd');
    nfa_sim_step(sim, 'a');
    nfa_sim_step(sim, 'b');
    SIM_STATUS status = nfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);

    status = nfa_accept_cstr(nfa, "abcdab");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);

    nfa_free(nfa);
}