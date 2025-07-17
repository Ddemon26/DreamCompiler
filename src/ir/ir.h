#ifndef IR_H
#define IR_H
#include <stddef.h>

typedef enum {
    IR_NOP,
    IR_PHI,
    IR_MOV,
    IR_BIN,
    IR_JUMP,
    IR_CJUMP,
    IR_RETURN,
} IROp;

typedef struct {
    int id; // unique value id
} IRValue;

typedef struct IRInstr IRInstr;
struct IRInstr {
    IROp op;
    IRValue dst;
    IRValue a, b;
};

IRInstr *ir_instr_new(IROp op, IRValue dst, IRValue a, IRValue b);

#endif
