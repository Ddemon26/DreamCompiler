#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>

typedef enum {
    TY_VAR,
    TY_INT,
    TY_FLOAT,
    TY_BOOL,
    TY_CHAR,
    TY_STRING,
    TY_FUNC,
    TY_ERROR
} TypeKind;

typedef struct Type Type;
struct Type {
    TypeKind kind;
    union {
        struct { Type *param; Type *ret; } func; // TY_FUNC
        struct { unsigned id; Type *instance; } var; // TY_VAR
    } as;
};

Type *type_new(TypeKind kind);
Type *type_new_var(void);
Type *type_func(Type *param, Type *ret);
Type *type_prune(Type *t);
void type_free(Type *t);

#endif
