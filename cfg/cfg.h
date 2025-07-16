#ifndef CFG_H
#define CFG_H
#include "../ir/ir.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct BasicBlock BasicBlock;
struct BasicBlock {
    int id;
    int dfnum;
    IRInstr **instrs;
    size_t ninstrs;
    BasicBlock **succ;
    size_t nsucc;
    BasicBlock **pred;
    size_t npred;
    BasicBlock *idom;
    BasicBlock **df;
    size_t ndf;
};

typedef struct {
    BasicBlock **blocks;
    size_t nblocks;
    BasicBlock *entry;
} CFG;

CFG *cfg_new(void);
BasicBlock *cfg_add_block(CFG *cfg);
void cfg_add_edge(BasicBlock *from, BasicBlock *to);
void cfg_compute_dominators(CFG *cfg);

#endif
