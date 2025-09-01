#include <criterion/criterion.h>

#include "nfa.h"

Test(nstate_tests, nstate_lifetime, .timeout = 5)
{
    NSTATE state = nstate_new();
    cr_assert(state != NULL, "Expected nstate_new to return nonnull. Got null");
    int ret = nstate_free(state);
    cr_assert(ret == 0, "Expected nstate_free to return 0. Got %d.", ret);   
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
Test(nstate_tests, nstate_add_transition_simple_0, .timeout = 5)
{
    int ret;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();

    ret = nstate_add_transition(A, 'a', B);
    cr_assert(ret == 0, "Expected nstate_add_transition to return zero (success). Got %d instead", ret);

    ret = nstate_add_transition(B, 'a', B);
    cr_assert(ret == 0, "Expected nstate_add_transition to return zero (success). Got %d instead", ret);

    ret = nstate_add_transition(B, 'b', C);
    cr_assert(ret == 0, "Expected nstate_add_transition to return zero (success). Got %d instead", ret);

    ret = nstate_add_transition(C, 'c', A);
    cr_assert(ret == 0, "Expected nstate_add_transition to return zero (success). Got %d instead", ret);

    // duplicate
    ret = nstate_add_transition(A, 'a', B);
    cr_assert(ret != 0, "Expected nstate_add_transition to return nonzero (failure). Got %d instead", ret);

    ret = nstate_has_transition(A, 'a', B);
    cr_assert(ret != 0, "Expected nstate_has_transition to return nonzero (true). Got %d instead", ret);

    ret = nstate_has_transition(B, 'a', B);
    cr_assert(ret != 0, "Expected nstate_has_transition to return nonzero (true). Got %d instead", ret);

    ret = nstate_has_transition(B, 'b', C);
    cr_assert(ret != 0, "Expected nstate_has_transition to return nonzero (true). Got %d instead", ret);

    ret = nstate_has_transition(C, 'c', A);
    cr_assert(ret != 0, "Expected nstate_has_transition to return nonzero (true). Got %d instead", ret);

    ret = nstate_has_transition(A, 'c', B);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d instead", ret);

    ret = nstate_has_transition(B, EPSILON, C);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d instead", ret);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
}

/**
 * Building the following automaton 
 *      _ [B]
 *  `E` /\
 *    /
 * [A] ---- 'E' ----> [C]
 *   \                 | b
 * a _\/              \|/
 *    [D] ---- b ----> [E]
 */
Test(nstate_tests, nstate_add_transition_simple_1, .timeout = 5)
{
    int ret;
    
    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();
    NSTATE D = nstate_new();
    NSTATE E = nstate_new();

    ret = nstate_add_transition(A, EPSILON, B);
    cr_assert(ret == 0, "Expected nstate_add_transition to return zero (success). Got %d instead", ret);

    ret = nstate_add_transition(A, EPSILON, C);
    cr_assert(ret == 0, "Expected nstate_add_transition to return zero (success). Got %d instead", ret);

    ret = nstate_add_transition(A, 'a', D);
    cr_assert(ret == 0, "Expected nstate_add_transition to return zero (success). Got %d instead", ret);

    ret = nstate_add_transition(D, 'b', E);
    cr_assert(ret == 0, "Expected nstate_add_transition to return zero (success). Got %d instead", ret);

    ret = nstate_add_transition(C, 'b', E);
    cr_assert(ret == 0, "Expected nstate_add_transition to return zero (success). Got %d instead", ret);

    ret = nstate_add_transition(A, EPSILON, C);
    cr_assert(ret != 0, "Expected nstate_add_transition to return nonzero (failure). Got %d instead", ret);

    ret = nstate_add_transition(A, 'a', D);
    cr_assert(ret != 0, "Expected nstate_add_transition to return nonzero (failure). Got %d instead", ret);

    ret = nstate_has_transition(A, EPSILON, B);
    cr_assert(ret != 0, "Expected nstate_has_transition to return nonzero (true). Got %d instead", ret);    

    ret = nstate_has_transition(A, EPSILON, C);
    cr_assert(ret != 0, "Expected nstate_has_transition to return nonzero (true). Got %d instead", ret); 

    ret = nstate_has_transition(A, 'a', D);
    cr_assert(ret != 0, "Expected nstate_has_transition to return nonzero (true). Got %d instead", ret); 

    ret = nstate_has_transition(D, 'b', E);
    cr_assert(ret != 0, "Expected nstate_has_transition to return nonzero (true). Got %d instead", ret); 

    ret = nstate_has_transition(C, 'b', E);
    cr_assert(ret != 0, "Expected nstate_has_transition to return nonzero (true). Got %d instead", ret);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
    nstate_free(D);
    nstate_free(E);
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
Test(nstate_tests, nstate_remove_transition_simple_0, .timeout = 5)
{
    int ret;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', B);
    nstate_add_transition(B, 'b', C);
    nstate_add_transition(C, 'c', A);

    ret = nstate_remove_transition(B, 'a', B);
    cr_assert(ret == 0, "Expected nstate_remove_transition to return zero (success). Got %d", ret);

    ret = nstate_has_transition(B, 'a', B);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_remove_transition(B, 'a', B);
    cr_assert(ret != 0, "Expected nstate_remove_transition to return nonzero (failure). Got %d", ret);

    ret = nstate_remove_transition(A, 'a', C);
    cr_assert(ret != 0, "Expected nstate_remove_transition to return nonzero (failure). Got %d", ret);

    ret = nstate_remove_transition(B, 'b', C);
    cr_assert(ret == 0, "Expected nstate_remove_transition to return zero (success). Got %d", ret);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
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
Test(nstate_tests, nstate_remove_transition_simple_1, .timeout = 5)
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

    ret = nstate_remove_transition(A, EPSILON, B);
    cr_assert(ret == 0, "Expected nstate_remove_transition to return zero (success). Got %d", ret);

    ret = nstate_remove_transition(A, EPSILON, C);
    cr_assert(ret == 0, "Expected nstate_remove_transition to return zero (success). Got %d", ret);

    ret = nstate_remove_transition(A, 'a', D);
    cr_assert(ret == 0, "Expected nstate_remove_transition to return zero (success). Got %d", ret);

    ret = nstate_remove_transition(D, 'b', E);
    cr_assert(ret == 0, "Expected nstate_remove_transition to return zero (success). Got %d", ret);

    ret = nstate_remove_transition(C, 'b', E);
    cr_assert(ret == 0, "Expected nstate_remove_transition to return zero (success). Got %d", ret);

    ret = nstate_remove_transition(B, EPSILON, E);
    cr_assert(ret != 0, "Expected nstate_remove_transition to return nonzero (failure). Got %d", ret);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
    nstate_free(D);
    nstate_free(E);
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
Test(nstate_tests, nstate_clear_transition_symbol_simple_0, .timeout = 5)
{
    int ret;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', B);
    nstate_add_transition(B, 'b', C);
    nstate_add_transition(C, 'c', A);

    ret = nstate_clear_transition_symbol(B, 'a');
    cr_assert(ret == 0, "Expected nstate_clear_transition_symbol to return zero (success). Got %d", ret);

    ret = nstate_has_transition(B, 'a', B);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_clear_transition_symbol(A, 'a');
    cr_assert(ret == 0, "Expected nstate_clear_transition_symbol to return nonzero (success). Got %d", ret);

    ret = nstate_has_transition(A, 'a', B);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
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
Test(nstate_tests, nstate_clear_transition_symbol_simple_1, .timeout = 5)
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

    ret = nstate_clear_transition_symbol(A, EPSILON);
    cr_assert(ret == 0, "Expected nstate_clear_transition_symbol to return zero (success). Got %d", ret);

    ret = nstate_has_transition(A, EPSILON, B);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_has_transition(A, EPSILON, E);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_clear_transition_symbol(D, 'b');
    cr_assert(ret == 0, "Expected nstate_clear_transition_symbol to return zero (success). Got %d", ret);

    ret = nstate_has_transition(D, 'b', E);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_clear_transition_symbol(C, 'b');
    cr_assert(ret == 0, "Expected nstate_clear_transition_symbol to return zero (success). Got %d", ret);

    ret = nstate_has_transition(C, 'b', E);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
    nstate_free(D);
    nstate_free(E);
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
Test(nstate_tests, nstate_clear_all_transitions_simple_0, .timeout = 5)
{
    int ret;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', B);
    nstate_add_transition(B, 'b', C);
    nstate_add_transition(C, 'c', A);

    ret = nstate_clear_all_transitions(B);
    cr_assert(ret == 0, "Expected nstate_clear_all_transitions to return zero (success). Got %d", ret);

    ret = nstate_has_transition(B, 'a', B);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_has_transition(B, 'b', C);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_clear_all_transitions(A);
    cr_assert(ret == 0, "Expected nstate_clear_all_transitions to return nonzero (success). Got %d", ret);

    ret = nstate_has_transition(A, 'a', B);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
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
Test(nstate_tests, nstate_clear_all_transitions_simple_1, .timeout = 5)
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

    ret = nstate_clear_all_transitions(A);
    cr_assert(ret == 0, "Expected nstate_clear_all_transitions to return zero (success). Got %d", ret);

    ret = nstate_has_transition(A, EPSILON, B);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_has_transition(A, EPSILON, E);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_has_transition(A, 'a', D);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_clear_all_transitions(D);
    cr_assert(ret == 0, "Expected nstate_clear_all_transitions to return zero (success). Got %d", ret);

    ret = nstate_has_transition(D, 'b', E);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    ret = nstate_clear_all_transitions(C);
    cr_assert(ret == 0, "Expected nstate_clear_all_transitions to return zero (success). Got %d", ret);

    ret = nstate_has_transition(C, 'b', E);
    cr_assert(ret == 0, "Expected nstate_has_transition to return zero (false). Got %d", ret);

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
    nstate_free(D);
    nstate_free(E);
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
Test(nstate_tests, nstate_transition_symbols_simple_0, .timeout = 5)
{
    SYMBOL *syms;
    size_t sz;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', B);
    nstate_add_transition(B, 'b', C);
    nstate_add_transition(C, 'c', A);

    {
        SYMBOL expected[] = { 'a', 'b' };
        size_t expected_sz = 2; 

        syms = nstate_get_transition_symbols(B);
        sz = nstate_count_transition_symbols(B);

        cr_assert(sym_arr_eq(expected, expected_sz, syms, sz), "Transition symbols do not match!");
        free(syms);
    }

    {
        SYMBOL expected[] = { 'a' };
        size_t expected_sz = 1; 

        syms = nstate_get_transition_symbols(A);
        sz = nstate_count_transition_symbols(A);

        cr_assert(sym_arr_eq(expected, expected_sz, syms, sz), "Transition symbols do not match!");
        free(syms);
    }

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
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
Test(nstate_tests, nstate_transition_symbols_simple_1, .timeout = 5)
{
    SYMBOL *syms;
    size_t sz;

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

    {
        SYMBOL expected[] = { EPSILON, 'a' };
        size_t expected_sz = 2; 

        syms = nstate_get_transition_symbols(A);
        sz = nstate_count_transition_symbols(A);

        cr_assert(sym_arr_eq(expected, expected_sz, syms, sz), "Transition symbols do not match!");
        free(syms);
    }

    {
        SYMBOL expected[] = { 'b' };
        size_t expected_sz = 1; 

        syms = nstate_get_transition_symbols(D);
        sz = nstate_count_transition_symbols(D);

        cr_assert(sym_arr_eq(expected, expected_sz, syms, sz), "Transition symbols do not match!");
        free(syms);
    }

    {
        SYMBOL expected[] = { 'b' };
        size_t expected_sz = 1; 

        syms = nstate_get_transition_symbols(C);
        sz = nstate_count_transition_symbols(C);

        cr_assert(sym_arr_eq(expected, expected_sz, syms, sz), "Transition symbols do not match!");
        free(syms);
    }

    {
        SYMBOL expected[] = { };
        size_t expected_sz = 0; 

        syms = nstate_get_transition_symbols(E);
        sz = nstate_count_transition_symbols(E);

        cr_assert(sym_arr_eq(expected, expected_sz, syms, sz), "Transition symbols do not match!");
        free(syms);
    }

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
    nstate_free(D);
    nstate_free(E);
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
 *                   a
 *                 /   \
 *                 \   /
 * [A] ---- a ----> [B] ---- b ----> [C]
 * /|\                                |
 *  |______________ c ________________|
 */
Test(nstate_tests, nstate_transition_states_simple_0, .timeout = 5)
{
    NSTATE *states;
    size_t sz;

    NSTATE A = nstate_new();
    NSTATE B = nstate_new();
    NSTATE C = nstate_new();

    nstate_add_transition(A, 'a', B);
    nstate_add_transition(B, 'a', B);
    nstate_add_transition(B, 'b', C);
    nstate_add_transition(C, 'c', A);

    {
        NSTATE expected[] = { B };
        size_t expected_sz = 1; 

        states = nstate_get_transition_states(B, 'a');
        sz = nstate_count_transition_states(B, 'a');

        cr_assert(nstate_arr_eq(expected, expected_sz, states, sz), "Transition states do not match!");
    }

    {
        NSTATE expected[] = { B };
        size_t expected_sz = 1; 

        states = nstate_get_transition_states(A, 'a');
        sz = nstate_count_transition_states(A, 'a');

        cr_assert(nstate_arr_eq(expected, expected_sz, states, sz), "Transition states do not match!");
    }

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
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
Test(nstate_tests, nstate_transition_states_simple_1, .timeout = 5)
{
    NSTATE *states;
    size_t sz;

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

    {
        NSTATE expected[] = { B, C };
        size_t expected_sz = 2; 

        states = nstate_get_transition_states(A, EPSILON);
        sz = nstate_count_transition_states(A, EPSILON);

        cr_assert(nstate_arr_eq(expected, expected_sz, states, sz), "Transition states do not match!");
    }

    {
        NSTATE expected[] = { E };
        size_t expected_sz = 1; 

        states = nstate_get_transition_states(C, 'b');
        sz = nstate_count_transition_states(C, 'b');

        cr_assert(nstate_arr_eq(expected, expected_sz, states, sz), "Transition states do not match!");
    }

    nstate_free(A);
    nstate_free(B);
    nstate_free(C);
    nstate_free(D);
    nstate_free(E);
}