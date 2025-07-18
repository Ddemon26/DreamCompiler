#include "dce.h"
#include <stdlib.h>
/**
 * @brief Performs dead code elimination on a control flow graph.
 *
 * @param cfg Pointer to the control flow graph to optimize.
 */
static int is_used(CFG *cfg, IRValue v) {
  if (v.id < 0)
    return 0;
  for (size_t i = 0; i < cfg->nblocks; i++) {
    BasicBlock *b = cfg->blocks[i];
    for (size_t j = 0; j < b->ninstrs; j++) {
      IRInstr *ins = b->instrs[j];
      if ((ins->a.id == v.id || ins->b.id == v.id) && ins->op != IR_PHI)
        return 1;
    }
  }
  return 0;
}

bool dce(CFG *cfg) {
  if (!cfg)
    return false;
  bool changed = false;
  for (size_t i = 0; i < cfg->nblocks; i++) {
    BasicBlock *b = cfg->blocks[i];
    size_t w = 0;
    for (size_t j = 0; j < b->ninstrs; j++) {
      IRInstr *ins = b->instrs[j];
      switch (ins->op) {
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
      case IR_MOV:
        if (!is_used(cfg, ins->dst)) {
          free(ins);
          changed = true;
          continue;
        }
        break;
      default:
        break;
      }
      b->instrs[w++] = ins;
    }
    b->ninstrs = w;
  }
  return changed;
}
