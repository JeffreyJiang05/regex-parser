#include <criterion/criterion.h>

#include "automata/algorithm.h"

// (a|b)*abb
Test(subset_construction_tests, subset_construct_0, .timeout = 5)
{
    int nfa_result;
    int dfa_result;
    char *input;

    NFA nfa = nfa_construct(
        nfa_concat(
            nfa_repeat(
                nfa_union(
                    nfa_symbol('a'),
                    nfa_symbol('b')
                )
            ),
            nfa_concat(
                nfa_symbol('a'),
                nfa_concat(
                    nfa_symbol('b'),
                    nfa_symbol('b')
                )
            )
        )
    );

    DFA dfa = subset_construction(nfa);

    input = "";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "abb";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "abbaabb";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "aabaa";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "c";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    nfa_free(nfa);
    dfa_free(dfa);
}

// (ab|cd){2,}dcb
Test(subset_construction_tests, subset_construct_1, .timeout = 5)
{
    int nfa_result;
    int dfa_result;
    char *input;

    NFA nfa = nfa_construct(
        nfa_concat(
            nfa_repeat_min(
                nfa_union(
                    nfa_concat(
                        nfa_symbol('a'),
                        nfa_symbol('b')
                    ),
                    nfa_concat(
                        nfa_symbol('c'),
                        nfa_symbol('d')
                    )
                ),
            2),
            nfa_concat(
                nfa_symbol('d'),
                nfa_concat(
                    nfa_symbol('c'),
                    nfa_symbol('b')
                )
            )
        )
    );

    DFA dfa = subset_construction(nfa);

    input = "";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "ababcdabdcb";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "abdcb";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "cddcb";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "abcddcb";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    nfa_free(nfa);
    dfa_free(dfa);
}

// (hi)? J(ill|ohn)
Test(subset_construction_tests, subset_construct_2, .timeout = 5)
{
    int nfa_result;
    int dfa_result;
    char *input;

    NFA nfa = nfa_construct(
        nfa_concat(
            nfa_repeat_min_max(
                nfa_concat(
                    nfa_symbol('h'),
                    nfa_symbol('i')
                ),
            0, 1),
            nfa_concat(
                nfa_symbol('J'),
                nfa_concat(
                    nfa_symbol(' '),
                    nfa_union(
                        nfa_concat(
                            nfa_symbol('i'),
                            nfa_concat(
                                nfa_symbol('l'),
                                nfa_symbol('l')
                            )
                        ),
                        nfa_concat(
                            nfa_symbol('o'),
                            nfa_concat(
                                nfa_symbol('h'),
                                nfa_symbol('n')
                            )
                        )
                    )
                )
            )
        )
    );

    DFA dfa = subset_construction(nfa);

    input = "";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "hi John";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "hi Jill";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "John";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "Jill";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "hello John";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    input = "hi john";
    nfa_result = nfa_accept_cstr(nfa, input);
    dfa_result = dfa_accept_cstr(dfa, input);
    cr_assert(nfa_result == dfa_result, "Expected NFA and DFA simulation for \"%s\" to yield the same result. NFA = %d, DFA = %d", 
        input, nfa_result, dfa_result);

    nfa_free(nfa);
    dfa_free(dfa);
}