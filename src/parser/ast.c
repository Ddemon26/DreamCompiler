#include "ast.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initializes the memory arena.
 *
 * This function sets the initial state of the memory arena by
 * resetting its pointer, length, and capacity.
 *
 * @param a Pointer to the memory arena to initialize.
 */
void arena_init(Arena *a) {
    a->ptr = NULL;
    a->len = 0;
    a->cap = 0;
}

/**
 * @brief Expands the memory arena to accommodate additional space.
 *
 * This function increases the capacity of the memory arena to ensure
 * it can hold at least the specified minimum additional size.
 *
 * @param a Pointer to the memory arena to grow.
 * @param min Minimum additional size required.
 */
static void arena_grow(Arena *a, size_t min) {
    size_t new_cap = a->cap ? a->cap * 2 : 4096;
    if (new_cap < a->len + min)
        new_cap = a->len + min;
    a->ptr = realloc(a->ptr, new_cap);
    a->cap = new_cap;
}

/**
 * @brief Allocates memory from the memory arena.
 *
 * This function allocates a block of memory of the specified size
 * from the memory arena. If the arena does not have enough capacity,
 * it grows to accommodate the requested size. The allocated memory
 * is zero-initialized for determinism.
 *
 * @param a Pointer to the memory arena.
 * @param size Size of the memory block to allocate.
 * @return Pointer to the allocated memory block.
 */
void *arena_alloc(Arena *a, size_t size) {
    if (a->len + size > a->cap)
        arena_grow(a, size);
    void *ptr = a->ptr + a->len;
    a->len += size;
    // zero memory for determinism
    memset(ptr, 0, size);
    return ptr;
}

/**
 * @brief Creates a new node in the memory arena.
 *
 * This function allocates memory for a new node of the specified kind
 * from the memory arena and initializes its kind field.
 *
 * @param a Pointer to the memory arena.
 * @param kind The kind of the node to create.
 * @return Pointer to the newly created node.
 */
Node *node_new(Arena *a, NodeKind kind) {
    Node *n = arena_alloc(a, sizeof(Node));
    n->kind = kind;
    return n;
}

