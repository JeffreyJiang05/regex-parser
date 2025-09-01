#include <stdint.h>
#include <time.h>

#include <criterion/criterion.h>

#include "set.h"

typedef union
{
    uint64_t val; 
    void *ptr;
} CVT;

#define PTR(value) ((CVT){(value)}.ptr)
#define INT(ptrval) ((CVT){.ptr=(ptrval)}.val)

Test(set_tests, set_lifecycle, .timeout = 5)
{
    SET set = NULL;
    
    set = set_init();
    cr_assert(set != NULL, "Failed to allocate for the set");
    size_t sz = set_size(set);
    cr_assert(sz == 0, "New set has size %lu not 0");
    set_fini(set);
}

Test(set_tests, set_add_simple, .timeout = 5)
{
    SET set = set_init();
    int ret;

    ret = set_add(set, PTR(1));
    cr_assert( ret == 0, "Expected set_add to return zero. Got %d", ret );

    ret = set_add(set, PTR(1));
    cr_assert( ret != 0, "Expected set_add to return nonzero. Got %d", ret );

    ret = set_add(set, PTR(2));
    cr_assert( ret == 0, "Expected set_add to return zero. Got %d", ret );

    ret = set_add(set, PTR(2));
    cr_assert( ret != 0, "Expected set_add to return nonzero. Got %d", ret );

    ret = set_add(set, PTR(22));
    cr_assert( ret == 0, "Expected set_add to return zero. Got %d", ret );
}

#define SET_ADD_REHASH_TRIALS 128
Test(set_tests, set_add_rehash, .timeout = 5)
{
    SET set = set_init();
    int ret;
    
    for (size_t i = 0; i < SET_ADD_REHASH_TRIALS; ++i)
    {
        ret = set_add(set, PTR(i));
        cr_assert(ret == 0, "Expected set_add to return zero. Got %d", ret);
    }
    set_fini(set);
}
#undef SET_ADD_REHASH_TRIALS

Test(set_tests, set_contains_simple, .timeout = 5)
{
    SET set = set_init();
    int ret;

    // NOTE: we cannot check 0 because that corresponds with NULL which we treat as an empty cell.
    for (size_t i = 1; i < 64; ++i) set_add(set, PTR(i));

    for (size_t i = 1; i < 64; ++i)
    {
        ret = set_contains(set, PTR(i));
        cr_assert(ret != 0, "Expected set_contains to return nonzero for checking %lu is in the set. Got %d", i, ret);
    }
    set_fini(set);
}

Test(set_tests, set_contains_random, .timeout = 5)
{
    unsigned seed = 22;
    size_t max_ptr_value = 64;
    size_t trial_count = max_ptr_value * 2;
    int ret;

    srand(seed == -1 ? time(NULL) : seed);
    SET set = set_init();

    // offset by 0 is because we cannot add 0 to the set 
    for (size_t i = 1; i < max_ptr_value; ++i) set_add(set, PTR(i));

    for (size_t i = 0; i < trial_count; ++i)
    {
        int test = rand() % (max_ptr_value * 2) + 1;
        
        ret = set_contains(set, PTR(test));
        if (test < max_ptr_value) cr_assert(ret != 0, "Expected set_contains to return nonzero for check %d is in the set. Got %d", test, ret);
        else cr_assert(ret == 0, "Expected set_contains to return zero for check %d is in the set. Got %d", test, ret);
    }
    set_fini(set);
}

// 64K trials
#define SET_ADD_RANDOM_TRIALS (1 << 16)
Test(set_tests, set_add_random, .timeout = 5)
{
    unsigned seed = 22;

    // populate the data
    srand(seed == -1 ? time(NULL) : seed);
    void *arr[SET_ADD_RANDOM_TRIALS] = { 0 };
    for (size_t i = 0; i < SET_ADD_RANDOM_TRIALS; ++i)
        arr[i] = PTR(rand());

    // initialized and add data to the set
    SET set = set_init();
    for (size_t i = 0; i < SET_ADD_RANDOM_TRIALS; ++i)
        set_add(set, arr[i]);

    // goes through the list and checks if it is in the set
    for (size_t i = 0; i < SET_ADD_RANDOM_TRIALS; ++i)
        cr_assert(set_contains(set, arr[i]), "Set did not contain pointer %p in trial %lu", arr[i], i);    
    set_fini(set);
}
#undef SET_ADD_RANDOM_TRIALS

#define SET_SIZE_RANDOM_TRIALS (1 << 4)
Test(set_tests, set_size_random, .timeout = 5)
{
    unsigned seed = 22;
    unsigned max_size = 256;

    // populate the data
    srand(seed == -1 ? time(NULL) : seed);
    int sizes[SET_SIZE_RANDOM_TRIALS] = { 0 };
    for (size_t i = 0; i < SET_SIZE_RANDOM_TRIALS; ++i)
        sizes[i] = rand() % max_size;

    // perform the trials
    for (size_t i = 0; i < SET_SIZE_RANDOM_TRIALS; ++i)
    {
        SET set = set_init();
        for (int j = 0; j < sizes[i]; ++j) set_add(set, PTR(j));
        size_t sz = set_size(set);
        set_fini(set);

        cr_assert(sz == sizes[i], "Trial %lu: Expected the size of the set to be %lu. Got %lu.", i, sizes[i], sz);
    }
}

Test(set_tests, set_remove_simple, .timeout = 5)
{
    unsigned max_size = 256;
    int ret;

    SET set = set_init();
    
    for (size_t i = 1; i < max_size; ++i) set_add(set, PTR(i));

    for (size_t i = 1; i < max_size; ++i)
    {
        ret = set_remove(set, PTR(i));
        cr_assert(ret == 0, "Expected set_remove to return zero for removing %lu. Got %d", i, ret);
    }

    set_fini(set);
}

// be careful not to set this value too high, this test case runs O(N^2) where N is number of trials
#define SET_REMOVE_RANDOM_TRIAL (1 << 7)
Test(set_tests, set_remove_random, .timeout = 5)
{
    unsigned seed = 22;

    size_t max_ptr_value = SET_REMOVE_RANDOM_TRIAL / 2;
    int removed_ptrs[SET_REMOVE_RANDOM_TRIAL] = { 0 };
    size_t removed_ptrs_sz = 0;
    int ret, removed_already;

    srand(seed == -1 ? time(NULL) : seed);
    SET set = set_init();

    // offset by 0 is because we cannot add 0 to the set 
    for (size_t i = 1; i < max_ptr_value; ++i) set_add(set, PTR(i));

    // attempt to remove the value if its there
    for (size_t i = 0; i < SET_REMOVE_RANDOM_TRIAL; ++i)
    {
        int test = rand() % (max_ptr_value * 2) + 1;

        removed_already = 0;
        for (size_t j = 0; j < removed_ptrs_sz; ++j)
        {
            if (removed_ptrs[j] == test)
            {
                removed_already = 1;
                break;
            }
        }
        
        ret = set_remove(set, PTR(test));
        if (test < max_ptr_value && !removed_already) 
        {
            cr_assert(ret == 0, "Expected set_remove to return zero for value %d. Got %d", test, ret);
            removed_ptrs[removed_ptrs_sz++] = test;
        }
        else cr_assert(ret != 0, "Expected set_remove to return nonzero for value %d. Got %d", test, ret);
    }

    // now check that those values are no longer in the set
    for (size_t i = 0; i < removed_ptrs_sz; ++i)
    {
        ret = set_contains(set, PTR(removed_ptrs[i]));
        cr_assert(ret == 0, "Expected set_contains to return zero for value %d. Got %d", removed_ptrs[i], ret);
    }

    set_fini(set);
}

#define SET_CLEAR_RANDOM_TRIALS 16
Test(set_tests, set_clear_random, .timeout = 5)
{
    int seed = 22;
    size_t ret = 0;

    size_t sizes[SET_CLEAR_RANDOM_TRIALS] = { 0 };
    srand(seed == -1 ? time(NULL) : seed);
    for (size_t i = 0; i < SET_CLEAR_RANDOM_TRIALS; ++i)
        sizes[i] = (rand() % 256) + 1;

    SET set = set_init();
    for (size_t i = 0; i < SET_CLEAR_RANDOM_TRIALS; ++i)
    {
        for (size_t j = 1; j < sizes[i]; ++j) set_add(set, PTR(j));
        set_clear(set);

        ret = set_size(set);
        cr_assert(ret == 0, "Expected size of the set to be 0 after set_clear. Got %lu", ret);
    }
    set_fini(set);
}

Test(set_tests, set_iterator_lifetime, .timeout = 5)
{
    SET set = set_init();
    SET_ITERATOR iter = set_iterator_init(set);
    set_iterator_fini(iter);
    set_fini(set);
}

#define SET_ITERATOR_SIMPLE_TEST_SIZE (1 << 8)
Test(set_tests, set_iterator_simple, .timeout = 5)
{
    SET set = set_init();
    for (size_t i = 1; i < SET_ITERATOR_SIMPLE_TEST_SIZE; ++i) set_add(set, PTR(i));
    
    char flag[SET_ITERATOR_SIMPLE_TEST_SIZE] = { 0 };
    // create iterator
    SET_ITERATOR iter = set_iterator_init(set);
    while (set_iterator_has_next(iter))
    {
        void *value = set_iterator_next(iter);
        flag[INT(value)]++; // increment count;
    }
    set_iterator_fini(iter);

    for (size_t i = 1; i < SET_ITERATOR_SIMPLE_TEST_SIZE; ++i)
    {
        cr_assert(flag[i] == 1, "Expected the iterator to read %lu once. Instead iterator returned %d times", i, flag[i]);
    }
    set_fini(set);
}
