#include "codegen.h"
#include "c_emit.h"
#include <stdio.h>

static const char *op_text(TokenKind k) {
  switch (k) {
  case TK_PLUS:
    return "+";
  case TK_MINUS:
    return "-";
  case TK_STAR:
    return "*";
  case TK_SLASH:
    return "/";
  case TK_PERCENT:
    return "%";
  case TK_EQEQ:
    return "==";
  case TK_NEQ:
    return "!=";
  case TK_LT:
    return "<";
  case TK_GT:
    return ">";
  case TK_LTEQ:
    return "<=";
  case TK_GTEQ:
    return ">=";
  case TK_EQ:
    return "=";
  default:
    return "?";
  }
}

static void emit_expr(COut *b, Node *n);
static void emit_stmt(COut *b, Node *n);

static void emit_expr(COut *b, Node *n) {
  switch (n->kind) {
  case ND_INT:
  case ND_STRING:
    c_out_write(b, "%.*s", (int)n->as.lit.len, n->as.lit.start);
    break;
  case ND_IDENT:
    c_out_write(b, "%.*s", (int)n->as.ident.len, n->as.ident.start);
    break;
  case ND_BINOP:
    c_out_write(b, "(");
    emit_expr(b, n->as.bin.lhs);
    c_out_write(b, " %s ", op_text(n->as.bin.op));
    emit_expr(b, n->as.bin.rhs);
    c_out_write(b, ")");
    break;
  default:
    c_out_write(b, "0");
    break;
  }
}

static void emit_stmt(COut *b, Node *n) {
  switch (n->kind) {
  case ND_VAR_DECL:
    c_out_write(b, "int %.*s = ", (int)n->as.var_decl.name.len,
                n->as.var_decl.name.start);
    emit_expr(b, n->as.var_decl.init);
    c_out_write(b, ";");
    c_out_newline(b);
    break;
  case ND_IF:
    c_out_write(b, "if (");
    emit_expr(b, n->as.if_stmt.cond);
    c_out_write(b, ") ");
    emit_stmt(b, n->as.if_stmt.then_br);
    if (n->as.if_stmt.else_br) {
      c_out_write(b, " else ");
      emit_stmt(b, n->as.if_stmt.else_br);
    }
    break;
  case ND_BLOCK:
    c_out_write(b, "{");
    c_out_newline(b);
    c_out_indent(b);
    for (size_t i = 0; i < n->as.block.len; i++)
      emit_stmt(b, n->as.block.items[i]);
    c_out_dedent(b);
    c_out_write(b, "}");
    c_out_newline(b);
    break;
  case ND_EXPR_STMT:
    emit_expr(b, n->as.expr_stmt.expr);
    c_out_write(b, ";");
    c_out_newline(b);
    break;
  default:
    break;
  }
}

void codegen_emit_c(Node *root, FILE *out) {
  COut builder;
  c_out_init(&builder);
  c_out_write(&builder, "#include <stdio.h>");
  c_out_newline(&builder);
  c_out_newline(&builder);
  c_out_write(&builder, "int main(void) ");
  emit_stmt(&builder, root);
  c_out_newline(&builder);
  c_out_dump(out, &builder);
  c_out_free(&builder);
}

void codegen_emit_obj(Node *root, const char *path) {
  (void)root;
  (void)path;
  fprintf(stderr, "--emit-obj not implemented\n");
}
