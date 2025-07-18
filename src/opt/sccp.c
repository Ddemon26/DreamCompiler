/**
 * @file sccp.c
 * @brief Implementation of Sparse Conditional Constant Propagation (SCCP).
 *
 * This file contains a simplified implementation of the SCCP optimization
 * technique for control flow graphs (CFGs).
 */

#include "sccp.h"
#include <stdlib.h>

// Simplified Sparse Conditional Constant Propagation implementation.
typedef enum { VAL_UNDEF, VAL_CONST, VAL_OVERDEF } ValKind;

/**
 * @brief Represents a lattice value in the SCCP algorithm.
 *
 * The lattice value can be undefined, a constant, or overdefined.
 */
typedef struct {
  ValKind kind;
  int value;
} LatticeVal;

/**
 * @brief Performs Sparse Conditional Constant Propagation (SCCP) on a control
 * flow graph (CFG).
 *
 * This function marks all instructions as reachable. A complete implementation
 * would propagate constants and remove unreachable blocks.
 *
 * @param cfg Pointer to the control flow graph to optimize.
 */
static int max_value_id(CFG *cfg) {
  int max = -1;
  for (size_t i = 0; i < cfg->nblocks; i++) {
    BasicBlock *b = cfg->blocks[i];
    for (size_t j = 0; j < b->ninstrs; j++) {
      IRInstr *ins = b->instrs[j];
      if (ins->dst.id > max)
        max = ins->dst.id;
      if (ins->a.id > max)
        max = ins->a.id;
      if (ins->b.id > max)
        max = ins->b.id;
    }
  }
  return max + 1;
}

static int is_binop(IROp op) { return op >= IR_ADD && op <= IR_NE; }

static IRValue fold_bin(IROp op, IRValue a, IRValue b) {
  if (a.id >= 0 || b.id >= 0)
    return (IRValue){0};
  int lhs = -a.id - 1;
  int rhs = -b.id - 1;
  int res = 0;
  switch (op) {
  case IR_ADD:
    res = lhs + rhs;
    break;
  case IR_SUB:
    res = lhs - rhs;
    break;
  case IR_MUL:
    res = lhs * rhs;
    break;
  case IR_DIV:
    if (rhs)
      res = lhs / rhs;
    break;
  case IR_MOD:
    if (rhs)
      res = lhs % rhs;
    break;
  case IR_AND:
    res = lhs & rhs;
    break;
  case IR_OR:
    res = lhs | rhs;
    break;
  case IR_XOR:
    res = lhs ^ rhs;
    break;
  case IR_SHL:
    res = lhs << rhs;
    break;
  case IR_SHR:
    res = lhs >> rhs;
    break;
  case IR_LT:
    res = lhs < rhs;
    break;
  case IR_LE:
    res = lhs <= rhs;
    break;
  case IR_GT:
    res = lhs > rhs;
    break;
  case IR_GE:
    res = lhs >= rhs;
    break;
  case IR_EQ:
    res = lhs == rhs;
    break;
  case IR_NE:
    res = lhs != rhs;
    break;
  default:
    break;
  }
  return (IRValue){.id = -res - 1};
}

bool sccp(CFG *cfg) {
  if (!cfg)
    return false;
  bool changed = false;
  int nvals = max_value_id(cfg);
  if (nvals <= 0)
    return false;
  LatticeVal *vals = calloc((size_t)nvals, sizeof(LatticeVal));
  for (size_t i = 0; i < cfg->nblocks; i++) {
    BasicBlock *b = cfg->blocks[i];
    for (size_t j = 0; j < b->ninstrs; j++) {
      IRInstr *ins = b->instrs[j];
      if (ins->op == IR_MOV && ins->a.id < 0) {
        vals[ins->dst.id].kind = VAL_CONST;
        vals[ins->dst.id].value = -ins->a.id - 1;
      } else if (is_binop(ins->op) && ins->a.id < 0 && ins->b.id < 0) {
        IRValue c = fold_bin(ins->op, ins->a, ins->b);
        vals[ins->dst.id].kind = VAL_CONST;
        vals[ins->dst.id].value = ir_const_value(c);
      } else {
        vals[ins->dst.id].kind = VAL_OVERDEF;
      }
    }
  }
  for (size_t i = 0; i < cfg->nblocks; i++) {
    BasicBlock *b = cfg->blocks[i];
    for (size_t j = 0; j < b->ninstrs; j++) {
      IRInstr *ins = b->instrs[j];
      if (ins->op == IR_MOV && vals[ins->dst.id].kind == VAL_CONST) {
        int newv = -vals[ins->dst.id].value - 1;
        if (ins->a.id != newv) {
          ins->a.id = newv;
          changed = true;
        }
      }
      if (is_binop(ins->op)) {
        if (vals[ins->a.id].kind == VAL_CONST) {
          int newv = -vals[ins->a.id].value - 1;
          if (ins->a.id != newv) {
            ins->a.id = newv;
            changed = true;
          }
        }
        if (vals[ins->b.id].kind == VAL_CONST) {
          int newv = -vals[ins->b.id].value - 1;
          if (ins->b.id != newv) {
            ins->b.id = newv;
            changed = true;
          }
        }
        if (ins->a.id < 0 && ins->b.id < 0) {
          IRValue c = fold_bin(ins->op, ins->a, ins->b);
          ins->op = IR_MOV;
          ins->a = c;
          ins->b.id = 0;
          changed = true;
        }
      }
      if (ins->op == IR_CJUMP && ir_is_const(ins->a)) {
        int cond = ir_const_value(ins->a);
        BasicBlock *taken = cond ? b->succ[0] : b->succ[1];
        BasicBlock *dead = cond ? b->succ[1] : b->succ[0];
        if (b->nsucc == 2) {
          b->succ[0] = taken;
          b->nsucc = 1;
          for (size_t k = 0; k < dead->npred; k++) {
            if (dead->pred[k] == b) {
              for (size_t m = k + 1; m < dead->npred; m++)
                dead->pred[m - 1] = dead->pred[m];
              dead->npred--;
              break;
            }
          }
          ins->op = IR_JUMP;
          changed = true;
        }
      }
    }
  }
  free(vals);
  return changed;
}