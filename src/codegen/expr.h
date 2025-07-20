#ifndef CG_EXPR_H
#define CG_EXPR_H

#include "c_emit.h"
#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

void cg_emit_expr(CGCtx *ctx, COut *b, Node *n);
int cg_is_string_expr(CGCtx *ctx, Node *n);
int cg_is_int_expr(CGCtx *ctx, Node *n);
int cg_is_float_expr(CGCtx *ctx, Node *n);
const char *cg_fmt_for_arg(CGCtx *ctx, Node *arg);

#ifdef __cplusplus
}
#endif

#endif // CG_EXPR_H
