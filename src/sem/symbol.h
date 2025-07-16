#ifndef SYMBOL_H
#define SYMBOL_H

#include <stddef.h>

typedef struct Symbol Symbol;

struct Symbol {
  const char *name;
};

Symbol *sym_intern(const char *name);

#endif
