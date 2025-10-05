#include "parser/ast.h"

#include <stdio.h>
#include <signal.h>

// ------------------------------------------------------------------------ //
// RTTI Functionality                                                       //
// ------------------------------------------------------------------------ //

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

// ------------------------------------------------------------------------ //
// DEFINE THE VTABLES STRUCTURE                                             //
// ------------------------------------------------------------------------ //

#define VTABLE()
#define EXTEND() VTABLE_DECL
#define VTABLE_DECL(tag) PRIMITIVE_CAT(tag, _VTABLE_DECL)
#define FROM ()()

typedef void (*AST_CONSTRUCTOR)(void *_this, va_list args);
typedef void (*AST_DESTRUCTOR)(void *_this);

#define void_VTABLE_DECL void

#define ASTNode_VTABLE_DECL                                 \
struct                                                      \
{                                                           \
    AST_RTTI type;                                          \
    const char *type_name;                                  \
    size_t size;                                            \
    AST_CONSTRUCTOR ctor;                                   \
    AST_DESTRUCTOR dtor;                                    \
    int (*isa)(void *other);                                \
    NFA_COMPONENT (*emit)(void *_this);                     \
    void (*print)(void *_this, int);                        \
}

#define ASTSymbol_VTABLE_DECL                               \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTNode);                            \
    SYMBOL (*get_sym)(void *_this);                         \
}

#define ASTClassSymbol_VTABLE_DECL                          \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTNode);                            \
    CLASS_SYMBOL_TYPE (*get_class_sym)(void *_this);        \
}

#define ASTUnaryOp_VTABLE_DECL                              \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTNode);                            \
    AST_NODE (*get_child)(void *_this);                     \
}

#define ASTBinaryOp_VTABLE_DECL                             \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTNode);                            \
    AST_NODE (*get_left_child)(void *_this);                \
    AST_NODE (*get_right_child)(void *_this);               \
}

#define ASTList_VTABLE_DECL                                 \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTNode);                            \
    size_t (*get_num_of_children)(void *_this);             \
    AST_NODE *(*get_children)(void *_this);                 \
    AST_NODE (*get_nth_child)(void *_this, size_t index);   \
}

#define ASTRange_VTABLE_DECL                                \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTUnaryOp);                         \
    size_t (*get_upper_range)(void *_this);                 \
    size_t (*get_lower_range)(void *_this);                 \
}

#define ASTGroup_VTABLE_DECL                                \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTUnaryOp);                         \
}

#define ASTConcat_VTABLE_DECL                               \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTBinaryOp);                        \
}

#define ASTUnion_VTABLE_DECL                                \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTBinaryOp);                        \
}

#define ASTCharRange_VTABLE_DECL                            \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTBinaryOp);                        \
}

#define ASTCharClass_VTABLE_DECL                            \
struct                                                      \
{                                                           \
    EXTEND VTABLE FROM(ASTList);                            \
}

#define DEFINE_AST_VTABLE_TYPE_HELPER(type, superclass)          \
struct type ## _VTABLE { VTABLE_DECL(type); const struct superclass ## _VTABLE *super; }

#define DEFINE_AST_VTABLE_TYPE(type, superclass)                 \
EVAL(DEFINE_AST_VTABLE_TYPE_HELPER(type, superclass))

DEFINE_AST_VTABLE_TYPE(ASTNode, void);
DEFINE_AST_VTABLE_TYPE(ASTSymbol, ASTNode);
DEFINE_AST_VTABLE_TYPE(ASTClassSymbol, ASTNode);
DEFINE_AST_VTABLE_TYPE(ASTUnaryOp, ASTNode);
DEFINE_AST_VTABLE_TYPE(ASTGroup, ASTUnaryOp);
DEFINE_AST_VTABLE_TYPE(ASTRange, ASTUnaryOp);
DEFINE_AST_VTABLE_TYPE(ASTBinaryOp, ASTNode);
DEFINE_AST_VTABLE_TYPE(ASTConcat, ASTBinaryOp);
DEFINE_AST_VTABLE_TYPE(ASTUnion, ASTBinaryOp);
DEFINE_AST_VTABLE_TYPE(ASTCharRange, ASTBinaryOp);
DEFINE_AST_VTABLE_TYPE(ASTList, ASTNode);
DEFINE_AST_VTABLE_TYPE(ASTCharClass, ASTList);

#define VTABLE_TYPE_OF(type) struct type ## _VTABLE

// ------------------------------------------------------------------------ //
// DECLARE THE DATA IN THE CLASSES                                          //
// ------------------------------------------------------------------------ //

#define DECLARE_VPTR(type) const struct type ## _VTABLE *_vptr
#define DATA()
#define INHERIT() BODY
#define BODY(tag) PRIMITIVE_CAT(tag, _BODY)
#define FROM ()()

#define ASTNode_BODY struct {}

#define ASTSymbol_BODY                  \
struct {                                \
    INHERIT DATA FROM(ASTNode);         \
    SYMBOL sym;                         \
}

#define ASTClassSymbol_BODY             \
struct {                                \
    INHERIT DATA FROM(ASTNode);         \
    CLASS_SYMBOL_TYPE class_sym;        \
}

#define ASTUnaryOp_BODY                 \
struct {                                \
    INHERIT DATA FROM(ASTNode);         \
    AST_NODE child;                     \
}

#define ASTGroup_BODY                   \
struct {                                \
    INHERIT DATA FROM(ASTUnaryOp);      \
}

#define ASTRange_BODY                   \
struct {                                \
    INHERIT DATA FROM(ASTUnaryOp);      \
    size_t min;                         \
    size_t max;                         \
}

#define ASTBinaryOp_BODY                \
struct {                                \
    INHERIT DATA FROM(ASTNode);         \
    AST_NODE left_child;                \
    AST_NODE right_child;               \
}

#define ASTConcat_BODY                  \
struct {                                \
    INHERIT DATA FROM(ASTBinaryOp);     \
}

#define ASTUnion_BODY                   \
struct {                                \
    INHERIT DATA FROM(ASTBinaryOp);     \
}

#define ASTCharRange_BODY               \
struct {                                \
    INHERIT DATA FROM(ASTBinaryOp);     \
}

#define ASTList_BODY                    \
struct {                                \
    INHERIT DATA FROM(ASTNode);         \
    AST_NODE *children;                 \
    size_t num_of_children;             \
}

#define ASTCharClass_BODY               \
struct {                                \
    INHERIT DATA FROM(ASTList);         \
}

#define DEFINE_AST_TYPE_HELPER(class)   \
struct class                            \
{                                       \
    DECLARE_VPTR(class);                \
    BODY(class);                        \
}

// DEFINE THE DATA OF THE TYPES
#define DEFINE_AST_TYPE(class) EVAL(DEFINE_AST_TYPE_HELPER(class))

DEFINE_AST_TYPE(ASTNode);
DEFINE_AST_TYPE(ASTSymbol);
DEFINE_AST_TYPE(ASTClassSymbol);
DEFINE_AST_TYPE(ASTUnaryOp);
DEFINE_AST_TYPE(ASTGroup);
DEFINE_AST_TYPE(ASTRange);
DEFINE_AST_TYPE(ASTBinaryOp);
DEFINE_AST_TYPE(ASTConcat);
DEFINE_AST_TYPE(ASTUnion);
DEFINE_AST_TYPE(ASTCharRange);
DEFINE_AST_TYPE(ASTList);
DEFINE_AST_TYPE(ASTCharClass);

// ------------------------------------------------------------------------ //
// Dynamic type checking                                                    //
// ------------------------------------------------------------------------ //

#define ERROR_CHECK_TYPE(input_object, expected_type)                                   \
do {                                                                                    \
    if (!ast_isa(input_object, expected_type))                                          \
    {                                                                                   \
        const VTABLE_TYPE_OF(ASTNode) *got_vtable = input_object;                       \
        const VTABLE_TYPE_OF(expected_type) *expected_vtable = expected_type;           \
        fprintf(stderr, "[%s:%s:%d] Type Mismatch: Expected type %s but got type %s.",  \
            __FILE__, __FUNCTION__, __LINE__,                                           \
            expected_vtable->type_name, got_vtable->type_name);                         \
        raise(SIGTERM);                                                                 \
    }                                                                                   \
} while (0)

#define WARN_CHECK_TYPE(input_object, expected_type)                                    \
do {                                                                                    \
    if (!ast_isa(input_object, expected_type))                                          \
    {                                                                                   \
        const VTABLE_TYPE_OF(ASTNode) *got_vtable = input_object;                       \
        const VTABLE_TYPE_OF(expected_type) *expected_vtable = expected_type;           \
        fprintf(stderr, "[%s:%s:%d] Type Mismatch: Expected type %s but got type %s.",  \
            __FILE__, __FUNCTION__, __LINE__,                                           \
            expected_vtable->type_name, got_vtable->type_name);                         \
    }                                                                                   \
} while (0)

// ------------------------------------------------------------------------ //
// Helper functions                                                         //
// ------------------------------------------------------------------------ //

// to access the super class
#define super(_this) ((_this)->_vptr->super)

static AST_RTTI ast_get_rtti(void *class_type)
{
    // zero cost abstraction
    VTABLE_DECL(ASTNode) *base = class_type;
    return base->type;
}

#define VPTR_TO(type) \
&type ## _vtable

#define DEFINE_AST_VTABLE(type) \
static const struct type ## _VTABLE type ## _vtable

// DEFINE CLASS HANDLER
#define DEFINE_AST_CLASS_HANDLER(type) \
const struct type ## _VTABLE *type = & type ## _vtable

// ------------------------------------------------------------------------ //
// IMPLEMENTING GENERAL METHODS                                             //
// ------------------------------------------------------------------------ //

void *ast_new(void *_class, ...)
{
    // allocate
    const VTABLE_TYPE_OF(ASTNode) *class = _class;
    size_t obj_size = class->size;
    AST_NODE object = calloc(1, obj_size);
    object->_vptr = class;
    // initialize
    va_list args;
    va_start(args, _class);
    class->ctor(object, args);
    va_end(args);

    return object;
}

void ast_delete(void *node /* AST_NODE */)
{
    const VTABLE_TYPE_OF(ASTNode) *class = node;
    class->dtor(node);
    free(node);
}

int ast_isa(void *node /* AST_NODE */, const void *_class)
{
    const VTABLE_TYPE_OF(ASTNode) *class = _class;
    return class->isa(node);
}

NFA_COMPONENT ast_emit(void *node /* AST_NODE */)
{
    const VTABLE_TYPE_OF(ASTNode) *class = node;
    return class->emit(node);
}

void ast_print(void *node /* AST_NODE */ , int indent)
{
    const VTABLE_TYPE_OF(ASTNode) *class = node;
    class->print(node, indent);
}

SYMBOL ast_get_sym(void *node /* AST_SYMBOL */)
{
    ERROR_CHECK_TYPE(node, ASTSymbol);
    const VTABLE_TYPE_OF(ASTSymbol) *class = node;
    return class->get_sym(node);
}

CLASS_SYMBOL_TYPE ast_get_class_sym(void *node /* AST_CLASS_SYMBOL */)
{
    ERROR_CHECK_TYPE(node, ASTClassSymbol);
    const VTABLE_TYPE_OF(ASTClassSymbol) *class = node;
    return class->get_class_sym(node);
}

AST_NODE ast_get_child(void *node /* AST_UNARY_OP */)
{
    ERROR_CHECK_TYPE(node, ASTUnaryOp);
    const VTABLE_TYPE_OF(ASTUnaryOp) *class = node;
    return class->get_child(node);
}

AST_NODE ast_get_left_child(void *node /* AST_BINARY_OP */)
{
    ERROR_CHECK_TYPE(node, ASTBinaryOp);
    const VTABLE_TYPE_OF(ASTBinaryOp) *class = node;
    return class->get_left_child(node);
}

AST_NODE ast_get_right_child(void *node /* AST_BINARY_OP */)
{
    ERROR_CHECK_TYPE(node, ASTBinaryOp);
    const VTABLE_TYPE_OF(ASTBinaryOp) *class = node;
    return class->get_right_child(node);
}

size_t ast_get_num_of_children(void *node /* AST_LIST */)
{
    ERROR_CHECK_TYPE(node, ASTList);
    const VTABLE_TYPE_OF(ASTList) *class = node;
    return class->get_num_of_children(node);
}

AST_NODE * ast_get_children(void *node /* AST_LIST */)
{
    ERROR_CHECK_TYPE(node, ASTList);
    const VTABLE_TYPE_OF(ASTList) *class = node;
    return class->get_children(node);
}

AST_NODE ast_get_nth_child(void *node /* AST_LIST */, size_t index)
{
    ERROR_CHECK_TYPE(node, ASTList);
    const VTABLE_TYPE_OF(ASTList) *class = node;
    return class->get_nth_child(node, index);
}

size_t ast_get_lower_range(void *node /* AST_RANGE */)
{
    ERROR_CHECK_TYPE(node, ASTRange);
    const VTABLE_TYPE_OF(ASTRange) *class = node;
    return class->get_lower_range(node);
}

size_t ast_get_upper_range(void *node /* AST_RANGE */)
{
    ERROR_CHECK_TYPE(node, ASTRange);
    const VTABLE_TYPE_OF(ASTRange) *class = node;
    return class->get_upper_range(node);
}

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTNode METHODS                                             //
// This is an interface meaning no methods are defined                      //
// ------------------------------------------------------------------------ //

static int ASTNode_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti >= RTTI(ASTNode) && rtti <= RTTI(ASTCharClass);
}

DEFINE_AST_VTABLE(ASTNode) = {
    .type  = RTTI(ASTNode),
    .size  = sizeof(struct ASTNode),
    .ctor  = NULL,
    .dtor  = NULL,
    .emit  = NULL,
    .isa   = ASTNode_isa,
    .print = NULL,
    .super = NULL,
    .type_name = "ASTNode"
};
DEFINE_AST_CLASS_HANDLER(ASTNode);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTSymbol METHODS                                           //
// ------------------------------------------------------------------------ //

// ASTSymbol(SYMBOL sym)
static void ASTSymbol_ctor(void *_this, va_list args)
{  
    AST_SYMBOL this = _this;
    SYMBOL sym = va_arg(args, SYMBOL);
    this->sym = sym;
}

static void ASTSymbol_dtor(void *_this) {}

static NFA_COMPONENT ASTSymbol_emit(void *_this)
{
    //* STUB
    (void) _this;
    return NULL;
}

static int ASTSymbol_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti == RTTI(ASTSymbol);
} 

static SYMBOL ASTSymbol_get_sym(void *_this)
{
    AST_SYMBOL this = _this;
    return this->sym;
}

static void ASTSymbol_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
    return;
}

DEFINE_AST_VTABLE(ASTSymbol) = {
    .type    = RTTI(ASTSymbol),
    .size    = sizeof(struct ASTSymbol),
    .ctor    = ASTSymbol_ctor,
    .dtor    = ASTSymbol_dtor,
    .emit    = ASTSymbol_emit,
    .isa     = ASTSymbol_isa,
    .get_sym = ASTSymbol_get_sym,
    .print   = ASTSymbol_print,
    .super   = VPTR_TO(ASTNode),
    .type_name = "ASTSymbol"
};
DEFINE_AST_CLASS_HANDLER(ASTSymbol);


// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTClassSymbol METHODS                                      //
// ------------------------------------------------------------------------ //

// ASTClassSymbol(SYMBOL sym)
static void ASTClassSymbol_ctor(void *_this, va_list args)
{  
    AST_CLASS_SYMBOL this = _this;
    CLASS_SYMBOL_TYPE class_sym = va_arg(args, CLASS_SYMBOL_TYPE);
    this->class_sym = class_sym;
}

static void ASTClassSymbol_dtor(void *_this) {}

static NFA_COMPONENT ASTClassSymbol_emit(void *_this)
{
    //* STUB
    (void) _this;
    return NULL;
}

static int ASTClassSymbol_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti == RTTI(ASTClassSymbol);
} 

static CLASS_SYMBOL_TYPE ASTClassSymbol_get_class_sym(void *_this)
{
    AST_CLASS_SYMBOL this = _this;
    return this->class_sym;
}

static void ASTClassSymbol_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
    return;
}

DEFINE_AST_VTABLE(ASTClassSymbol) = {
    .type = RTTI(ASTClassSymbol),
    .size = sizeof(struct ASTClassSymbol),
    .ctor = ASTClassSymbol_ctor,
    .dtor = ASTClassSymbol_dtor,
    .emit = ASTClassSymbol_emit,
    .isa  = ASTClassSymbol_isa,
    .get_class_sym = ASTClassSymbol_get_class_sym,
    .super = VPTR_TO(ASTNode),
    .type_name = "ASTClassSymbol"
};
DEFINE_AST_CLASS_HANDLER(ASTClassSymbol);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTUnaryOp METHODS                                          //
// Abstract class, cannot be instantiated                                   //
// ------------------------------------------------------------------------ //

// ASTUnaryOp(ASTNode child)
static void ASTUnaryOp_ctor(void *_this, va_list args)
{
    AST_UNARY_OP this = _this;
    this->child = va_arg(args, AST_NODE);
}

//* NOTE: We may want to destroy the child node in the destructor
static void ASTUnaryOp_dtor(void *_this) {}

static int ASTUnaryOp_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti >= RTTI(ASTUnaryOp) && rtti <= RTTI(ASTRange);
}

static AST_NODE ASTUnaryOp_get_child(void *_this)
{
    AST_UNARY_OP this = _this;
    return this->child;
}

static void ASTUnaryOp_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
}

DEFINE_AST_VTABLE(ASTUnaryOp) = {
    .type = RTTI(ASTUnaryOp),
    .size = sizeof(struct ASTUnaryOp),
    .ctor = ASTUnaryOp_ctor,
    .dtor = ASTUnaryOp_dtor,
    .emit = NULL,
    .isa = ASTUnaryOp_isa,
    .get_child = ASTUnaryOp_get_child,
    .super = VPTR_TO(ASTNode),
    .type_name = "ASTUnaryOp"
};
DEFINE_AST_CLASS_HANDLER(ASTUnaryOp);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTGroup METHODS                                            //
// ------------------------------------------------------------------------ //

static void ASTGroup_ctor(void *_this, va_list args)
{
    AST_GROUP this = _this;
    super(this)->ctor(this, args);
}

static NFA_COMPONENT ASTGroup_emit(void *_this)
{
    //* STUB
    (void) _this;
    return NULL;
}

static int ASTGroup_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti == RTTI(ASTGroup);
}

static void ASTGroup_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
}

DEFINE_AST_VTABLE(ASTGroup) = {
    .type = RTTI(ASTGroup),
    .size = sizeof(struct ASTGroup),
    .ctor = ASTGroup_ctor,
    .dtor = ASTUnaryOp_dtor,
    .emit = ASTGroup_emit,
    .isa = ASTGroup_isa,
    .get_child = ASTUnaryOp_get_child,
    .print = ASTGroup_print,
    .super = VPTR_TO(ASTUnaryOp),
    .type_name = "ASTGroup"
};
DEFINE_AST_CLASS_HANDLER(ASTGroup);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTRange METHODS                                            //
// ------------------------------------------------------------------------ //

// ASTRange(size_t lower, size_t upper)
static void ASTRange_ctor(void *_this, va_list args)
{
    AST_RANGE this = _this; 
    super(this)->ctor(this, args);
    this->min = va_arg(args, size_t);
    this->max = va_arg(args, size_t);
}

static NFA_COMPONENT ASTRange_emit(void *_this)
{
    //* stub
    (void) _this;
    return NULL;
}

static int ASTRange_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti == TYPE_ASTRange;
}

static size_t ASTRange_get_lower_range(void *_this)
{
    AST_RANGE this = _this;
    return this->min;
}

static size_t ASTRange_get_upper_range(void *_this)
{
    AST_RANGE this = _this;
    return this->max;
}

static void ASTRange_print(void *_this, int index)
{
    //* stub
    (void) _this;
    (void) index;
}

DEFINE_AST_VTABLE(ASTRange) = {
    .type = RTTI(ASTRange),
    .size = sizeof(struct ASTRange),
    .ctor = ASTRange_ctor,
    .dtor = ASTUnaryOp_dtor,
    .emit = ASTRange_emit,
    .isa  = ASTRange_isa,
    .get_child = ASTUnaryOp_get_child,
    .get_lower_range = ASTRange_get_lower_range,
    .get_upper_range = ASTRange_get_upper_range,
    .print = ASTRange_print,
    .super = VPTR_TO(ASTUnaryOp),
    .type_name = "ASTRange"
};
DEFINE_AST_CLASS_HANDLER(ASTRange);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTBinaryOp METHODS                                         //
// ------------------------------------------------------------------------ //

// ASTBinaryOp(ASTNode left, ASTNode right)
static void ASTBinaryOp_ctor(void *_this, va_list args)
{
    AST_BINARY_OP this = _this;
    this->left_child = va_arg(args, AST_NODE);
    this->right_child = va_arg(args, AST_NODE);
}

//* NOTE: We may want to destroy the children nodes in the destructor
static void ASTBinaryOp_dtor(void *_this) {}

static NFA_COMPONENT ASTBinaryOp_emit(void *_this)
{
    //* STUB
    (void) _this;
    return NULL;
}

static int ASTBinaryOp_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti >= RTTI(ASTBinaryOp) && rtti <= RTTI(ASTCharRange);
}

static AST_NODE ASTBinaryOp_get_left_child(void *_this)
{
    AST_BINARY_OP this = _this;
    return this->left_child;
}

static AST_NODE ASTBinaryOp_get_right_child(void *_this)
{
    AST_BINARY_OP this = _this;
    return this->right_child;
}

static void ASTBinaryOp_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
}

DEFINE_AST_VTABLE(ASTBinaryOp) = {
    .type = RTTI(ASTBinaryOp),
    .size = sizeof(struct ASTBinaryOp),
    .ctor = ASTBinaryOp_ctor,
    .dtor = ASTBinaryOp_dtor,
    .emit = ASTBinaryOp_emit,
    .isa  = ASTBinaryOp_isa,
    .get_left_child = ASTBinaryOp_get_left_child,
    .get_right_child = ASTBinaryOp_get_right_child,
    .print = ASTBinaryOp_print,
    .super = VPTR_TO(ASTNode),
    .type_name = "ASTBinaryOp"
};
DEFINE_AST_CLASS_HANDLER(ASTBinaryOp);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTConcat METHODS                                           //
// ------------------------------------------------------------------------ //

static NFA_COMPONENT ASTConcat_emit(void *_this)
{
    //* STUB
    (void) _this;
    return NULL;
}

static int ASTConcat_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti == RTTI(ASTConcat);
}

static void ASTConcat_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
}

DEFINE_AST_VTABLE(ASTConcat) = {
    .type = RTTI(ASTConcat),
    .size = sizeof(struct ASTConcat),
    .ctor = ASTBinaryOp_ctor,
    .dtor = ASTBinaryOp_dtor,
    .emit = ASTConcat_emit,
    .isa  = ASTConcat_isa,
    .get_left_child = ASTBinaryOp_get_left_child,
    .get_right_child = ASTBinaryOp_get_right_child,
    .print = ASTConcat_print,
    .super = VPTR_TO(ASTBinaryOp),
    .type_name = "ASTConcat"
};
DEFINE_AST_CLASS_HANDLER(ASTConcat);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTUnion METHODS                                            //
// ------------------------------------------------------------------------ //

static NFA_COMPONENT ASTUnion_emit(void *_this)
{
    //* STUB
    (void) _this;
    return NULL;
}

static int ASTUnion_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti == RTTI(ASTUnion);
}

static void ASTUnion_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
}

DEFINE_AST_VTABLE(ASTUnion) = {
    .type = RTTI(ASTUnion),
    .size = sizeof(struct ASTUnion),
    .ctor = ASTBinaryOp_ctor,
    .dtor = ASTBinaryOp_dtor,
    .emit = ASTUnion_emit,
    .isa  = ASTUnion_isa,
    .get_left_child = ASTBinaryOp_get_left_child,
    .get_right_child = ASTBinaryOp_get_right_child,
    .print = ASTUnion_print,
    .super = VPTR_TO(ASTBinaryOp),
    .type_name = "ASTUnion"
};
DEFINE_AST_CLASS_HANDLER(ASTUnion);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTCharRange METHODS                                        //
// ------------------------------------------------------------------------ //

static NFA_COMPONENT ASTCharRange_emit(void *_this)
{
    //* STUB
    (void) _this;
    return NULL;
}

static int ASTCharRange_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti == RTTI(ASTCharRange);
}

static void ASTCharRange_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
}

DEFINE_AST_VTABLE(ASTCharRange) = {
    .type = RTTI(ASTCharRange),
    .size = sizeof(struct ASTCharRange),
    .ctor = ASTBinaryOp_ctor,
    .dtor = ASTBinaryOp_dtor,
    .emit = ASTCharRange_emit,
    .isa  = ASTCharRange_isa,
    .get_left_child = ASTBinaryOp_get_left_child,
    .get_right_child = ASTBinaryOp_get_right_child,
    .print = ASTCharRange_print,
    .super = VPTR_TO(ASTBinaryOp),
    .type_name = "ASTCharRange"
};
DEFINE_AST_CLASS_HANDLER(ASTCharRange);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTList METHODS                                             //
// ------------------------------------------------------------------------ //

// ASTList(size_t list_len, AST_NODE children[])
static void ASTList_ctor(void *_this, va_list args)
{
    AST_LIST this = _this;
    size_t len = va_arg(args, size_t);
    AST_NODE *children = va_arg(args, AST_NODE *);
    this->num_of_children = len;
    this->children = malloc(len * sizeof(AST_NODE));
    for (size_t i = 0; i < len; ++i)
        this->children[i] = children[i];
}

//* NOTE: We may want to destroy the children nodes in the destructor
static void ASTList_dtor(void *_this)
{
    AST_LIST this = _this;
    free(this->children);
}

static int ASTList_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti >= RTTI(ASTList) && rtti <= RTTI(ASTCharClass);
}

static size_t ASTList_get_num_of_children(void *_this)
{
    AST_LIST this = _this;
    return this->num_of_children;
}

static AST_NODE *ASTList_get_children(void *_this)
{
    AST_LIST this = _this;
    return this->children;
}

static AST_NODE ASTList_get_nth_child(void *_this, size_t index)
{
    AST_LIST this = _this;
    return this->children[index];
}

static void ASTList_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
}

DEFINE_AST_VTABLE(ASTList) = {
    .type = RTTI(ASTList),
    .size = sizeof(struct ASTList),
    .ctor = ASTList_ctor,
    .dtor = ASTList_dtor,
    .emit = NULL,
    .isa  = ASTList_isa,
    .get_num_of_children = ASTList_get_num_of_children,
    .get_children = ASTList_get_children,
    .get_nth_child = ASTList_get_nth_child,
    .print = ASTList_print,
    .super = VPTR_TO(ASTNode),
    .type_name = "ASTList"
};
DEFINE_AST_CLASS_HANDLER(ASTList);

// ------------------------------------------------------------------------ //
// IMPLEMENTING ASTCharClass METHODS                                        //
// ------------------------------------------------------------------------ //

static NFA_COMPONENT ASTCharClass_emit(void *_this)
{
    //* STUB
    (void) _this;
    return NULL;
}

static int ASTCharClass_isa(void *other)
{
    AST_RTTI rtti = ast_get_rtti(other);
    return rtti == RTTI(ASTCharClass);
}

static void ASTCharClass_print(void *_this, int index)
{
    //* STUB
    (void) _this;
    (void) index;
}

DEFINE_AST_VTABLE(ASTCharClass) = {
    .type = RTTI(ASTCharClass),
    .size = sizeof(struct ASTCharClass),
    .ctor = ASTList_ctor,
    .dtor = ASTList_dtor,
    .emit = ASTCharClass_emit,
    .isa  = ASTCharClass_isa,
    .get_num_of_children = ASTList_get_num_of_children,
    .get_children = ASTList_get_children,
    .get_nth_child = ASTList_get_nth_child,
    .print = ASTCharClass_print,
    .super = VPTR_TO(ASTList),
    .type_name = "ASTCharClass"
};
DEFINE_AST_CLASS_HANDLER(ASTCharClass);