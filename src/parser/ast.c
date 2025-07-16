#include "ast.h"
#include <stdlib.h>
#include <string.h>

void arena_init(Arena *a) {
    a->ptr = NULL;
    a->len = 0;
    a->cap = 0;
}

static void arena_grow(Arena *a, size_t min) {
    size_t new_cap = a->cap ? a->cap * 2 : 4096;
    if (new_cap < a->len + min)
        new_cap = a->len + min;
    a->ptr = realloc(a->ptr, new_cap);
    a->cap = new_cap;
}

void *arena_alloc(Arena *a, size_t size) {
    if (a->len + size > a->cap)
        arena_grow(a, size);
    void *ptr = a->ptr + a->len;
    a->len += size;
    // zero memory for determinism
    memset(ptr, 0, size);
    return ptr;
}

Node *node_new(Arena *a, NodeKind kind) {
    Node *n = arena_alloc(a, sizeof(Node));
    n->kind = kind;
    return n;
}

