#include "parser/ast.h"

#define DECLARE_VPTR() const void *_vptr
#define DATA()
#define INHERIT() BODY
#define BODY(tag) PRIMITIVE_CAT(tag, _BODY)
#define FROM ()()

#define ast_node_BODY struct {}

#define ast_symbol_BODY                 \
struct {                                \
    INHERIT DATA FROM(ast_node);        \
    SYMBOL sym;                         \
}

#define ast_class_symbol_BODY           \
struct {                                \
    INHERIT DATA FROM(ast_node);        \
    CLASS_SYMBOL_TYPE class_sym;        \
}

#define ast_unary_op_BODY               \
struct {                                \
    INHERIT DATA FROM(ast_node);        \
    AST_NODE child;                     \
}

#define ast_group_BODY                  \
struct {                                \
    INHERIT DATA FROM(ast_unary_op);    \
}

#define ast_range_BODY                  \
struct {                                \
    INHERIT DATA FROM(ast_unary_op);    \
    size_t min;                         \
    size_t max;                         \
}

#define ast_binary_op_BODY              \
struct {                                \
    INHERIT DATA FROM(ast_node);        \
    AST_NODE left_child;                \
    AST_NODE right_child;               \
}

#define ast_concat_BODY                 \
struct {                                \
    INHERIT DATA FROM(ast_binary_op);   \
}

#define ast_union_BODY                  \
struct {                                \
    INHERIT DATA FROM(ast_binary_op);   \
}

#define ast_char_range_BODY             \
struct {                                \
    INHERIT DATA FROM(ast_binary_op);   \
}

#define ast_list_BODY                   \
struct {                                \
    INHERIT DATA FROM(ast_node);        \
    AST_NODE *children;                 \
    size_t num_of_children;             \
}

#define ast_char_class_BODY             \
struct {                                \
    INHERIT DATA FROM(ast_list);        \
}

#define DEFINE_AST_TYPE_HELPER(tag)     \
struct tag                              \
{                                       \
    DECLARE_VPTR();                     \
    BODY(tag);                          \
}

#define DEFINE_AST_TYPE(tag) EVAL(DEFINE_AST_TYPE_HELPER(tag))

DEFINE_AST_TYPE(ast_node);
DEFINE_AST_TYPE(ast_symbol);
DEFINE_AST_TYPE(ast_class_symbol);
DEFINE_AST_TYPE(ast_unary_op);
DEFINE_AST_TYPE(ast_group);
DEFINE_AST_TYPE(ast_range);
DEFINE_AST_TYPE(ast_binary_op);
DEFINE_AST_TYPE(ast_concat);
DEFINE_AST_TYPE(ast_union);
DEFINE_AST_TYPE(ast_char_range);
DEFINE_AST_TYPE(ast_list);
DEFINE_AST_TYPE(ast_char_class);

// to access the super class
#define super(_this) ((_this)->_vptr->super)


