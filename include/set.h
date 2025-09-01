#ifndef SET_H
#define SET_H

#include <stdlib.h>

typedef struct set * SET;
typedef struct set_iterator * SET_ITERATOR;

/**
 * initializes an empty set of pointers. 
 * all sets initialized via this function should be destroyed using `set_fini`
 * 
 * @return newly created map set 
 */
SET set_init();

/**
 * destroys a set of pointers.
 * 
 * @param set a set to destroy 
 * @warning it is an error to use the set after it has been destroyed with this function
 */
void set_fini(SET set);

/**
 * obtain the cardinality of a set, i.e. the number of elements within the set
 * 
 * @param set set to get the size of
 * @return the cardinality of `set`
 */
size_t set_size(SET set);

/**
 * checks a pointer is contained in the set
 * 
 * @param set set to check if `ptr` is in
 * @param ptr the pointer to check if is in `set`
 * @return zero if the `ptr` is not in `set`; otherwise nonzero. 
 */
int set_contains(SET set, void *ptr);

/**
 * adds a pointer to the set
 * 
 * @param set set to add the pointer `ptr` to
 * @param ptr pointer to add to `set`
 * @return zero if adding the pointer succeeded; otherwise nonzero. 
 */
int set_add(SET set, void *ptr);

/**
 * removes a pointer from the set 
 * 
 * @param set set to remove the pointer `ptr` from
 * @param ptr pointer to remove from `set`
 * @return zero if removing the pointer succeeded; otherwise nonzero.
 */
int set_remove(SET set, void *ptr);

/**
 * clears the content in a set
 * 
 * @param set set to clear
 */
void set_clear(SET set);

/**
 * returns a list of the values stored in the set in no particular order. the
 * size of this list is given by `set_size`
 * 
 * @param set the set to retrieve the values
 * @returns a dynamically allocated list of values stored in the set
 * @warning the list returned by this function is dynamically allocated and must be freed
 */
void **set_values(SET set);

/**
 * creates a new set that is the union of the two sets
 * 
 * @param A the first set
 * @param B the second set
 * @return the union of sets `A` and `B`
 * @warning this function creates a new set that must be destroyed using `set_fini`
 */
SET set_union(SET A, SET B);

/**
 * creates a new set that is the intersection of the two sets
 * 
 * @param A the first set
 * @param B the second set
 * @return the intersection of sets `A` and `B`
 * @warning this function creates a new set that must be destroyed using `set_fini`
 */
SET set_intersection(SET A, SET B);

/**
 * creates an iterator for a set
 * 
 * @note the order of the output from the iterator is not defined. it may not be like the order of insertion.
 * 
 * @param set the set to iterate
 * @return an iterator for the elements in the set
 * @warning the iterator (and any copies) is invalidated the moment the set is modified in anyway or destroyed
 */
SET_ITERATOR set_iterator_init(SET set);

/**
 * destroys an iterator object for the set
 * 
 * @param iterator the iterator to destroy
 * @warning the iterator is invalidated the moment the set is modified in anyway
 * @warning the iterator (and any copies) is invalidated the moment the set is modified in anyway
 */
void set_iterator_fini(SET_ITERATOR iterator);

/**
 * determines if the next value for iteration exists. to obtain this value, use `set_iterator_next`
 * 
 * @param iterator the iterator to determine if there is a next value
 * @return nonzero if there is a next value; otherwise zero.
 * @warning the iterator (and any copies) is invalidated the moment the set is modified in anyway or destroyed
 */
int set_iterator_has_next(SET_ITERATOR iterator);

/**
 * obtains the next value of the iterator and moves the iterator forward
 * 
 * @param iterator the iterator to determine if there is a next value
 * @return the next pointer in the set from the iterator
 * @warning the iterator (and any copies) is invalidated the moment the set is modified in anyway or destroyed
 */
void *set_iterator_next(SET_ITERATOR iterator);

#endif