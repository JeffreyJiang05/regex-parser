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
                                                          │+ int get_lower_range()   │
                                                          |+ int get_upper_range()   |
                                                          └──────────────────────────┘
 */

// DEFINE THE TYPES IN THE INHERITANCE TREE

typedef struct ASTNode          *AST_NODE;
typedef struct ASTSymbol        *AST_SYMBOL;
typedef struct ASTClassSymbol   *AST_CLASS_SYMBOL;
typedef struct ASTUnaryOp       *AST_UNARY_OP;
typedef struct ASTGroup         *AST_GROUP;
typedef struct ASTRange         *AST_RANGE;
typedef struct ASTBinaryOp      *AST_BINARY_OP;
typedef struct ASTConcat        *AST_CONCAT;
typedef struct ASTUnion         *AST_UNION;
typedef struct ASTCharRange     *AST_CHAR_RANGE;
typedef struct ASTList          *AST_LIST;
typedef struct ASTCharClass     *AST_CHAR_CLASS;

void *ast_new(const void *_class, ...);

void ast_delete(void *_this /* AST_NODE */);

int ast_isa(void *_this /* AST_NODE */, const void *_class);

#define ast_typeof(node) ((node)->_vptr)

NFA_COMPONENT ast_emit(void *_this /* AST_NODE */);

void ast_print(void *_this /* AST_NODE */ , int indent);

SYMBOL ast_get_sym(void *_this /* AST_SYMBOL */);

CLASS_SYMBOL_TYPE ast_get_class_sym(void *_this /* AST_CLASS_SYMBOL */); 

AST_NODE ast_get_child(void *_this /* AST_UNARY_OP */);

AST_NODE ast_get_left_child(void *_this /* AST_BINARY_OP */);

AST_NODE ast_get_right_child(void *_this /* AST_BINARY_OP */);

size_t ast_get_num_of_children(void *_this /* AST_LIST */);

AST_NODE * ast_get_children(void *_this /* AST_LIST */);

AST_NODE ast_get_nth_child(void *_this /* AST_LIST */, size_t index);

int ast_get_lower_range(void *_this /* AST_RANGE */);

int ast_get_upper_range(void *_this /* AST_RANGE */);

// --------------------------------------------------------------------------------- //

#define DECLARE_AST_CLASS_HANDLER(type) \
extern const struct type ## _VTABLE *type;

DECLARE_AST_CLASS_HANDLER(ASTNode);
DECLARE_AST_CLASS_HANDLER(ASTSymbol);
DECLARE_AST_CLASS_HANDLER(ASTClassSymbol);
DECLARE_AST_CLASS_HANDLER(ASTUnaryOp);
DECLARE_AST_CLASS_HANDLER(ASTGroup);
DECLARE_AST_CLASS_HANDLER(ASTRange);
DECLARE_AST_CLASS_HANDLER(ASTBinaryOp);
DECLARE_AST_CLASS_HANDLER(ASTConcat);
DECLARE_AST_CLASS_HANDLER(ASTUnion);
DECLARE_AST_CLASS_HANDLER(ASTCharRange);
DECLARE_AST_CLASS_HANDLER(ASTList);
DECLARE_AST_CLASS_HANDLER(ASTCharClass);

#define TYPEOF_ASTNode AST_NODE
#define TYPEOF_ASTSymbol AST_SYMBOL
#define TYPEOF_ASTClassSymbol AST_CLASS_SYMBOL
#define TYPEOF_ASTUnaryOp AST_UNARY_OP
#define TYPEOF_ASTGroup AST_GROUP
#define TYPEOF_ASTRange AST_RANGE
#define TYPEOF_ASTBinaryOp AST_BINARY_OP
#define TYPEOF_ASTConcat AST_CONCAT
#define TYPEOF_ASTUnion AST_UNION
#define TYPEOF_ASTCharRange AST_CHAR_RANGE
#define TYPEOF_ASTList AST_LIST
#define TYPEOF_ASTCharClass AST_CHAR_CLASS

#define TYPEOF(_class) PRIMITIVE_CAT(TYPEOF_, _class)

#endif