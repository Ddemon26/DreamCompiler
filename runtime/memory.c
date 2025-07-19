#include "memory.h"
#include <stdlib.h>

static DrRef *dr_head = NULL;

void *dr_alloc(size_t size) {
    DrRef *r = calloc(1, sizeof(DrRef) + size);
    if (!r) return NULL;
    r->refcount = 1;
    r->next = dr_head;
    dr_head = r;
    return (void *)(r + 1);
}

static inline DrRef *to_ref(void *ptr) {
    return ((DrRef *)ptr) - 1;
}

void dr_retain(void *ptr) {
    if (!ptr) return;
    to_ref(ptr)->refcount++;
}

void dr_release(void *ptr) {
    if (!ptr) return;
    DrRef *r = to_ref(ptr);
    if (r->refcount > 0 && --r->refcount == 0) {
        DrRef **cur = &dr_head;
        while (*cur && *cur != r) cur = &(*cur)->next;
        if (*cur) *cur = r->next;
        free(r);
    }
}

void dr_release_all(void) {
    DrRef *cur = dr_head;
    while (cur) {
        DrRef *next = cur->next;
        free(cur);
        cur = next;
    }
    dr_head = NULL;
}
