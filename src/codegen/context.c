#include "context.h"
#include <stdlib.h>
#include <string.h>

void cgctx_push(CGCtx *ctx, const char *start, size_t len, TokenKind ty,
                 Slice type_name) {
  if (ctx->len + 1 > ctx->cap) {
    ctx->cap = ctx->cap ? ctx->cap * 2 : 8;
    ctx->vars = realloc(ctx->vars, ctx->cap * sizeof(VarBinding));
  }
  ctx->vars[ctx->len++] =
      (VarBinding){start, len, ty, type_name, ctx->depth, 0, 0};
}

void cgctx_push_decl(CGCtx *ctx, Node *decl) {
  cgctx_push(ctx, decl->as.var_decl.name.start, decl->as.var_decl.name.len,
             decl->as.var_decl.type,
             decl->as.var_decl.type == TK_IDENT ? decl->as.var_decl.type_name
                                                : (Slice){NULL, 0});
  ctx->vars[ctx->len - 1].indirections =
      (decl->as.var_decl.array_len > 0) + !!decl->as.var_decl.is_pointer;
}

const VarBinding *cgctx_binding(CGCtx *ctx, const char *start, size_t len) {
  for (size_t i = ctx->len; i-- > 0;) {
    const VarBinding *v = &ctx->vars[i];
    if (v->len == len && strncmp(v->start, start, len) == 0) return v;
  }
  return NULL;
}

void cgctx_scope_enter(CGCtx *ctx) { ctx->depth++; }

void cgctx_scope_leave(CGCtx *ctx) {
  while (ctx->len && ctx->vars[ctx->len - 1].depth >= ctx->depth)
    ctx->len--;
  if (ctx->depth > 0)
    ctx->depth--;
}

TokenKind cgctx_lookup(CGCtx *ctx, const char *start, size_t len) {
  for (size_t i = ctx->len; i-- > 0;) {
    VarBinding *v = &ctx->vars[i];
    if (v->len == len && strncmp(v->start, start, len) == 0)
      return v->type;
  }
  return (TokenKind)0;
}

Slice cgctx_lookup_name(CGCtx *ctx, const char *start, size_t len) {
  for (size_t i = ctx->len; i-- > 0;) {
    VarBinding *v = &ctx->vars[i];
    if (v->len == len && strncmp(v->start, start, len) == 0)
      return v->type_name;
  }
  return (Slice){NULL, 0};
}

int cgctx_has_var(CGCtx *ctx, const char *start, size_t len) {
  for (size_t i = ctx->len; i-- > 0;) {
    VarBinding *v = &ctx->vars[i];
    if (v->len == len && strncmp(v->start, start, len) == 0)
      return 1;
  }
  return 0;
}
