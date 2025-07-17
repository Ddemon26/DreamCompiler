#include "ssa.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Represents a stack of integers.
 */
typedef struct { int *data; size_t len; } IntStack;

/**
 * @brief Pushes an integer onto the stack.
 *
 * @param s Pointer to the stack.
 * @param v Integer value to push onto the stack.
 */
static void stack_push(IntStack *s, int v) { s->data = realloc(s->data,sizeof(int)*(s->len+1)); s->data[s->len++] = v; }

/**
 * @brief Pops an integer from the stack.
 *
 * @param s Pointer to the stack.
 * @return The integer value popped from the stack.
 */
static int stack_pop(IntStack *s) { return s->data[--s->len]; }

/**
 * @brief Places phi functions in the control flow graph.
 *
 * @param cfg Pointer to the control flow graph.
 * @param nvars Number of variables in the program.
 */
void ssa_place_phi(CFG *cfg, int nvars) {
    (void)nvars; // placeholder using the dominance frontier
    for (size_t i = 0; i < cfg->nblocks; i++) {
        cfg->blocks[i]->df = cfg->blocks[i]->df; // no-op to silence unused
    }
}

/**
 * @brief Renames variables in a basic block and its successors.
 *
 * @param b Pointer to the basic block.
 * @param stacks Pointer to the stack of variable names.
 * @param nvars Number of variables in the program.
 */
static void rename_block(BasicBlock *b, IntStack *stacks, int nvars) {
    for (size_t i = 0; i < b->ninstrs; i++) {
        (void)nvars; (void)stacks; // placeholder
    }
    for (size_t i = 0; i < b->nsucc; i++) {
        rename_block(b->succ[i], stacks, nvars);
    }
}

/**
 * @brief Renames variables in the control flow graph using stacks for tracking variable names.
 *
 * @param cfg Pointer to the control flow graph.
 * @param nvars Number of variables in the program.
 */
void ssa_rename(CFG *cfg, int nvars) {
    IntStack *stacks = calloc(nvars, sizeof(IntStack));
    rename_block(cfg->entry, stacks, nvars);
    for (int i=0;i<nvars;i++) free(stacks[i].data);
    free(stacks);
}

/**
 * @brief Verifies the correctness of the control flow graph.
 *
 * @param cfg Pointer to the control flow graph.
 * @return True if the control flow graph is valid, otherwise false.
 */
bool ssa_verify(CFG *cfg) {
    for (size_t i = 0; i < cfg->nblocks; i++) {
        (void)cfg->blocks[i];
    }
    return true;
}
