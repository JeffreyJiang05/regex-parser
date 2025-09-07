#include <criterion/criterion.h>

#include <stdint.h>
#include <stdarg.h>

#include "utility/setmap.h"
#include "macro.h"

typedef union
{
    uint64_t val; 
    void *ptr;
} CVT;

#define PTR(value) ((CVT){(value)}.ptr)
#define INT(ptrval) ((CVT){.ptr=(ptrval)}.val)

SET make_test_set(size_t count, ...)
{
    SET set = set_init();
    va_list va;
    va_start(va, count);
    for (size_t i = 0; i < count; ++i)
    {
        int value = va_arg(va, int);
        set_add(set, PTR(value));
    }
    va_end(va);
    return set;
}
#define MAKE_TEST_SET(...) make_test_set(NARGS(__VA_ARGS__), __VA_ARGS__)

Test(setmap_tests, setmap_lifetime, .timeout=5)
{
    SET_MAP map = setmap_init();
    cr_assert(map != NULL, "Failed to allocate for the set_map");
    size_t sz = setmap_size(map);
    cr_assert(sz == 0, "New set map has size %lu not 0", sz);
    setmap_fini(map);
}

Test(setmap_tests, setmap_set_simple, .timeout = 5)
{
    int ret;
    SET_MAP map = setmap_init();

    SET A = MAKE_TEST_SET(1, 2, 3);
    ret = setmap_set(map, A, PTR(0));
    cr_assert(ret == 0, "Expected setmap_set to return zero. Got %d", ret);

    SET B = MAKE_TEST_SET(1, 2);
    ret = setmap_set(map, B, PTR(1));
    cr_assert(ret == 0, "Expected setmap_set to return zero. Got %d", ret);

    SET C = MAKE_TEST_SET(4, 5, 7);
    ret = setmap_set(map, C, PTR(2));
    cr_assert(ret == 0, "Expected setmap_set to return zero. Got %d", ret);

    ret = setmap_set(map, A, PTR(3));
    cr_assert(ret == 0, "Expected setmap_set to return zero. Got %d", ret);
    
    setmap_fini(map);
    set_fini(A);
    set_fini(B);
    set_fini(C);
}

Test(setmap_tests, setmap_set_and_get_simple, .timeout = 5)
{
    int ret;
    int expected;
    int output;
    SET_MAP map = setmap_init();

    SET A = MAKE_TEST_SET(1, 2, 3);
    ret = setmap_set(map, A, PTR(0));
    cr_assert(ret == 0, "Expected setmap_set to return zero. Got %d", ret);

    output = INT(setmap_get(map, A));
    expected = 0;
    cr_assert(output == expected, "Expected setmap_get to return %d. Got %d", expected, output);

    SET B = MAKE_TEST_SET(1, 2);
    ret = setmap_set(map, B, PTR(1));
    cr_assert(ret == 0, "Expected setmap_set to return zero. Got %d", ret);

    output = INT(setmap_get(map, A));
    expected = 0;
    cr_assert(output == expected, "Expected setmap_get to return %d. Got %d", expected, output);

    output = INT(setmap_get(map, B));
    expected = 1;
    cr_assert(output == expected, "Expected setmap_get to return %d. Got %d", expected, output);

    SET C = MAKE_TEST_SET(4, 5, 7);
    ret = setmap_set(map, C, PTR(2));
    cr_assert(ret == 0, "Expected setmap_set to return zero. Got %d", ret);

    output = INT(setmap_get(map, C));
    expected = 2;
    cr_assert(output == expected, "Expected setmap_get to return %d. Got %d", expected, output);

    SET D = MAKE_TEST_SET(1, 2, 3);
    ret = setmap_set(map, D, PTR(3));
    cr_assert(ret == 0, "Expected setmap_set to return zero. Got %d", ret);

    output = INT(setmap_get(map, A));
    expected = 3;
    cr_assert(output == expected, "Expected setmap_get to return %d. Got %d", expected, output);

    SET E = MAKE_TEST_SET(8);
    ret = setmap_set(map, E, PTR(4));
    cr_assert(ret == 0, "Expected setmap_set to return zero. Got %d", ret);

    output = INT(setmap_get(map, E));
    expected = 4;
    cr_assert(output == expected, "Expected setmap_get to return %d. Got %d", expected, output);
    
    setmap_fini(map);
    set_fini(A);
    set_fini(B);
    set_fini(C);
    set_fini(D);
    set_fini(E);
}

Test(setmap_tests, setmap_contains_simple, .timeout = 5)
{
    int ret;
    SET_MAP map = setmap_init();

    SET A = MAKE_TEST_SET(1, 2, 3);
    setmap_set(map, A, PTR(0));

    SET B = MAKE_TEST_SET(1, 2);
    setmap_set(map, B, PTR(1));

    SET C = MAKE_TEST_SET(4, 5, 7);
    setmap_set(map, C, PTR(2));

    SET D = MAKE_TEST_SET(0);
    setmap_set(map, D, PTR(3));

    SET E = MAKE_TEST_SET(1);
    setmap_set(map, E, PTR(4));

    SET F = MAKE_TEST_SET(1, 2, 3);
    SET G = MAKE_TEST_SET(90);

    ret = setmap_contains(map, A);
    cr_assert(ret != 0, "Expected setmap_contains to return nonzero. Got %d", ret);

    ret = setmap_contains(map, B);
    cr_assert(ret != 0, "Expected setmap_contains to return nonzero. Got %d", ret);

    ret = setmap_contains(map, C);
    cr_assert(ret != 0, "Expected setmap_contains to return nonzero. Got %d", ret);

    ret = setmap_contains(map, D);
    cr_assert(ret != 0, "Expected setmap_contains to return nonzero. Got %d", ret);

    ret = setmap_contains(map, E);
    cr_assert(ret != 0, "Expected setmap_contains to return nonzero. Got %d", ret);

    ret = setmap_contains(map, F);
    cr_assert(ret != 0, "Expected setmap_contains to return nonzero. Got %d", ret);

    ret = setmap_contains(map, G);
    cr_assert(ret == 0, "Expected setmap_contains to return zero. Got %d", ret);

    setmap_fini(map);
    set_fini(A);
    set_fini(B);
    set_fini(C);
    set_fini(D);
    set_fini(E);
    set_fini(F);
    set_fini(G);
}