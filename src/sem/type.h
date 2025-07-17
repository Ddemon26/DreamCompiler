#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>

/**
 * @brief Enum representing different kinds of types.
 */
typedef enum {
    TY_VAR,    /**< Type variable. */
    TY_INT,    /**< Integer type. */
    TY_FLOAT,  /**< Floating-point type. */
    TY_BOOL,   /**< Boolean type. */
    TY_CHAR,   /**< Character type. */
    TY_STRING, /**< String type. */
    TY_FUNC,   /**< Function type. */
    TY_ERROR   /**< Error type. */
} TypeKind;

/**
 * @brief Represents a type in the type system.
 */
typedef struct Type Type;
struct Type {
    TypeKind kind; /**< The kind of the type. */
    union {
        struct { Type *param; Type *ret; } func; /**< Function type details. */
        struct { unsigned id; Type *instance; } var; /**< Type variable details. */
    } as;
};

/**
 * @brief Creates a new type of the specified kind.
 *
 * @param kind The kind of the type to create.
 * @return Pointer to the newly created type.
 */
Type *type_new(TypeKind kind);

/**
 * @brief Creates a new type variable.
 *
 * @return Pointer to the newly created type variable.
 */
Type *type_new_var(void);

/**
 * @brief Creates a new function type with the given parameter and return types.
 *
 * @param param Pointer to the parameter type.
 * @param ret Pointer to the return type.
 * @return Pointer to the newly created function type.
 */
Type *type_func(Type *param, Type *ret);

/**
 * @brief Prunes a type to its most specific instance.
 *
 * @param t Pointer to the type to prune.
 * @return Pointer to the pruned type.
 */
Type *type_prune(Type *t);

/**
 * @brief Frees the memory associated with a type.
 *
 * @param t Pointer to the type to free.
 */
void type_free(Type *t);

#endif
