#ifndef CG_CONTEXT_H
#define CG_CONTEXT_H

#include "../lexer/lexer.h"
#include "../parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VarBinding {
  const char *start;
  size_t len;
  TokenKind type;
  int depth;
} VarBinding;

typedef struct CGCtx {
  VarBinding *vars;
  size_t len;
  size_t cap;
  int depth;
  TokenKind ret_type;
} CGCtx;

void cgctx_push(CGCtx *ctx, const char *start, size_t len, TokenKind ty);
void cgctx_scope_enter(CGCtx *ctx);
void cgctx_scope_leave(CGCtx *ctx);
TokenKind cgctx_lookup(CGCtx *ctx, const char *start, size_t len);

#ifdef __cplusplus
}
#endif

#endif // CG_CONTEXT_H
