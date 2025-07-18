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
 * @brief Performs Sparse Conditional Constant Propagation (SCCP) on a control flow graph (CFG).
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

static IRValue fold_bin(IROp op, IRValue a, IRValue b) {
    if (a.id >= 0 || b.id >= 0)
        return (IRValue){0};
    int lhs = -a.id - 1;
    int rhs = -b.id - 1;
    int res = 0;
    switch (op) {
    case IR_BIN:
        res = lhs + rhs;
        break;
    default:
        break;
    }
    return (IRValue){ .id = -res - 1 };
}

void sccp(CFG *cfg) {
    if (!cfg)
        return;
    int nvals = max_value_id(cfg);
    if (nvals <= 0)
        return;
    LatticeVal *vals = calloc((size_t)nvals, sizeof(LatticeVal));
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *b = cfg->blocks[i];
        for (size_t j = 0; j < b->ninstrs; j++) {
            IRInstr *ins = b->instrs[j];
            if (ins->op == IR_MOV && ins->a.id < 0) {
                vals[ins->dst.id].kind = VAL_CONST;
                vals[ins->dst.id].value = -ins->a.id - 1;
            } else if (ins->op == IR_BIN && ins->a.id < 0 && ins->b.id < 0) {
                int lhs = -ins->a.id - 1;
                int rhs = -ins->b.id - 1;
                vals[ins->dst.id].kind = VAL_CONST;
                vals[ins->dst.id].value = lhs + rhs;
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
                ins->a.id = -vals[ins->dst.id].value - 1;
            }
            if (ins->op == IR_BIN) {
                if (vals[ins->a.id].kind == VAL_CONST)
                    ins->a.id = -vals[ins->a.id].value - 1;
                if (vals[ins->b.id].kind == VAL_CONST)
                    ins->b.id = -vals[ins->b.id].value - 1;
                if (ins->a.id < 0 && ins->b.id < 0) {
                    IRValue c = fold_bin(ins->op, ins->a, ins->b);
                    ins->op = IR_MOV;
                    ins->a = c;
                    ins->b.id = 0;
                }
            }
        }
    }
    free(vals);
}