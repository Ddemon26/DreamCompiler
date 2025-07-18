#include "peephole.h"
#include "../ir/ir.h"

bool peephole(CFG *cfg) {
    if(!cfg) return false;
    bool changed=false;
    for(size_t bi=0; bi<cfg->nblocks; bi++) {
        BasicBlock *b = cfg->blocks[bi];
        for(size_t i=0;i<b->ninstrs;i++) {
            IRInstr *ins = b->instrs[i];
            if(ins->op==IR_MOV && ins->a.id==ins->dst.id) {
                ins->op = IR_NOP;
                changed=true;
            }
            if(ins->op==IR_ADD && ir_is_const(ins->b) && ir_const_value(ins->b)==0) {
                ins->op=IR_MOV; ins->a=ins->a; ins->b.id=0; changed=true;
            }
            if(ins->op==IR_SUB && ir_is_const(ins->b) && ir_const_value(ins->b)==0) {
                ins->op=IR_MOV; ins->a=ins->a; ins->b.id=0; changed=true;
            }
            if(ins->op==IR_MUL) {
                if(ir_is_const(ins->b) && ir_const_value(ins->b)==1) {
                    ins->op=IR_MOV; ins->b.id=0; changed=true;
                } else if(ir_is_const(ins->b) && ir_const_value(ins->b)==0) {
                    ins->op=IR_MOV; ins->a=ir_const(0); ins->b.id=0; changed=true;
                }
            }
            if(ins->op==IR_DIV && ir_is_const(ins->b) && ir_const_value(ins->b)==1) {
                ins->op=IR_MOV; ins->b.id=0; changed=true;
            }
        }
    }
    return changed;
}
