#include <criterion/criterion.h>

#include "dfa.h"

Test(dstate_tests, dstate_lifetime, .timeout = 5)
{
    DSTATE state = dstate_new();
    cr_assert(state != NULL, "Expected dstate_new to return nonnull. Got %p", state);
    int ret = dstate_free(state);
    cr_assert(ret == 0, "Expected dstate_free to return 0. Got %d", ret);
}

/**
 * Building the following automaton 
 *                   a
 *                 /   \
 *                 \   /
 * [A] ---- a ----> [B] ---- b ----> [C]
 * /|\                                |
 *  |______________ c ________________|
 */
Test(dstate_tests, dstate_add_transition_simple_0, .timeout = 5)
{
    int ret;

    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    ret = dstate_add_transition(A, 'a', B);
    cr_assert(ret == 0, "Expected dstate_add_transition to return zero (success). Got %d instead", ret);

    ret = dstate_add_transition(B, 'a', B);
    cr_assert(ret == 0, "Expected dstate_add_transition to return zero (success). Got %d instead", ret);

    ret = dstate_add_transition(B, 'b', C);
    cr_assert(ret == 0, "Expected dstate_add_transition to return zero (success). Got %d instead", ret);

    ret = dstate_add_transition(C, 'c', A);
    cr_assert(ret == 0, "Expected dstate_add_transition to return zero (success). Got %d instead", ret);

    // duplicate
    ret = dstate_add_transition(A, 'a', B);
    cr_assert(ret != 0, "Expected dstate_add_transition to return nonzero (failure). Got %d instead", ret);

    ret = dstate_has_transition(A, 'a', B);
    cr_assert(ret != 0, "Expected dstate_has_transition to return nonzero (true). Got %d instead", ret);

    ret = dstate_has_transition(B, 'a', B);
    cr_assert(ret != 0, "Expected dstate_has_transition to return nonzero (true). Got %d instead", ret);

    ret = dstate_has_transition(B, 'b', C);
    cr_assert(ret != 0, "Expected dstate_has_transition to return nonzero (true). Got %d instead", ret);

    ret = dstate_has_transition(C, 'c', A);
    cr_assert(ret != 0, "Expected dstate_has_transition to return nonzero (true). Got %d instead", ret);

    ret = dstate_has_transition(A, 'c', B);
    cr_assert(ret == 0, "Expected dstate_has_transition to return zero (false). Got %d instead", ret);

    ret = dstate_has_transition(B, EPSILON, C);
    cr_assert(ret == 0, "Expected dstate_has_transition to return zero (false). Got %d instead", ret);

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
}

/**
 * Building this automaton
 * 
 * [A] ---- a ----> [B]
 * 
 */
Test(dstate_tests, dstate_add_transition_simple_1, .timeout = 5)
{
    int ret;
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    ret = dstate_add_transition(A, 'a', B);
    cr_assert(ret == 0, "Expected dstate_add_transition to return zero (success). Got %d instead", ret);

    ret = dstate_add_transition(A, EPSILON, C);
    cr_assert(ret != 0, "Expected dstate_add_transition to return nonzero (failure). Got %d instead", ret);

    ret = dstate_add_transition(A, 'a', C);
    cr_assert(ret != 0, "Expected dstate_add_transition to return nonzero (failure). Got %d instead", ret);

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
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
Test(dstate_tests, dstate_remove_transition_simple_0, .timeout = 5)
{
    int ret;

    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'c', A);

    ret = dstate_remove_transition(B, 'a', B);
    cr_assert(ret == 0, "Expected dstate_remove_transition to return zero (success). Got %d", ret);

    ret = dstate_has_transition(B, 'a', B);
    cr_assert(ret == 0, "Expected dstate_has_transition to return zero (false). Got %d", ret);

    ret = dstate_remove_transition(B, 'a', B);
    cr_assert(ret != 0, "Expected dstate_remove_transition to return nonzero (failure). Got %d", ret);

    ret = dstate_remove_transition(A, 'a', C);
    cr_assert(ret != 0, "Expected dstate_remove_transition to return nonzero (failure). Got %d", ret);

    ret = dstate_remove_transition(B, 'b', C);
    cr_assert(ret == 0, "Expected dstate_remove_transition to return zero (success). Got %d", ret);

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
}

/**
 * Building this automaton
 * 
 * [A] ---- a ----> [B]
 * 
 */
Test(dstate_tests, dstate_remove_transition_simple_1, .timeout = 5)
{
    int ret;
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    dstate_add_transition(A, 'a', B);

    ret = dstate_remove_transition(C, EPSILON, C);
    cr_assert(ret != 0, "Expected dstate_remove_transition to return nonzero (failure). Got %d", ret);

    ret = dstate_remove_transition(B, 'a', A);
    cr_assert(ret != 0, "Expected dstate_remove_transition to return nonzero (failure). Got %d", ret);

    ret = dstate_remove_transition(A, 'a', B);
    cr_assert(ret == 0, "Expected dstate_remove_transition to return zero (success). Got %d", ret);

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
}

/**
 * [A] ---- a ----> [B]
 *     ---- b ----> [C]
 *     ---- c ----> [D]
 */
Test(dstate_tests, dstate_clear_all_transitions_simple_0, .timeout = 5)
{
    int ret;
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(A, 'b', C);
    dstate_add_transition(A, 'c', D);

    ret = dstate_clear_all_transitions(A);
    cr_assert(ret == 0, "Expected dstate_clear_all_transitions to return zero (success). Got %d", ret);

    ret = dstate_has_transition(A, 'a', B);
    cr_assert(ret == 0, "Expected dstate_has_transition to return zero (false). Got %d", ret);

    ret = dstate_has_transition(A, 'b', C);
    cr_assert(ret == 0, "Expected dstate_has_transition to return zero (false). Got %d", ret);

    ret = dstate_has_transition(A, 'c', D);
    cr_assert(ret == 0, "Expected dstate_has_transition to return zero (false). Got %d", ret);

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
    dstate_free(D);
}

static int sym_arr_eq(SYMBOL *arr1, size_t arr1_sz, SYMBOL *arr2, size_t arr2_sz)
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
 *                   a
 *                 /   \
 *                 \   /
 * [A] ---- a ----> [B] ---- b ----> [C]
 * /|\                                |
 *  |______________ c ________________|
 */
Test(dstate_tests, dstate_transition_symbols_simple_0, .timeout = 5)
{
    SYMBOL *syms;
    size_t sz;

    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'c', A);

    {
        SYMBOL expected[] = { 'a', 'b' };
        size_t expected_sz = 2; 

        syms = dstate_get_transition_symbols(B);
        sz = dstate_count_transition_symbols(B);

        cr_assert(sym_arr_eq(expected, expected_sz, syms, sz), "Transition symbols do not match!");
        free(syms);
    }

    {
        SYMBOL expected[] = { 'a' };
        size_t expected_sz = 1; 

        syms = dstate_get_transition_symbols(A);
        sz = dstate_count_transition_symbols(A);

        cr_assert(sym_arr_eq(expected, expected_sz, syms, sz), "Transition symbols do not match!");
        free(syms);
    }

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
}

/**
 * [A] ---- a ----> [B]
 *     ---- b ----> [C]
 *     ---- c ----> [D]
 */
Test(dstate_tests, dstate_transition_symbols_simple_1, .timeout = 5)
{
    SYMBOL *syms;
    size_t sz;    
    
    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(A, 'b', C);
    dstate_add_transition(A, 'c', D);

    {
        SYMBOL expected[] = { 'a', 'b', 'c' };
        size_t expected_sz = 3; 

        syms = dstate_get_transition_symbols(A);
        sz = dstate_count_transition_symbols(A);

        cr_assert(sym_arr_eq(expected, expected_sz, syms, sz), "Transition symbols do not match!");
        free(syms);
    }

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
    dstate_free(D);
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
Test(dstate_tests, dstate_transition_states_simple_0, .timeout = 5)
{
    DSTATE state;

    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(B, 'a', B);
    dstate_add_transition(B, 'b', C);
    dstate_add_transition(C, 'c', A);

    {
        DSTATE expected = B;
        state = dstate_get_transition_state(B, 'a');
        cr_assert(state == expected, "Expected `dstate_get_transition_state` to return %p. Got %p", expected, state);
    }

    {
        DSTATE expected = B;
        state = dstate_get_transition_state(A, 'a');
        cr_assert(state == expected, "Expected `dstate_get_transition_state` to return %p. Got %p", expected, state);
    }

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
}

/**
 * [A] ---- a ----> [B]
 *     ---- b ----> [C]
 *     ---- c ----> [D]
 */
Test(dstate_tests, dstate_transition_states_simple_1, .timeout = 5)
{
    DSTATE state;

    DSTATE A = dstate_new();
    DSTATE B = dstate_new();
    DSTATE C = dstate_new();
    DSTATE D = dstate_new();

    dstate_add_transition(A, 'a', B);
    dstate_add_transition(A, 'b', C);
    dstate_add_transition(A, 'c', D);

    {
        DSTATE expected = B;
        state = dstate_get_transition_state(A, 'a');
        cr_assert(state == expected, "Expected `dstate_get_transition_state` to return %p. Got %p", expected, state);
    }

    {
        DSTATE expected = C;
        state = dstate_get_transition_state(A, 'b');
        cr_assert(state == expected, "Expected `dstate_get_transition_state` to return %p. Got %p", expected, state);
    }

    {
        DSTATE expected = D;
        state = dstate_get_transition_state(A, 'c');
        cr_assert(state == expected, "Expected `dstate_get_transition_state` to return %p. Got %p", expected, state);
    }

    {
        DSTATE expected = NULL;
        state = dstate_get_transition_state(A, 'd');
        cr_assert(state == expected, "Expected `dstate_get_transition_state` to return %p. Got %p", expected, state);
    }

    {
        DSTATE expected = NULL;
        state = dstate_get_transition_state(B, 'a');
        cr_assert(state == expected, "Expected `dstate_get_transition_state` to return %p. Got %p", expected, state);
    }

    dstate_free(A);
    dstate_free(B);
    dstate_free(C);
    dstate_free(D);
}

