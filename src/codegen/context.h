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
  Slice type_name;
  int depth;
  int indirections; /* Array and explicit pointer layers, excluding string storage. */
} VarBinding;

typedef struct CGCtx {
  VarBinding *vars;
  size_t len;
  size_t cap;
  int depth;
  TokenKind ret_type;
  int is_async_worker;
  Slice async_func_name;
} CGCtx;

void cgctx_push(CGCtx *ctx, const char *start, size_t len, TokenKind ty,
                Slice type_name);
void cgctx_push_decl(CGCtx *ctx, Node *decl);
const VarBinding *cgctx_binding(CGCtx *ctx, const char *start, size_t len);
void cgctx_scope_enter(CGCtx *ctx);
void cgctx_scope_leave(CGCtx *ctx);
TokenKind cgctx_lookup(CGCtx *ctx, const char *start, size_t len);
Slice cgctx_lookup_name(CGCtx *ctx, const char *start, size_t len);
int cgctx_has_var(CGCtx *ctx, const char *start, size_t len);

#ifdef __cplusplus
}
#endif

#endif // CG_CONTEXT_H
