#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct node * NODE;
typedef struct list * LIST;
typedef struct list_iterator * LIST_ITERATOR;

NODE node_init();

void node_fini(NODE node);

NODE node_next(NODE node);

NODE node_prev(NODE node);

void *node_value(NODE node);

void *node_set_value(NODE node, void *new_value);


LIST list_init();

void list_fini(LIST list);

size_t list_size(LIST list);

int list_is_sentinel(LIST list, NODE node);

void list_push_front(LIST list, NODE new_head);

NODE list_pop_front(LIST list);

void list_push_back(LIST list, NODE new_tail);

NODE list_pop_back(LIST list);

void list_add_next_node(LIST list, NODE node, NODE new_next);

void list_add_prev_node(LIST list, NODE node, NODE new_prev);

NODE list_remove_next_node(LIST list, NODE node);

NODE list_remove_prev_node(LIST list, NODE node);

NODE list_remove_node(LIST list, NODE node);

typedef int(*list_search_func)(NODE);
NODE list_find(LIST list, list_search_func func);


LIST_ITERATOR list_iterator_init(LIST list);

void list_iterator_fini(LIST_ITERATOR iter);

int list_iterator_has_next(LIST_ITERATOR iter);

NODE list_iterator_next(LIST_ITERATOR iter);

#endif