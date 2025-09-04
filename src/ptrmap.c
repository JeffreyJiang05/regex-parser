#include "ptrmap.h"

#include <stdint.h>

#include "debug.h"

#define DEFAULT_MAP_CAPACITY 32

#define MAP_LOADFACTOR 0.5

#define REACHED_THRESHOLD(map) ((size_t) ((map)->capacity * MAP_LOADFACTOR) == (map)->size)

#define TOMBSTONE ((void*) -1)
#define EMPTY ((void*) 0)

#define IS_TOMBSTONE(value) ((value) == TOMBSTONE)
#define IS_EMPTY(value) ((value) == NULL)

#define HAS_PAIR(buf, pos) (!IS_TOMBSTONE((buf)[pos].key) && !IS_EMPTY((buf)[pos].key))

#define KEY(pair) ((pair).key)
#define VALUE(pair) ((pair).value)

#define INSERT_PAIR(buf, pos, key, value) do {  \
    (buf)[pos].key = key;                       \
    (buf)[pos].value = value;                   \
} while(0)

typedef uint64_t hash_t;

typedef union
{ 
    void *ptr;
    uint64_t val;
} CVT;

// random hash function
#define HASH(ptr) ( (CVT){ptr}.val * 67 + 0x123456 );

struct key_value_pair { void *key, *value; };

struct ptr_map
{
    struct key_value_pair *buffer;
    size_t capacity;
    size_t size;
};

struct ptr_map_iterator
{
    PTR_MAP map; 
    size_t idx;
};

PTR_MAP ptrmap_init()
{
    PTR_MAP map = malloc(sizeof(struct ptr_map));
    map->capacity = DEFAULT_MAP_CAPACITY;
    map->size = 0;
    map->buffer = calloc(map->capacity, sizeof(struct key_value_pair));
    info("Map[%p] initialized.", map);
    return map;
}

void ptrmap_fini(PTR_MAP map)
{
    info("Map[%p] destroyed.", map);
    free(map->buffer);
    free(map);
}

size_t ptrmap_size(PTR_MAP map)
{
    return map->size;
}

static struct key_value_pair * ptrmap_find_pair(PTR_MAP map, void *key)
{
    info("Searching for %p in Map[%p].", key, map);
    size_t init_pos, pos;
    hash_t hash = HASH(key);

    pos = init_pos = hash % map->capacity;
    while (1)
    {
        info("\tChecking index %lu which contains [%p : %p].", pos, KEY(map->buffer[pos]), VALUE(map->buffer[pos]));
        // if we run into an empty cell, then the set does not contain key
        if (IS_EMPTY( KEY(map->buffer[pos]) ))
        {
            info("\tKey %p is not in Map[%p] (empty cell).", key, map);
            return NULL;
        }

        // return nonzero if we find the key
        if (HAS_PAIR(map->buffer, pos) && KEY(map->buffer[pos]) == key)
        {
            info("\tKey %p is in Map[%p].", key, map);
            return &map->buffer[pos];
        }

        // we move the position forward and loop to the beginning of the buffer if necessary
        pos = (pos + 1) % map->capacity;

        // if we returned to the initial position, then the key is not in the map
        if (pos == init_pos)
        {
            info("\tKey %p is not in Map[%p] (complete search).", key, map);
            return NULL;
        }
    }
}

int ptrmap_contains_key(PTR_MAP map, void *key)
{
    return ptrmap_find_pair(map, key) != NULL;
}

// it is guaranteed that the buf does not need to be resized, i.e. there is an empty slot
// in the buffer for the element that is being added
static void no_rehash_add_pair(struct key_value_pair *buf, size_t buf_size, void *key, void *value)
{
    hash_t hash = HASH(key);
    size_t pos = hash % buf_size;

    // find a TOMBSTONE or EMPTY
    while (HAS_PAIR(buf, pos))
    {
        ++pos;
        if (pos == buf_size) pos = 0;
    }
    INSERT_PAIR(buf, pos, key, value);
    info("\t[%p : %p] inserted into index %lu.", key, value, pos);
}

static void rehash(PTR_MAP map)
{
    info("Map[%p] rehashed.", map);
    size_t new_capacity;

    // double the capacity
    new_capacity = map->capacity * 2;
    struct key_value_pair *new_buffer = calloc(new_capacity, sizeof(struct key_value_pair));

    for (size_t i = 0; i < map->capacity; ++i)
    {
        if (HAS_PAIR(map->buffer, i))
            no_rehash_add_pair(new_buffer, new_capacity, KEY(map->buffer[i]), VALUE(map->buffer[i]));
    }

    map->capacity = new_capacity;
    free(map->buffer);
    map->buffer = new_buffer;
}

int ptrmap_set(PTR_MAP map, void *key, void *value)
{
    info("Attempting to set pair [%p : %p] in Map[%p].", key, value, map);

    if (key == TOMBSTONE || key == EMPTY)
    {
        info("Failed to set pair [%p : %p] in Map[%p] (invalid key).", key, value, map);
        return -1;
    }

    if (!value)
    {
        info("Failed to set pair [%p : %p] in Map[%p] (null value).", key, value, map)
        return -1;
    }

    struct key_value_pair *pair = ptrmap_find_pair(map, key);
    if (pair) // if the pair is in the set already, we just set the value
    {
        pair->value = value;
        info("Successfully set [%p : %p] in Map[%p].", key, value, map);
        return 0;
    }
    
    if (REACHED_THRESHOLD(map)) rehash(map);
    no_rehash_add_pair(map->buffer, map->capacity, key, value);
    map->size++;
    info("Successfully set [%p : %p] to Map[%p].", key, value, map);
    return 0;
}

void * ptrmap_get(PTR_MAP map, void *key)
{
    info("Attempting to retrieve key %p from Map[%p].", key, map);
    struct key_value_pair *pair = ptrmap_find_pair(map, key);
    if (pair)
    {
        info("\tFound pair [%p : %p] in Map[%p].", KEY(*pair), VALUE(*pair), map);
        return VALUE(*pair);
    }
    else
    {
        info("\tDid not find key %p in Map[%p].", key, map);
        return NULL;
    }
}

PTR_MAP_ITERATOR ptrmap_iterator_init(PTR_MAP map)
{
    PTR_MAP_ITERATOR iter = malloc(sizeof(struct ptr_map_iterator));
    iter->map = map;
    size_t pos = 0;
    while (pos < map->capacity)
    {
        if (HAS_PAIR(map->buffer, pos)) break;
        pos++;
    }
    iter->idx = pos;
    return iter;
}

void ptrmap_iterator_fini(PTR_MAP_ITERATOR iterator)
{
    free(iterator);
}

int ptrmap_iterator_has_next(PTR_MAP_ITERATOR iterator)
{
    return iterator->idx != iterator->map->capacity;
}

void ptrmap_iterator_next(PTR_MAP_ITERATOR iterator, void **key, void **value)
{
    PTR_MAP map = iterator->map;
    
    if (key) *key = KEY(map->buffer[iterator->idx]);
    if (value) *value = VALUE(map->buffer[iterator->idx]);

    size_t pos = iterator->idx + 1;
    while (pos < map->capacity)
    {
        if (HAS_PAIR(map->buffer, pos)) break;
        pos++;
    }
    iterator->idx = pos;
}
