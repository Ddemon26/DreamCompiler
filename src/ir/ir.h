#ifndef IR_H
#define IR_H
#include <stddef.h>

/**
 * @brief Enum representing the various IR operation codes.
 */
typedef enum {
  IR_NOP, /**< No operation. */
  IR_PHI, /**< Phi operation for SSA form. */
  IR_MOV, /**< Move operation. */
  /* arithmetic */
  IR_ADD,
  IR_SUB,
  IR_MUL,
  IR_DIV,
  IR_MOD,
  /* bitwise */
  IR_AND,
  IR_OR,
  IR_XOR,
  IR_SHL,
  IR_SHR,
  /* comparisons */
  IR_LT,
  IR_LE,
  IR_GT,
  IR_GE,
  IR_EQ,
  IR_NE,
  /* control flow */
  IR_JUMP,  /**< Unconditional jump. */
  IR_CJUMP, /**< Conditional jump. */
  IR_RETURN /**< Return operation. */
} IROp;

/**
 * @brief Structure representing a unique IR value.
 */
typedef struct {
  int id; /**< Unique value identifier. */
} IRValue;

/**
 * @brief Forward declaration of the IR instruction structure.
 */
typedef struct IRInstr IRInstr;

/**
 * @brief Structure representing an IR instruction.
 */
struct IRInstr {
  IROp op;     /**< Operation code of the instruction. */
  IRValue dst; /**< Destination value of the instruction. */
  IRValue a;   /**< First operand of the instruction. */
  IRValue b;   /**< Second operand of the instruction. */
};

/**
 * @brief Creates a new IR instruction.
 * @param op The operation code for the instruction.
 * @param dst The destination value of the instruction.
 * @param a The first operand of the instruction.
 * @param b The second operand of the instruction.
 * @return Pointer to the newly created IR instruction.
 */
IRInstr *ir_instr_new(IROp op, IRValue dst, IRValue a, IRValue b);

static inline IRValue ir_const(int v) {
  IRValue r;
  r.id = -v - 1;
  return r;
}

static inline int ir_is_const(IRValue v) { return v.id < 0; }

static inline int ir_const_value(IRValue v) { return -v.id - 1; }

#endif
