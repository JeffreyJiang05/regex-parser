#include <criterion/criterion.h>

#include "nfa.h"

/**
 * Using the following automaton 
 *                   a
 *                 /   \
 *                 \   /
 * [A] ---- a ----> [B] ---- b ----> [C]
 * /|\                                |
 *  |______________ c ________________|
 */
Test(nfa_tests, nfa_lifetime, .timeout = 5)
{
    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', B);
    nstate_add_transition(B, 'b', C);
    nstate_add_transition(C, 'c', A);

    NFA nfa = nfa_new(A, &C, 1);
    cr_assert(nfa != NULL, "Expected nfa_new to return nonnull. Got null");
    nfa_free(nfa);
}

Test(nfa_tests, nfa_bad_arguments, .timeout = 5)
{
    NFA nfa;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();
    
    NSTATE accepting_states[] = { B, C };

    nfa = nfa_new(NULL, accepting_states, 2);
    cr_assert(nfa == NULL, "Expected nfa_new to return null. Got %p", nfa);

    nfa = nfa_new(A, NULL, 3);
    cr_assert(nfa == NULL, "Expected nfa_new to return null. Got %p", nfa);

    nfa = nfa_new(A, accepting_states, 0);
    cr_assert(nfa == NULL, "Expected nfa_new to return null. Got %p", nfa);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
}

/**
 * using this automaton
 *                  ___
 *                 / E \
 *                 \   /
 * [A] ---- a ----> [B] ---- b ----> [C]
 * 
 * [D]
 */
Test(nfa_tests, nfa_bad_accepting_states, .timeout = 5)
{
    NFA nfa;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();
    NSTATE D = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'b', C);

    NSTATE accepting_states[] = { B, C, D };
    NSTATE starting_state = A;

    nfa = nfa_new(starting_state, accepting_states, 3);
    cr_assert(nfa == NULL, "Expected nfa_new to return null");

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
    nstate_free(D);
}

#ifdef NFA_STATE_LOCKING
/**
 * Using the following automaton 
 *      _ [B]
 *  `E` /\
 *    /
 * [A] ---- 'E' ----> [C]
 *   \                 | b
 * a _\/              \|/
 *    [D] ---- b ----> [E]
 */
Test(nfa_tests, nfa_nstate_locks, .timeout = 5)
{
    int ret;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();
    NSTATE D = nstate_new();
    NSTATE E = nstate_new();

    nstate_add_transition(A, EPSILON, B);
    nstate_add_transition(A, EPSILON, C);
    nstate_add_transition(A, 'a', D);
    nstate_add_transition(D, 'b', E);
    nstate_add_transition(C, 'b', E);

    NSTATE accepting_states[] = { E };
    
    NFA nfa = nfa_new(A, accepting_states, 1);

    ret = nstate_free(D);
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    ret = nstate_free(E);
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    ret = nstate_add_transition(B, 'a', E);
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    ret = nstate_add_transition(D, 'c', C);
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    ret = nstate_remove_transition(C, 'b', E);
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    ret = nstate_remove_transition(A, EPSILON, C);
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    ret = nstate_clear_transition_symbol(A, EPSILON);
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    ret = nstate_clear_transition_symbol(C, 'B');
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    ret = nstate_clear_all_transitions(D);
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    ret = nstate_clear_all_transitions(C);
    cr_assert(ret != 0, "Expected nstate_free to return nonzero. Got %d", ret);

    nfa_free(nfa);
}
#endif

/**
 * Using the following automaton 
 *      _ [B]
 *  `E` /\
 *    /
 * [A] ---- 'E' ----> [C]
 *   \                 | b
 * a _\/              \|/
 *    [D] ---- b ----> [E]
 */
Test(nfa_tests, nfa_get_starting_state_simple, .timeout = 5)
{
    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();
    NSTATE D = nstate_new();
    NSTATE E = nstate_new();

    nstate_add_transition(A, EPSILON, B);
    nstate_add_transition(A, EPSILON, C);
    nstate_add_transition(A, 'a', D);
    nstate_add_transition(D, 'b', E);
    nstate_add_transition(C, 'b', E);

    NSTATE accepting_states[] = { E };
    NFA nfa = nfa_new(A, accepting_states, 1);

    NSTATE start = nfa_get_starting_state(nfa);
    cr_assert(start == A, "Expecting nfa_get_starting_state to return %p. Got %p", A, start);

    nfa_free(nfa);
}

static int nstate_arr_eq(NSTATE *arr1, size_t arr1_sz, NSTATE *arr2, size_t arr2_sz)
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
 * Using the following automaton 
 *      _ [B]
 *  `E` /\
 *    /
 * [A] ---- 'E' ----> [C]
 *   \                 | b
 * a _\/              \|/
 *    [D] ---- b ----> [E]
 */
Test(nfa_tests, nfa_accepting_states_simple, .timeout = 5)
{
    int ret;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();
    NSTATE D = nstate_new();
    NSTATE E = nstate_new();

    nstate_add_transition(A, EPSILON, B);
    nstate_add_transition(A, EPSILON, C);
    nstate_add_transition(A, 'a', D);
    nstate_add_transition(D, 'b', E);
    nstate_add_transition(C, 'b', E);

    NSTATE accepting_states[] = { C, D, E };
    NFA nfa = nfa_new(A, accepting_states, 3);

    NSTATE *accepting = nfa_get_accepting_states(nfa);
    size_t accept_sz = nfa_count_accepting_states(nfa);

    ret = nstate_arr_eq(accepting_states, 3, accepting, accept_sz);
    cr_assert(ret != 0, "Expected that the nstate arrays are equal.");

    free(accepting);
    nfa_free(nfa);
}

/**
 * Using the following automaton 
 *      _ [B]
 *  `E` /\
 *    /
 * [A] ---- 'E' ----> [C]
 *   \                 | b
 * a _\/              \|/
 *    [D] ---- b ----> [E]
 */
Test(nfa_tests, nfa_all_states_simple, .timeout = 5)
{
    int ret;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();
    NSTATE D = nstate_new();
    NSTATE E = nstate_new();

    nstate_add_transition(A, EPSILON, B);
    nstate_add_transition(A, EPSILON, C);
    nstate_add_transition(A, 'a', D);
    nstate_add_transition(D, 'b', E);
    nstate_add_transition(C, 'b', E);

    NSTATE accepting_states[] = { C, D, E };
    NFA nfa = nfa_new(A, accepting_states, 3);

    NSTATE *all = nfa_get_states(nfa);
    size_t all_sz = nfa_count_states(nfa);

    NSTATE all_states[] = { A, B, C, D, E };

    ret = nstate_arr_eq(all_states, 5, all, all_sz);
    cr_assert(ret != 0, "Expected that the nstate arrays are equal.");

    free(all);
    nfa_free(nfa);
}