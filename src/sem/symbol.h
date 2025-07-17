#ifndef SYMBOL_H
#define SYMBOL_H

#include <stddef.h>

/**
 * @brief Represents a symbol with a name.
 */
typedef struct Symbol Symbol;

/**
 * @brief Structure defining a symbol with a name.
 */
struct Symbol {
  const char *name; /**< Name of the symbol. */
};

/**
 * @brief Interns a symbol by its name.
 *
 * @param name Name of the symbol to intern.
 * @return Pointer to the interned symbol.
 */
Symbol *sym_intern(const char *name);

#endif
