#include <criterion/criterion.h>

#include <stdint.h>

#include "utility/list.h"

typedef union
{
    uint64_t val; 
    void *ptr;
} CVT;

#define PTR(value) ((CVT){(value)}.ptr)
#define INT(ptrval) ((CVT){.ptr=(ptrval)}.val)

Test(node_tests, node_lifetime, .timeout = 5)
{
    NODE node = node_init();
    cr_assert(node != NULL, "expected node_init to return nonnull. Got %p", node);
    node_fini(node);
}

Test(node_tests, node_next_prev_simple_0, .timeout = 5)
{
    NODE node = node_init();
    NODE next = node_next(node);
    NODE prev = node_prev(node);
    cr_assert(next == NULL, "Expected node->next = NULL. node->next = %p", next);
    cr_assert(prev == NULL, "Expected node->prev = NULL. node->prev = %p", prev);
    node_fini(node);
}

Test(node_tests, node_value_simple_0, .timeout = 5)
{
    NODE A = node_init();
    int expected = 67;
    node_set_value(A, PTR(expected));
    int output = INT(node_value(A));
    cr_assert(output == expected, "Expected node_value to return %d. Got %d", expected, output);
    node_fini(A);
}

Test(list_tests, list_lifetime, .timeout = 5)
{
    LIST ls = list_init();
    cr_assert(ls != NULL, "Expected list_init to return nonnull. Got %p", ls);
    list_fini(ls);
}

static NODE make_node(int value)
{
    NODE node = node_init();
    node_set_value(node, PTR(value));
    return node;
}

Test(list_tests, list_push_pop_front, .timeout = 5)
{
    LIST list = list_init();
    int expected0, expected1, expected2;
    NODE node0, node1, node2;
    int output0, output1, output2;

    expected2 = 43;
    list_push_front(list, make_node(expected2));
    expected1 = 12;
    list_push_front(list, make_node(expected1));
    expected0 = 6;
    list_push_front(list, make_node(expected0));

    node0 = list_pop_front(list);
    output0 = INT(node_value(node0));
    cr_assert(output0 == expected0, "Expected list_pop_front to return %d. Got %d", expected0, output0);
    node1 = list_pop_front(list);
    output1 = INT(node_value(node1));
    cr_assert(output1 == expected1, "Expected list_pop_front to return %d. Got %d", expected1, output1);
    node2 = list_pop_front(list);
    output2 = INT(node_value(node2));
    cr_assert(output2 == expected2, "Expected list_pop_front to return %d. Got %d", expected2, output2);

    node_fini(node0);
    node_fini(node1);
    node_fini(node2);

    list_fini(list);
}

Test(list_tests, list_push_pop_back, .timeout = 5)
{
    LIST list = list_init();
    int expected0, expected1, expected2;
    NODE node0, node1, node2;
    int output0, output1, output2;

    expected2 = 43;
    list_push_back(list, make_node(expected2));
    expected1 = 12;
    list_push_back(list, make_node(expected1));
    expected0 = 6;
    list_push_back(list, make_node(expected0));

    node0 = list_pop_back(list);
    output0 = INT(node_value(node0));
    cr_assert(output0 == expected0, "Expected list_pop_back to return %d. Got %d", expected0, output0);
    node1 = list_pop_back(list);
    output1 = INT(node_value(node1));
    cr_assert(output1 == expected1, "Expected list_pop_back to return %d. Got %d", expected1, output1);
    node2 = list_pop_back(list);
    output2 = INT(node_value(node2));
    cr_assert(output2 == expected2, "Expected list_pop_back to return %d. Got %d", expected2, output2);

    node_fini(node0);
    node_fini(node1);
    node_fini(node2);

    list_fini(list);
}

Test(list_tests, list_add_next_node_simple_0, .timeout = 5)
{
    LIST list = list_init();
    NODE n0, n1, inserted;
    n0 = node_init();
    n1 = node_init();
    inserted = node_init();

    list_push_back(list, n0);
    list_push_back(list, n1);

    list_add_next_node(list, n0, inserted);

    cr_assert(node_next(n0) == inserted, "Expected n0->next = inserted. Did not get result");
    cr_assert(node_prev(n1) == inserted, "Expected n1->prev = inserted. Did not get result");
    cr_assert(node_next(inserted) == n1, "Expected inserted->next = n1. Did not get result");
    cr_assert(node_prev(inserted) == n0, "Expected inserted->prev = n0. Did not get result");

    list_fini(list);
}

Test(list_tests, list_add_prev_node_simple_0, .timeout = 5)
{
    LIST list = list_init();
    NODE n0, n1, inserted;
    n0 = node_init();
    n1 = node_init();
    inserted = node_init();

    list_push_back(list, n0);
    list_push_back(list, n1);

    list_add_prev_node(list, n1, inserted);

    cr_assert(node_next(n0) == inserted, "Expected n0->next = inserted. Did not get result");
    cr_assert(node_prev(n1) == inserted, "Expected n1->prev = inserted. Did not get result");
    cr_assert(node_next(inserted) == n1, "Expected inserted->next = n1. Did not get result");
    cr_assert(node_prev(inserted) == n0, "Expected inserted->prev = n0. Did not get result");

    list_fini(list);
}

Test(list_tests, list_remove_next_node_simple_0, .timeout = 5)
{
    LIST list = list_init();
    NODE n0, n1, n2;
    n0 = node_init();
    n1 = node_init();
    n2 = node_init();
    list_push_back(list, n0);
    list_push_back(list, n1);
    list_push_back(list, n2);

    NODE output = list_remove_next_node(list, n0);
    cr_assert(output == n1, "Expected list_remove_next_node to return %p. Got %p", n1, output);

    cr_assert(node_next(n0) == n2, "Expected n0->next == n2. Did not get result");
    cr_assert(node_prev(n2) == n0, "Expected n2->prev == n0. Did not get result");

    node_fini(output);
    list_fini(list);
}

Test(list_tests, list_remove_prev_node_simple_0, .timeout = 5)
{
    LIST list = list_init();
    NODE n0, n1, n2;
    n0 = node_init();
    n1 = node_init();
    n2 = node_init();
    list_push_back(list, n0);
    list_push_back(list, n1);
    list_push_back(list, n2);

    NODE output = list_remove_prev_node(list, n2);
    cr_assert(output == n1, "Expected list_remove_prev_node to return %p. Got %p", n1, output);

    cr_assert(node_next(n0) == n2, "Expected n0->next == n2. Did not get result");
    cr_assert(node_prev(n2) == n0, "Expected n2->prev == n0. Did not get result");

    node_fini(output);
    list_fini(list);
}

Test(list_tests, list_remove_node_simple_0, .timeout = 5)
{
    LIST list = list_init();
    NODE n0, n1, n2;
    n0 = node_init();
    n1 = node_init();
    n2 = node_init();
    list_push_back(list, n0);
    list_push_back(list, n1);
    list_push_back(list, n2);

    NODE output = list_remove_node(list, n1);
    cr_assert(output == n1, "Expected list_remove_node to return %p. Got %p", n1, output);

    cr_assert(node_next(n0) == n2, "Expected n0->next == n2. Did not get result");
    cr_assert(node_prev(n2) == n0, "Expected n2->prev == n0. Did not get result");

    node_fini(output);
    list_fini(list);
}

static int search_func(NODE node)
{
    return INT(node_value(node)) == 5;
}

Test(list_tests, list_find_simple_0, .timeout = 5)
{
    LIST list = list_init();
    
    NODE expected = node_init();
    node_set_value(expected, PTR(5));

    list_push_back(list, make_node(1));
    list_push_back(list, make_node(2));
    list_push_back(list, expected);
    list_push_back(list, make_node(4));
    list_push_back(list, make_node(5));

    NODE output = list_find(list, search_func);
    cr_assert(output == expected, "Expected list_find to return %p. Got %p", expected, output);

    list_fini(list);
}