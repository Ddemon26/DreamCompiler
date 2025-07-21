#ifndef CG_STMT_H
#define CG_STMT_H

#include "../parser/ast.h"
#include "c_emit.h"
#include "context.h"
#include "expr.h"

#ifdef __cplusplus
extern "C" {
#endif

void cg_emit_stmt(CGCtx *ctx, COut *b, Node *n, const char *src_file);
void emit_type_decl(COut *b, Node *n, const char *src_file);
void emit_enum_decl(COut *b, Node *n, const char *src_file);
void emit_func(COut *b, Node *n, const char *src_file);
void emit_method(COut *b, Slice class_name, Node *n, const char *src_file);

typedef struct {
  Slice name;
  int is_class;
  int has_init;
} CGTypeInfo;

void cg_register_types(CGTypeInfo *types, size_t n);
int cg_is_class_type(Slice name);
int cg_is_known_type(Slice name);
int cg_has_init(Slice name);

#ifdef __cplusplus
}
#endif

#endif // CG_STMT_H
