#include "lower.h"
#include "ir.h"
#include "../cfg/cfg.h"
#include "../parser/ast.h"
#include <stdlib.h>
#include <string.h>

static IRValue emit_expr(BasicBlock *bb, Node *n, int *next) {
    switch (n->kind) {
    case ND_INT: {
        char buf[32];
        memcpy(buf, n->as.lit.start, n->as.lit.len);
        buf[n->as.lit.len] = '\0';
        int v = atoi(buf);
        return ir_const(v);
    }
    case ND_BINOP: {
        IRValue lhs = emit_expr(bb, n->as.bin.lhs, next);
        IRValue rhs = emit_expr(bb, n->as.bin.rhs, next);
        IRValue dst = { .id = (*next)++ };
        IRInstr *ins = ir_instr_new(IR_BIN, dst, lhs, rhs);
        bb->instrs = realloc(bb->instrs, sizeof(IRInstr*) * (bb->ninstrs + 1));
        bb->instrs[bb->ninstrs++] = ins;
        return dst;
    }
    default:
        return ir_const(0);
    }
}

static void emit_stmt(BasicBlock *bb, Node *n, int *next) {
    switch (n->kind) {
    case ND_BLOCK:
        for (size_t i = 0; i < n->as.block.len; i++)
            emit_stmt(bb, n->as.block.items[i], next);
        break;
    case ND_EXPR_STMT:
        emit_expr(bb, n->as.expr_stmt.expr, next);
        break;
    default:
        break;
    }
}

CFG *ir_lower_program(Node *root, int *nvars) {
    CFG *cfg = cfg_new();
    BasicBlock *bb = cfg_add_block(cfg);
    int next = 0;
    emit_stmt(bb, root, &next);
    if (nvars) *nvars = next;
    return cfg;
}

void cfg_free(CFG *cfg) {
    if (!cfg) return;
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *b = cfg->blocks[i];
        for (size_t j = 0; j < b->ninstrs; j++)
            free(b->instrs[j]);
        free(b->instrs);
        free(b->succ);
        free(b->pred);
        free(b->df);
        free(b);
    }
    free(cfg->blocks);
    free(cfg);
}
