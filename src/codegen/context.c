#include "context.h"
#include <stdlib.h>
#include <string.h>

void cgctx_push(CGCtx *ctx, const char *start, size_t len, TokenKind ty) {
  if (ctx->len + 1 > ctx->cap) {
    ctx->cap = ctx->cap ? ctx->cap * 2 : 8;
    ctx->vars = realloc(ctx->vars, ctx->cap * sizeof(VarBinding));
  }
  ctx->vars[ctx->len++] = (VarBinding){start, len, ty, ctx->depth};
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
