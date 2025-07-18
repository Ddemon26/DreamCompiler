#include "stmt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static CGTypeInfo *g_types = NULL;
static size_t g_type_len = 0;

void cg_register_types(CGTypeInfo *types, size_t n) {
  g_types = types;
  g_type_len = n;
}

int cg_is_class_type(Slice name) {
  for (size_t i = 0; i < g_type_len; i++) {
    if (g_types[i].name.len == name.len &&
        strncmp(g_types[i].name.start, name.start, name.len) == 0)
      return g_types[i].is_class;
  }
  return 0;
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
  case TK_KW_VOID:
    return "void";
  default:
    return "int";
  }
}

static void emit_type(COut *b, TokenKind k, Slice name) {
  if (k == TK_IDENT) {
    c_out_write(b, "struct %.*s", (int)name.len, name.start);
    if (cg_is_class_type(name))
      c_out_write(b, " *");
    return;
  }
  c_out_write(b, "%s", type_to_c(k));
}

void emit_type_decl(COut *b, Node *n) {
  c_out_write(b, "/* Dream struct %.*s */\n", (int)n->as.type_decl.name.len,
              n->as.type_decl.name.start);
  c_out_write(b, "struct %.*s {", (int)n->as.type_decl.name.len,
              n->as.type_decl.name.start);
  c_out_newline(b);
  c_out_indent(b);
  for (size_t i = 0; i < n->as.type_decl.len; i++) {
    Node *m = n->as.type_decl.members[i];
    if (m->kind == ND_VAR_DECL) {
      emit_type(b, m->as.var_decl.type, m->as.var_decl.type_name);
      c_out_write(b, " %.*s;", (int)m->as.var_decl.name.len,
                  m->as.var_decl.name.start);
      c_out_newline(b);
    }
  }
  c_out_dedent(b);
  c_out_write(b, "};");
  c_out_newline(b);
  c_out_newline(b);
  for (size_t i = 0; i < n->as.type_decl.len; i++) {
    Node *m = n->as.type_decl.members[i];
    if (m->kind == ND_FUNC) {
      emit_method(b, n->as.type_decl.name, m);
    }
  }
  c_out_newline(b);
}

static void emit_func_impl(COut *b, Slice prefix, Node *n) {
  c_out_write(b, "/* Dream function %.*s */\n", (int)n->as.func.name.len,
              n->as.func.name.start);
  if (prefix.len)
    c_out_write(b, "static %s %.*s_%.*s(", type_to_c(n->as.func.ret_type),
                (int)prefix.len, prefix.start, (int)n->as.func.name.len,
                n->as.func.name.start);
  else
    c_out_write(b, "static %s %.*s(", type_to_c(n->as.func.ret_type),
                (int)n->as.func.name.len, n->as.func.name.start);
  for (size_t i = 0; i < n->as.func.param_len; i++) {
    Node *p = n->as.func.params[i];
    if (i)
      c_out_write(b, ", ");
    c_out_write(b, "%s %.*s", type_to_c(p->as.var_decl.type),
                (int)p->as.var_decl.name.len, p->as.var_decl.name.start);
  }
  c_out_write(b, ") ");
  CGCtx ctx = {0};
  ctx.ret_type = n->as.func.ret_type;
  cgctx_scope_enter(&ctx);
  for (size_t i = 0; i < n->as.func.param_len; i++) {
    Node *p = n->as.func.params[i];
    cgctx_push(&ctx, p->as.var_decl.name.start, p->as.var_decl.name.len,
               p->as.var_decl.type,
               p->as.var_decl.type == TK_IDENT ? p->as.var_decl.type_name
                                                : (Slice){NULL, 0});
  }
  cg_emit_stmt(&ctx, b, n->as.func.body);
  cgctx_scope_leave(&ctx);
  free(ctx.vars);
  c_out_newline(b);
}

void emit_func(COut *b, Node *n) { emit_func_impl(b, (Slice){NULL, 0}, n); }

void emit_method(COut *b, Slice class_name, Node *n) {
  // implicit this parameter
  // copy params with extra first param
  c_out_write(b, "/* Dream method %.*s.%.*s */\n", (int)class_name.len,
              class_name.start, (int)n->as.func.name.len, n->as.func.name.start);
  c_out_write(b, "static %s %.*s_%.*s(struct %.*s *this",
              type_to_c(n->as.func.ret_type), (int)class_name.len, class_name.start,
              (int)n->as.func.name.len, n->as.func.name.start, (int)class_name.len,
              class_name.start);
  for (size_t i = 0; i < n->as.func.param_len; i++) {
    Node *p = n->as.func.params[i];
    c_out_write(b, ", %s %.*s", type_to_c(p->as.var_decl.type),
                (int)p->as.var_decl.name.len, p->as.var_decl.name.start);
  }
  c_out_write(b, ") ");
  CGCtx ctx = {0};
  ctx.ret_type = n->as.func.ret_type;
  cgctx_scope_enter(&ctx);
  cgctx_push(&ctx, "this", 4, TK_IDENT, class_name);
  for (size_t i = 0; i < n->as.func.param_len; i++) {
    Node *p = n->as.func.params[i];
    cgctx_push(&ctx, p->as.var_decl.name.start, p->as.var_decl.name.len,
               p->as.var_decl.type,
               p->as.var_decl.type == TK_IDENT ? p->as.var_decl.type_name
                                                : (Slice){NULL, 0});
  }
  cg_emit_stmt(&ctx, b, n->as.func.body);
  cgctx_scope_leave(&ctx);
  free(ctx.vars);
  c_out_newline(b);
}

void cg_emit_stmt(CGCtx *ctx, COut *b, Node *n) {
  switch (n->kind) {
  case ND_VAR_DECL:
    if (n->as.var_decl.array_len > 0) {
      emit_type(b, n->as.var_decl.type, n->as.var_decl.type_name);
      c_out_write(b, " %.*s[%zu]", (int)n->as.var_decl.name.len,
                  n->as.var_decl.name.start, n->as.var_decl.array_len);
      if (n->as.var_decl.init) {
        c_out_write(b, " = ");
        cg_emit_expr(ctx, b, n->as.var_decl.init);
      }
    } else {
      emit_type(b, n->as.var_decl.type, n->as.var_decl.type_name);
      c_out_write(b, " %.*s", (int)n->as.var_decl.name.len,
                  n->as.var_decl.name.start);
      if (n->as.var_decl.init) {
        c_out_write(b, " = ");
        cg_emit_expr(ctx, b, n->as.var_decl.init);
      }
    }
    cgctx_push(ctx, n->as.var_decl.name.start, n->as.var_decl.name.len,
               n->as.var_decl.type,
               n->as.var_decl.type == TK_IDENT ? n->as.var_decl.type_name
                                                : (Slice){NULL, 0});
    c_out_write(b, ";");
    c_out_newline(b);
    break;
  case ND_FUNC:
    emit_func(b, n);
    break;
  case ND_IF:
    c_out_write(b, "if (");
    cg_emit_expr(ctx, b, n->as.if_stmt.cond);
    c_out_write(b, ") ");
    cg_emit_stmt(ctx, b, n->as.if_stmt.then_br);
    if (n->as.if_stmt.else_br) {
      c_out_write(b, " else ");
      cg_emit_stmt(ctx, b, n->as.if_stmt.else_br);
    }
    break;
  case ND_WHILE:
    c_out_write(b, "while (");
    cg_emit_expr(ctx, b, n->as.while_stmt.cond);
    c_out_write(b, ") ");
    cg_emit_stmt(ctx, b, n->as.while_stmt.body);
    break;
  case ND_DO_WHILE:
    c_out_write(b, "do ");
    cg_emit_stmt(ctx, b, n->as.do_while_stmt.body);
    c_out_write(b, " while (");
    cg_emit_expr(ctx, b, n->as.do_while_stmt.cond);
    c_out_write(b, ");");
    c_out_newline(b);
    break;
  case ND_FOR:
    c_out_write(b, "for (");
    if (n->as.for_stmt.init) {
      if (n->as.for_stmt.init->kind == ND_VAR_DECL) {
        Node *vd = n->as.for_stmt.init;
        if (vd->as.var_decl.array_len > 0) {
          emit_type(b, vd->as.var_decl.type, vd->as.var_decl.type_name);
          c_out_write(b, " %.*s[%zu]", (int)vd->as.var_decl.name.len,
                      vd->as.var_decl.name.start, vd->as.var_decl.array_len);
          if (vd->as.var_decl.init) {
            c_out_write(b, " = ");
            cg_emit_expr(ctx, b, vd->as.var_decl.init);
          }
        } else {
          emit_type(b, vd->as.var_decl.type, vd->as.var_decl.type_name);
          c_out_write(b, " %.*s", (int)vd->as.var_decl.name.len,
                      vd->as.var_decl.name.start);
          if (vd->as.var_decl.init) {
            c_out_write(b, " = ");
            cg_emit_expr(ctx, b, vd->as.var_decl.init);
          }
        }
        cgctx_push(ctx, vd->as.var_decl.name.start, vd->as.var_decl.name.len,
                   vd->as.var_decl.type,
                   vd->as.var_decl.type == TK_IDENT ? vd->as.var_decl.type_name
                                                    : (Slice){NULL, 0});
      } else {
        cg_emit_expr(ctx, b, n->as.for_stmt.init);
      }
    }
    c_out_write(b, "; ");
    if (n->as.for_stmt.cond)
      cg_emit_expr(ctx, b, n->as.for_stmt.cond);
    c_out_write(b, "; ");
    if (n->as.for_stmt.update)
      cg_emit_expr(ctx, b, n->as.for_stmt.update);
    c_out_write(b, ") ");
    cg_emit_stmt(ctx, b, n->as.for_stmt.body);
    break;
  case ND_SWITCH:
    c_out_write(b, "switch (");
    cg_emit_expr(ctx, b, n->as.switch_stmt.expr);
    c_out_write(b, ") {");
    c_out_newline(b);
    c_out_indent(b);
    for (size_t i = 0; i < n->as.switch_stmt.len; i++) {
      SwitchCase *sc = &n->as.switch_stmt.cases[i];
      if (sc->is_default) {
        c_out_write(b, "default:");
      } else {
        c_out_write(b, "case ");
        cg_emit_expr(ctx, b, sc->value);
        c_out_write(b, ":");
      }
      c_out_newline(b);
      cg_emit_stmt(ctx, b, sc->body);
    }
    c_out_dedent(b);
    c_out_write(b, "}");
    c_out_newline(b);
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
      cg_emit_expr(ctx, b, n->as.ret.expr);
    } else if (ctx->ret_type != TK_KW_VOID) {
      c_out_write(b, " 0");
    }
    c_out_write(b, ";");
    c_out_newline(b);
    break;
  case ND_THROW:
    if (n->as.throw_stmt.expr) {
      c_out_write(b, "(void)(");
      cg_emit_expr(ctx, b, n->as.throw_stmt.expr);
      c_out_write(b, ");\n");
    }
    c_out_write(b, "dream_throw();");
    c_out_newline(b);
    break;
  case ND_BLOCK:
    c_out_write(b, "{");
    c_out_newline(b);
    c_out_indent(b);
    cgctx_scope_enter(ctx);
    for (size_t i = 0; i < n->as.block.len; i++)
      cg_emit_stmt(ctx, b, n->as.block.items[i]);
    cgctx_scope_leave(ctx);
    c_out_dedent(b);
    c_out_write(b, "}");
    c_out_newline(b);
    break;
  case ND_EXPR_STMT:
    if (n->as.expr_stmt.expr->kind == ND_CONSOLE_CALL) {
      Node *call = n->as.expr_stmt.expr;
      if (call->as.console.read) {
        c_out_write(b, "dream_readline();");
        c_out_newline(b);
      } else if (cg_is_string_expr(ctx, call->as.console.arg)) {
        c_out_write(b, call->as.console.newline ? "dr_console_writeln("
                                                : "dr_console_write(");
        cg_emit_expr(ctx, b, call->as.console.arg);
        c_out_write(b, ");");
        c_out_newline(b);
      } else {
        c_out_write(b, "printf(\"");
        c_out_write(b, "%s", cg_fmt_for_arg(ctx, call->as.console.arg));
        if (call->as.console.newline)
          c_out_write(b, "\\n");
        c_out_write(b, "\", ");
        cg_emit_expr(ctx, b, call->as.console.arg);
        c_out_write(b, ");");
        c_out_newline(b);
      }
    } else {
      cg_emit_expr(ctx, b, n->as.expr_stmt.expr);
      c_out_write(b, ";");
      c_out_newline(b);
    }
    break;
  case ND_CONSOLE_CALL:
    if (n->as.console.read) {
      c_out_write(b, "dream_readline()");
    } else if (cg_is_string_expr(ctx, n->as.console.arg)) {
      c_out_write(b, n->as.console.newline ? "dr_console_writeln("
                                           : "dr_console_write(");
      cg_emit_expr(ctx, b, n->as.console.arg);
      c_out_write(b, ")");
    } else {
      c_out_write(b, "printf(\"");
      c_out_write(b, "%s", cg_fmt_for_arg(ctx, n->as.console.arg));
      if (n->as.console.newline)
        c_out_write(b, "\\n");
      c_out_write(b, "\", ");
      cg_emit_expr(ctx, b, n->as.console.arg);
      c_out_write(b, ");");
    }
    c_out_newline(b);
    break;
  case ND_TRY:
    c_out_write(b, "{");
    c_out_newline(b);
    c_out_indent(b);
    c_out_write(b, "dream_jmp_top++;\n");
    c_out_write(b, "if (!setjmp(dream_jmp_buf[dream_jmp_top])) {\n");
    c_out_indent(b);
    cg_emit_stmt(ctx, b, n->as.try_stmt.body);
    c_out_write(b, "dream_jmp_top--;\n");
    c_out_dedent(b);
    c_out_write(b, "} else {\n");
    c_out_indent(b);
    c_out_write(b, "dream_jmp_top--;\n");
    if (n->as.try_stmt.catch_body)
      cg_emit_stmt(ctx, b, n->as.try_stmt.catch_body);
    c_out_dedent(b);
    c_out_write(b, "}\n");
    if (n->as.try_stmt.finally_body)
      cg_emit_stmt(ctx, b, n->as.try_stmt.finally_body);
    c_out_dedent(b);
    c_out_write(b, "}");
    c_out_newline(b);
    break;
  default:
    break;
  }
}
