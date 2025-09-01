#include "set.h"

#include "debug.h"

#include <stdint.h>
#include <string.h>

// reasonable default set size
#define DEFAULT_SET_CAPACITY 32

#define SET_LOADFACTOR 0.5

#define REACHED_THRESHOLD(set) ((size_t) ((set)->capacity * SET_LOADFACTOR) == (set)->size)

// magic number for the tombstone. should be 0xFFFFFF
#define TOMBSTONE ((void*) -1)
#define HAS_VALUE(buf, pos) ((buf)[pos] != NULL && (buf)[pos] != TOMBSTONE)

typedef uint64_t hash_t;

typedef union {
    void * ptr;
    hash_t val;
} CVT;

// random hash function
#define HASH(ptr) ((CVT){ ptr }.val * 67 + 0x123456)

struct set
{
    void **buffer;
    size_t capacity;
    size_t size; 
};

struct set_iterator
{
    SET set;
    size_t idx;
};


SET set_init()
{
    SET set = malloc(sizeof(struct set));
    set->capacity = DEFAULT_SET_CAPACITY;
    set->size = 0;
    set->buffer = calloc(set->capacity, sizeof(void*));  
    info("Set[%p] initialized.", set);
    return set; 
}

void set_fini(SET set)
{
    info("Set[%p] destroyed.", set);
    free(set->buffer);
    free(set);
}

size_t set_size(SET set)
{
    info("Set[%p] has size %lu.", set, set->size);
    return set->size;
}

int set_contains(SET set, void *ptr)
{
    info("Searching for %p in Set[%p].", ptr, set);
    size_t init_pos, pos;
    hash_t p_hash = HASH(ptr);

    pos = init_pos = p_hash % set->capacity;
    while (1)
    {
        info("\tChecking index %lu which contains %p.", pos, set->buffer[pos]);
        // if we run into a NULL ptr, then the set does not contain the ptr
        if (!set->buffer[pos]) 
        {
            info("\tPointer %p is not in Set[%p].", ptr, set);
            return 0;
        }

        // return nonzero if the ptr is found in the set 
        if (set->buffer[pos] == ptr) 
        {
            info("\tPointer %p is in Set[%p].", ptr, set);
            return 1; 
        }

        // we move the position forward and loop to the beginning of the buffer
        // if necessary
        pos = (pos + 1) % set->capacity; 

        // if we have come back to our initial position, then the ptr is not 
        // in the set
        if (pos == init_pos) 
        {
            info("\tPointer %p is not in Set[%p].", ptr, set);
            return 0;
        }
    }
}

// it is guaranteed that the buf does not need to be resized, i.e. there is
// an empty slot in the buffer for the element that is being added
static void no_rehash_add_elem(void **buf, size_t buf_size, void *elem)
{
    hash_t elem_hash = HASH(elem);
    size_t pos = elem_hash % buf_size;
    
    // find a TOMBSTONE or NULL
    while (HAS_VALUE(buf, pos)) 
    {
        ++pos;
        if (pos == buf_size) pos = 0;
    }
    buf[pos] = elem;
    info("\tPointer %p inserted into index %lu.", elem, pos);
}

// if size reaches the capacity, we need to increase the capacity of the set 
// and rehash its entries
static void rehash(SET set)
{
    info("Set[%p] rehashed.", set);
    size_t new_capacity;

    // double the capacity
    new_capacity = set->capacity * 2;
    void **new_buffer = calloc(new_capacity, sizeof(void*));

    for (size_t i = 0; i < set->capacity; ++i)
    {
        if (HAS_VALUE(set->buffer, i)) 
            no_rehash_add_elem(new_buffer, new_capacity, set->buffer[i]);
    }

    set->capacity = new_capacity;
    free(set->buffer);
    set->buffer = new_buffer;
}

int set_add(SET set, void *ptr)
{
    info("Attempting to add %p to Set[%p].", ptr, set);
    // we cannot add duplicate elements in the set
    if (set_contains(set, ptr)) 
    {
        info("Failed to add %p to Set[%p] (ptr in set already).", ptr, set);
        return -1;
    }

    if (REACHED_THRESHOLD(set)) rehash(set);

    // add the element to the buffer
    no_rehash_add_elem(set->buffer, set->capacity, ptr);
    set->size++;
    info("Successfully added %p to Set[%p].", ptr, set);
    return 0;
}

int set_remove(SET set, void *ptr)
{
    info("Attempting to remove %p to Set[%p].", ptr, set);
    size_t init_pos, pos;
    hash_t p_hash = HASH(ptr);

    pos = init_pos = p_hash % set->capacity;
    while (1)
    {
        // if we run into a NULL ptr, then the set does not contain the ptr
        if (!set->buffer[pos]) 
        {
            info("Failed to remove %p from Set[%p] (ptr not in set).", ptr, set);
            return -1;
        }

        // return nonzero if the ptr is found in the set 
        if (set->buffer[pos] == ptr) 
        {
            // make it a tombstone
            set->buffer[pos] = TOMBSTONE;
            set->size--;
            info("Successfully removed %p from Set[%p].", ptr, set);
            return 0; 
        }

        // we move the position forward and loop to the beginning of the buffer
        // if necessary
        pos = (pos + 1) % set->capacity; 

        // if we have come back to our initial position, then the ptr is not 
        // in the set
        if (pos == init_pos) 
        {
            info("Failed to remove %p from Set[%p] (ptr not in set).", ptr, set);
            return -1;
        }
    }
}

void set_clear(SET set)
{
    // set all the data to zero except the capacity
    memset(set->buffer, 0, set->capacity * sizeof(void*));
    set->size = 0;
    info("Cleared Set[%p].", set);
}

void **set_values(SET set)
{
    void **values = malloc(set->size * sizeof(void*));
    size_t i = 0;
    SET_ITERATOR iter = set_iterator_init(set);
    while (set_iterator_has_next(iter))
        values[i++] = set_iterator_next(iter);
    set_iterator_fini(iter);
    return values;
}

SET set_union(SET A, SET B)
{
    SET set_union = set_init();
    
    for (size_t i = 0; i < A->capacity; ++i)
    {
        if (HAS_VALUE(A->buffer, i))
            set_add(set_union, A->buffer[i]);
    }

    for (size_t i = 0; i < B->capacity; ++i)
    {
        if (HAS_VALUE(B->buffer, i))
            set_add(set_union, B->buffer[i]);
    }

    return set_union;
}

SET set_intersection(SET A, SET B)
{
    SET set_intersection = set_init();

    SET smaller_set = A, larger_set = B;
    if (A->capacity > B->capacity)
    {
        smaller_set = B; 
        larger_set = A;
    }
    
    for (size_t i = 0; i < smaller_set->capacity; ++i)
    {
        if (HAS_VALUE(smaller_set->buffer, i) && 
            set_contains(larger_set, smaller_set->buffer[i]))
                set_add(set_intersection, smaller_set->buffer[i]);
    }

    return set_intersection;
}

SET_ITERATOR set_iterator_init(SET set)
{
    if (!set) return NULL;

    SET_ITERATOR iter = malloc(sizeof(SET_ITERATOR));
    iter->set = set; 
    size_t pos = 0;
    while (pos < set->capacity)
    {
        if (HAS_VALUE(set->buffer, pos)) break;
        pos++;
    }
    iter->idx = pos;
    return iter;
}

void set_iterator_fini(SET_ITERATOR iterator)
{
    free(iterator);
}

int set_iterator_has_next(SET_ITERATOR iterator)
{
    return iterator->idx != iterator->set->capacity;
}

void *set_iterator_next(SET_ITERATOR iterator)
{
    SET set = iterator->set;
    void *data = set->buffer[iterator->idx];

    // find the index of the next elem or the end of the set
    size_t pos = iterator->idx + 1;
    while (pos < set->capacity)
    {
        if (HAS_VALUE(set->buffer, pos)) break;
        ++pos;
    }
    iterator->idx = pos; 

    return data;
}