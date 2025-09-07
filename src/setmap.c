#include "utility/setmap.h"

#include <stdint.h>

#include "debug.h"

#define DEFAULT_MAP_CAPACITY 16

#define MAP_LOADFACTOR 0.5

#define REACHED_THRESHOLD(map) ((size_t) ((map)->capacity * MAP_LOADFACTOR) == (map)->size)

#define TOMBSTONE ((void*) -1)
#define EMPTY ((void*) 0)

#define IS_TOMBSTONE(value) ((void*) (value) == TOMBSTONE)
#define IS_EMPTY(value) ((value) == NULL)

#define HAS_PAIR(buf, pos) (!IS_TOMBSTONE((buf)[pos].key) && !IS_EMPTY((buf)[pos].key))

#define KEY(pair) ((pair).key)
#define VALUE(pair) ((pair).value)

#define INSERT_PAIR(buf, pos, key, value) do {  \
    (buf)[pos].key = key;                       \
    (buf)[pos].value = value;                   \
} while(0)

// random hash function
#define HASH(value) ( value * 67 + 0x123456 );

typedef uint64_t hash_t;

struct key_value_pair
{
    SET key;
    void *value;
};

struct set_map
{
    struct key_value_pair *buffer;
    size_t capacity;
    size_t size;
};

struct set_map_iterator
{
    SET_MAP map;
    size_t idx;
};

typedef union
{ 
    void *ptr;
    uint64_t val;
} CVT;
#define INT(ptr) ((CVT){ptr}.val)

hash_t set_hash(SET set)
{
    hash_t hashes = 0;
    void *ptr;
    SET_ITERATOR iter = set_iterator_init(set);
    while (set_iterator_has_next(iter))
    {
        ptr = set_iterator_next(iter);
        hashes += INT(ptr);
    }
    set_iterator_fini(iter);
    return hashes;
}

SET_MAP setmap_init()
{
    SET_MAP map = malloc(sizeof(struct set_map));
    map->capacity = DEFAULT_MAP_CAPACITY;
    map->size = 0;
    map->buffer = calloc(map->capacity, sizeof(struct key_value_pair));
    return map;
}

void setmap_fini(SET_MAP map)
{
    free(map->buffer);
    free(map);
}

size_t setmap_size(SET_MAP map)
{
    return map->size;
}

static int set_is_equal(SET a, SET b)
{
    return set_size(a) == set_size(b) && is_subset(a, b);
}

static struct key_value_pair * setmap_find_pair(SET_MAP map, SET key)
{
    info("Searching for SET[%p] in SETMAP[%p].", key, map);
    size_t init_pos, pos;
    hash_t hash = HASH(set_hash(key));

    pos = init_pos = hash % map->capacity;
    while (1)
    {
        info("\tChecking index %lu which contains key SET[%p] and value %p.", pos, KEY(map->buffer[pos]), VALUE(map->buffer[pos]));
        // if we run into an empty cell, then the set does not contain key
        if (IS_EMPTY( KEY(map->buffer[pos]) ))
        {
            info("\tKey SET[%p] is not in SETMAP[%p] (empty cell).", key, map);
            return NULL;
        }

        // return nonzero if we find the key
        if (HAS_PAIR(map->buffer, pos) && set_is_equal(KEY(map->buffer[pos]), key))
        {
            info("\tKey SET[%p] is in SETMAP[%p].", key, map);
            return &map->buffer[pos];
        }

        // we move the position forward and loop to the beginning of the buffer if necessary
        pos = (pos + 1) % map->capacity;

        // if we returned to the initial position, then the key is not in the map
        if (pos == init_pos)
        {
            info("\tKey SET[%p] is not in SETMAP[%p] (complete search).", key, map);
            return NULL;
        }
    }
}

// it is guaranteed that the buf does not need to be resized, i.e. there is an empty slot
// in the buffer for the element that is being added
static void no_rehash_add_pair(struct key_value_pair *buf, size_t buf_size, SET key, void *value)
{
    hash_t hash = HASH(set_hash(key));
    size_t pos = hash % buf_size;

    // find a TOMBSTONE or EMPTY
    while (HAS_PAIR(buf, pos))
    {
        ++pos;
        if (pos == buf_size) pos = 0;
    }
    INSERT_PAIR(buf, pos, key, value);
    info("\tKey SET[%p] and value %p inserted into index %lu.", key, value, pos);
}

int setmap_contains(SET_MAP map, SET key)
{
    return setmap_find_pair(map, key) != NULL;
}

static void rehash(SET_MAP map)
{
    info("SETMAP[%p] rehashed.", map);
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

int setmap_set(SET_MAP map, SET key, void *value)
{
    info("Attempting to set pair SET[%p] : %p in SETMAP[%p].", key, value, map);

    if (key == TOMBSTONE || key == EMPTY)
    {
        info("Failed to set pair key SET[%p] and value %p in SETMAP[%p] (invalid key).", key, value, map);
        return -1;
    }

    struct key_value_pair *pair = setmap_find_pair(map, key);
    if (pair)
    {
        pair->value = value;
        info("Successfully set key SET[%p] to value %p in SETMAP[%p].", key, value, map);
        return 0;
    }

    if (REACHED_THRESHOLD(map)) rehash(map);
    no_rehash_add_pair(map->buffer, map->capacity, key, value);
    map->size++;
    info("Successfully set key SET[%p] and value %p to SETMAP[%p].", key, value, map);
    return 0;
}

void * setmap_get(SET_MAP map, SET key)
{
    info("Attempting to retrive key SET[%p] from SETMAP[%p].", key, map);
    struct key_value_pair *pair = setmap_find_pair(map, key);
    if (pair)
    {
        info("\tFound key SET[%p] in SETMAP[%p].", key, map);
        return VALUE(*pair);
    }
    else
    {
        info("\tDid not find key SET[%p] in SETMAP[%p].", key, map);
        return NULL;
    }
}


SET_MAP_ITERATOR setmap_iterator_init(SET_MAP map)
{
    SET_MAP_ITERATOR iter = malloc(sizeof(struct set_map_iterator));
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

void setmap_iterator_fini(SET_MAP_ITERATOR iterator)
{
    free(iterator);
}

int setmap_iterator_has_next(SET_MAP_ITERATOR iterator)
{
    return iterator->idx != iterator->map->capacity;
}

void setmap_iterator_next(SET_MAP_ITERATOR iterator, SET *key, void **value)
{
    SET_MAP map = iterator->map;
    
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