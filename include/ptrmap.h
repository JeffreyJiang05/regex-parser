#ifndef PTR_MAP_H
#define PTR_MAP_H

#include <stdlib.h>

// we never use this map for deletion so deletion of a key:value pair is not implemented 

typedef struct ptr_map * PTR_MAP;
typedef struct ptr_map_iterator * PTR_MAP_ITERATOR;

/**
 * initializes an empty map with integer keys and pointer values.
 * all maps initialized via this function should be destroyed using `map_fini`
 * 
 * @return newly created empty map
 */
PTR_MAP ptrmap_init();

/**
 * destroys a map
 * 
 * @param map the map to destroy
 * @warning it is an error to use the map after it has been destroyed
 */
void ptrmap_fini(PTR_MAP map);

/**
 * returns the number of key-value pairs in the map
 * 
 * @param map the map to get the size of
 * @return the number of key-value pairs in the map
 */
size_t ptrmap_size(PTR_MAP map);

/**
 * checks if a key is in the map
 * 
 * @param map the map to check for the key
 * @param key the key to check for in the map
 * @return nonzero if the key is in the map; otherwise nonzero
 */
int ptrmap_contains_key(PTR_MAP map, void *key);

/**
 * adds a key value pair into the map. if the key is already in the map, the
 * corresponding value is replaced. 
 * 
 * @param map the map to add the key-value pair into
 * @param key the key to be added to the map
 * @param value the non-null value to be added to the map
 * @return zero if adding the pair succeeded; otherwise nonzero
 */
int ptrmap_set(PTR_MAP map, void *key, void *value);

/**
 * retrives the value for a corresponding key in the map. 
 * 
 * @param map the map to retrieve the value corresponding to the key
 * @param key the key
 * @return the value corresponding to the key; null otherwise. 
 */
void * ptrmap_get(PTR_MAP map, void *key);

/**
 * creates an iterator for a ptr map
 * 
 * @note the order of the output from the iterator is not defined. it may not be like the order of insertion.
 * 
 * @param map the ptr map to iterate
 * @return an iterator for the elements in the ptr map
 * @warning the iterator (and any copies) is invalidated the moment the map is modified in anyway or destroyed
 */
PTR_MAP_ITERATOR ptrmap_iterator_init(PTR_MAP map);

/**
 * destroys an iterator object for the ptr map
 * 
 * @param iterator the iterator to destroy
 * @warning the iterator is invalidated the moment the map is modified in anyway
 * @warning the iterator (and any copies) is invalidated the moment the map is modified in anyway
 */
void ptrmap_iterator_fini(PTR_MAP_ITERATOR iterator);

/**
 * determines if the next value for iteration exists. to obtain this value, use `ptrmap_iterator_next`
 * 
 * @param iterator the iterator to determine if there is a next value
 * @return nonzero if there is a next value; otherwise zero.
 * @warning the iterator (and any copies) is invalidated the moment the map is modified in anyway or destroyed
 */
int ptrmap_iterator_has_next(PTR_MAP_ITERATOR iterator);

/**
 * obtains the next value of the iterator and moves the iterator forward
 * 
 * @param iterator the iterator to determine if there is a next value
 * @param key a reference to a poijter to store the next key in the map from the iterator
 * @param value a reference to a pointer to store the next value in the map from the iterator
 * @warning the iterator (and any copies) is invalidated the moment the map is modified in anyway or destroyed
 */
void ptrmap_iterator_next(PTR_MAP_ITERATOR iterator, void **key, void **value);

#endif