#include "licm.h"
#include <stdbool.h>
#include <stdlib.h>
/**
 * @brief Checks if a basic block dominates another basic block.
 *
 * @param dom Pointer to the potential dominator basic block.
 * @param b Pointer to the basic block being checked.
 * @return true if `dom` dominates `b`, false otherwise.
 */
static bool dominates(BasicBlock *dom, BasicBlock *b) {
  while (b && b != dom)
    b = b->idom;
  return b == dom;
}

/**
 * @brief Checks if a basic block is part of a loop.
 *
 * @param loop Array of pointers to basic blocks in the loop.
 * @param n Number of basic blocks in the loop.
 * @param b Pointer to the basic block to check.
 * @return true if the basic block is in the loop, false otherwise.
 */
static bool in_loop(BasicBlock **loop, size_t n, BasicBlock *b) {
  for (size_t i = 0; i < n; i++)
    if (loop[i] == b)
      return true;
  return false;
}

/**
 * @brief Collects all basic blocks forming a loop starting from the latch up to
 * the header.
 *
 * This function traverses the loop beginning at the latch block, collecting
 * each reached basic block until completion, then appends the header block to
 * the output.
 *
 * @param header Pointer to the loop header basic block.
 * @param latch Pointer to the loop latch basic block.
 * @param out Address of the pointer that will store the array of basic blocks.
 * @param n Pointer to the variable storing the number of collected basic
 * blocks.
 */
static void collect_loop(BasicBlock *header, BasicBlock *latch,
                         BasicBlock ***out, size_t *n) {
  BasicBlock **stack = NULL;
  size_t sp = 0;
  *out = NULL;
  *n = 0;
  stack = realloc(stack, sizeof(BasicBlock *));
  stack[sp++] = latch;
  while (sp) {
    BasicBlock *x = stack[--sp];
    if (in_loop(*out, *n, x))
      continue;
    *out = realloc(*out, sizeof(BasicBlock *) * (*n + 1));
    (*out)[(*n)++] = x;
    for (size_t i = 0; i < x->npred; i++)
      if (x->pred[i] != header)
        stack = realloc(stack, sizeof(BasicBlock *) * (sp + 1)),
        stack[sp++] = x->pred[i];
  }
  free(stack);
  *out = realloc(*out, sizeof(BasicBlock *) * (*n + 1));
  (*out)[(*n)++] = header;
}

/**
 * @brief Finds the unique preheader of a loop.
 *
 * This function iterates over the predecessors of the loop header and returns
 * the unique predecessor that is not part of the loop. If more than one such
 * predecessor exists, NULL is returned.
 *
 * @param header Pointer to the loop header basic block.
 * @param loop Array of basic blocks that constitute the loop.
 * @param n Number of basic blocks in the loop.
 * @return Pointer to the unique non-loop predecessor, or NULL if not unique.
 */
static BasicBlock *find_preheader(BasicBlock *header, BasicBlock **loop,
                                  size_t n) {
  BasicBlock *pre = NULL;
  for (size_t i = 0; i < header->npred; i++) {
    BasicBlock *p = header->pred[i];
    if (!in_loop(loop, n, p)) {
      if (pre)
        return NULL; // not unique
      pre = p;
    }
  }
  return pre;
}

/**
 * @brief Determines if an instruction is pure.
 *
 * A pure instruction does not have side effects and always produces the same
 * output for the same input.
 *
 * @param ins Pointer to the instruction to check.
 * @return true if the instruction is pure, false otherwise.
 */
static bool is_binop(IROp op) { return op >= IR_ADD && op <= IR_NE; }

static bool is_pure(IRInstr *ins) {
  switch (ins->op) {
  case IR_MOV:
  case IR_ADD:
  case IR_SUB:
  case IR_MUL:
  case IR_DIV:
  case IR_MOD:
  case IR_AND:
  case IR_OR:
  case IR_XOR:
  case IR_SHL:
  case IR_SHR:
  case IR_LT:
  case IR_LE:
  case IR_GT:
  case IR_GE:
  case IR_EQ:
  case IR_NE:
    return true;
  default:
    return false;
  }
}

/**
 * @brief Placeholder function for alias analysis.
 *
 * This function always returns true, indicating that the two instructions
 * may alias.
 *
 * @param a Pointer to the first instruction.
 * @param b Pointer to the second instruction.
 * @return true indicating potential aliasing.
 */
static bool may_alias(IRInstr *a, IRInstr *b) {
  (void)a;
  (void)b;
  return true; // placeholder alias analysis
}

/**
 * @brief Checks if a value is present in a set of integers.
 *
 * @param v The value to check for.
 * @param vals Pointer to the array of integers.
 * @param n The number of elements in the array.
 * @return true if the value is in the set, false otherwise.
 */
static bool value_in_set(int v, int *vals, size_t n) {
  for (size_t i = 0; i < n; i++)
    if (vals[i] == v)
      return true;
  return false;
}

/**
 * @brief Hoists loop-invariant instructions out of a loop to a preheader block.
 *
 * This function identifies instructions within a loop that are loop-invariant
 * and moves them to the preheader block to optimize execution.
 *
 * @param loop Array of pointers to basic blocks in the loop.
 * @param n Number of basic blocks in the loop.
 * @param pre Pointer to the preheader basic block where instructions will be
 * hoisted.
 */
static void hoist_loop(BasicBlock **loop, size_t n, BasicBlock *pre) {
  int *defined = NULL;
  size_t ndef = 0;
  for (size_t i = 0; i < n; i++) {
    BasicBlock *b = loop[i];
    for (size_t j = 0; j < b->ninstrs; j++) {
      IRInstr *ins = b->instrs[j];
      defined = realloc(defined, sizeof(int) * (ndef + 1));
      defined[ndef++] = ins->dst.id;
    }
  }
  int *hoisted = NULL;
  size_t nhoisted = 0;
  for (size_t i = 0; i < n; i++) {
    BasicBlock *b = loop[i];
    for (size_t j = 0; j < b->ninstrs; j++) {
      IRInstr *ins = b->instrs[j];
      if (!is_pure(ins))
        continue;
      if (value_in_set(ins->a.id, defined, ndef) &&
          !value_in_set(ins->a.id, hoisted, nhoisted))
        continue;
      if (value_in_set(ins->b.id, defined, ndef) &&
          !value_in_set(ins->b.id, hoisted, nhoisted))
        continue;
      // alias check for stores would go here
      (void)may_alias;
      pre->instrs =
          realloc(pre->instrs, sizeof(IRInstr *) * (pre->ninstrs + 1));
      pre->instrs[pre->ninstrs++] = ins;
      ins->op = IR_NOP;
      hoisted = realloc(hoisted, sizeof(int) * (nhoisted + 1));
      hoisted[nhoisted++] = ins->dst.id;
    }
  }
  free(defined);
  free(hoisted);
}

/**
 * @brief Performs loop-invariant code motion (LICM) optimization on a control
 * flow graph (CFG).
 *
 * This function identifies loops in the CFG, determines their preheaders, and
 * hoists loop-invariant instructions to the preheader blocks to optimize
 * execution.
 *
 * @param cfg Pointer to the control flow graph to optimize.
 */
void licm(CFG *cfg) {
  if (!cfg)
    return;
  for (size_t i = 0; i < cfg->nblocks; i++) {
    BasicBlock *b = cfg->blocks[i];
    for (size_t s = 0; s < b->nsucc; s++) {
      BasicBlock *t = b->succ[s];
      if (dominates(t, b)) {
        BasicBlock **loop = NULL;
        size_t n = 0;
        collect_loop(t, b, &loop, &n);
        BasicBlock *pre = find_preheader(t, loop, n);
        if (pre)
          hoist_loop(loop, n, pre);
        free(loop);
      }
    }
  }
}
