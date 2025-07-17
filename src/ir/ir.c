#include "ir.h"
#include <stdlib.h>
/**
 * @brief Creates a new IR instruction.
 *
 * @param op The operation code for the instruction.
 * @param dst The destination value of the instruction.
 * @param a The first operand of the instruction.
 * @param b The second operand of the instruction.
 * @return Pointer to the newly created IR instruction.
 */
IRInstr *ir_instr_new(IROp op, IRValue dst, IRValue a, IRValue b) {
    IRInstr *in = malloc(sizeof(IRInstr));
    in->op = op;
    in->dst = dst;
    in->a = a;
    in->b = b;
    return in;
}
