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

void dce(CFG *cfg) {
    if (!cfg)
        return;
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *b = cfg->blocks[i];
        size_t w = 0;
        for (size_t j = 0; j < b->ninstrs; j++) {
            IRInstr *ins = b->instrs[j];
            switch (ins->op) {
            case IR_BIN:
            case IR_MOV:
                if (!is_used(cfg, ins->dst)) {
                    free(ins);
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
}
