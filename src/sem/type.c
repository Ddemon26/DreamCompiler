#include "type.h"
        #include <stdlib.h>

        /**
         * Static variable to generate unique IDs for type variables.
         */
        static unsigned next_id = 0;

        /**
         * Creates a new Type object with the specified kind.
         *
         * @param kind The kind of the type (e.g., TY_VAR, TY_FUNC).
         * @return A pointer to the newly created Type object.
         */
        Type *type_new(TypeKind kind) {
            Type *t = malloc(sizeof(Type));
            t->kind = kind;
            t->as.var.instance = NULL;
            t->as.var.id = 0;
            return t;
        }

        /**
         * Creates a new type variable with a unique ID.
         *
         * @return A pointer to the newly created type variable.
         */
        Type *type_new_var(void) {
            Type *t = type_new(TY_VAR);
            t->as.var.id = ++next_id;
            return t;
        }

        /**
         * Creates a new function type with the specified parameter and return types.
         *
         * @param param The parameter type of the function.
         * @param ret The return type of the function.
         * @return A pointer to the newly created function type.
         */
        Type *type_func(Type *param, Type *ret) {
            Type *t = type_new(TY_FUNC);
            t->as.func.param = param;
            t->as.func.ret = ret;
            return t;
        }

        /**
         * Recursively frees the memory allocated for a Type object and its subtypes.
         *
         * @param t The Type object to free.
         */
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

        /**
         * Frees the memory allocated for a Type object.
         *
         * @param t The Type object to free.
         */
        void type_free(Type *t) { free_rec(t); }

        /**
         * Resolves and returns the most specific instance of a type variable.
         *
         * @param t The Type object to prune.
         * @return The pruned Type object.
         */
        Type *type_prune(Type *t) {
            if (t->kind == TY_VAR && t->as.var.instance) {
                t->as.var.instance = type_prune(t->as.var.instance);
                return t->as.var.instance;
            }
            return t;
        }