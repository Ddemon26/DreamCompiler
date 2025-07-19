#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DrRef {
    struct DrRef *next;
    size_t refcount;
} DrRef;

void *dr_alloc(size_t size);
void dr_retain(void *ptr);
void dr_release(void *ptr);
void dr_release_all(void);

#ifdef __cplusplus
}
#endif
