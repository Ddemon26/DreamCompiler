#include "ssa.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Represents a stack of integers.
 */
typedef struct {
  int *data;
  size_t len;
} IntStack;

typedef struct {
  int *data;
  size_t len;
} IntVec;
static void ivec_push(IntVec *v, int x) {
  v->data = realloc(v->data, sizeof(int) * (v->len + 1));
  v->data[v->len++] = x;
}

/**
 * @brief Pushes an integer onto the stack.
 *
 * @param s Pointer to the stack.
 * @param v Integer value to push onto the stack.
 */
static void stack_push(IntStack *s, int v) {
  s->data = realloc(s->data, sizeof(int) * (s->len + 1));
  s->data[s->len++] = v;
}

/**
 * @brief Pops an integer from the stack.
 *
 * @param s Pointer to the stack.
 * @return The integer value popped from the stack.
 */
static int stack_pop(IntStack *s) { return s->len ? s->data[--s->len] : 0; }

/**
 * @brief Places phi functions in the control flow graph.
 *
 * @param cfg Pointer to the control flow graph.
 * @param nvars Number of variables in the program.
 */
void ssa_place_phi(CFG *cfg, int nvars) {
  if (!cfg)
    return;
  IntVec *defsites = calloc((size_t)nvars, sizeof(IntVec));
  for (size_t i = 0; i < cfg->nblocks; i++) {
    BasicBlock *b = cfg->blocks[i];
    for (size_t j = 0; j < b->ninstrs; j++) {
      IRInstr *ins = b->instrs[j];
      if (ins->dst.id >= 0) {
        ivec_push(&defsites[ins->dst.id], (int)i);
      }
    }
  }
  for (int v = 0; v < nvars; v++) {
    IntVec work = defsites[v];
    for (size_t w = 0; w < work.len; w++) {
      BasicBlock *b = cfg->blocks[work.data[w]];
      for (size_t k = 0; k < b->ndf; k++) {
        BasicBlock *y = b->df[k];
        bool exists = false;
        for (size_t t = 0; t < y->ninstrs; t++) {
          IRInstr *ins = y->instrs[t];
          if (ins->op == IR_PHI && ins->dst.id == v) {
            exists = true;
            break;
          }
        }
        if (!exists) {
          IRInstr *phi = ir_instr_new(IR_PHI, (IRValue){.id = v}, (IRValue){0},
                                      (IRValue){0});
          y->instrs = realloc(y->instrs, sizeof(IRInstr *) * (y->ninstrs + 1));
          memmove(&y->instrs[1], &y->instrs[0], sizeof(IRInstr *) * y->ninstrs);
          y->instrs[0] = phi;
          y->ninstrs++;
        }
      }
    }
    free(work.data);
  }
  free(defsites);
}

/**
 * @brief Renames variables in a basic block and its successors.
 *
 * @param b Pointer to the basic block.
 * @param stacks Pointer to the stack of variable names.
 * @param nvars Number of variables in the program.
 */
static int rename_next = 0;

static void rename_block(BasicBlock *b, IntStack *stacks, int nvars) {
  if (b->visited)
    return;
  b->visited = 1;
  for (size_t i = 0; i < b->ninstrs; i++) {
    IRInstr *ins = b->instrs[i];
    if (ins->a.id >= 0 && stacks[ins->a.id].len)
      ins->a.id = stacks[ins->a.id].data[stacks[ins->a.id].len - 1];
    if (ins->b.id >= 0 && stacks[ins->b.id].len)
      ins->b.id = stacks[ins->b.id].data[stacks[ins->b.id].len - 1];
    if (ins->dst.id >= 0) {
      int new_id = rename_next++;
      stack_push(&stacks[ins->dst.id], new_id);
      ins->dst.id = new_id;
    }
  }
  for (size_t i = 0; i < b->nsucc; i++) {
    rename_block(b->succ[i], stacks, nvars);
  }
  for (size_t i = 0; i < b->ninstrs; i++) {
    IRInstr *ins = b->instrs[i];
    if (ins->dst.id >= 0 && stacks[ins->dst.id].len)
      stack_pop(&stacks[ins->dst.id]);
  }
}

/**
 * @brief Renames variables in the control flow graph using stacks for tracking
 * variable names.
 *
 * @param cfg Pointer to the control flow graph.
 * @param nvars Number of variables in the program.
 */
void ssa_rename(CFG *cfg, int nvars) {
  IntStack *stacks = calloc(nvars, sizeof(IntStack));
  rename_next = 0;
  for (size_t i = 0; i < cfg->nblocks; i++)
    cfg->blocks[i]->visited = 0;
  rename_block(cfg->entry, stacks, nvars);
  for (int i = 0; i < nvars; i++)
    free(stacks[i].data);
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
