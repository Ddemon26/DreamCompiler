#include "lower.h"
#include "../cfg/cfg.h"
#include "../parser/ast.h"
#include "ir.h"
#include <stdlib.h>
#include <string.h>

typedef struct Var Var;
struct Var {
  char *name;
  int id;
  Var *next;
};

static char *strdup_n(const char *s, size_t n) {
  char *r = malloc(n + 1);
  memcpy(r, s, n);
  r[n] = '\0';
  return r;
}

static Var *vars;

static int get_var_id(const char *s, size_t n, int *next) {
  for (Var *v = vars; v; v = v->next) {
    if (strlen(v->name) == n && strncmp(v->name, s, n) == 0)
      return v->id;
  }
  Var *v = malloc(sizeof(Var));
  v->name = strdup_n(s, n);
  v->id = (*next)++;
  v->next = vars;
  vars = v;
  return v->id;
}

typedef struct CFContext CFContext;
struct CFContext {
  BasicBlock *brk;
  BasicBlock *cont;
  CFContext *parent;
};

static IROp binop_from_token(TokenKind tk) {
  switch (tk) {
  case TK_PLUS:
    return IR_ADD;
  case TK_MINUS:
    return IR_SUB;
  case TK_STAR:
    return IR_MUL;
  case TK_SLASH:
    return IR_DIV;
  case TK_PERCENT:
    return IR_MOD;
  case TK_AND:
    return IR_AND;
  case TK_OR:
    return IR_OR;
  case TK_CARET:
    return IR_XOR;
  case TK_LSHIFT:
    return IR_SHL;
  case TK_RSHIFT:
    return IR_SHR;
  case TK_LT:
    return IR_LT;
  case TK_LTEQ:
    return IR_LE;
  case TK_GT:
    return IR_GT;
  case TK_GTEQ:
    return IR_GE;
  case TK_EQEQ:
    return IR_EQ;
  case TK_NEQ:
    return IR_NE;
  default:
    return IR_ADD;
  }
}

static IRValue emit_expr(BasicBlock *bb, Node *n, int *next) {
  switch (n->kind) {
  case ND_INT: {
    char buf[32];
    memcpy(buf, n->as.lit.start, n->as.lit.len);
    buf[n->as.lit.len] = '\0';
    int v = atoi(buf);
    return ir_const(v);
  }
  case ND_IDENT: {
    int id = get_var_id(n->as.ident.start, n->as.ident.len, next);
    return (IRValue){.id = id};
  }
  case ND_BINOP: {
    if (n->as.bin.op == TK_EQ && n->as.bin.lhs->kind == ND_IDENT) {
      IRValue rhs = emit_expr(bb, n->as.bin.rhs, next);
      int id = get_var_id(n->as.bin.lhs->as.ident.start,
                          n->as.bin.lhs->as.ident.len, next);
      IRInstr *ins =
          ir_instr_new(IR_MOV, (IRValue){.id = id}, rhs, (IRValue){0});
      bb->instrs = realloc(bb->instrs, sizeof(IRInstr *) * (bb->ninstrs + 1));
      bb->instrs[bb->ninstrs++] = ins;
      return (IRValue){.id = id};
    }
    IRValue lhs = emit_expr(bb, n->as.bin.lhs, next);
    IRValue rhs = emit_expr(bb, n->as.bin.rhs, next);
    IRValue dst = {.id = (*next)++};
    IRInstr *ins = ir_instr_new(binop_from_token(n->as.bin.op), dst, lhs, rhs);
    bb->instrs = realloc(bb->instrs, sizeof(IRInstr *) * (bb->ninstrs + 1));
    bb->instrs[bb->ninstrs++] = ins;
    return dst;
  }
  default:
    return ir_const(0);
  }
}

static BasicBlock *emit_stmt(CFG *cfg, BasicBlock *bb, Node *n, int *next,
                             CFContext *ctx) {
  switch (n->kind) {
  case ND_BLOCK:
    for (size_t i = 0; i < n->as.block.len; i++)
      bb = emit_stmt(cfg, bb, n->as.block.items[i], next, ctx);
    return bb;
  case ND_VAR_DECL: {
    int id =
        get_var_id(n->as.var_decl.name.start, n->as.var_decl.name.len, next);
    if (n->as.var_decl.init) {
      IRValue val = emit_expr(bb, n->as.var_decl.init, next);
      IRInstr *ins =
          ir_instr_new(IR_MOV, (IRValue){.id = id}, val, (IRValue){0});
      bb->instrs = realloc(bb->instrs, sizeof(IRInstr *) * (bb->ninstrs + 1));
      bb->instrs[bb->ninstrs++] = ins;
    }
    return bb;
  }
  case ND_EXPR_STMT:
    emit_expr(bb, n->as.expr_stmt.expr, next);
    return bb;
  case ND_IF: {
    IRValue cond = emit_expr(bb, n->as.if_stmt.cond, next);
    IRInstr *cj =
        ir_instr_new(IR_CJUMP, (IRValue){.id = -1}, cond, (IRValue){0});
    bb->instrs = realloc(bb->instrs, sizeof(IRInstr *) * (bb->ninstrs + 1));
    bb->instrs[bb->ninstrs++] = cj;
    BasicBlock *then_bb = cfg_add_block(cfg);
    BasicBlock *else_bb = cfg_add_block(cfg);
    cfg_add_edge(bb, then_bb);
    cfg_add_edge(bb, else_bb);
    BasicBlock *end_then =
        emit_stmt(cfg, then_bb, n->as.if_stmt.then_br, next, ctx);
    IRInstr *j1 =
        ir_instr_new(IR_JUMP, (IRValue){.id = -1}, (IRValue){0}, (IRValue){0});
    end_then->instrs =
        realloc(end_then->instrs, sizeof(IRInstr *) * (end_then->ninstrs + 1));
    end_then->instrs[end_then->ninstrs++] = j1;
    BasicBlock *after = cfg_add_block(cfg);
    cfg_add_edge(end_then, after);
    BasicBlock *end_else = else_bb;
    if (n->as.if_stmt.else_br)
      end_else = emit_stmt(cfg, else_bb, n->as.if_stmt.else_br, next, ctx);
    IRInstr *j2 =
        ir_instr_new(IR_JUMP, (IRValue){.id = -1}, (IRValue){0}, (IRValue){0});
    end_else->instrs =
        realloc(end_else->instrs, sizeof(IRInstr *) * (end_else->ninstrs + 1));
    end_else->instrs[end_else->ninstrs++] = j2;
    cfg_add_edge(end_else, after);
    return after;
  }
  case ND_WHILE: {
    BasicBlock *cond_bb = cfg_add_block(cfg);
    IRInstr *j =
        ir_instr_new(IR_JUMP, (IRValue){.id = -1}, (IRValue){0}, (IRValue){0});
    bb->instrs = realloc(bb->instrs, sizeof(IRInstr *) * (bb->ninstrs + 1));
    bb->instrs[bb->ninstrs++] = j;
    cfg_add_edge(bb, cond_bb);
    IRValue cond = emit_expr(cond_bb, n->as.while_stmt.cond, next);
    IRInstr *cj =
        ir_instr_new(IR_CJUMP, (IRValue){.id = -1}, cond, (IRValue){0});
    cond_bb->instrs =
        realloc(cond_bb->instrs, sizeof(IRInstr *) * (cond_bb->ninstrs + 1));
    cond_bb->instrs[cond_bb->ninstrs++] = cj;
    BasicBlock *body_bb = cfg_add_block(cfg);
    BasicBlock *after = cfg_add_block(cfg);
    cfg_add_edge(cond_bb, body_bb);
    cfg_add_edge(cond_bb, after);
    CFContext inner = {after, cond_bb, ctx};
    BasicBlock *body_end =
        emit_stmt(cfg, body_bb, n->as.while_stmt.body, next, &inner);
    IRInstr *bj =
        ir_instr_new(IR_JUMP, (IRValue){.id = -1}, (IRValue){0}, (IRValue){0});
    body_end->instrs =
        realloc(body_end->instrs, sizeof(IRInstr *) * (body_end->ninstrs + 1));
    body_end->instrs[body_end->ninstrs++] = bj;
    cfg_add_edge(body_end, cond_bb);
    return after;
  }
  case ND_FOR: {
    if (n->as.for_stmt.init)
      bb = emit_stmt(cfg, bb, n->as.for_stmt.init, next, ctx);
    BasicBlock *cond_bb = cfg_add_block(cfg);
    IRInstr *j =
        ir_instr_new(IR_JUMP, (IRValue){.id = -1}, (IRValue){0}, (IRValue){0});
    bb->instrs = realloc(bb->instrs, sizeof(IRInstr *) * (bb->ninstrs + 1));
    bb->instrs[bb->ninstrs++] = j;
    cfg_add_edge(bb, cond_bb);
    IRValue cond = n->as.for_stmt.cond
                       ? emit_expr(cond_bb, n->as.for_stmt.cond, next)
                       : ir_const(1);
    IRInstr *cj =
        ir_instr_new(IR_CJUMP, (IRValue){.id = -1}, cond, (IRValue){0});
    cond_bb->instrs =
        realloc(cond_bb->instrs, sizeof(IRInstr *) * (cond_bb->ninstrs + 1));
    cond_bb->instrs[cond_bb->ninstrs++] = cj;
    BasicBlock *body_bb = cfg_add_block(cfg);
    BasicBlock *after = cfg_add_block(cfg);
    cfg_add_edge(cond_bb, body_bb);
    cfg_add_edge(cond_bb, after);
    CFContext inner = {after, cond_bb, ctx};
    BasicBlock *body_end =
        emit_stmt(cfg, body_bb, n->as.for_stmt.body, next, &inner);
    if (n->as.for_stmt.update)
      body_end = emit_stmt(cfg, body_end, n->as.for_stmt.update, next, &inner);
    IRInstr *bj =
        ir_instr_new(IR_JUMP, (IRValue){.id = -1}, (IRValue){0}, (IRValue){0});
    body_end->instrs =
        realloc(body_end->instrs, sizeof(IRInstr *) * (body_end->ninstrs + 1));
    body_end->instrs[body_end->ninstrs++] = bj;
    cfg_add_edge(body_end, cond_bb);
    return after;
  }
  case ND_BREAK: {
    if (ctx && ctx->brk) {
      IRInstr *jbr = ir_instr_new(IR_JUMP, (IRValue){.id = -1}, (IRValue){0},
                                  (IRValue){0});
      bb->instrs = realloc(bb->instrs, sizeof(IRInstr *) * (bb->ninstrs + 1));
      bb->instrs[bb->ninstrs++] = jbr;
      cfg_add_edge(bb, ctx->brk);
    }
    return cfg_add_block(cfg);
  }
  case ND_CONTINUE: {
    if (ctx && ctx->cont) {
      IRInstr *jc = ir_instr_new(IR_JUMP, (IRValue){.id = -1}, (IRValue){0},
                                 (IRValue){0});
      bb->instrs = realloc(bb->instrs, sizeof(IRInstr *) * (bb->ninstrs + 1));
      bb->instrs[bb->ninstrs++] = jc;
      cfg_add_edge(bb, ctx->cont);
    }
    return cfg_add_block(cfg);
  }
  case ND_RETURN: {
    IRValue val =
        n->as.ret.expr ? emit_expr(bb, n->as.ret.expr, next) : ir_const(0);
    IRInstr *r =
        ir_instr_new(IR_RETURN, (IRValue){.id = -1}, val, (IRValue){0});
    bb->instrs = realloc(bb->instrs, sizeof(IRInstr *) * (bb->ninstrs + 1));
    bb->instrs[bb->ninstrs++] = r;
    return cfg_add_block(cfg);
  }
  default:
    return bb;
  }
}

CFG *ir_lower_program(Node *root, int *nvars) {
  CFG *cfg = cfg_new();
  BasicBlock *bb = cfg_add_block(cfg);
  int next = 0;
  vars = NULL;
  emit_stmt(cfg, bb, root, &next, NULL);
  if (nvars)
    *nvars = next;
  return cfg;
}

void cfg_free(CFG *cfg) {
  if (!cfg)
    return;
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
