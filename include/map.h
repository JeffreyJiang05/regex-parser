#ifndef MAP_H
#define MAP_H

#include <stdlib.h>

typedef struct map * MAP;
typedef struct map_iterator * MAP_ITERATOR;

/**
 * initializes an empty map with integer keys and pointer values.
 * all maps initialized via this function should be destroyed using `map_fini`
 * 
 * @return newly created empty map
 */
MAP map_init();

/**
 * destroys a map
 * 
 * @param map the map to destroy
 * @warning it is an error to use the map after it has been destroyed
 */
void map_fini(MAP map);

/**
 * returns the number of key-value pairs in the map
 * 
 * @param map the map to get the size of
 * @return the number of key-value pairs in the map
 */
size_t map_size(MAP map);

/**
 * checks if a key is in the map
 * 
 * @param map the map to check for the key
 * @param key the key to check for in the map
 * @return nonzero if the key is in the map; otherwise nonzero
 */
int map_contains_key(MAP map, int key);

/**
 * adds a key value pair into the map. if the key is already in the map, the
 * corresponding value is replaced. 
 * 
 * @param map the map to add the key-value pair into
 * @param key the key to be added to the map
 * @param value the non-null value to be added to the map
 * @return zero if adding the pair succeeded; otherwise nonzero
 */
int map_set(MAP map, int key, void *value);

/**
 * sets the value for a key in the map. if the key is in the map already, the
 * call fails unlike `map_set`
 * 
 * @param map the map to set the value for the specified key
 * @param key the key to replace the corresponding value of in the map
 * @param value the new non-null value for the key in the map
 * @return zero if adding the pair succeeded; otherwise nonzero
 */
int map_add(MAP map, int key, void *value);

/**
 * retrives the value for a corresponding key in the map. 
 * 
 * @param map the map to retrieve the value corresponding to the key
 * @param key the key
 * @return the value corresponding to the key; null otherwise. 
 */
void * map_get(MAP map, int key);

/**
 * removes a key-value pair from the map
 * 
 * @param map the map to remove the pair
 * @param key the key to remove from the map
 * @return zero if adding the pair succeeded; otherwise nonzero
 */
int map_remove(MAP map, int key);

/**
 * clears the content of a map
 * 
 * @param map the map to clear 
 */
void map_clear(MAP map);

/**
 * returns a list of the map's keys whose size is determined by the `map_size` function
 * 
 * @param map the map to retrieve the keys of
 * @return a dynamically-allocated array of the keys used in the set
 * @warning this function returns a dynamically allocated array that MUST be freed
 */
int * map_keys(MAP map);

/**
 * returns a list of the map's values whose size is determined by the `map_size` function
 * 
 * @param map the map to retrieve the values of
 * @return a dynamically-allocated array of the values used in the set
 * @warning this function returns a dynamically allocated array that MUST be freed
 */
void **map_values(MAP map);

/**
 * creates an iterator for a map
 * 
 * @note the order of the output from the iterator is not defined. it may not be like the order of insertion.
 * 
 * @param map the map to iterate
 * @return an iterator for the elements in the map
 * @warning the iterator (and any copies) is invalidated the moment the map is modified in anyway or destroyed
 */
MAP_ITERATOR map_iterator_init(MAP map);

/**
 * destroys an iterator object for the map
 * 
 * @param iterator the iterator to destroy
 * @warning the iterator is invalidated the moment the map is modified in anyway
 * @warning the iterator (and any copies) is invalidated the moment the map is modified in anyway
 */
void map_iterator_fini(MAP_ITERATOR iterator);

/**
 * determines if the next value for iteration exists. to obtain this value, use `map_iterator_next`
 * 
 * @param iterator the iterator to determine if there is a next value
 * @return nonzero if there is a next value; otherwise zero.
 * @warning the iterator (and any copies) is invalidated the moment the map is modified in anyway or destroyed
 */
int map_iterator_has_next(MAP_ITERATOR iterator);

/**
 * obtains the next value of the iterator and moves the iterator forward
 * 
 * @param iterator the iterator to determine if there is a next value
 * @param key a reference to store the next key in the map from the iterator
 * @param value a reference to a pointer to store the next value in the map from the iterator
 * @warning the iterator (and any copies) is invalidated the moment the map is modified in anyway or destroyed
 */
void map_iterator_next(MAP_ITERATOR iterator, int *key, void **value);

#endif