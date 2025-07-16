#include "type.h"
#include <stdlib.h>

static unsigned next_id = 0;

Type *type_new(TypeKind kind) {
    Type *t = malloc(sizeof(Type));
    t->kind = kind;
    t->as.var.instance = NULL;
    t->as.var.id = 0;
    return t;
}

Type *type_new_var(void) {
    Type *t = type_new(TY_VAR);
    t->as.var.id = ++next_id;
    return t;
}

Type *type_func(Type *param, Type *ret) {
    Type *t = type_new(TY_FUNC);
    t->as.func.param = param;
    t->as.func.ret = ret;
    return t;
}

static void free_rec(Type *t) {
    if (!t) return;
    if (t->kind == TY_FUNC) {
        free_rec(t->as.func.param);
        free_rec(t->as.func.ret);
    }
    if (t->kind == TY_VAR && t->as.var.instance) {
        free_rec(t->as.var.instance);
    }
    free(t);
}

void type_free(Type *t) { free_rec(t); }

Type *type_prune(Type *t) {
    if (t->kind == TY_VAR && t->as.var.instance) {
        t->as.var.instance = type_prune(t->as.var.instance);
        return t->as.var.instance;
    }
    return t;
}
