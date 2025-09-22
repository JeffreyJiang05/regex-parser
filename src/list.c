#include "utility/list.h"

struct node
{
    struct node *next;
    struct node *prev;
    void *value;
};

NODE node_init()
{
    NODE node = malloc(sizeof(struct node));
    node->next = node->prev = NULL;
    node->value = NULL;
    return node;
}

static NODE sentinel_init()
{
    NODE node = malloc(sizeof(struct node));
    node->next = node->prev = node;
    node->value = NULL;
    return node;
}

void node_fini(NODE node)
{
    free(node);
}

NODE node_next(NODE node)
{
    return node->next;
}

NODE node_prev(NODE node)
{
    return node->prev;
}

void *node_value(NODE node)
{
    return node->value;
}

void *node_set_value(NODE node, void *new_value)
{
    void *old_value = node->value;
    node->value = new_value;
    return old_value;
}

struct list
{
    NODE sentinel;
    size_t size;
};

LIST list_init()
{
    LIST list = malloc(sizeof(struct list));
    list->sentinel = sentinel_init();
    list->size = 0;
    return list;
}

void list_fini(LIST list)
{
    // loop through the whole list to free everything
    LIST_ITERATOR iter = list_iterator_init(list);
    while (list_iterator_has_next(iter))
    {
        NODE node = list_iterator_next(iter);
        node_fini(node);
    }
    list_iterator_fini(iter);

    node_fini(list->sentinel);
    free(list);
}

size_t list_size(LIST list)
{
    return list->size;
}

int list_is_sentinel(LIST list, NODE node)
{
    return node == list->sentinel;
}

void list_push_front(LIST list, NODE new_head)
{
    return list_add_next_node(list, list->sentinel, new_head);
}

NODE list_pop_front(LIST list)
{
    if (list->size == 0) return NULL;
    return list_remove_node(list, list->sentinel->next);
}

void list_push_back(LIST list, NODE new_tail)
{
    return list_add_prev_node(list, list->sentinel, new_tail);
}

NODE list_pop_back(LIST list)
{
    if (list->size == 0) return NULL;
    return list_remove_node(list, list->sentinel->prev);
}

void list_add_next_node(LIST list, NODE node, NODE new_next)
{
    new_next->next = node->next;
    new_next->prev = node;
    node->next = new_next;
    new_next->next->prev = new_next;
    list->size++;
}

void list_add_prev_node(LIST list, NODE node, NODE new_prev)
{
    new_prev->next = node;
    new_prev->prev = node->prev;
    node->prev = new_prev;
    new_prev->prev->next = new_prev;
    list->size++;
}

NODE list_remove_next_node(LIST list, NODE node)
{
    return list_remove_node(list, node->next);
}

NODE list_remove_prev_node(LIST list, NODE node)
{
    return list_remove_node(list, node->prev);
}

NODE list_remove_node(LIST list, NODE node)
{
    if (node == list->sentinel) return NULL;

    NODE prev = node->prev;
    NODE next = node->next;

    prev->next = next;
    next->prev = prev;

    node->next = node->prev = NULL;
    list->size--;
    return node;
}

NODE list_find(LIST list, list_search_func func)
{
    if (!list || !func) return NULL;
    LIST_ITERATOR iter = list_iterator_init(list);
    while (list_iterator_has_next(iter))
    {
        NODE node = list_iterator_next(iter);
        if (func(node))
        {
            list_iterator_fini(iter);
            return node;
        }
    }
    list_iterator_fini(iter);
    return NULL;
}

struct list_iterator
{
    NODE sentinel;
    NODE current;
};

LIST_ITERATOR list_iterator_init(LIST list)
{
    LIST_ITERATOR iter = malloc(sizeof(struct list_iterator));
    iter->sentinel = list->sentinel;
    iter->current = list->sentinel->next;
    return iter;
}

void list_iterator_fini(LIST_ITERATOR iter)
{
    free(iter);
}

int list_iterator_has_next(LIST_ITERATOR iter)
{
    return iter->sentinel != iter->current;
}

NODE list_iterator_next(LIST_ITERATOR iter)
{
    NODE node = iter->current;
    iter->current = iter->current->next;
    return node;
}