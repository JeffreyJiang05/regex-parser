#include <stdint.h>
#include <time.h>

#include <criterion/criterion.h>

#include "utility/map.h"

typedef union
{
    uint64_t val; 
    void *ptr;
} CVT;

#define PTR(value) ((CVT){(value)}.ptr)
#define INT(ptrval) ((CVT){.ptr=(ptrval)}.val)

Test(map_tests, map_lifetime, .timeout = 5)
{
    MAP map = map_init();
    cr_assert(map != NULL, "Failed to allocate for the set");
    size_t sz = map_size(map);
    cr_assert(sz == 0, "New set has size %lu not 0", sz);
    map_fini(map);
}

Test(map_tests, map_set_simple, .timeout = 5)
{
    int ret;
    MAP map = map_init();

    // should fail
    ret = map_set(map, 0, NULL);
    cr_assert(ret != 0, "Expected map_set to return nonzero. Got %d", ret);

    ret = map_set(map, 0, PTR(1));
    cr_assert(ret == 0, "Expected map_set to return zero. Got %d", ret);

    ret = map_set(map, 1, PTR(1));
    cr_assert(ret == 0, "Expected map_set to return zero. Got %d", ret);

    ret = map_set(map, 0xFF00FF00, PTR(0xFF00FF00));
    cr_assert(ret == 0, "Expected map_set to return zero. Got %d", ret);

    ret = map_set(map, 1, PTR(4));
    cr_assert(ret == 0, "Expected map_set to return zero. Got %d", ret);

    map_fini(map);
}   

Test(map_tests, map_add_simple, .timeout = 5)
{
    int ret;
    MAP map = map_init();

    ret = map_add(map, 0, NULL);
    cr_assert(ret != 0, "Expected map_add to return nonzero. Got %d", ret);

    ret = map_add(map, 0, PTR(1));
    cr_assert(ret == 0, "Expected map_add to return zero. Got %d", ret);

    ret = map_add(map, 1, PTR(1));
    cr_assert(ret == 0, "Expected map_add to return zero. Got %d", ret);

    ret = map_add(map, 0xFF00FF00, PTR(0xFF00FF00));
    cr_assert(ret == 0, "Expected map_add to return zero. Got %d", ret);

    ret = map_add(map, 1, PTR(4));
    cr_assert(ret != 0, "Expected map_add to return nonzero. Got %d", ret);

    map_fini(map);
}

#define MAP_SET_RANDOM_SIZE 256
#define MAP_SET_RANDOM_TRIALS 256
Test(map_tests, map_set_and_get_random, .timeout = 5)
{
    unsigned seed = 22;
    srand(seed == -1 ? time(NULL) : seed);
    int ret;

    int values[MAP_SET_RANDOM_SIZE] = { 0 };
    for (size_t i = 0; i < MAP_SET_RANDOM_SIZE; ++i) values[i] = -1;

    MAP map = map_init();
    for (size_t i = 0; i < MAP_SET_RANDOM_TRIALS; ++i)
    {
        int idx = rand() % MAP_SET_RANDOM_SIZE;
        int value = rand();
        if (value == 0) value++; // cant have value of 0 in the map

        values[idx] = value;
        ret = map_set(map, idx, PTR(value));
        cr_assert(ret == 0, "Expected map_set to return zero. Got %d", ret);
    }

    void *output;
    for (size_t i = 0; i < MAP_SET_RANDOM_SIZE; ++i)
    {
        output = map_get(map, i);
        // index i was not set in the map
        if (values[i] == -1) cr_assert(output == NULL, "Erroneously found value %d for key %lu in map.", INT(output), i);
        else cr_assert(INT(output) == values[i], "Mismatch between value %d associated with key %lu. Expected %d", INT(output), i, values[i]);
    }

    map_fini(map);
}

#define MAP_ADD_RANDOM_SIZE 256
#define MAP_ADD_RANDOM_TRIALS 256
Test(map_tests, map_add_and_get_random, .timeout = 5)
{
    unsigned seed = 22;
    srand(seed == -1 ? time(NULL) : seed);
    int ret;

    int values[MAP_ADD_RANDOM_SIZE] = { 0 };
    for (size_t i = 0; i < MAP_ADD_RANDOM_SIZE; ++i) values[i] = -1;

    MAP map = map_init();
    for (size_t i = 0; i < MAP_ADD_RANDOM_TRIALS; ++i)
    {
        int idx = rand() % MAP_ADD_RANDOM_SIZE;
        int value = rand();
        if (value == 0) value++; // cant have value of 0 in the map

        ret = map_add(map, idx, PTR(value));
        if (values[idx] == -1) 
        {
            cr_assert(ret == 0, "Expected map_add to return zero. Got %d", ret);
            values[idx] = value;
        }
        else cr_assert(ret != 0, "Expected map_add to return nonzero. Got %d", ret);
    }

    void *output;
    for (size_t i = 0; i < MAP_ADD_RANDOM_SIZE; ++i)
    {
        output = map_get(map, i);
        // index i was not set in the map
        if (values[i] == -1) cr_assert(output == NULL, "Erroneously found value %d for key %lu in map.", INT(output), i);
        else cr_assert(INT(output) == values[i], "Mismatch between value %d associated with key %lu. Expected %d", INT(output), i, values[i]);
    }

    map_fini(map);
}

#define MAP_SIZE_TRIALS 64
#define MAP_SIZE_MAX_SIZE 128
Test(map_tests, map_size_random, .timeout = 5)
{
    unsigned seed = 22;
    srand(seed == -1 ? time(NULL) : seed);
    size_t ret;
    MAP map;

    int sizes[MAP_SIZE_TRIALS] = { 0 };
    for (size_t i = 0; i < MAP_SIZE_TRIALS; ++i)
        sizes[i] = rand() % MAP_SIZE_MAX_SIZE;

    for (size_t i = 0; i < MAP_SIZE_TRIALS; ++i)
    {
        map = map_init();
        for (int k = 0; k < sizes[i]; ++k) map_add(map, k, PTR(k + 1));
        ret = map_size(map);
        cr_assert(ret == sizes[i], "Incorrect map size %lu. Expected %d", ret, sizes[i]);
        map_fini(map);
    }
}

#define MAP_CONTAINS_KEY_RANDOM_MAX_SIZE 128
#define MAP_CONTAINS_KEY_RANDOM_TRIALS 256
Test(map_tests, map_contains_key_random, .timeout = 5)
{
    unsigned seed = 22;
    srand(seed == -1 ? time(NULL) : seed);
    int ret;

    MAP map = map_init();
    for (size_t i = 0; i < MAP_CONTAINS_KEY_RANDOM_MAX_SIZE; ++i) map_add(map, i, PTR(i + 1));

    for (size_t i = 0; i < MAP_CONTAINS_KEY_RANDOM_TRIALS; ++i)
    {
        int key = rand() % (MAP_CONTAINS_KEY_RANDOM_MAX_SIZE * 2);
        ret = map_contains_key(map, key);
        if (key < MAP_CONTAINS_KEY_RANDOM_MAX_SIZE) cr_assert(ret != 0, "Expected map_contains_key to return true value. Got false");
        else cr_assert(ret == 0, "Expected map_contains_key to return false value. Got true");
    }
    map_fini(map);
}

#define MAP_REMOVE_RANDOM_SIZE 256
#define MAP_REMOVE_RANDOM_COUNT 64
Test(map_tests, map_remove_random, .timeout = 5)
{
    unsigned seed = 22;
    srand(seed == -1 ? time(NULL) : seed);
    int ret;

    int removed_keys[MAP_REMOVE_RANDOM_SIZE] = { 0 };
    MAP map = map_init();

    for (size_t i = 0; i < MAP_REMOVE_RANDOM_SIZE; ++i)
        map_add(map, i, PTR(i + 1));

    for (size_t i = 0; i < MAP_REMOVE_RANDOM_COUNT; ++i)
    {
        int key = rand() % MAP_REMOVE_RANDOM_SIZE;
        int ret = map_remove(map, key);
        // key has not been removed, so should be successful ret
        if (removed_keys[key] == 0) cr_assert(ret == 0, "Expected map_remove to return zero. Got %d", ret);
        else cr_assert(ret != 0, "Expected map_remove to return nonzero. Got %d", ret);
        removed_keys[key] = 1;
    }

    for (size_t i = 0; i < MAP_REMOVE_RANDOM_SIZE; ++i)
    {
        ret = map_contains_key(map, i);
        if (removed_keys[i] == 0) cr_assert(ret != 0, "Expected map_contains_key to return nonzero. Got %d", ret);
        else cr_assert(ret == 0, "Expected map_contains_key to return zero. Got %d", ret);
    }
    map_fini(map);
}

Test(map_tests, map_clear_simple, .timeout = 5)
{
    size_t map_size = 64;
    int ret = 0;

    MAP map = map_init();
    for (size_t i = 0; i < map_size; ++i) map_add(map, i, PTR(i + 1));
    map_clear(map);
    for (size_t i = 0; i < map_size * 2; ++i)
    {
        ret = map_contains_key(map, i);
        cr_assert(ret == 0, "Expected map_contains_key to return 0. Got %d", ret);
    }
    map_fini(map);
}

Test(map_tests, map_iterator_lifetime, .timeout = 5)
{
    MAP map = map_init();
    MAP_ITERATOR iter = map_iterator_init(map);
    cr_assert(iter != NULL, "Expected map_iterator_init to return nonnull. Returned null.");
    map_iterator_fini(iter);
    map_fini(map);
}

#define MAP_ITERATOR_SIMPLE_TEST_SIZE (1 << 8)
Test(map_tests, map_iterator_simple, .timeout = 5)
{
    int key;
    void *value;

    MAP map = map_init();
    for (size_t i = 1; i < MAP_ITERATOR_SIMPLE_TEST_SIZE; ++i) map_add(map, i, PTR(i + 1));
    
    char flag[MAP_ITERATOR_SIMPLE_TEST_SIZE] = { 0 };
    // create iterator
    MAP_ITERATOR iter = map_iterator_init(map);
    while (map_iterator_has_next(iter))
    {
        map_iterator_next(iter, &key, &value);
        cr_assert(key + 1 == INT(value), "Expected the value to be one more than the pointer. Did not happen.");
        flag[key]++; // increment count;
    }
    map_iterator_fini(iter);

    for (size_t i = 1; i < MAP_ITERATOR_SIMPLE_TEST_SIZE; ++i)
    {
        cr_assert(flag[i] == 1, "Expected the iterator to read key %lu once. Instead iterator returned %d times", i, flag[i]);
    }
    map_fini(map);
}

#define MAP_KEYS_SIMPLE_TEST_SIZE (1 << 8)
Test(map_tests, map_keys_simple, .timeout = 5)
{
    char flag[MAP_KEYS_SIMPLE_TEST_SIZE] = { 0 };

    MAP map = map_init();
    for (size_t i = 0; i < MAP_KEYS_SIMPLE_TEST_SIZE; ++i) map_add(map, i, PTR(i + 1));

    int *buf = map_keys(map);

    size_t buf_sz = map_size(map);
    for (size_t i = 0; i < buf_sz; ++i)
    {
        cr_assert(buf[i] >= 0 && buf[i] < MAP_KEYS_SIMPLE_TEST_SIZE, "Expected the key %d to fall into valid range [0, %d)", buf[i], MAP_KEYS_SIMPLE_TEST_SIZE);
        if (buf[i] >= 0 && buf[i] < MAP_KEYS_SIMPLE_TEST_SIZE)
        {
            flag[buf[i]]++;
        }
    }

    for (size_t i = 0; i < MAP_KEYS_SIMPLE_TEST_SIZE; ++i)
        cr_assert(flag[i] == 1, "Expected key %d to be appear once in the buf array", i);

    free(buf);
    map_fini(map);
}

#define MAP_VALUES_SIMPLE_TEST_SIZE (1 << 8)
Test(map_tests, map_values_simple, .timeout = 5)
{
    char flag[MAP_VALUES_SIMPLE_TEST_SIZE] = { 0 };

    MAP map = map_init();
    for (size_t i = 0; i < MAP_VALUES_SIMPLE_TEST_SIZE; ++i) map_add(map, i, PTR(i + 1));

    void **buf = map_values(map);

    size_t buf_sz = map_size(map);
    for (size_t i = 0; i < buf_sz; ++i)
    {
        int buf_val = INT(buf[i]) - 1;
        cr_assert(buf_val >= 0 && buf_val < MAP_VALUES_SIMPLE_TEST_SIZE, "Expected the value %d to fall into valid range [0, %d)", buf_val, MAP_VALUES_SIMPLE_TEST_SIZE);
        if (buf_val >= 0 && buf_val < MAP_VALUES_SIMPLE_TEST_SIZE)
        {
            flag[buf_val]++;
        }
    }

    for (size_t i = 0; i < MAP_VALUES_SIMPLE_TEST_SIZE; ++i)
        cr_assert(flag[i] == 1, "Expected value %d to be appear once in the buf array", i);

    free(buf);
    map_fini(map);
}