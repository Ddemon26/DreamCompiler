#include "expr.h"
#include "stmt.h"
#include <string.h>

static Slice expr_type(CGCtx *ctx, Node *n) {
  switch (n->kind) {
  case ND_IDENT: {
    Slice found = cgctx_lookup_name(ctx, n->as.ident.start, n->as.ident.len);
    if (found.len == 0) {
      Slice id = {n->as.ident.start, n->as.ident.len};
      if (cg_is_known_type(id))
        return id;
    }
    return found;
  }
  case ND_NEW:
    return n->as.new_expr.type_name;
  default:
    return (Slice){NULL, 0};
  }
}

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
  case TK_TILDE:
    return "~";
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

const char *cg_fmt_for_arg(CGCtx *ctx, Node *arg) {
  if (cg_is_string_expr(ctx, arg))
    return "%s";
  if (arg->kind == ND_IDENT) {
    TokenKind ty = cgctx_lookup(ctx, arg->as.ident.start, arg->as.ident.len);
    switch (ty) {
    case TK_KW_CHAR:
      return "%c";
    case TK_KW_FLOAT:
      return "%f";
    case TK_KW_STRING:
      return "%s";
    default:
      break;
    }
  }
  switch (arg->kind) {
  case ND_CHAR:
    return "%c";
  case ND_CONSOLE_CALL:
    if (arg->as.console.read)
      return "%s";
    return "%d";
  case ND_FLOAT:
    return "%f";
  default:
    return "%d";
  }
}

int cg_is_string_expr(CGCtx *ctx, Node *n) {
  switch (n->kind) {
  case ND_STRING:
    return 1;
  case ND_IDENT:
    return cgctx_lookup(ctx, n->as.ident.start, n->as.ident.len) ==
           TK_KW_STRING;
  case ND_CONSOLE_CALL:
    return n->as.console.read;
  default:
    return 0;
  }
}

void cg_emit_expr(CGCtx *ctx, COut *b, Node *n) {
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
    cg_emit_expr(ctx, b, n->as.unary.expr);
    c_out_write(b, ")");
    break;
  case ND_POST_UNARY:
    c_out_write(b, "(");
    cg_emit_expr(ctx, b, n->as.unary.expr);
    c_out_write(b, "%s", op_text(n->as.unary.op));
    c_out_write(b, ")");
    break;
  case ND_BINOP:
    c_out_write(b, "(");
    if (n->as.bin.op == TK_PLUS && (cg_is_string_expr(ctx, n->as.bin.lhs) ||
                                    cg_is_string_expr(ctx, n->as.bin.rhs))) {
      c_out_write(b, "dream_concat(");
      cg_emit_expr(ctx, b, n->as.bin.lhs);
      c_out_write(b, ", ");
      cg_emit_expr(ctx, b, n->as.bin.rhs);
      c_out_write(b, ")");
    } else {
      cg_emit_expr(ctx, b, n->as.bin.lhs);
      c_out_write(b, " %s ", op_text(n->as.bin.op));
      cg_emit_expr(ctx, b, n->as.bin.rhs);
    }
    c_out_write(b, ")");
    break;
  case ND_COND:
    c_out_write(b, "(");
    cg_emit_expr(ctx, b, n->as.cond.cond);
    c_out_write(b, " ? ");
    cg_emit_expr(ctx, b, n->as.cond.then_expr);
    c_out_write(b, " : ");
    cg_emit_expr(ctx, b, n->as.cond.else_expr);
    c_out_write(b, ")");
    break;
  case ND_INDEX:
    c_out_write(b, "(");
    cg_emit_expr(ctx, b, n->as.index.array);
    c_out_write(b, "[");
    cg_emit_expr(ctx, b, n->as.index.index);
    c_out_write(b, "])");
    break;
  case ND_FIELD: {
    Slice ty = expr_type(ctx, n->as.field.object);
    int is_var = 0;
    if (n->as.field.object->kind == ND_IDENT)
      is_var = cgctx_has_var(ctx, n->as.field.object->as.ident.start,
                             n->as.field.object->as.ident.len);
    if (ty.len && !is_var) {
      c_out_write(b, "%.*s_%.*s", (int)ty.len, ty.start,
                  (int)n->as.field.name.len, n->as.field.name.start);
    } else {
      cg_emit_expr(ctx, b, n->as.field.object);
      if (cg_is_class_type(ty))
        c_out_write(b, "->%.*s", (int)n->as.field.name.len,
                    n->as.field.name.start);
      else
        c_out_write(b, ".%.*s", (int)n->as.field.name.len,
                    n->as.field.name.start);
    }
    break;
  }
  case ND_CALL:
    if (n->as.call.callee->kind == ND_FIELD) {
      Node *fld = n->as.call.callee;
      Slice ty = expr_type(ctx, fld->as.field.object);
      int is_var = 0;
      if (fld->as.field.object->kind == ND_IDENT)
        is_var = cgctx_has_var(ctx, fld->as.field.object->as.ident.start,
                               fld->as.field.object->as.ident.len);
      if (ty.len) {
        c_out_write(b, "%.*s_%.*s(", (int)ty.len, ty.start,
                    (int)fld->as.field.name.len, fld->as.field.name.start);
        if (is_var) {
          if (cg_is_class_type(ty))
            cg_emit_expr(ctx, b, fld->as.field.object);
          else {
            c_out_write(b, "&");
            cg_emit_expr(ctx, b, fld->as.field.object);
          }
        }
        for (size_t i = 0; i < n->as.call.len; i++) {
          if (is_var || i)
            c_out_write(b, ", ");
          cg_emit_expr(ctx, b, n->as.call.args[i]);
        }
        c_out_write(b, ")");
        break;
      }
    }
    cg_emit_expr(ctx, b, n->as.call.callee);
    c_out_write(b, "(");
    for (size_t i = 0; i < n->as.call.len; i++) {
      if (i)
        c_out_write(b, ", ");
      cg_emit_expr(ctx, b, n->as.call.args[i]);
    }
    c_out_write(b, ")");
    break;
  case ND_NEW:
    if (cg_is_class_type(n->as.new_expr.type_name)) {
      c_out_write(b, "({struct %.*s *tmp = dr_alloc(sizeof(struct %.*s));",
                  (int)n->as.new_expr.type_name.len,
                  n->as.new_expr.type_name.start,
                  (int)n->as.new_expr.type_name.len,
                  n->as.new_expr.type_name.start);
      if (cg_has_init(n->as.new_expr.type_name)) {
        c_out_write(b, "%.*s_init(tmp",
                    (int)n->as.new_expr.type_name.len,
                    n->as.new_expr.type_name.start);
        for (size_t i = 0; i < n->as.new_expr.arg_len; i++) {
          c_out_write(b, ", ");
          cg_emit_expr(ctx, b, n->as.new_expr.args[i]);
        }
        c_out_write(b, ");");
      }
      c_out_write(b, " tmp; })");
    } else {
      c_out_write(b, "(struct %.*s){0}",
                  (int)n->as.new_expr.type_name.len,
                  n->as.new_expr.type_name.start);
    }
    break;
  default:
    c_out_write(b, "0");
    break;
  }
}
