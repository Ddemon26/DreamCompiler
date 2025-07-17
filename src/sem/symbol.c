#include "symbol.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Represents an entry in the symbol table, containing a symbol and its name.
 */
typedef struct {
  Symbol *sym;
  const char *name;
} Entry;

static Entry *table = NULL;
static size_t cap = 0;
static size_t len = 0;

/**
 * Computes the FNV-1a hash for a given string.
 *
 * @param s The input string.
 * @return The 64-bit hash value.
 */
static uint64_t hash_str(const char *s) {
  uint64_t h = 14695981039346656037ull; // FNV-1a 64-bit
  while (*s) {
    h ^= (unsigned char)*s++;
    h *= 1099511628211ull;
  }
  return h;
}

/**
 * Rehashes the symbol table to increase its capacity and redistribute entries.
 */
static void rehash(void) {
  size_t new_cap = cap ? cap * 2 : 1024;
  Entry *new_tab = calloc(new_cap, sizeof(Entry));
  for (size_t i = 0; i < cap; i++) {
    if (!table[i].sym)
      continue;
    uint64_t h = hash_str(table[i].name);
    size_t j = h & (new_cap - 1);
    while (new_tab[j].sym)
      j = (j + 1) & (new_cap - 1);
    new_tab[j] = table[i];
  }
  free(table);
  table = new_tab;
  cap = new_cap;
}

/**
 * Interns a symbol with the given name into the symbol table.
 *
 * @param name The name of the symbol.
 * @return A pointer to the interned Symbol.
 */
Symbol *sym_intern(const char *name) {
  if (cap == 0)
    rehash();
  if ((len + 1) * 2 > cap)
    rehash();

  uint64_t h = hash_str(name);
  size_t i = h & (cap - 1);
  while (table[i].sym) {
    if (strcmp(table[i].name, name) == 0)
      return table[i].sym;
    i = (i + 1) & (cap - 1);
  }
  Symbol *sym = malloc(sizeof(Symbol));
  char *copy = strdup(name);
  sym->name = copy;
  table[i].sym = sym;
  table[i].name = copy;
  len++;
  return sym;
}