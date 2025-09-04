#include "map.h"

#ifdef DEBUG
    #undef DEBUG
#endif

#include "debug.h"

#include <stdint.h>

// reasonable default set size
#define DEFAULT_MAP_CAPACITY 32

#define MAP_LOADFACTOR 0.5

#define REACHED_THRESHOLD(map) ((size_t) ((map)->capacity * MAP_LOADFACTOR) == (map)->size)

#define TOMBSTONE ((size_t) -1)
#define EMPTY (0xFFFFFFFF00000000ul)

// a tombstone is any value whose upper 32 bits is non-zero
#define IS_TOMBSTONE(value) ( ((size_t) (value)) == TOMBSTONE )

#define IS_EMPTY(value) ( ((size_t) (value)) == EMPTY )

// we do not want to sign extend any integer value, so we cast the integer to an unsigned integer
// next we cast the unsigned integer to a size_t which is will zero extend instead
#define ZERO_EXTEND(int_value) ( (size_t) ((unsigned) int_value) )

#define KEY(pair) ( (int) ((pair).key) )

#define VALUE(pair) ( (pair).value )

#define HAS_PAIR(buf, pos) (!IS_TOMBSTONE((buf)[pos].key) && !IS_EMPTY((buf)[pos].key))

#define INSERT_PAIR(buf, pos, key, value) do {  \
    (buf)[pos].key = ZERO_EXTEND(key);          \
    (buf)[pos].value = value;                   \
} while(0)

#define CLEAR_BUF(buf, buf_size) do { \
    for (size_t i = 0; i < buf_size; ++i) (buf)[i].key = EMPTY; \
} while(0)

typedef uint64_t hash_t;

// random hash function
#define HASH(value) (value * 67 + 0x123456)

struct key_value_pair
{
    size_t key;
    void *value;
};

struct map
{
    struct key_value_pair *buffer;
    size_t capacity;
    size_t size;
};

struct map_iterator
{
    MAP map;
    size_t idx;
};

MAP map_init()
{
    MAP map = malloc(sizeof(struct map));
    map->capacity = DEFAULT_MAP_CAPACITY;
    map->size = 0;
    map->buffer = calloc(map->capacity, sizeof(struct key_value_pair));
    CLEAR_BUF(map->buffer, map->capacity);
    info("Map[%p] initialized.", map);
    return map;
}

void map_fini(MAP map)
{
    info("Map[%p] destroyed.", map);
    free(map->buffer);
    free(map);
}

size_t map_size(MAP map)
{
    return map->size;
}

static struct key_value_pair * map_find_pair(MAP map, int key)
{
    info("Searching for %d in Map[%p].", key, map);
    size_t init_pos, pos;
    hash_t hash = HASH(key);

    pos = init_pos = hash % map->capacity;
    while (1)
    {
        info("\tChecking index %lu which contains [%d : %p].", pos, KEY(map->buffer[pos]), VALUE(map->buffer[pos]));
        // if we run into an empty cell, then the set does not contain key
        if (IS_EMPTY( KEY(map->buffer[pos]) ))
        {
            info("\tKey %d is not in Map[%p] (empty cell).", key, map);
            return NULL;
        }

        // return nonzero if we find the key
        if (HAS_PAIR(map->buffer, pos) && KEY(map->buffer[pos]) == key)
        {
            info("\tKey %d is in Map[%p].", key, map);
            return &map->buffer[pos];
        }

        // we move the position forward and loop to the beginning of the buffer if necessary
        pos = (pos + 1) % map->capacity;

        // if we returned to the initial position, then the key is not in the map
        if (pos == init_pos)
        {
            info("\tKey %d is not in Map[%p] (complete search).", key, map);
            return NULL;
        }
    }
}

int map_contains_key(MAP map, int key)
{
    return map_find_pair(map, key) != NULL;
}

// it is guaranteed that the buf does not need to be resized, i.e. there is an empty slot
// in the buffer for the element that is being added
static void no_rehash_add_pair(struct key_value_pair *buf, size_t buf_size, int key, void *value)
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
    info("\t[%d : %p] inserted into index %lu.", key, value, pos);
}

static void rehash(MAP map)
{
    info("Map[%p] rehashed.", map);
    size_t new_capacity;

    // double the capacity
    new_capacity = map->capacity * 2;
    struct key_value_pair *new_buffer = calloc(new_capacity, sizeof(struct key_value_pair));
    CLEAR_BUF(new_buffer, new_capacity);

    for (size_t i = 0; i < map->capacity; ++i)
    {
        if (HAS_PAIR(map->buffer, i))
            no_rehash_add_pair(new_buffer, new_capacity, KEY(map->buffer[i]), VALUE(map->buffer[i]));
    }

    map->capacity = new_capacity;
    free(map->buffer);
    map->buffer = new_buffer;
}

int map_set(MAP map, int key, void *value)
{
    info("Attempting to set pair [%d : %p] in Map[%p].", key, value, map);

    if (!value)
    {
        info("Failed to set pair [%d : %p] in Map[%p] (null value).", key, value, map)
        return -1;
    }

    struct key_value_pair *pair = map_find_pair(map, key);
    if (pair) // if the pair is in the set already, we just set the value
    {
        pair->value = value;
        info("Successfully set [%d : %p] in Map[%p].", key, value, map);
        return 0;
    }
    
    if (REACHED_THRESHOLD(map)) rehash(map);
    no_rehash_add_pair(map->buffer, map->capacity, key, value);
    map->size++;
    info("Successfully set [%d : %p] to Map[%p].", key, value, map);
    return 0;
}

int map_add(MAP map, int key, void *value)
{
    info("Attempting to add new pair [%d : %p] to Map[%p].", key, value, map);

    if (!value)
    {
        info("Failed to add new pair [%d : %p] in Map[%p] (null value).", key, value, map)
        return -1;
    }

    // we cannot add duplicate keys to the set
    if (map_contains_key(map, key))
    {
        info("Failed to add new pair [%d : %p] to Map[%p] (key already in map).", key, value, map);
        return -1;
    }
    
    if (REACHED_THRESHOLD(map)) rehash(map);
    no_rehash_add_pair(map->buffer, map->capacity, key, value);
    map->size++;
    info("Successfully added [%d : %p] to Map[%p].", key, value, map);
    return 0;
}

void * map_get(MAP map, int key)
{
    info("Attempting to retrieve key %d from Map[%p].", key, map);
    struct key_value_pair *pair = map_find_pair(map, key);
    if (pair)
    {
        info("\tFound pair [%d : %p] in Map[%p].", KEY(*pair), VALUE(*pair), map);
        return VALUE(*pair);
    }
    else
    {
        info("\tDid not find key %d in Map[%p].", key, map);
        return NULL;
    }
}

int map_remove(MAP map, int key)
{
    info("Attempting to remove key %d from Map[%p].", key, map);
    struct key_value_pair *pair = map_find_pair(map, key);
    if (!pair)
    {
        info("\tFailed to remove key %d in Map[%p] (key not found).", key, map);
        return -1;
    }

    info("\tSuccessfully removed [%d : %p] in Map[%p].", KEY(*pair), VALUE(*pair), map);
    pair->key = TOMBSTONE;
    map->size--;
    return 0;
}

void map_clear(MAP map)
{
    info("Clearing Map[%p].", map);
    CLEAR_BUF(map->buffer, map->capacity);
    map->size = 0;
}

int * map_keys(MAP map)
{
    if (!map) return NULL;

    size_t sz = map_size(map);
    int *keys = malloc(sz * sizeof(int));

    size_t i = 0;
    MAP_ITERATOR iter = map_iterator_init(map);
    while (map_iterator_has_next(iter))
        map_iterator_next(iter, &keys[i++], NULL);
    map_iterator_fini(iter);

    return keys;
}

void **map_values(MAP map)
{
    size_t sz = map_size(map);
    void **values = malloc(sz * sizeof(void*));

    size_t i = 0;
    MAP_ITERATOR iter = map_iterator_init(map);
    while (map_iterator_has_next(iter))
        map_iterator_next(iter, NULL, &values[i++]);
    map_iterator_fini(iter);

    return values;
}

MAP_ITERATOR map_iterator_init(MAP map)
{
    MAP_ITERATOR iter = malloc(sizeof(struct map_iterator));
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

void map_iterator_fini(MAP_ITERATOR iterator)
{
    free(iterator);
}

int map_iterator_has_next(MAP_ITERATOR iterator)
{
    return iterator->idx != iterator->map->capacity;
}

void map_iterator_next(MAP_ITERATOR iterator, int *key, void **value)
{
    MAP map = iterator->map;
    
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
