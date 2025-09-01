#ifndef STACK_H
#define STACK_H

#include <stdlib.h>

typedef struct stack * STACK;

/**
 * creates a stack
 * 
 * @return the newly created stack
 */
STACK stack_init();

/**
 * destroys a stack
 * 
 * @param stack the stack to destroy
 * @warning destroying the stack does not destroy its contents
 */
void stack_fini(STACK stack);

/**
 * retrieves the size of the stack
 * 
 * @param stack the stack whose size to determine
 * @return the size of the stack 
 */
size_t stack_size(STACK stack);

/**
 * pushes a value onto the stack
 * 
 * @param stack the stack to push data onto
 * @param elem the element to push onto the stack
 * @return zero on success; nonzero otherwise
 */
int stack_push(STACK stack, void *elem);

/**
 * pops a value from the stack if there is one
 * 
 * @param stack the stack to pop data from
 * @param ref_data the location to store the data popped from the stack
 * @return zero if data is popped off the stack, nonzero otherwise
 */
int stack_pop(STACK stack, void **ref_data);

/**
 * peeks a value on the top of the stack
 * 
 * @param stack the stack to peek
 * @param ref_data the location to store the peeked value on the stack
 * @return zero on success; nonzero otherwise
 */
int stack_peek(STACK stack, void **ref_data);

/**
 * clears the stack
 * 
 * @param stack the stack to clear
 */
void stack_clear(STACK stack);

#endif