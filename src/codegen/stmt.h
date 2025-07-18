#ifndef CG_STMT_H
#define CG_STMT_H

#include "c_emit.h"
#include "context.h"
#include "expr.h"
#include "../parser/ast.h"

#ifdef __cplusplus
extern "C" {
#endif

void cg_emit_stmt(CGCtx *ctx, COut *b, Node *n);
void emit_type_decl(COut *b, Node *n);
void emit_func(COut *b, Node *n);
void emit_method(COut *b, Slice class_name, Node *n);

typedef struct {
  Slice name;
  int is_class;
} CGTypeInfo;

void cg_register_types(CGTypeInfo *types, size_t n);
int cg_is_class_type(Slice name);

#ifdef __cplusplus
}
#endif

#endif // CG_STMT_H
