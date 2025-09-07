#ifndef SET_MAP_H
#define SET_MAP_H

#include "set.h"

typedef struct set_map * SET_MAP;
typedef struct set_map_iterator * SET_MAP_ITERATOR;

SET_MAP setmap_init();

void setmap_fini(SET_MAP map);

size_t setmap_size(SET_MAP map);

int setmap_contains(SET_MAP map, SET key);

int setmap_set(SET_MAP map, SET key, void *value);

void * setmap_get(SET_MAP map, SET key);

SET_MAP_ITERATOR setmap_iterator_init(SET_MAP map);

void setmap_iterator_fini(SET_MAP_ITERATOR iterator);

int setmap_iterator_has_next(SET_MAP_ITERATOR iterator);

void setmap_iterator_next(SET_MAP_ITERATOR iterator, SET *key, void **value);


#endif