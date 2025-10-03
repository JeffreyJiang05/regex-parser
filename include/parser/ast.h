#ifndef PARSER_AST_H
#define PARSER_AST_H

#include <stdarg.h>

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
 * 
 * UML DIAGRAM FOR THE CLASS HEIRARCHY
                                                                                 ┌───────────────────────────────┐
                                                                                 │ [I] ASTNode                   │
                                                                                 ├───────────────────────────────┤
                                                                                 │+ NFA_COMPONENT emit()         │
                                                                                 │+ const char *print(int indent)│
                                                                                 └─────────┰──┰──┰──┰──┰─────────┘
                                                                                           ┇  ┇  ┇  ┇  ┗╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍┓
             ┎╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍┚  ┇  ┇  ┗╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍┓                                                          ┇
             ┇                                        ┎╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍╍┚  ┇                  ┌────────────────────────────┸─────────────────────────────┐   ┌────────────────────────┸───────────────────────┐
┌────────────┸──────────────┐  ┌──────────────────────┸──────────────────────┐   ┌───────────────┸──────────────┐   │ [A] ASTBinaryOp                                          │   │ [A] ASTList                                    │
│ [C] ASTSymbol             │  │ [C] ASTClassSymbol                          │   │ [A] ASTUnaryOp               │   ├──────────────────────────────────────────────────────────┤   ├────────────────────────────────────────────────┤
├───────────────────────────┤  ├─────────────────────────────────────────────┤   ├──────────────────────────────┤   │+ AST_BINARY_OP(AST_NODE left_child, AST_NODE right_child)│   │+ AST_LIST(size_t list_len, AST_NODE children[])│
│+ AST_SYMBOL(SYMBOL symbol)│  │+ AST_META_SYMBOL(CLASS_SYMBOL_TYPE meta_sym)│   │+ AST_UNARY_OP(AST_NODE child)│   │+ AST_NODE get_left_child()                               │   │+ size_t get_num_of_children()                  │
│+ SYMBOL get_sym()         │  │+ CLASS_SYMBOL_TYPE get_class_sym()          │   │+ AST_NODE get_child()        │   │+ AST_NODE get_right_child()                              │   │+ AST_NODE *get_children()                      │
└───────────────────────────┘  └─────────────────────────────────────────────┘   └────────────┰──┰──────────────┘   └─────────┰──────────────────┰──────────────────┰──────────┘   │+ AST_NODE get_nth_child(size_t index)          │
                                                                                              ┃  ┃                            ┃                  ┃                  ┃              └───────────────────────┰────────────────────────┘
                                                                        ┎─────────────────────┚  ┃                            ┃                  ┃                  ┃                                      ┃
                                                          ┌─────────────┸────────────┐   ┌───────┸───────┐           ┌────────┸───────┐ ┌────────┸──────┐ ┌─────────┸──────────┐                 ┌─────────┸──────────┐
                                                          │ [C] ASTRange             │   │ [C] ASTGroup  │           │ [C] ASTConcat  │ │ [C] ASTUnion  │ │ [C] ASTCharRange   │                 │ [C] ASTCharClass   |
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

// --------------------------------------------------------------------------------- //
// INTERNAL INFORMATION

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

// DEFINE THE VTABLE TYPES AND OBJECTS 

#define VTABLE()
#define INHERIT() VTABLE_DECL
#define VTABLE_DECL(tag) PRIMITIVE_CAT(tag, _VTABLE_DECL)
#define FROM ()()

// INHERIT DATA FROM

#define void_VTABLE_DECL void

typedef void *(*AST_CONSTRUCTOR)(va_list);
typedef void (*AST_DESTRUCTOR)(void *_this);
#define ASTNode_VTABLE_DECL                                 \
struct                                                      \
{                                                           \
    AST_RTTI type;                                          \
    AST_CONSTRUCTOR ctor;                                   \
    AST_DESTRUCTOR dtor;                                    \
    int (*isa)(void *_this, void *class_type);              \
    NFA_COMPONENT (*emit)(void *_this);                     \
    void (*print)(int);                                     \
}

#define ASTSymbol_VTABLE_DECL                               \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTNode);                           \
    SYMBOL (*get_sym)(void *_this);                         \
}

#define ASTClassSymbol_VTABLE_DECL                          \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTNode);                           \
    CLASS_SYMBOL_TYPE (*get_class_sym)(void *_this);        \
}

#define ASTUnaryOp_VTABLE_DECL                              \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTNode);                           \
    AST_NODE (*get_child)(void *_this);                     \
}

#define ASTBinaryOp_VTABLE_DECL                             \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTNode);                           \
    AST_NODE (*get_left_child)(void *_this);                \
    AST_NODE (*get_right_child)(void *_this);               \
}

#define ASTList_VTABLE_DECL                                 \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTNode);                           \
    size_t (*get_num_of_children)(void *_this);             \
    AST_NODE *(*get_children)(void *_this);                 \
    AST_NODE (*get_nth_child)(void *_this, size_t index);   \
}

#define ASTRange_VTABLE_DECL                                \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTUnaryOp);                        \
    size_t (*get_upper_range)(void *_this);                 \
    size_t (*get_lower_range)(void *_this);                 \
}

#define ASTGroup_VTABLE_DECL                                \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTUnaryOp);                        \
}

#define ASTConcat_VTABLE_DECL                               \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTBinaryOp);                       \
}

#define ASTUnion_VTABLE_DECL                                \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTBinaryOp);                       \
}

#define ASTCharRange_VTABLE_DECL                            \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTBinaryOp);                       \
}

#define ASTCharClass_VTABLE_DECL                            \
struct                                                      \
{                                                           \
    INHERIT VTABLE FROM(ASTList);                           \
}



#define DECLARE_AST_VTABLE_HELPER(type, superclass)  \
struct { VTABLE_DECL(type); VTABLE_DECL(superclass) *super; }

#define AST_VTABLE(type, superclass) EVAL(DECLARE_AST_VTABLE_HELPER(type, superclass))

#define DECLARE_AST_CLASS_HANDLER(type, superclass) \
extern AST_VTABLE(type, superclass) type

DECLARE_AST_CLASS_HANDLER(ASTNode, void);
DECLARE_AST_CLASS_HANDLER(ASTSymbol, ASTNode);
DECLARE_AST_CLASS_HANDLER(ASTClassSymbol, ASTNode);
DECLARE_AST_CLASS_HANDLER(ASTUnaryOp, ASTNode);
DECLARE_AST_CLASS_HANDLER(ASTGroup, ASTUnaryOp);
DECLARE_AST_CLASS_HANDLER(ASTRange, ASTUnaryOp);
DECLARE_AST_CLASS_HANDLER(ASTBinaryOp, ASTNode);
DECLARE_AST_CLASS_HANDLER(ASTConcat, ASTBinaryOp);
DECLARE_AST_CLASS_HANDLER(ASTUnion, ASTBinaryOp);
DECLARE_AST_CLASS_HANDLER(ASTCharRange, ASTBinaryOp);
DECLARE_AST_CLASS_HANDLER(ASTList, ASTNode);
DECLARE_AST_CLASS_HANDLER(ASTCharClass, ASTList);

#undef VTABLE
#undef INHERIT
#undef VTABLE_DECL
#undef FROM

#endif