#include "scope.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static size_t sym_hash(Symbol *s, size_t cap) {
  return ((uintptr_t)s >> 3) & (cap - 1);
}

void scope_init(Scope *s, Scope *parent) {
  s->parent = parent;
  s->cap = 16;
  s->len = 0;
  s->entries = calloc(s->cap, sizeof(Binding));
}

Scope *scope_push(Scope *s) {
  Scope *child = malloc(sizeof(Scope));
  scope_init(child, s);
  return child;
}

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

Scope *scope_pop(Scope *s) {
  Scope *p = s->parent;
  free(s->entries);
  free(s);
  return p;
}

void scope_free(Scope *s) { free(s->entries); }
