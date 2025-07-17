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

/**
 * @brief Converts a token kind to its corresponding operator text.
 *
 * Maps a given token kind to its string representation of the operator.
 *
 * @param k The token kind to convert.
 * @return The string representation of the operator.
 */
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

/**
 * @brief Determines the appropriate format specifier for a given argument.
 *
 * Maps the kind of the provided node to its corresponding format specifier
 * used in formatted output functions.
 *
 * @param arg Pointer to the node representing the argument.
 * @return The format specifier as a string.
 */
static const char *fmt_for_arg(Node *arg) {
  switch (arg->kind) {
  case ND_STRING:
    return "%s";
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

/**
 * @brief Represents a variable binding in the code generation context.
 *
 * Stores information about a variable, including its name, length,
 * type, and the scope depth at which it is defined.
 *
 * @param start Pointer to the start of the variable name.
 * @param len Length of the variable name.
 * @param type Token kind representing the variable's type.
 * @param depth Scope depth where the variable is defined.
 */
typedef struct {
  const char *start;
  size_t len;
  TokenKind type;
  int depth;
} VarBinding;

/**
 * @brief Represents the code generation context.
 *
 * Manages variable bindings, scope depth, and capacity for code generation.
 *
 * @param vars Pointer to the array of variable bindings.
 * @param len Current number of variable bindings.
 * @param cap Maximum capacity of the variable bindings array.
 * @param depth Current scope depth.
 */
typedef struct {
  VarBinding *vars;
  size_t len;
  size_t cap;
  int depth;
} CGCtx;

/**
 * @brief Adds a variable binding to the code generation context.
 *
 * Expands the variable bindings array if necessary and appends a new binding
 * with the specified name, length, type, and current scope depth.
 *
 * @param ctx Pointer to the code generation context.
 * @param start Pointer to the start of the variable name.
 * @param len Length of the variable name.
 * @param ty Token kind representing the variable's type.
 */
static void cgctx_push(CGCtx *ctx, const char *start, size_t len,
                       TokenKind ty) {
  if (ctx->len + 1 > ctx->cap) {
    ctx->cap = ctx->cap ? ctx->cap * 2 : 8;
    ctx->vars = realloc(ctx->vars, ctx->cap * sizeof(VarBinding));
  }
  ctx->vars[ctx->len++] = (VarBinding){start, len, ty, ctx->depth};
}

/**
 * @brief Enters a new scope in the code generation context.
 *
 * Increments the current scope depth to reflect the entry into a new scope.
 *
 * @param ctx Pointer to the code generation context.
 */
static void cgctx_scope_enter(CGCtx *ctx) { ctx->depth++; }

/**
 * @brief Exits the current scope in the code generation context.
 *
 * Removes variable bindings defined in the current scope and
 * decrements the scope depth.
 *
 * @param ctx Pointer to the code generation context.
 */
static void cgctx_scope_leave(CGCtx *ctx) {
  while (ctx->len && ctx->vars[ctx->len - 1].depth >= ctx->depth)
    ctx->len--;
  if (ctx->depth > 0)
    ctx->depth--;
}

/**
 * @brief Looks up a variable binding in the code generation context.
 *
 * Searches for a variable by name and length in the context's variable bindings
 * and returns its token kind if found.
 *
 * @param ctx Pointer to the code generation context.
 * @param start Pointer to the start of the variable name.
 * @param len Length of the variable name.
 * @return The token kind of the variable if found, or 0 if not found.
 */
static TokenKind cgctx_lookup(CGCtx *ctx, const char *start, size_t len) {
  for (size_t i = ctx->len; i-- > 0;) {
    VarBinding *v = &ctx->vars[i];
    if (v->len == len && strncmp(v->start, start, len) == 0)
      return v->type;
  }
  return (TokenKind)0;
}

/**
 * @brief Checks if a node represents a string expression.
 *
 * Determines whether the given node is a string literal or an identifier
 * with a string type in the current code generation context.
 *
 * @param ctx Pointer to the code generation context.
 * @param n Pointer to the node to check.
 * @return 1 if the node is a string expression, 0 otherwise.
 */
static int is_string_expr(CGCtx *ctx, Node *n) {
  switch (n->kind) {
  case ND_STRING:
    return 1;
  case ND_IDENT:
    return cgctx_lookup(ctx, n->as.ident.start, n->as.ident.len) ==
           TK_KW_STRING;
  default:
    return 0;
  }
}

/**
 * @brief Emits an expression node to the output buffer.
 *
 * Generates C code for the given expression node and writes it to the output buffer.
 *
 * @param ctx Pointer to the code generation context.
 * @param b Pointer to the output buffer.
 * @param n Pointer to the expression node.
 */
static void emit_expr(CGCtx *ctx, COut *b, Node *n);

/**
 * @brief Emits a statement node to the output buffer.
 *
 * Generates C code for the given statement node and writes it to the output buffer.
 *
 * @param ctx Pointer to the code generation context.
 * @param b Pointer to the output buffer.
 * @param n Pointer to the statement node.
 */
static void emit_stmt(CGCtx *ctx, COut *b, Node *n);

/**
 * @brief Emits a function node to the output buffer.
 *
 * Generates C code for the given function node and writes it to the output buffer.
 *
 * @param b Pointer to the output buffer.
 * @param n Pointer to the function node.
 */
static void emit_func(COut *b, Node *n);

/**
 * @brief Emits an expression node to the output buffer.
 *
 * Generates C code for the given expression node and writes it to the output buffer.
 * Handles various node kinds, including literals, identifiers, unary and binary operations,
 * conditional expressions, array indexing, and function calls.
 *
 * @param ctx Pointer to the code generation context.
 * @param b Pointer to the output buffer.
 * @param n Pointer to the expression node.
 */
static void emit_expr(CGCtx *ctx, COut *b, Node *n) {
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
    emit_expr(ctx, b, n->as.unary.expr);
    c_out_write(b, ")");
    break;
  case ND_POST_UNARY:
    c_out_write(b, "(");
    emit_expr(ctx, b, n->as.unary.expr);
    c_out_write(b, "%s", op_text(n->as.unary.op));
    c_out_write(b, ")");
    break;
  case ND_BINOP:
    c_out_write(b, "(");
    if (n->as.bin.op == TK_PLUS && (is_string_expr(ctx, n->as.bin.lhs) ||
                                    is_string_expr(ctx, n->as.bin.rhs))) {
      c_out_write(b, "dream_concat(");
      emit_expr(ctx, b, n->as.bin.lhs);
      c_out_write(b, ", ");
      emit_expr(ctx, b, n->as.bin.rhs);
      c_out_write(b, ")");
    } else {
      emit_expr(ctx, b, n->as.bin.lhs);
      c_out_write(b, " %s ", op_text(n->as.bin.op));
      emit_expr(ctx, b, n->as.bin.rhs);
    }
    c_out_write(b, ")");
    break;
  case ND_COND:
    c_out_write(b, "(");
    emit_expr(ctx, b, n->as.cond.cond);
    c_out_write(b, " ? ");
    emit_expr(ctx, b, n->as.cond.then_expr);
    c_out_write(b, " : ");
    emit_expr(ctx, b, n->as.cond.else_expr);
    c_out_write(b, ")");
    break;
  case ND_INDEX:
    c_out_write(b, "(");
    emit_expr(ctx, b, n->as.index.array);
    c_out_write(b, "[");
    emit_expr(ctx, b, n->as.index.index);
    c_out_write(b, "])");
    break;
  case ND_CALL:
    emit_expr(ctx, b, n->as.call.callee);
    c_out_write(b, "(");
    for (size_t i = 0; i < n->as.call.len; i++) {
      if (i)
        c_out_write(b, ", ");
      emit_expr(ctx, b, n->as.call.args[i]);
    }
    c_out_write(b, ")");
    break;
  default:
    c_out_write(b, "0");
    break;
  }
}

/**
 * @brief Converts a token kind to its corresponding C type as a string.
 *
 * Maps a given token kind to its string representation of the C type.
 *
 * @param k The token kind to convert.
 * @return The string representation of the C type.
 */
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

/**
 * @brief Emits a function node to the output buffer.
 *
 * Generates C code for the given function node, including its return type,
 * name, parameters, and body, and writes it to the output buffer.
 *
 * @param b Pointer to the output buffer.
 * @param n Pointer to the function node.
 */
static void emit_func(COut *b, Node *n) {
  c_out_write(b, "%s %.*s(", type_to_c(n->as.func.ret_type),
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
  cgctx_scope_enter(&ctx);
  for (size_t i = 0; i < n->as.func.param_len; i++) {
    Node *p = n->as.func.params[i];
    cgctx_push(&ctx, p->as.var_decl.name.start, p->as.var_decl.name.len,
               p->as.var_decl.type);
  }
  emit_stmt(&ctx, b, n->as.func.body);
  cgctx_scope_leave(&ctx);
  free(ctx.vars);
  c_out_newline(b);
}

/**
 * @brief Emits a statement node to the output buffer.
 *
 * Generates C code for the given statement node and writes it to the output buffer.
 * Handles various statement types, including variable declarations, function definitions,
 * control flow statements (if, while, do-while, for, switch), and console calls.
 *
 * @param ctx Pointer to the code generation context.
 * @param b Pointer to the output buffer.
 * @param n Pointer to the statement node.
 */
static void emit_stmt(CGCtx *ctx, COut *b, Node *n) {
  switch (n->kind) {
  case ND_VAR_DECL:
    if (n->as.var_decl.array_len > 0) {
      c_out_write(b, "%s %.*s[%zu]", type_to_c(n->as.var_decl.type),
                  (int)n->as.var_decl.name.len, n->as.var_decl.name.start,
                  n->as.var_decl.array_len);
      if (n->as.var_decl.init) {
        c_out_write(b, " = ");
        emit_expr(ctx, b, n->as.var_decl.init);
      }
    } else {
      c_out_write(b, "%s %.*s", type_to_c(n->as.var_decl.type),
                  (int)n->as.var_decl.name.len, n->as.var_decl.name.start);
      if (n->as.var_decl.init) {
        c_out_write(b, " = ");
        emit_expr(ctx, b, n->as.var_decl.init);
      }
    }
    cgctx_push(ctx, n->as.var_decl.name.start, n->as.var_decl.name.len,
               n->as.var_decl.type);
    c_out_write(b, ";");
    c_out_newline(b);
    break;
  case ND_FUNC:
    emit_func(b, n);
    break;
  case ND_IF:
    c_out_write(b, "if (");
    emit_expr(ctx, b, n->as.if_stmt.cond);
    c_out_write(b, ") ");
    emit_stmt(ctx, b, n->as.if_stmt.then_br);
    if (n->as.if_stmt.else_br) {
      c_out_write(b, " else ");
      emit_stmt(ctx, b, n->as.if_stmt.else_br);
    }
    break;
  case ND_WHILE:
    c_out_write(b, "while (");
    emit_expr(ctx, b, n->as.while_stmt.cond);
    c_out_write(b, ") ");
    emit_stmt(ctx, b, n->as.while_stmt.body);
    break;
  case ND_DO_WHILE:
    c_out_write(b, "do ");
    emit_stmt(ctx, b, n->as.do_while_stmt.body);
    c_out_write(b, " while (");
    emit_expr(ctx, b, n->as.do_while_stmt.cond);
    c_out_write(b, ");");
    c_out_newline(b);
    break;
  case ND_FOR:
    c_out_write(b, "for (");
    if (n->as.for_stmt.init) {
      if (n->as.for_stmt.init->kind == ND_VAR_DECL) {
        Node *vd = n->as.for_stmt.init;
        if (vd->as.var_decl.array_len > 0) {
          c_out_write(b, "%s %.*s[%zu]", type_to_c(vd->as.var_decl.type),
                      (int)vd->as.var_decl.name.len, vd->as.var_decl.name.start,
                      vd->as.var_decl.array_len);
          if (vd->as.var_decl.init) {
            c_out_write(b, " = ");
            emit_expr(ctx, b, vd->as.var_decl.init);
          }
        } else {
          c_out_write(b, "%s %.*s", type_to_c(vd->as.var_decl.type),
                      (int)vd->as.var_decl.name.len, vd->as.var_decl.name.start);
          if (vd->as.var_decl.init) {
            c_out_write(b, " = ");
            emit_expr(ctx, b, vd->as.var_decl.init);
          }
        }
        cgctx_push(ctx, vd->as.var_decl.name.start, vd->as.var_decl.name.len,
                   vd->as.var_decl.type);
      } else {
        emit_expr(ctx, b, n->as.for_stmt.init);
      }
    }
    c_out_write(b, "; ");
    if (n->as.for_stmt.cond)
      emit_expr(ctx, b, n->as.for_stmt.cond);
    c_out_write(b, "; ");
    if (n->as.for_stmt.update)
      emit_expr(ctx, b, n->as.for_stmt.update);
    c_out_write(b, ") ");
    emit_stmt(ctx, b, n->as.for_stmt.body);
    break;
  case ND_SWITCH:
    c_out_write(b, "switch (");
    emit_expr(ctx, b, n->as.switch_stmt.expr);
    c_out_write(b, ") {");
    c_out_newline(b);
    c_out_indent(b);
    for (size_t i = 0; i < n->as.switch_stmt.len; i++) {
      SwitchCase *sc = &n->as.switch_stmt.cases[i];
      if (sc->is_default) {
        c_out_write(b, "default:");
      } else {
        c_out_write(b, "case ");
        emit_expr(ctx, b, sc->value);
        c_out_write(b, ":");
      }
      c_out_newline(b);
      emit_stmt(ctx, b, sc->body);
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
      emit_expr(ctx, b, n->as.ret.expr);
    }
    c_out_write(b, ";");
    c_out_newline(b);
    break;
  case ND_BLOCK:
    c_out_write(b, "{");
    c_out_newline(b);
    c_out_indent(b);
    cgctx_scope_enter(ctx);
    for (size_t i = 0; i < n->as.block.len; i++)
      emit_stmt(ctx, b, n->as.block.items[i]);
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
      } else {
        c_out_write(b, "printf(\"");
        c_out_write(b, "%s", fmt_for_arg(call->as.console.arg));
        if (call->as.console.newline)
          c_out_write(b, "\\n");
        c_out_write(b, "\", ");
        emit_expr(ctx, b, call->as.console.arg);
        c_out_write(b, ");");
        c_out_newline(b);
      }
    } else {
      emit_expr(ctx, b, n->as.expr_stmt.expr);
      c_out_write(b, ";");
      c_out_newline(b);
    }
    break;
  case ND_CONSOLE_CALL:
    if (n->as.console.read) {
      c_out_write(b, "dream_readline()");
    } else {
      c_out_write(b, "printf(\"");
      c_out_write(b, "%s", fmt_for_arg(n->as.console.arg));
      if (n->as.console.newline)
        c_out_write(b, "\\n");
      c_out_write(b, "\", ");
      emit_expr(ctx, b, n->as.console.arg);
      c_out_write(b, ");");
    }
    c_out_newline(b);
    break;
  default:
    break;
  }
}

/**
 * @brief Emits C code for the given AST root node to the specified output file.
 *
 * Generates C code by traversing the abstract syntax tree (AST) and writing
 * the corresponding C code to the output file. Includes necessary headers,
 * helper functions, and the main function.
 *
 * @param root Pointer to the root node of the AST.
 * @param out Pointer to the output file.
 */
void codegen_emit_c(Node *root, FILE *out) {
  COut builder;
  c_out_init(&builder);
  c_out_write(&builder, "#include <stdio.h>");
  c_out_newline(&builder);
  c_out_write(&builder, "#include <string.h>");
  c_out_newline(&builder);
  c_out_write(&builder, "#include <stdlib.h>");
  c_out_newline(&builder);
  c_out_newline(&builder);
  c_out_write(&builder,
              "static char *dream_concat(const char *a, const char *b) {\n");
  c_out_write(&builder, "    const size_t la = strlen(a);\n");
  c_out_write(&builder, "    const size_t lb = strlen(b)\n");
  c_out_write(&builder, "    char *r = malloc(la + lb + 1);\n");
  c_out_write(&builder, "    memcpy(r, a, la);\n");
  c_out_write(&builder, "    memcpy(r + la, b, lb);\n");
  c_out_write(&builder, "    r[la + lb] = 0;\n");
  c_out_write(&builder, "    return r;\n}");
  c_out_newline(&builder);
  c_out_newline(&builder);
  c_out_write(&builder, "static char *dream_readline(void) {\n");
  c_out_write(&builder, "    static char buf[256];\n");
  c_out_write(&builder, "    if (!fgets(buf, sizeof buf, stdin)) buf[0] = 0;\n");
  c_out_write(&builder, "    size_t len = strlen(buf);\n");
  c_out_write(&builder, "    if (len && buf[len-1] == '\\n') buf[len-1] = 0;\n");
  c_out_write(&builder, "    return buf;\n}");
  c_out_newline(&builder);
  c_out_newline(&builder);
  for (size_t i = 0; i < root->as.block.len; i++) {
    Node *it = root->as.block.items[i];
    if (it->kind == ND_FUNC)
      emit_func(&builder, it);
  }
  c_out_write(&builder, "int main(void) ");
  CGCtx ctx = {0};
  cgctx_scope_enter(&ctx);
  for (size_t i = 0; i < root->as.block.len; i++) {
    Node *it = root->as.block.items[i];
    if (it->kind != ND_FUNC)
      emit_stmt(&ctx, &builder, it);
  }
  cgctx_scope_leave(&ctx);
  free(ctx.vars);
  c_out_newline(&builder);
  c_out_dump(out, &builder);
  c_out_free(&builder);
}

/**
 * @brief Emits an object file for the given AST root node.
 *
 * Generates an intermediate C file from the AST, compiles it into an object file,
 * and writes the result to the specified path. Handles platform-specific
 * temporary file creation and cleanup.
 *
 * @param root Pointer to the root node of the AST.
 * @param path Path to the output object file.
 */
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
