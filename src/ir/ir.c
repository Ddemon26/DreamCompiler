#include "ir.h"
#include <stdlib.h>

IRInstr *ir_instr_new(IROp op, IRValue dst, IRValue a, IRValue b) {
    IRInstr *in = malloc(sizeof(IRInstr));
    in->op = op;
    in->dst = dst;
    in->a = a;
    in->b = b;
    return in;
}
