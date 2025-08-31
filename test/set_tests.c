#include <stdint.h>

#include <criterion/criterion.h>

#include "set.h"

typedef union
{
    uint64_t val; 
    void *ptr;
} CVT;

#define PTR(value) ((CVT){(value)}.ptr)

Test(hash_set_tests, lifecycle, .timeout = 5)
{
    SET set = NULL;
    
    set = set_init();
    cr_assert(set != NULL, "Failed to allocate for the set");
    size_t sz = set_size(set);
    cr_assert(sz == 0, "New set has size %lu not 0");
    set_fini(set);
}

Test(hash_set_tests, set_add_returns, .timeout = 5)
{
    SET set = set_init();
    int ret;

    ret = set_add(set, PTR(1));
    cr_assert( ret == 0, "Expected set_add to return zero. Got %d", ret );

    ret = set_contains(set, PTR(1));
    cr_assert( ret == 1, "Expected set_contains to return true. Got %d", ret );

    ret = set_add(set, PTR(1));
    cr_assert( ret != 0, "Expected set_add to return nonzero. Got %d", ret );
}