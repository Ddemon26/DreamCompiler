#include "codegen.h"
#include "c_emit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

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
  case TK_OR:
    return "|";
  case TK_CARET:
    return "^";
  case TK_AND:
    return "&";
  case TK_LSHIFT:
    return "<<";
  case TK_RSHIFT:
    return ">>";
  case TK_PLUSEQ:
    return "+=";
  case TK_MINUSEQ:
    return "-=";
  case TK_STAREQ:
    return "*=";
  case TK_SLASHEQ:
    return "/=";
  case TK_PERCENTEQ:
    return "%=";
  case TK_ANDEQ:
    return "&=";
  case TK_OREQ:
    return "|=";
  case TK_XOREQ:
    return "^=";
  case TK_LSHIFTEQ:
    return "<<=";
  case TK_RSHIFTEQ:
    return ">>=";
  case TK_QMARKQMARKEQ:
    return "??=";
  case TK_PLUSPLUS:
    return "++";
  case TK_MINUSMINUS:
    return "--";
  case TK_ANDAND:
    return "&&";
  case TK_OROR:
    return "||";
  case TK_BANG:
    return "!";
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

static const char *fmt_for_arg(Node *arg) {
  switch (arg->kind) {
  case ND_STRING:
    return "%s";
  case ND_CHAR:
    return "%c";
  case ND_FLOAT:
    return "%f";
  default:
    return "%d";
  }
}

static void emit_expr(COut *b, Node *n);
static void emit_stmt(COut *b, Node *n);

static void emit_expr(COut *b, Node *n) {
  switch (n->kind) {
  case ND_INT:
  case ND_FLOAT:
  case ND_CHAR:
  case ND_STRING:
  case ND_BOOL:
    if (n->kind == ND_BOOL) {
      if (n->as.lit.len == 4 && strncmp(n->as.lit.start, "true", 4) == 0)
        c_out_write(b, "1");
      else
        c_out_write(b, "0");
    } else if (n->kind == ND_CHAR) {
      c_out_write(b, "'%.*s'", (int)n->as.lit.len, n->as.lit.start);
    } else if (n->kind == ND_STRING) {
      c_out_write(b, "\"%.*s\"", (int)n->as.lit.len, n->as.lit.start);
    } else {
      c_out_write(b, "%.*s", (int)n->as.lit.len, n->as.lit.start);
    }
    break;
  case ND_IDENT:
    c_out_write(b, "%.*s", (int)n->as.ident.len, n->as.ident.start);
    break;
  case ND_UNARY:
    c_out_write(b, "(");
    c_out_write(b, "%s", op_text(n->as.unary.op));
    emit_expr(b, n->as.unary.expr);
    c_out_write(b, ")");
    break;
  case ND_POST_UNARY:
    c_out_write(b, "(");
    emit_expr(b, n->as.unary.expr);
    c_out_write(b, "%s", op_text(n->as.unary.op));
    c_out_write(b, ")");
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

static const char *type_to_c(TokenKind k) {
  switch (k) {
  case TK_KW_INT:
    return "int";
  case TK_KW_FLOAT:
    return "float";
  case TK_KW_CHAR:
    return "char";
  case TK_KW_STRING:
    return "const char *";
  case TK_KW_BOOL:
    return "int";
  default:
    return "int";
  }
}

static void emit_stmt(COut *b, Node *n) {
  switch (n->kind) {
  case ND_VAR_DECL:
    c_out_write(b, "%s %.*s = ", type_to_c(n->as.var_decl.type),
                (int)n->as.var_decl.name.len, n->as.var_decl.name.start);
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
  case ND_WHILE:
    c_out_write(b, "while (");
    emit_expr(b, n->as.while_stmt.cond);
    c_out_write(b, ") ");
    emit_stmt(b, n->as.while_stmt.body);
    break;
  case ND_DO_WHILE:
    c_out_write(b, "do ");
    emit_stmt(b, n->as.do_while_stmt.body);
    c_out_write(b, " while (");
    emit_expr(b, n->as.do_while_stmt.cond);
    c_out_write(b, ");");
    c_out_newline(b);
    break;
  case ND_FOR:
    c_out_write(b, "for (");
    if (n->as.for_stmt.init) {
      if (n->as.for_stmt.init->kind == ND_VAR_DECL) {
        c_out_write(b, "%s %.*s = ",
                    type_to_c(n->as.for_stmt.init->as.var_decl.type),
                    (int)n->as.for_stmt.init->as.var_decl.name.len,
                    n->as.for_stmt.init->as.var_decl.name.start);
        emit_expr(b, n->as.for_stmt.init->as.var_decl.init);
      } else {
        emit_expr(b, n->as.for_stmt.init);
      }
    }
    c_out_write(b, "; ");
    if (n->as.for_stmt.cond)
      emit_expr(b, n->as.for_stmt.cond);
    c_out_write(b, "; ");
    if (n->as.for_stmt.update)
      emit_expr(b, n->as.for_stmt.update);
    c_out_write(b, ") ");
    emit_stmt(b, n->as.for_stmt.body);
    break;
  case ND_BREAK:
    c_out_write(b, "break;");
    c_out_newline(b);
    break;
  case ND_CONTINUE:
    c_out_write(b, "continue;");
    c_out_newline(b);
    break;
  case ND_RETURN:
    c_out_write(b, "return");
    if (n->as.ret.expr) {
      c_out_write(b, " ");
      emit_expr(b, n->as.ret.expr);
    }
    c_out_write(b, ";");
    c_out_newline(b);
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
    if (n->as.expr_stmt.expr->kind == ND_CONSOLE_CALL) {
      Node *call = n->as.expr_stmt.expr;
      c_out_write(b, "printf(\"");
      c_out_write(b, "%s", fmt_for_arg(call->as.console.arg));
      if (call->as.console.newline)
        c_out_write(b, "\\n");
      c_out_write(b, "\", ");
      emit_expr(b, call->as.console.arg);
      c_out_write(b, ");");
      c_out_newline(b);
    } else {
      emit_expr(b, n->as.expr_stmt.expr);
      c_out_write(b, ";");
      c_out_newline(b);
    }
    break;
  case ND_CONSOLE_CALL:
    c_out_write(b, "printf(\"");
    c_out_write(b, "%s", fmt_for_arg(n->as.console.arg));
    if (n->as.console.newline)
      c_out_write(b, "\\n");
    c_out_write(b, "\", ");
    emit_expr(b, n->as.console.arg);
    c_out_write(b, ");");
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
#ifdef _WIN32
  char tmp[L_tmpnam] = {0};
  if (tmpnam_s(tmp, L_tmpnam) != 0) {
    perror("tmpnam_s");
    return;
  }
  strcat(tmp, ".c");
  FILE *f = fopen(tmp, "w");
  if (!f) {
    perror("fopen");
    return;
  }
#else
  char tmp[] = "/tmp/dreamXXXXXX.c";
  int fd = mkstemps(tmp, 2);
  if (fd == -1) {
    perror("mkstemps");
    return;
  }
  FILE *f = fdopen(fd, "w");
  if (!f) {
    perror("fdopen");
    close(fd);
    unlink(tmp);
    return;
  }
#endif
  codegen_emit_c(root, f);
  fclose(f);
  char cmd[512];
  snprintf(cmd, sizeof(cmd), "zig cc -std=c11 -c %s -o %s", tmp, path);
  int res = system(cmd);
  if (res != 0)
    fprintf(stderr, "failed to run: %s\n", cmd);
#ifdef _WIN32
  remove(tmp);
#else
  unlink(tmp);
#endif
}
