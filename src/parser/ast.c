#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct ArenaBlock {
  ArenaBlock *next;
  max_align_t data[];
};

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
  a->blocks = NULL;
}

void arena_free(Arena *a) {
  ArenaBlock *block = a->blocks;
  while (block) {
    ArenaBlock *next = block->next;
    free(block);
    block = next;
  }
  arena_init(a);
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
static int arena_grow(Arena *a, size_t min) {
  size_t new_cap = a->cap && a->cap <= SIZE_MAX / 2 ? a->cap * 2 : 4096;
  if (new_cap < min) new_cap = min;
  if (new_cap > SIZE_MAX - sizeof(ArenaBlock)) return 0;
  ArenaBlock *block = malloc(sizeof(ArenaBlock) + new_cap);
  if (!block) return 0;
  block->next = a->blocks;
  a->blocks = block;
  a->ptr = (char *)block->data;
  a->len = 0;
  a->cap = new_cap;
  return 1;
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
void *arena_try_alloc(Arena *a, size_t size) {
  const size_t alignment = _Alignof(max_align_t);
  if (size > SIZE_MAX - (alignment - 1)) return NULL;
  size = (size + alignment - 1) & ~(alignment - 1);
  if ((!a->ptr || size > a->cap - a->len) && !arena_grow(a, size))
    return NULL;
  void *ptr = a->ptr + a->len;
  a->len += size;
  // zero memory for determinism
  memset(ptr, 0, size);
  return ptr;
}

void *arena_alloc(Arena *a, size_t size) {
  void *ptr = arena_try_alloc(a, size);
  if (!ptr) abort();
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
  n->pos = (Pos){0, 0};
  return n;
}
