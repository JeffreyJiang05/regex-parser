#include <criterion/criterion.h>

#include "dfa.h"

/**
 * Using the following automaton 
 *                   a
 *                 /   \
 *                 \   /
 * [A] ---- a ----> [B] ---- b ----> [C]
 * /|\                                |
 *  |______________ c ________________|
 */
Test(dfa_tests, dfa_lifetime, .timeout = 5)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'c', A);

    DFA dfa = dfa_new(A, &C, 1);
    cr_assert(dfa != NULL, "Expected dfa_new to return nonnull. Got null");
    dfa_free(dfa);
}

Test(dfa_tests, dfa_bad_arguments, .timeout = 5)
{
    DFA dfa;

    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    
    DSTATE accepting_states[] = { B, C };

    dfa = dfa_new(NULL, accepting_states, 2);
    cr_assert(dfa == NULL, "Expected dfa_new to return null. Got %p", dfa);

    dfa = dfa_new(A, NULL, 3);
    cr_assert(dfa == NULL, "Expected dfa_new to return null. Got %p", dfa);

    dfa = dfa_new(A, accepting_states, 0);
    cr_assert(dfa == NULL, "Expected dfa_new to return null. Got %p", dfa);

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
}

/**
 * using this automaton
 * 
 * [A] ---- a ----> [B] ---- b ----> [C]
 * 
 * [D]
 */
Test(dfa_tests, dfa_bad_accepting_states, .timeout = 5)
{
    DFA dfa;

    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'b', C);

    DSTATE accepting_states[] = { B, C, D };
    DSTATE starting_state = A;

    dfa = dfa_new(starting_state, accepting_states, 3);
    cr_assert(dfa == NULL, "Expected nfa_new to return null");

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
    dstate_free(D);
}

#ifdef DFA_STATE_LOCKING
/**
 * Using the following automaton 
 *                   a
 *                 /   \
 *                 \   /
 * [A] ---- a ----> [B] ---- b ----> [C]
 * /|\                                |
 *  |______________ c ________________|
 */
Test(dfa_tests, dfa_nstate_locks, .timeout = 5)
{
    int ret;

    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'c', A);

    DFA dfa = dfa_new(A, &C, 1);

    ret = dstate_free(C);
    cr_assert(ret != 0, "Expected dstate_free to return nonzero. Got %d", ret);

    ret = dstate_free(B);
    cr_assert(ret != 0, "Expected dstate_free to return nonzero. Got %d", ret);
    
    ret = dstate_add_transition(C, 'a', C);
    cr_assert(ret != 0, "Expected dstate_add_transition to return nonzero. Got %d", ret);

    ret = dstate_add_transition(A, 'b', C);
    cr_assert(ret != 0, "Expected dstate_add_transition to return nonzero. Got %d", ret);

    ret = dstate_remove_transition(A, 'a', B);
    cr_assert(ret != 0, "Expected dstate_remove_transition to return nonzero. Got %d", ret);

    ret = dstate_remove_transition(B, 'a', B);
    cr_assert(ret != 0, "Expected dstate_remove_transition to return nonzero. Got %d", ret);

    ret = dstate_clear_all_transitions(B);
    cr_assert(ret != 0, "Expected dstate_clear_all_transitions to return nonzero. Got %d", ret);

    ret = dstate_clear_all_transitions(C);
    cr_assert(ret != 0, "Expected dstate_clear_all_transitions to return nonzero. Got %d", ret);

    dfa_free(dfa);
}
#endif

/**
 * Using the following automaton 
 *                   a
 *                 /   \
 *                 \   /
 * [A] ---- a ----> [B] ---- b ----> [C]
 * /|\                                |
 *  |______________ c ________________|
 */
Test(dfa_tests, dfa_get_starting_state_simple, .timeout = 5)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'c', A);

    DFA dfa = dfa_new(A, &C, 1);

    DSTATE start = dfa_get_starting_state(dfa);
    cr_assert(start == A, "Expecting dfa_get_starting_state to return %p. Got %p", A, start);

    dfa_free(dfa);
}

static int dstate_arr_eq(DSTATE *arr1, size_t arr1_sz, DSTATE *arr2, size_t arr2_sz)
{
    if (arr1_sz != arr2_sz) return 0;
    for (size_t i = 0; i < arr1_sz; ++i)
    {
        int found_match = 0;
        for (size_t j = 0; j < arr2_sz; ++j)
        {
            if (arr1[i] == arr2[j])
            {
                found_match = 1;
                break;
            }
        }

        if (!found_match)
            return 0;
    }
    return 1; 
}

/**
 * [A] ---- a ----> [B]
 *     ---- b ----> [C]
 *     ---- c ----> [D]
 */
Test(dfa_tests, dfa_accepting_states_simple, .timeout = 5)
{     
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(A, 'b', C);
    dstate_add_transition(A, 'c', D);

    DSTATE accepting[] = { B, C, D };
    DFA dfa = dfa_new(A, accepting, 3);

    DSTATE *output = dfa_get_accepting_states(dfa);
    size_t output_sz = dfa_count_accepting_states(dfa);

    int ret = dstate_arr_eq(accepting, 3, output, output_sz);
    cr_assert(ret != 0, "Expected that the dstate arrays are equal.");

    dfa_free(dfa);
}

/**
 * Using the following automaton 
 *                   a
 *                 /   \
 *                 \   /
 * [A] ---- a ----> [B] ---- b ----> [C]
 * /|\                                |
 *  |______________ c ________________|
 */
Test(dfa_tests, dfa_get_states_simple, .timeout = 5)
{
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'c', A);

    DFA dfa = dfa_new(A, &C, 1);

    DSTATE expected[] = { A, B, C };
    size_t expected_sz = 3;

    DSTATE *output = dfa_get_states(dfa);
    size_t output_sz = dfa_count_states(dfa);

    int ret = dstate_arr_eq(expected, expected_sz, output, output_sz);
    cr_assert(ret != 0, "Expected that the dstate arrays are equal.");

    dfa_free(dfa);
}