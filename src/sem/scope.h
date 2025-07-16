#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"
#include <stddef.h>

typedef struct Decl Decl;

typedef struct {
  Symbol *sym;
  Decl *decl;
} Binding;

typedef struct Scope Scope;

struct Scope {
  Scope *parent;
  size_t cap;
  size_t len;
  Binding *entries;
};

void scope_init(Scope *s, Scope *parent);
Scope *scope_push(Scope *s);
Scope *scope_pop(Scope *s);
void scope_bind(Scope *s, Symbol *sym, Decl *decl);
Decl *scope_lookup(Scope *s, Symbol *sym);
void scope_free(Scope *s);

#endif
