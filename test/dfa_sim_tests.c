#include <criterion/criterion.h>

#include "automata/dfa.h"

/**
 * The following DFA can match (Starting A)
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
Test(dfa_sim_tests, dfa_sim_trivial, .timeout = 5)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();
    DSTATE E = dstate_new();
    DSTATE F = dstate_new();
    DSTATE G = dstate_new();
    DSTATE H = dstate_new();
    DSTATE I = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', C);
    dstate_add_transition(C, 'b', G);
    dstate_add_transition(G, 'b', H);
    dstate_add_transition(C, 'c', I);
    dstate_add_transition(A, 'b', D);
    dstate_add_transition(D, 'a', E);
    dstate_add_transition(E, 'b', F);

    DSTATE starting_state = A;
    DSTATE accepting_states[] = { F, H, I };

    DFA dfa = dfa_new(starting_state, accepting_states, 3);

    DFA_SIM sim = dfa_sim_init(dfa);
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);

    status = dfa_accept_cstr(dfa, "");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);

    dfa_free(dfa);
}

/**
 * The following DFA can match (Starting A)
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
Test(dfa_sim_tests, dfa_sim_simple_0a)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();
    DSTATE E = dstate_new();
    DSTATE F = dstate_new();
    DSTATE G = dstate_new();
    DSTATE H = dstate_new();
    DSTATE I = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', C);
    dstate_add_transition(C, 'b', G);
    dstate_add_transition(G, 'b', H);
    dstate_add_transition(C, 'c', I);
    dstate_add_transition(A, 'b', D);
    dstate_add_transition(D, 'a', E);
    dstate_add_transition(E, 'b', F);

    DSTATE starting_state = A;
    DSTATE accepting_states[] = { F, H, I };

    DFA dfa = dfa_new(starting_state, accepting_states, 3);

    DFA_SIM sim = dfa_sim_init(dfa);
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'b');
    dfa_sim_step(sim, 'b');
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = dfa_accept_cstr(dfa, "aabb");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    dfa_free(dfa);
}

/**
 * The following DFA can match (Starting A)
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
Test(dfa_sim_tests, dfa_sim_simple_0b)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();
    DSTATE E = dstate_new();
    DSTATE F = dstate_new();
    DSTATE G = dstate_new();
    DSTATE H = dstate_new();
    DSTATE I = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', C);
    dstate_add_transition(C, 'b', G);
    dstate_add_transition(G, 'b', H);
    dstate_add_transition(C, 'c', I);
    dstate_add_transition(A, 'b', D);
    dstate_add_transition(D, 'a', E);
    dstate_add_transition(E, 'b', F);

    DSTATE starting_state = A;
    DSTATE accepting_states[] = { F, H, I };

    DFA dfa = dfa_new(starting_state, accepting_states, 3);

    DFA_SIM sim = dfa_sim_init(dfa);
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'c');
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = dfa_accept_cstr(dfa, "aac");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    dfa_free(dfa);
}

/**
 * The following DFA can match (Starting A)
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
Test(dfa_sim_tests, dfa_sim_simple_0c)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();
    DSTATE E = dstate_new();
    DSTATE F = dstate_new();
    DSTATE G = dstate_new();
    DSTATE H = dstate_new();
    DSTATE I = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', C);
    dstate_add_transition(C, 'b', G);
    dstate_add_transition(G, 'b', H);
    dstate_add_transition(C, 'c', I);
    dstate_add_transition(A, 'b', D);
    dstate_add_transition(D, 'a', E);
    dstate_add_transition(E, 'b', F);

    DSTATE starting_state = A;
    DSTATE accepting_states[] = { F, H, I };

    DFA dfa = dfa_new(starting_state, accepting_states, 3);

    DFA_SIM sim = dfa_sim_init(dfa);
    dfa_sim_step(sim, 'b');
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'b');
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = dfa_accept_cstr(dfa, "bab");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    dfa_free(dfa);
}

/**
 * The following DFA can match (Starting A)
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
Test(dfa_sim_tests, dfa_sim_simple_0d)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();
    DSTATE E = dstate_new();
    DSTATE F = dstate_new();
    DSTATE G = dstate_new();
    DSTATE H = dstate_new();
    DSTATE I = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', C);
    dstate_add_transition(C, 'b', G);
    dstate_add_transition(G, 'b', H);
    dstate_add_transition(C, 'c', I);
    dstate_add_transition(A, 'b', D);
    dstate_add_transition(D, 'a', E);
    dstate_add_transition(E, 'b', F);

    DSTATE starting_state = A;
    DSTATE accepting_states[] = { F, H, I };

    DFA dfa = dfa_new(starting_state, accepting_states, 3);

    DFA_SIM sim = dfa_sim_init(dfa);
    dfa_sim_step(sim, 'b');
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'b');
    dfa_sim_step(sim, 'd');
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);

    status = dfa_accept_cstr(dfa, "babd");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);

    dfa_free(dfa);
}

/**
 * Following DFA matches the regex (a|b)*abb, starting A
 *  _ _          _ _
 * / b \        / a \ <_<_<_<
 * \   /        \   //   a   \
 *  [A] -- a --> [B] -- b --> [C] -- b --> [[D]]
 *  /|\            \                        / |
 *   |              <---------- a <--------   |
 *   \____________________ b ________________/
 */
Test(dfa_sim_tests, dfa_sim_simple_1a)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(A, 'b', A);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'a', B);
    dstate_add_transition(C, 'b', D);
    dstate_add_transition(D, 'a', B);
    dstate_add_transition(D, 'b', A);

    DSTATE starting_state = A;
    DFA dfa = dfa_new(starting_state, &D, 1);
    DFA_SIM sim = dfa_sim_init(dfa);
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);

    status = dfa_accept_cstr(dfa, "");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);

    dfa_free(dfa);
}

/**
 * Following DFA matches the regex (a|b)*abb, starting A
 *  _ _          _ _
 * / b \        / a \ <_<_<_<
 * \   /        \   //   a   \
 *  [A] -- a --> [B] -- b --> [C] -- b --> [[D]]
 *  /|\            \                        / |
 *   |              <---------- a <--------   |
 *   \____________________ b ________________/
 */
Test(dfa_sim_tests, dfa_sim_simple_1b)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(A, 'b', A);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'a', B);
    dstate_add_transition(C, 'b', D);
    dstate_add_transition(D, 'a', B);
    dstate_add_transition(D, 'b', A);

    DSTATE starting_state = A;
    DFA dfa = dfa_new(starting_state, &D, 1);
    DFA_SIM sim = dfa_sim_init(dfa);
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'b');
    dfa_sim_step(sim, 'b');
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = dfa_accept_cstr(dfa, "abb");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    dfa_free(dfa);
}

/**
 * Following DFA matches the regex (a|b)*abb, starting A
 *  _ _          _ _
 * / b \        / a \ <_<_<_<
 * \   /        \   //   a   \
 *  [A] -- a --> [B] -- b --> [C] -- b --> [[D]]
 *  /|\            \                        / |
 *   |              <---------- a <--------   |
 *   \____________________ b ________________/
 */
Test(dfa_sim_tests, dfa_sim_simple_1c)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(A, 'b', A);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'a', B);
    dstate_add_transition(C, 'b', D);
    dstate_add_transition(D, 'a', B);
    dstate_add_transition(D, 'b', A);

    DSTATE starting_state = A;
    DFA dfa = dfa_new(starting_state, &D, 1);
    DFA_SIM sim = dfa_sim_init(dfa);
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'b');
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'b');
    dfa_sim_step(sim, 'b');
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_SUCCESS, "Expected status to be %d. Got %d", SIM_SUCCESS, status);

    status = dfa_accept_cstr(dfa, "aabaabb");
    cr_assert(status != 0, "Expected status to be nonzero. Got %d", status);

    dfa_free(dfa);
}

/**
 * Following DFA matches the regex (a|b)*abb, starting A
 *  _ _          _ _
 * / b \        / a \ <_<_<_<
 * \   /        \   //   a   \
 *  [A] -- a --> [B] -- b --> [C] -- b --> [[D]]
 *  /|\            \                        / |
 *   |              <---------- a <--------   |
 *   \____________________ b ________________/
 */
Test(dfa_sim_tests, dfa_sim_simple_1d)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(A, 'b', A);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'a', B);
    dstate_add_transition(C, 'b', D);
    dstate_add_transition(D, 'a', B);
    dstate_add_transition(D, 'b', A);

    DSTATE starting_state = A;
    DFA dfa = dfa_new(starting_state, &D, 1);
    DFA_SIM sim = dfa_sim_init(dfa);
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'b');
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);

    status = dfa_accept_cstr(dfa, "ab");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);

    dfa_free(dfa);
}

/**
 * Following DFA matches the regex (a|b)*abb, starting A
 *  _ _          _ _
 * / b \        / a \ <_<_<_<
 * \   /        \   //   a   \
 *  [A] -- a --> [B] -- b --> [C] -- b --> [[D]]
 *  /|\            \                        / |
 *   |              <---------- a <--------   |
 *   \____________________ b ________________/
 */
Test(dfa_sim_tests, dfa_sim_simple_1e)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(A, 'b', A);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'a', B);
    dstate_add_transition(C, 'b', D);
    dstate_add_transition(D, 'a', B);
    dstate_add_transition(D, 'b', A);

    DSTATE starting_state = A;
    DFA dfa = dfa_new(starting_state, &D, 1);
    DFA_SIM sim = dfa_sim_init(dfa);
    dfa_sim_step(sim, 'c');
    dfa_sim_step(sim, 'a');
    dfa_sim_step(sim, 'b');
    dfa_sim_step(sim, 'b');
    SIM_STATUS status = dfa_sim_fini(sim);

    cr_assert(status == SIM_FAILURE, "Expected status to be %d. Got %d", SIM_FAILURE, status);

    status = dfa_accept_cstr(dfa, "cabb");
    cr_assert(status == 0, "Expected status to be zero. Got %d", status);

    dfa_free(dfa);
}