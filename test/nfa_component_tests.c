#include <criterion/criterion.h>

#include "nfa.h"

/**
 * Using the following regex:
 *  z{3}(ab|cd){1,}
 */
Test(nfa_component_tests, component_lifetime, .timeout = 5)
{
    NFA_COMPONENT combined = nfa_concat(
        nfa_repeat_exact(nfa_symbol('z'), 3),
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
        1)
    );

    NFA nfa = nfa_construct(combined);
    cr_assert(nfa != NULL, "Expected nfa_construct to return nonnull. Got %p", nfa);
    nfa_free(nfa);
}

/**
 * Using the following regex:
 *  z{3}(ab|cd){1,}
 */
Test(nfa_component_tests, component_simple_0, .timeout = 5)
{
    NFA_COMPONENT combined = nfa_concat(
        nfa_repeat_exact(nfa_symbol('z'), 3),
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
        1)
    );

    NFA nfa = nfa_construct(combined);

    char *input = NULL;
    int ret;

    input = "zzzab";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "zzzcd";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "zzzababcd";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "zzzabcdabababcdcdcdabababababcdabcdab";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "zzabab";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret == 0, "Expected nfa_accept for \"%s\" to return zero. Got %d", input, ret);

    input = "zzzzabab";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret == 0, "Expected nfa_accept for \"%s\" to return zero. Got %d", input, ret);

    input = "";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret == 0, "Expected nfa_accept for \"%s\" to return zero. Got %d", input, ret);

    input = "zzzababzz";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret == 0, "Expected nfa_accept for \"%s\" to return zero. Got %d", input, ret);

    nfa_free(nfa);
}

/**
 * Using the following regex:
 *  (abcd){,3}
 */
Test(nfa_component_tests, component_simple_1, .timeout = 5)
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

    char *input = NULL;
    int ret;

    input = "";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "abcd";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "abcdabcd";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "abcdabcdabcd";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "abcdabcdabcdabcd";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret == 0, "Expected nfa_accept for \"%s\" to return zero. Got %d", input, ret);

    nfa_free(nfa);
}

/**
 * Using the following regex:
 *  (ab)*zz
 */
Test(nfa_component_tests, component_simple_2, .timeout = 5)
{
    NFA_COMPONENT component = nfa_concat(
        nfa_repeat(
            nfa_concat(
                nfa_symbol('a'),
                nfa_symbol('b')
            )
        ),
        nfa_concat(
            nfa_symbol('z'),
            nfa_symbol('z')
        )
    );

    NFA nfa = nfa_construct(component);

    char *input = NULL;
    int ret;

    input = "zz";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "abzz";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "ababzz";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "abababzz";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret != 0, "Expected nfa_accept for \"%s\" to return nonzero. Got %d", input, ret);

    input = "";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret == 0, "Expected nfa_accept for \"%s\" to return zero. Got %d", input, ret);

    input = "cd";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret == 0, "Expected nfa_accept for \"%s\" to return zero. Got %d", input, ret);

    input = "abababab";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret == 0, "Expected nfa_accept for \"%s\" to return zero. Got %d", input, ret);

    input = "abababz";
    ret = nfa_accept_cstr(nfa, input);
    cr_assert(ret == 0, "Expected nfa_accept for \"%s\" to return zero. Got %d", input, ret);

    nfa_free(nfa);
}