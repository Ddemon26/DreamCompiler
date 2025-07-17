#include "scope.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
  * @brief Computes the hash of a symbol for a given capacity.
  *
  * This function calculates a hash value for the provided symbol
  * based on the given hash table capacity.
  *
  * @param s The symbol to hash.
  * @param cap The capacity of the hash table.
  * @return The computed hash value.
  */
 static size_t sym_hash(Symbol *s, size_t cap) {
   return ((uintptr_t)s >> 3) & (cap - 1);
 }

/**
  * @brief Initializes a scope with a parent scope.
  *
  * Sets up the initial properties of the scope, including its parent,
  * capacity, length, and entries.
  *
  * @param s The scope to initialize.
  * @param parent The parent scope.
  */
 void scope_init(Scope *s, Scope *parent) {
   s->parent = parent;
   s->cap = 16;
   s->len = 0;
   s->entries = calloc(s->cap, sizeof(Binding));
 }

/**
  * @brief Creates a new child scope and links it to the current scope.
  *
  * @param s The current scope.
  * @return The newly created child scope.
  */
 Scope *scope_push(Scope *s) {
   Scope *child = malloc(sizeof(Scope));
   scope_init(child, s);
   return child;
 }

/**
  * @brief Rehashes the scope to increase its capacity and redistribute entries.
  *
  * Doubles the capacity of the scope's hash table and repositions all existing
  * entries to their new locations based on the updated capacity.
  *
  * @param s The scope to rehash.
  */
 static void scope_rehash(Scope *s) {
   size_t new_cap = s->cap * 2;
   Binding *new_ent = calloc(new_cap, sizeof(Binding));
   for (size_t i = 0; i < s->cap; i++) {
     if (!s->entries[i].sym)
       continue;
     size_t j = sym_hash(s->entries[i].sym, new_cap);
     while (new_ent[j].sym)
       j = (j + 1) & (new_cap - 1);
     new_ent[j] = s->entries[i];
   }
   free(s->entries);
   s->entries = new_ent;
   s->cap = new_cap;
 }

/**
 * @brief Binds a symbol to a declaration in the scope.
 *
 * Associates a given symbol with a declaration in the specified scope.
 * If the symbol already exists, updates its associated declaration.
 * Rehashes the scope if necessary to accommodate the new entry.
 *
 * @param s The scope to bind the symbol in.
 * @param sym The symbol to bind.
 * @param decl The declaration to associate with the symbol.
 */
void scope_bind(Scope *s, Symbol *sym, Decl *decl) {
   if ((s->len + 1) * 2 > s->cap)
     scope_rehash(s);
   size_t i = sym_hash(sym, s->cap);
   while (s->entries[i].sym) {
     if (s->entries[i].sym == sym) {
       s->entries[i].decl = decl;
       return;
     }
     i = (i + 1) & (s->cap - 1);
   }
   s->entries[i].sym = sym;
   s->entries[i].decl = decl;
   s->len++;
 }

/**
 * @brief Looks up a symbol in the scope hierarchy.
 *
 * Searches for a symbol starting from the given scope and traversing
 * up the parent scopes if necessary. Returns the associated declaration
 * if the symbol is found, or NULL otherwise.
 *
 * @param s The scope to start the lookup from.
 * @param sym The symbol to look up.
 * @return The associated declaration, or NULL if not found.
 */
Decl *scope_lookup(Scope *s, Symbol *sym) {
   for (Scope *cur = s; cur; cur = cur->parent) {
     size_t i = sym_hash(sym, cur->cap);
     while (cur->entries[i].sym) {
       if (cur->entries[i].sym == sym)
         return cur->entries[i].decl;
       i = (i + 1) & (cur->cap - 1);
     }
   }
   return NULL;
 }

/**
 * @brief Pops the current scope and returns its parent.
 *
 * Frees the memory allocated for the current scope and its entries,
 * then returns the parent scope.
 *
 * @param s The scope to pop.
 * @return The parent scope.
 */
Scope *scope_pop(Scope *s) {
   Scope *p = s->parent;
   free(s->entries);
   free(s);
   return p;
 }

/**
 * @brief Frees the memory allocated for a scope.
 *
 * Releases the memory used by the entries in the given scope.
 *
 * @param s The scope to free.
 */
void scope_free(Scope *s) { free(s->entries); }