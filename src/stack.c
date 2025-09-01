#include "stack.h"

#include <string.h>

#include "debug.h"

#define DEFAULT_STACK_CAPACITY 8

struct stack
{
    void **buffer;
    size_t size;
    size_t capacity;
};

STACK stack_init()
{
    STACK stack = malloc(sizeof(struct stack));
    stack->size = 0;
    stack->capacity = DEFAULT_STACK_CAPACITY;
    stack->buffer = malloc(stack->capacity * sizeof(void*));
    info("Initialized Stack[%p].", stack);
    return stack;
}

void stack_fini(STACK stack)
{
    free(stack->buffer);
    free(stack);
    info("Destroyed Stack[%p].", stack);
}

size_t stack_size(STACK stack)
{
    return stack->size;
}

static void resize(STACK stack)
{
    // double the size
    size_t new_capacity = 2 * stack->capacity;
    void **new_buffer = malloc(new_capacity * sizeof(void*));
    memcpy(new_buffer, stack->buffer, stack->capacity * sizeof(void*));
    free(stack->buffer);
    stack->buffer = new_buffer;
    stack->capacity = new_capacity;
    info("Stack[%p] was resized.", stack);
}

int stack_push(STACK stack, void *elem)
{
    info("Attempting to push %p onto Stack[%p].", elem, stack);

    if (stack->size == stack->capacity) resize(stack);
    stack->buffer[stack->size++] = elem;

    info("Successfully pushed %p onto Stack[%p].", elem, stack);
    return 0;
}

int stack_pop(STACK stack, void **ref_data)
{
    info("Attempting to pop off Stack[%p].", stack);

    if (stack->size == 0) 
    {
        info("Failed to pop off Stack[%p] (empty stack).", stack);
        return -1;
    }

    void *data = stack->buffer[--stack->size];
    if (ref_data) *ref_data = data;
    info("Successfully popped %p off Stack[%p].", data, stack);
    return 0;
}

int stack_peek(STACK stack, void **ref_data)
{
    if (!ref_data || !stack) return -1;

    if (stack->size == 0) return -1;

    *ref_data = stack->buffer[stack->size - 1];
    return 0;
}

void stack_clear(STACK stack)
{
    stack->size = 0;
}