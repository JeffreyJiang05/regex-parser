#ifndef PARSER_AST_H
#define PARSER_AST_H

#include "macro.h"

#include "parser/lexer.h"
#include "automata/nfa.h"

/**
 * the ast node types here are designed in an object oriented manner.
 * 
 * the inheritance tree is as follows:
 *   
 * ASTNode
 *      ASTSymbol
 *      ASTClassSymbol
 *      ASTUnaryOp
 *           ASTGroup
 *           ASTRange
 *      ASTBinaryOp
 *           ASTConcat
 *           ASTUnion
 *           ASTCharRange
 *      ASTList
 *           ASTCharClass
 */

 /**
  * runtime type information (RTTI) for
  * the types
  */
typedef enum
{
    TYPE_ASTNode,
    TYPE_ASTSymbol,
    TYPE_ASTClassSymbol,
    TYPE_ASTUnaryOp,
    TYPE_ASTGroup,
    TYPE_ASTRange,
    TYPE_ASTBinaryOp,
    TYPE_ASTConcat,
    TYPE_ASTUnion,
    TYPE_ASTCharRange,
    TYPE_ASTList,
    TYPE_ASTCharClass
} AST_RTTI;

#define RTTI(handle) PRIMITIVE_CAT(TYPE_, handle)

/*
UML DIAGRAM FOR THE CLASS HEIRARCHY
                                                                                 ┌───────────────────────────────┐
                                                                                 │ [I] AST_NODE                  │
                                                                                 ├───────────────────────────────┤
                                                                                 │+ NFA_COMPONENT emit()         │
                                                                                 │+ const char *print(int indent)│
                                                                                 └─────────┰──┰──┰──┰──┰─────────┘
                                                                                           ┇  ┇  ┇  ┇  ┗╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍┓
             ┎╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍┚  ┇  ┇  ┗╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍┓                                                          ┇
             ┇                                        ┎╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍┚  ┇                  ┌────────────────────────────┸─────────────────────────────┐   ┌────────────────────────┸───────────────────────┐
┌────────────┸──────────────┐  ┌──────────────────────┸──────────────────────┐   ┌───────────────┸──────────────┐   │ [A] AST_BINARY_OP                                        │   │ [A] AST_LIST                                   │
│ [C] AST_SYMBOL            │  │ [C] AST_CLASS_SYMBOL                        │   │ [A] AST_UNARY_OP             │   ├──────────────────────────────────────────────────────────┤   ├────────────────────────────────────────────────┤
├───────────────────────────┤  ├─────────────────────────────────────────────┤   ├──────────────────────────────┤   │+ AST_BINARY_OP(AST_NODE left_child, AST_NODE right_child)│   │+ AST_LIST(size_t list_len, AST_NODE children[])│
│+ AST_SYMBOL(SYMBOL symbol)│  │+ AST_META_SYMBOL(CLASS_SYMBOL_TYPE meta_sym)│   │+ AST_UNARY_OP(AST_NODE child)│   │+ AST_NODE get_left_child()                               │   │+ size_t get_num_of_children()                  │
│+ SYMBOL get_sym()         │  │+ CLASS_SYMBOL_TYPE get_class_sym()          │   │+ AST_NODE get_child()        │   │+ AST_NODE get_right_child()                              │   │+ AST_NODE *get_children()                      │
└───────────────────────────┘  └─────────────────────────────────────────────┘   └────────────┰──┰──────────────┘   └─────────┰──────────────────┰──────────────────┰──────────┘   │+ AST_NODE get_nth_child(size_t index)          │
                                                                                              ┃  ┃                            ┃                  ┃                  ┃              └───────────────────────┰────────────────────────┘
                                                                        ┎─────────────────────┚  ┃                            ┃                  ┃                  ┃                                      ┃
                                                          ┌─────────────┸────────────┐   ┌───────┸───────┐           ┌────────┸───────┐ ┌────────┸──────┐ ┌─────────┸──────────┐                 ┌─────────┸──────────┐
                                                          │ [C] AST_Range            │   │ [C] AST_GROUP │           │ [C] AST_CONCAT │ │ [C] AST_UNION │ │ [C] AST_CHAR_RANGE │                 │ [C] AST_CHAR_CLASS |
                                                          ├──────────────────────────┤   └───────────────┘           └────────────────┘ └───────────────┘ └────────────────────┘                 └────────────────────┘
                                                          │+ size_t get_lower_range()│
                                                          |+ size_t get_upper_range()|
                                                          └──────────────────────────┘
 */

// DEFINE THE TYPES IN THE INHERITANCE TREE

typedef struct ast_node         *AST_NODE;
typedef struct ast_symbol       *AST_SYMBOL;
typedef struct ast_class_symbol *AST_CLASS_SYMBOL;
typedef struct ast_unary_op     *AST_UNARY_OP;
typedef struct ast_group        *AST_GROUP;
typedef struct ast_range        *AST_RANGE;
typedef struct ast_binary_op    *AST_BINARY_OP;
typedef struct ast_concat       *AST_CONCAT;
typedef struct ast_union        *AST_UNION;
typedef struct ast_char_range   *AST_CHAR_RANGE;
typedef struct ast_list         *AST_LIST;
typedef struct ast_char_class   *AST_CHAR_CLASS;

void *ast_new(void *class_type, ...);

void ast_delete(void *node /* AST_NODE */);

int ast_isa(void *node /* AST_NODE */, void *class_type);

NFA_COMPONENT ast_emit(void *node /* AST_NODE */);

void ast_print(void *node /* AST_NODE */ , int indent);

SYMBOL ast_get_sym(void *node /* AST_SYMBOL */);

CLASS_SYMBOL_TYPE ast_get_class_sym(void *node /* AST_CLASS_SYMBOL */); 

AST_NODE ast_get_child(void *node /* AST_UNARY_OP */);

AST_NODE ast_get_left_child(void *node /* AST_BINARY_OP */);

AST_NODE ast_get_right_child(void *node /* AST_BINARY_OP */);

size_t ast_get_num_of_children(void *node /* AST_LIST */);

size_t ast_get_nth_child(void *node /* AST_LIST */);

size_t ast_get_lower_range(void *node /* AST_RANGE */);

size_t ast_get_upper_range(void *node /* AST_RANGE */);

// DEFINE THE VTABLE TYPES AND OBJECTS 

typedef void *(*AST_CONSTRUCTOR)(...);
typedef void (*AST_DESTRUCTOR)(void *_this);
#define AST_NODE_VTABLE_DECL                                \
struct                                                      \
{                                                           \
    AST_RTTI type;                                          \
    AST_CONSTRUCTOR ctor;                                   \
    AST_DESTRUCTOR dtor;                                    \
    int (*isa)(void *_this, void *class_type);              \
    NFA_COMPONENT (*emit)(void *_this);                     \
    void (*print)(int);                                     \
}
extern AST_NODE_VTABLE_DECL ASTNode;

#define AST_SYMBOL_VTABLE_DECL                              \
struct                                                      \
{                                                           \
    AST_NODE_VTABLE_DECL;                                   \
    AST_NODE_VTABLE_DECL *super;                            \
    SYMBOL (*get_sym)(void *_this);                         \
}
extern AST_SYMBOL_VTABLE_DECL ASTSymbol;

#define AST_CLASS_SYMBOL_VTABLE_DECL                        \
struct                                                      \
{                                                           \
    AST_NODE_VTABLE_DECL;                                   \
    AST_NODE_VTABLE_DECL *super;                            \
    CLASS_SYMBOL_TYPE (*get_class_sym)(void *_this);        \
}
extern AST_CLASS_SYMBOL_VTABLE_DECL ASTClassSymbol;

#define AST_UNARY_OP_VTABLE_DECL                            \
struct                                                      \
{                                                           \
    AST_NODE_VTABLE_DECL;                                   \
    AST_NODE_VTABLE_DECL *super;                            \
    AST_NODE (*get_child)(void *_this);                     \
}
extern AST_UNARY_OP_VTABLE_DECL ASTUnaryOp;

#define AST_BINARY_OP_VTABLE_DECL                           \
struct                                                      \
{                                                           \
    AST_NODE_VTABLE_DECL;                                   \
    AST_NODE_VTABLE_DECL *super;                            \
    AST_NODE (*get_left_child)(void *_this);                \
    AST_NODE (*get_right_child)(void *_this);               \
}
extern AST_BINARY_OP_VTABLE_DECL ASTBinaryOp;

#define AST_LIST_VTABLE_DECL                                \
struct                                                      \
{                                                           \
    AST_NODE_VTABLE_DECL;                                   \
    AST_NODE_VTABLE_DECL *super;                            \
    size_t (*get_num_of_children)(void *_this);             \
    AST_NODE *(*get_children)(void *_this);                 \
    AST_NODE (*get_nth_child)(void *_this, size_t index);   \
}
extern AST_LIST_VTABLE_DECL ASTList;

#define AST_RANGE_VTABLE_DECL                               \
struct                                                      \
{                                                           \
    AST_UNARY_OP_VTABLE_DECL;                               \
    AST_UNARY_OP_VTABLE_DECL *super;                        \
    size_t (*get_upper_range)(void *_this);                 \
    size_t (*get_lower_range)(void *_this);                 \
}
extern AST_RANGE_VTABLE_DECL ASTRange;

#define AST_GROUP_VTABLE_DECL                               \
struct                                                      \
{                                                           \
    AST_UNARY_OP_VTABLE_DECL;                               \
    AST_UNARY_OP_VTABLE_DECL *super;                        \
}
extern AST_GROUP_VTABLE_DECL ASTGroup;

#define AST_CONCAT_VTABLE_DECL                              \
struct                                                      \
{                                                           \
    struct ast_binary_op_vtable;                            \
    struct ast_binary_op_vtable *super;                     \
}
extern AST_CONCAT_VTABLE_DECL ASTConcat;                    

#define AST_UNION_VTABLE_DECL                               \
struct                                                      \
{                                                           \
    struct ast_binary_op_vtable;                            \
    struct ast_binary_op_vtable *super;                     \
}
extern AST_UNION_VTABLE_DECL ASTUnion;

#define AST_CHAR_RANGE_VTABLE_DECL                          \
struct                                                      \
{                                                           \
    struct ast_binary_op_vtable;                            \
    struct ast_binary_op_vtable *super;                     \
}
extern AST_CHAR_RANGE_VTABLE_DECL ASTCharRange;

#define AST_CHAR_CLASS_VTABLE_DECL                          \
struct                                                      \
{                                                           \
    struct ast_binary_op_vtable;                            \
    struct ast_binary_op_vtable *super;                     \
}
extern AST_CHAR_CLASS_VTABLE_DECL ASTCharClass;

#define super(_this) ((_this)->_vptr->super)

#endif