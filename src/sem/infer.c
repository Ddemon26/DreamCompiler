#include "infer.h"

/**
 * @brief Checks if a type variable occurs within a given type.
 *
 * @param v Pointer to the type variable to check for.
 * @param t Pointer to the type to search within.
 * @return True if the type variable occurs within the type, false otherwise.
 */
static bool occurs(Type *v, Type *t) {
    t = type_prune(t);
    if (t == v) return true;
    switch (t->kind) {
    case TY_FUNC:
        return occurs(v, t->as.func.param) || occurs(v, t->as.func.ret);
    case TY_VAR:
        if (t->as.var.instance)
            return occurs(v, t->as.var.instance);
        return false;
    default:
        return false;
    }
}

/**
 * @brief Unifies two types, ensuring they are compatible.
 *
 * @param a Pointer to the first type.
 * @param b Pointer to the second type.
 * @return True if the types can be unified, false otherwise.
 */
bool unify(Type *a, Type *b) {
    a = type_prune(a);
    b = type_prune(b);
    if (a->kind == TY_VAR) {
        if (a != b) {
            if (occurs(a, b))
                return false;
            a->as.var.instance = b;
        }
        return true;
    }
    if (b->kind == TY_VAR)
        return unify(b, a);
    if (a->kind != b->kind)
        return false;
    if (a->kind == TY_FUNC) {
        return unify(a->as.func.param, b->as.func.param) &&
               unify(a->as.func.ret, b->as.func.ret);
    }
    return true;
}
