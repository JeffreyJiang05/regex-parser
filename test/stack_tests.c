#include <criterion/criterion.h>

#include <stdint.h>
#include <time.h>

#include "stack.h"

typedef union
{
    uint64_t val; 
    void *ptr;
} CVT;

#define PTR(value) ((CVT){(value)}.ptr)
#define INT(ptrval) ((CVT){.ptr=(ptrval)}.val)

Test(stack_tests, stack_lifetime, .timeout = 5)
{
    STACK stack = stack_init();
    cr_assert(stack != NULL, "Expected stack_init to return nonnull. Got null.");
    stack_fini(stack);
}

#define STACK_PUSH_POP_RANDOM_MAX_SIZE (1 << 8)
Test(stack_tests, stack_push_pop_random, .timeout = 5)
{
    unsigned seed = 22;
    srand(seed == -1 ? time(NULL) : seed);

    void *random_data[STACK_PUSH_POP_RANDOM_MAX_SIZE] = { 0 };
    for (size_t i = 0; i < STACK_PUSH_POP_RANDOM_MAX_SIZE; ++i)
        random_data[i] = PTR(i);

    int ret;

    STACK stack = stack_init();
    // push all data onto the stack
    for (size_t i = 0; i < STACK_PUSH_POP_RANDOM_MAX_SIZE; ++i)
    {
        ret = stack_push(stack, random_data[i]);
        cr_assert(ret == 0, "Expected stack_push to return zero. Got %d for trial %lu", ret, i);
    }

    void *data;
    for (int i = STACK_PUSH_POP_RANDOM_MAX_SIZE - 1; i >= 0; --i)
    {
        ret = stack_pop(stack, &data);
        cr_assert(ret == 0, "Expected stack_pop to return zero. Got %d for trial %d", ret, i);
        cr_assert(data == random_data[i], "Expected popped data to be %lu. Got %lu", INT(random_data[i]), INT(data));
    }

    ret = stack_pop(stack, &data);
    cr_assert(ret != 0, "Expected stack_pop to return nonzero. Got %d", ret);

    stack_fini(stack);
}

#define STACK_PEEK_MAX_SIZE (1 << 8)
Test(stack_tests, stack_peek_random, .timeout = 5)
{
    unsigned seed = 22;
    srand(seed == -1 ? time(NULL) : seed);

    void *random_data[STACK_PUSH_POP_RANDOM_MAX_SIZE] = { 0 };
    for (size_t i = 0; i < STACK_PUSH_POP_RANDOM_MAX_SIZE; ++i)
        random_data[i] = PTR(i);

    int ret;
    void *data;

    STACK stack = stack_init();
    // push all data onto the stack
    for (size_t i = 0; i < STACK_PUSH_POP_RANDOM_MAX_SIZE; ++i) 
    {
        stack_push(stack, random_data[i]);
        int chance = rand() % 5;
        if (chance == 0)
        {
            // peek
            ret = stack_peek(stack, &data);
            cr_assert(ret == 0, "Expected stack_peek to return zero. Got %d for trial %d", ret, i);
            cr_assert(data == random_data[i], "Expected peeked data to be %lu. Got %lu", INT(random_data[i]), INT(data));
        }
    }

    for (int i = STACK_PUSH_POP_RANDOM_MAX_SIZE - 1; i >= 0; --i)
    {
        int chance = rand() % 5;
        if (chance == 0)
        {
            // peek
            ret = stack_peek(stack, &data);
            cr_assert(ret == 0, "Expected stack_peek to return zero. Got %d for trial %d", ret, i);
            cr_assert(data == random_data[i], "Expected peeked data to be %lu. Got %lu", INT(random_data[i]), INT(data));
        }
        stack_pop(stack, &data);
    }

    ret = stack_peek(stack, &data);
    cr_assert(ret != 0, "Expected stack_pop to return nonzero. Got %d", ret);

    stack_fini(stack);
}

#define STACK_CLEAR_SIMPLE_MAX_SIZE (1 << 5)
Test(stack_tests, stack_clear_simple, .timeout = 5)
{
    int ret;

    STACK stack = stack_init();

    for (size_t i = 0; i < STACK_CLEAR_SIMPLE_MAX_SIZE; ++i)
        stack_push(stack, PTR(i));

    stack_clear(stack);

    size_t sz = stack_size(stack);
    cr_assert(sz == 0, "Expected stack_size to return 0. Got %lu", sz);

    void *data;
    ret = stack_peek(stack, &data);
    cr_assert(ret != 0, "Expected stack_peek to return nonzero. Got %d", ret);

    ret = stack_pop(stack, NULL);
    cr_assert(ret != 0, "Expected stack_pop to return nonzero. Got %d", ret);

    stack_fini(stack);
}