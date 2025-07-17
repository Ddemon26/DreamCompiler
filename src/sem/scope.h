#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"
#include <stddef.h>

typedef struct Decl Decl;

/**
 * @brief Represents a binding between a symbol and a declaration.
 */
typedef struct {
  Symbol *sym;
  Decl *decl;
} Binding;

/**
 * @brief Represents a scope in the symbol table.
 */
typedef struct Scope Scope;

struct Scope {
  Scope *parent; /**< Pointer to the parent scope. */
  size_t cap;    /**< Capacity of the entries array. */
  size_t len;    /**< Number of entries in the scope. */
  Binding *entries; /**< Array of bindings in the scope. */
};

/**
 * @brief Initializes a scope with a parent scope.
 *
 * @param s Pointer to the scope to initialize.
 * @param parent Pointer to the parent scope.
 */
void scope_init(Scope *s, Scope *parent);

/**
 * @brief Creates a new child scope and pushes it onto the stack.
 *
 * @param s Pointer to the current scope.
 * @return Pointer to the new child scope.
 */
Scope *scope_push(Scope *s);

/**
 * @brief Pops the current scope from the stack.
 *
 * @param s Pointer to the current scope.
 * @return Pointer to the parent scope.
 */
Scope *scope_pop(Scope *s);

/**
 * @brief Binds a symbol to a declaration in the current scope.
 *
 * @param s Pointer to the current scope.
 * @param sym Pointer to the symbol to bind.
 * @param decl Pointer to the declaration to bind.
 */
void scope_bind(Scope *s, Symbol *sym, Decl *decl);

/**
 * @brief Looks up a symbol in the current scope and its ancestors.
 *
 * @param s Pointer to the current scope.
 * @param sym Pointer to the symbol to look up.
 * @return Pointer to the declaration associated with the symbol, or NULL if not found.
 */
Decl *scope_lookup(Scope *s, Symbol *sym);

/**
 * @brief Frees the memory associated with a scope.
 *
 * @param s Pointer to the scope to free.
 */
void scope_free(Scope *s);

#endif
