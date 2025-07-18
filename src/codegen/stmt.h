#ifndef CG_STMT_H
#define CG_STMT_H

#include "c_emit.h"
#include "context.h"
#include "expr.h"

#ifdef __cplusplus
extern "C" {
#endif

void cg_emit_stmt(CGCtx *ctx, COut *b, Node *n);
void emit_type_decl(COut *b, Node *n);
void emit_func(COut *b, Node *n);

#ifdef __cplusplus
}
#endif

#endif // CG_STMT_H
