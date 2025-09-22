#include "utility/partition.h"

#include "utility/list.h"
#include "utility/ptrmap.h"

struct partition
{
    SET original_set;
    // sentinel doubly linked list of sets in the partition
    LIST set_list;
    // MAP<ELEM : NODE of LIST>
    PTR_MAP elem_set_map;
};

static NODE make_set_node()
{
    NODE node = node_init();
    SET node_set = set_init();
    node_set_value(node, node_set);
    return node;
}

static void destroy_set_node_internals(NODE node)
{
    SET node_set = node_value(node);
    set_fini(node_set);
}

static void add_to_set_node(NODE node, void *data)
{
    SET node_set = node_value(node);
    set_add(node_set, data);
}

PARTITION partition_init(void **data, size_t sz)
{
    if (!data || !sz) return NULL;
    PARTITION partition = malloc(sizeof(struct partition));
    NODE initial_partition = make_set_node();
    partition->original_set = set_init();
    partition->set_list = list_init();
    list_push_front(partition->set_list, initial_partition);
    partition->elem_set_map = ptrmap_init();
    for (size_t i = 0; i < sz; ++i)
    {
        set_add(partition->original_set, data[i]);
        add_to_set_node(initial_partition, data[i]);
        ptrmap_set(partition->elem_set_map, data[i], initial_partition);
    }
    return partition;
}

void partition_fini(PARTITION partition)
{
    NODE set_node;   
    LIST_ITERATOR iter = list_iterator_init(partition->set_list);
    while (list_iterator_has_next(iter))
    {
        set_node = list_iterator_next(iter);
        destroy_set_node_internals(set_node);
    }
    list_iterator_fini(iter);

    set_fini(partition->original_set);
    list_fini(partition->set_list);
    ptrmap_fini(partition->elem_set_map);
}

SET *partition_sets(PARTITION partition)
{
    return NULL;
}

size_t partition_count_sets(PARTITION partition)
{
    return -1;
}