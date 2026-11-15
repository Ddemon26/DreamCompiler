#include "literal_fold.h"
#include "sccp.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

enum { MAX_OPS = 256, MAX_DEPTH = 64, LIMIT = INT_MAX - 1 };

/* Stack-owned IR avoids the unchecked general CFG/instruction allocators. */
typedef struct {
  IRInstr instructions[MAX_OPS];
  IRInstr *pointers[MAX_OPS];
  size_t count;
} ExpressionIR;

static bool decimal(Slice text, int *value) {
  if (!text.start || !text.len || (text.len > 1 && text.start[0] == '0'))
    return false;
  int result = 0;
  for (size_t i = 0; i < text.len; ++i) {
    int digit = text.start[i] - '0';
    if (digit < 0 || digit > 9 || result > (LIMIT - digit) / 10)
      return false;
    result = result * 10 + digit;
  }
  *value = result;
  return true;
}

static bool preflight(Node *n, size_t depth, size_t *ops, int *value) {
  if (!n || depth > MAX_DEPTH) return false;
  if (n->kind == ND_INT) return decimal(n->as.lit, value);
  if (n->kind != ND_BINOP ||
      (n->as.bin.op != TK_PLUS && n->as.bin.op != TK_MINUS &&
       n->as.bin.op != TK_STAR) || ++*ops > MAX_OPS)
    return false;
  int lhs, rhs;
  if (!preflight(n->as.bin.lhs, depth + 1, ops, &lhs) ||
      !preflight(n->as.bin.rhs, depth + 1, ops, &rhs))
    return false;
  switch (n->as.bin.op) {
  case TK_PLUS:
    if (lhs > LIMIT - rhs) return false;
    *value = lhs + rhs;
    return true;
  case TK_MINUS:
    if (lhs < rhs) return false;
    *value = lhs - rhs;
    return true;
  case TK_STAR:
    if (rhs && lhs > LIMIT / rhs) return false;
    *value = lhs * rhs;
    return true;
  default: return false;
  }
}

/* Only called after the complete candidate passed preflight. */
static IRValue lower(ExpressionIR *ir, Node *n) {
  if (n->kind == ND_INT) {
    int value = 0;
    decimal(n->as.lit, &value);
    return ir_const(value);
  }
  IRValue lhs = lower(ir, n->as.bin.lhs);
  IRValue rhs = lower(ir, n->as.bin.rhs);
  size_t id = ir->count++;
  IRInstr *in = &ir->instructions[id];
  *in = (IRInstr){.op = n->as.bin.op == TK_PLUS ? IR_ADD :
                        n->as.bin.op == TK_MINUS ? IR_SUB : IR_MUL,
                  .dst = {(int)id}, .a = lhs, .b = rhs};
  ir->pointers[id] = in;
  return in->dst;
}

static bool transformed_value(ExpressionIR *ir, IRValue root, int expected,
                              int *result) {
  if (root.id < 0 || (size_t)root.id >= ir->count) return false;
  IRInstr *in = ir->pointers[root.id];
  if (!in || in->dst.id != root.id || in->op != IR_MOV ||
      !ir_is_const(in->a) || in->a.id == INT_MIN)
    return false;
  *result = ir_const_value(in->a);
  return *result >= 0 && *result <= LIMIT && *result == expected;
}

static bool internal_error(Node *n, const char *reason) {
  fprintf(stderr, "%s:%zu:%zu: error: literal optimization failed: %s\n",
          n->source_file ? n->source_file : "<source>",
          n->pos.line, n->pos.column, reason);
  return false;
}

static bool fold_candidate(Arena *arena, Node *n) {
  size_t ops = 0;
  int expected;
  if (!preflight(n, 0, &ops, &expected)) return true; /* Whole-tree fallback. */
  ExpressionIR ir = {0};
  IRValue root = lower(&ir, n);
  BasicBlock block = {.instrs = ir.pointers, .ninstrs = ir.count};
  BasicBlock *blocks[] = {&block};
  CFG cfg = {.blocks = blocks, .nblocks = 1, .entry = &block};
  bool converged = false;
  for (size_t i = 0; i <= ops; ++i) {
    int changed = sccp_checked(&cfg);
    if (changed < 0) return internal_error(n, "out of memory in SCCP");
    if (!changed) { converged = true; break; }
  }
  int result;
  if (!converged || !transformed_value(&ir, root, expected, &result))
    return internal_error(n, "invalid transformed expression");
  char text[32];
  int len = snprintf(text, sizeof(text), "%d", result);
  if (len < 1 || (size_t)len >= sizeof(text))
    return internal_error(n, "invalid literal spelling");
  char *owned = arena_try_alloc(arena, (size_t)len + 1);
  if (!owned) return internal_error(n, "out of memory for literal");
  memcpy(owned, text, (size_t)len + 1);
  memset(&n->as, 0, sizeof(n->as));
  n->as.lit = (Slice){owned, (size_t)len};
  n->kind = ND_INT; /* Retains source metadata; no pointers into temporary IR. */
  return true;
}

bool fold_literal_arithmetic(Arena *arena, Node *n) {
  if (!n) return true;
#define VISIT(child) do { if (!fold_literal_arithmetic(arena, (child))) return false; } while (0)
  switch (n->kind) {
  case ND_BINOP: return fold_candidate(arena, n);
  /* Unary operations are also whole-expression fallback. */
  case ND_VAR_DECL: VISIT(n->as.var_decl.init); break;
  case ND_RETURN: VISIT(n->as.ret.expr); break;
  case ND_EXPR_STMT: VISIT(n->as.expr_stmt.expr); break;
  case ND_CONSOLE_CALL: VISIT(n->as.console.arg); break;
  case ND_BLOCK:
    for (size_t i = 0; i < n->as.block.len; ++i) VISIT(n->as.block.items[i]);
    break;
  case ND_FUNC: VISIT(n->as.func.body); break;
  case ND_CLASS_DECL: case ND_STRUCT_DECL:
    for (size_t i = 0; i < n->as.type_decl.len; ++i) VISIT(n->as.type_decl.members[i]);
    break;
  case ND_IF:
    VISIT(n->as.if_stmt.cond); VISIT(n->as.if_stmt.then_br); VISIT(n->as.if_stmt.else_br); break;
  case ND_WHILE:
    VISIT(n->as.while_stmt.cond); VISIT(n->as.while_stmt.body); break;
  case ND_DO_WHILE:
    VISIT(n->as.do_while_stmt.body); VISIT(n->as.do_while_stmt.cond); break;
  case ND_FOR:
    VISIT(n->as.for_stmt.init); VISIT(n->as.for_stmt.cond);
    VISIT(n->as.for_stmt.update); VISIT(n->as.for_stmt.body); break;
  case ND_CALL:
    VISIT(n->as.call.callee);
    for (size_t i = 0; i < n->as.call.len; ++i) VISIT(n->as.call.args[i]);
    break;
  case ND_NEW:
    for (size_t i = 0; i < n->as.new_expr.arg_len; ++i) VISIT(n->as.new_expr.args[i]);
    break;
  case ND_COND:
    VISIT(n->as.cond.cond); VISIT(n->as.cond.then_expr); VISIT(n->as.cond.else_expr); break;
  case ND_INDEX: VISIT(n->as.index.array); VISIT(n->as.index.index); break;
  case ND_FIELD: VISIT(n->as.field.object); break;
  case ND_SWITCH:
    VISIT(n->as.switch_stmt.expr);
    for (size_t i = 0; i < n->as.switch_stmt.len; ++i) {
      VISIT(n->as.switch_stmt.cases[i].value); VISIT(n->as.switch_stmt.cases[i].body);
    }
    break;
  case ND_TRY:
    VISIT(n->as.try_stmt.body); VISIT(n->as.try_stmt.catch_body); VISIT(n->as.try_stmt.finally_body); break;
  case ND_THROW: VISIT(n->as.throw_stmt.expr); break;
  case ND_AWAIT: VISIT(n->as.await_expr.expr); break;
  case ND_EXPORT: VISIT(n->as.export.decl); break;
  default: break; /* Unknown containers are not qualified. */
  }
#undef VISIT
  return true;
}
