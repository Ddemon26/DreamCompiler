#include "copy_prop.h"
#include "../ir/ir.h"
#include <stdlib.h>

bool copy_propagation(CFG *cfg) {
    if (!cfg) return false;
    bool changed = false;
    for (size_t bi = 0; bi < cfg->nblocks; bi++) {
        BasicBlock *b = cfg->blocks[bi];
        for (size_t i = 0; i < b->ninstrs; i++) {
            IRInstr *ins = b->instrs[i];
            if (!ins) continue;
            if (ins->op == IR_MOV) {
                // propagate forward within block
                IRValue src = ins->a;
                int dst_id = ins->dst.id;
                for (size_t j = i + 1; j < b->ninstrs; j++) {
                    IRInstr *use = b->instrs[j];
                    if (use->a.id == dst_id) {
                        use->a = src;
                        changed = true;
                    }
                    if (use->b.id == dst_id) {
                        use->b = src;
                        changed = true;
                    }
                    if (use->dst.id == dst_id)
                        break; // killed
                }
            }
        }
    }
    return changed;
}
