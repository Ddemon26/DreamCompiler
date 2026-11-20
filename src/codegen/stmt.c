#include "stmt.h"
#include "../sem/analysis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static CGTypeInfo *g_types = NULL;
static size_t g_type_len = 0;
static Node *g_program = NULL;

void cg_register_program(Node *program) { g_program = program; }

Node *cg_function(Slice name) {
  if (!g_program) return NULL;
  for (size_t i = 0; i < g_program->as.block.len; ++i) {
    Node *node = g_program->as.block.items[i];
    if (node->kind == ND_FUNC && node->as.func.name.len == name.len &&
        memcmp(node->as.func.name.start, name.start, name.len) == 0) return node;
  }
  return NULL;
}

static void emit_func_impl(COut *b, Slice prefix, Node *n,
                           const char *src_file, bool external);

/**
 * Emits a #line directive for enhanced debugging if the node has position info.
 * This provides more granular source mapping for debugging.
 *
 * @param b Output builder
 * @param n AST node with position information
 * @param src_file Source file name
 * @param prefix Optional comment prefix describing the Dream construct
 */
static void emit_debug_line(COut *b, Node *n, const char *src_file, const char *prefix) {
  if (n->pos.line) {
    if (!b->at_line_start)
      c_out_newline(b);
    c_out_write(b, "#line %zu \"%s\"\n", n->pos.line, src_file);
    if (prefix) {
      c_out_write(b, "/* Debug: %s at line %zu */\n", prefix, n->pos.line);
    }
  }
}

void cg_register_types(CGTypeInfo *types, size_t n) {
  g_types = types;
  g_type_len = n;
}

Node *cg_type_decl(Slice name) {
  for (size_t i = 0; i < g_type_len; ++i) {
    if (g_types[i].name.len == name.len &&
        strncmp(g_types[i].name.start, name.start, name.len) == 0) return g_types[i].decl;
  }
  return NULL;
}

Node *cg_member(Slice type, Slice name, Node **owner) {
  return sem_find_member(cg_type_decl(type), name, owner);
}

int cg_is_class_type(Slice name) {
  for (size_t i = 0; i < g_type_len; i++) {
    if (g_types[i].name.len == name.len &&
        strncmp(g_types[i].name.start, name.start, name.len) == 0)
      return g_types[i].is_class;
  }
  return 0;
}

int cg_is_known_type(Slice name) {
  for (size_t i = 0; i < g_type_len; i++) {
    if (g_types[i].name.len == name.len &&
        strncmp(g_types[i].name.start, name.start, name.len) == 0)
      return 1;
  }
  return 0;
}

int cg_has_init(Slice name) {
  for (size_t i = 0; i < g_type_len; i++) {
    if (g_types[i].name.len == name.len &&
        strncmp(g_types[i].name.start, name.start, name.len) == 0)
      return g_types[i].has_init;
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
  case TK_KW_TASK:
    return "Task*";
  case TK_KW_TASKRESULT:
    return "TaskResult";
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

static void emit_type_with_pointer(COut *b, TokenKind k, Slice name, int is_pointer) {
  if (k == TK_KW_TASK && is_pointer) {
    // Task* in Dream should map to Task* in C, not Task**
    c_out_write(b, "Task*");
  } else {
    emit_type(b, k, name);
    if (is_pointer) {
      c_out_write(b, "*");
    }
  }
}

void emit_func_prototype(COut *b, Node *n, bool external) {
  bool main_function = n->as.func.name.len == 4 && memcmp(n->as.func.name.start, "main", 4) == 0;
  c_out_write(b, "%s%s %.*s(", (main_function || external) ? "" : "static ",
              n->as.func.is_async ? "Task*" : type_to_c(n->as.func.ret_type),
              (int)n->as.func.name.len, n->as.func.name.start);
  if (!n->as.func.param_len) c_out_write(b, "void");
  for (size_t i = 0; i < n->as.func.param_len; ++i) {
    if (i) c_out_write(b, ", ");
    Node *p = n->as.func.params[i];
    emit_type_with_pointer(b, p->as.var_decl.type, p->as.var_decl.type_name,
                           p->as.var_decl.is_pointer);
  }
  c_out_write(b, ");\n");
}

void emit_type_decl(COut *b, Node *n, const char *src_file) {
  for (size_t i = 0; i < g_type_len; ++i) {
    if (g_types[i].decl == n) {
      if (g_types[i].emitted) return;
      g_types[i].emitted = 1;
      break;
    }
  }
  if (n->as.type_decl.base_decl) emit_type_decl(b, n->as.type_decl.base_decl, src_file);
  const char *type_kind = (n->kind == ND_CLASS_DECL) ? "class" : "struct";
  c_out_write(b, "/* Dream %s %.*s", type_kind, (int)n->as.type_decl.name.len,
              n->as.type_decl.name.start);
  if (n->kind == ND_CLASS_DECL && n->as.type_decl.base_name.len > 0) {
    c_out_write(b, " : %.*s", (int)n->as.type_decl.base_name.len,
                n->as.type_decl.base_name.start);
  }
  c_out_write(b, " with %zu member%s at line %zu */\n", n->as.type_decl.len,
              n->as.type_decl.len == 1 ? "" : "s", n->pos.line);
  
  c_out_write(b, "struct %.*s {", (int)n->as.type_decl.name.len,
              n->as.type_decl.name.start);
  c_out_newline(b);
  c_out_indent(b);
  
  // An actual base subobject preserves its layout and makes upcasts well-defined.
  if (n->as.type_decl.base_decl) {
    Slice base_name = n->as.type_decl.base_decl->as.type_decl.name;
    c_out_write(b, "struct %.*s base;\n", (int)base_name.len, base_name.start);
  }
  
  for (size_t i = 0; i < n->as.type_decl.len; i++) {
    Node *m = n->as.type_decl.members[i];
    if (m->kind == ND_VAR_DECL && !m->as.var_decl.is_static) {
      emit_type_with_pointer(b, m->as.var_decl.type, m->as.var_decl.type_name, m->as.var_decl.is_pointer);
      c_out_write(b, " %.*s", (int)m->as.var_decl.name.len,
                  m->as.var_decl.name.start);
      if (m->as.var_decl.array_len) c_out_write(b, "[%zu]", m->as.var_decl.array_len);
      c_out_write(b, ";");
      c_out_newline(b);
    }
  }
  c_out_dedent(b);
  c_out_write(b, "};");
  c_out_newline(b);
  c_out_newline(b);
  for (size_t i = 0; i < n->as.type_decl.len; i++) {
    Node *m = n->as.type_decl.members[i];
    if (m->kind == ND_VAR_DECL && m->as.var_decl.is_static) {
      c_out_write(b, "%s %.*s_%.*s", type_to_c(m->as.var_decl.type),
                  (int)n->as.type_decl.name.len, n->as.type_decl.name.start,
                  (int)m->as.var_decl.name.len, m->as.var_decl.name.start);
      if (m->as.var_decl.init) {
        c_out_write(b, " = ");
        CGCtx ctx = {0};
        cg_emit_expr(&ctx, b, m->as.var_decl.init);
      }
      c_out_write(b, ";\n");
    }
  }
  for (size_t i = 0; i < n->as.type_decl.len; i++) {
    Node *m = n->as.type_decl.members[i];
    if (m->kind == ND_FUNC) {
      if (m->as.func.is_static)
        emit_func_impl(b, n->as.type_decl.name, m, src_file, false);
      else
        emit_method(b, n->as.type_decl.name, m, src_file);
    }
  }
  c_out_newline(b);
}

void emit_enum_decl(COut *b, Node *n, const char *src_file) {
  c_out_write(b, "/* Dream enum %.*s with %zu member%s at line %zu */\n", 
              (int)n->as.enum_decl.name.len, n->as.enum_decl.name.start,
              n->as.enum_decl.len, n->as.enum_decl.len == 1 ? "" : "s", n->pos.line);
  
  c_out_write(b, "enum %.*s {", (int)n->as.enum_decl.name.len, n->as.enum_decl.name.start);
  c_out_newline(b);
  c_out_indent(b);
  
  for (size_t i = 0; i < n->as.enum_decl.len; i++) {
    Node *member = n->as.enum_decl.members[i];
    c_out_write(b, "%.*s", (int)member->as.var_decl.name.len, member->as.var_decl.name.start);
    
    if (member->as.var_decl.init) {
      c_out_write(b, " = ");
      CGCtx ctx = {0};
      cg_emit_expr(&ctx, b, member->as.var_decl.init);
    }
    
    if (i < n->as.enum_decl.len - 1) {
      c_out_write(b, ",");
    }
    c_out_newline(b);
  }
  
  c_out_dedent(b);
  c_out_write(b, "};");
  c_out_newline(b);
  c_out_newline(b);
}

static void emit_func_impl(COut *b, Slice prefix, Node *n,
                           const char *src_file, bool external) {
  if (n->pos.line)
    c_out_write(b, "#line %zu \"%s\"\n", n->pos.line, src_file);
  c_out_write(b, "/* Dream function %.*s", (int)n->as.func.name.len,
              n->as.func.name.start);
  if (n->as.func.param_len > 0) {
    c_out_write(b, " with %zu parameter%s", n->as.func.param_len, 
                n->as.func.param_len == 1 ? "" : "s");
  }
  if (n->as.func.is_async) {
    c_out_write(b, " (async)");
  }
  c_out_write(b, " at line %zu */\n", n->pos.line);
  
  // For async functions, we need to emit a wrapper that returns Task* and a worker function
  if (n->as.func.is_async) {
    // If function has parameters, emit a parameter struct
    if (n->as.func.param_len > 0) {
      c_out_write(b, "typedef struct {\n");
      c_out_indent(b);
      for (size_t i = 0; i < n->as.func.param_len; i++) {
        Node *p = n->as.func.params[i];
        emit_type_with_pointer(b, p->as.var_decl.type, p->as.var_decl.type_name, p->as.var_decl.is_pointer);
        c_out_write(b, " %.*s;\n", (int)p->as.var_decl.name.len, p->as.var_decl.name.start);
      }
      c_out_dedent(b);
      c_out_write(b, "} %.*s_args;\n\n", (int)n->as.func.name.len, n->as.func.name.start);
    }
    
    // Create a global task variable for this worker function
    c_out_write(b, "static Task* %.*s_current_task = NULL;\n\n", 
                (int)n->as.func.name.len, n->as.func.name.start);
    
    // Emit the worker function that contains the actual logic
    c_out_write(b, "static void* %.*s_worker(void* arg) {\n", 
                (int)n->as.func.name.len, n->as.func.name.start);
    c_out_indent(b);
    
    // Extract parameters from arg if any
    if (n->as.func.param_len > 0) {
      c_out_write(b, "%.*s_args* params = (%.*s_args*)arg;\n",
                  (int)n->as.func.name.len, n->as.func.name.start,
                  (int)n->as.func.name.len, n->as.func.name.start);
      for (size_t i = 0; i < n->as.func.param_len; i++) {
        Node *p = n->as.func.params[i];
        emit_type_with_pointer(b, p->as.var_decl.type, p->as.var_decl.type_name, p->as.var_decl.is_pointer);
        c_out_write(b, " %.*s = params->%.*s;\n", 
                    (int)p->as.var_decl.name.len, p->as.var_decl.name.start,
                    (int)p->as.var_decl.name.len, p->as.var_decl.name.start);
      }
    }
    
    // Create a context to track return value handling
    CGCtx ctx = {0};
    ctx.ret_type = n->as.func.ret_type;
    ctx.is_async_worker = 1;
    ctx.async_func_name = n->as.func.name;
    cgctx_scope_enter(&ctx);
    
    // Handle the function body with return value interception
    cg_emit_stmt(&ctx, b, n->as.func.body, src_file);
    
    cgctx_scope_leave(&ctx);
    free(ctx.vars);
    
    c_out_write(b, "return NULL;\n");
    c_out_dedent(b);
    c_out_write(b, "}\n\n");
    
    // Now emit the wrapper function that creates and returns a Task
    if (prefix.len)
      c_out_write(b, "static Task* %.*s_%.*s(", 
                  (int)prefix.len, prefix.start, (int)n->as.func.name.len,
                  n->as.func.name.start);
    else
      c_out_write(b, "static Task* %.*s(", 
                  (int)n->as.func.name.len, n->as.func.name.start);
  } else {
    // Regular synchronous function
    if (prefix.len)
      c_out_write(b, "static %s %.*s_%.*s(", type_to_c(n->as.func.ret_type),
                  (int)prefix.len, prefix.start, (int)n->as.func.name.len,
                  n->as.func.name.start);
    else if (external || (n->as.func.name.len == 4 &&
             strncmp(n->as.func.name.start, "main", 4) == 0))
      c_out_write(b, "%s %.*s(", type_to_c(n->as.func.ret_type),
                  (int)n->as.func.name.len, n->as.func.name.start);
    else
      c_out_write(b, "static %s %.*s(", type_to_c(n->as.func.ret_type),
                  (int)n->as.func.name.len, n->as.func.name.start);
  }
  for (size_t i = 0; i < n->as.func.param_len; i++) {
    Node *p = n->as.func.params[i];
    if (i)
      c_out_write(b, ", ");
    emit_type_with_pointer(b, p->as.var_decl.type, p->as.var_decl.type_name,
                           p->as.var_decl.is_pointer);
    c_out_write(b, " %.*s",
                (int)p->as.var_decl.name.len, p->as.var_decl.name.start);
  }
  c_out_write(b, ") ");
  
  if (n->as.func.is_async) {
    // For async functions, create task and return it
    c_out_write(b, "{\n");
    c_out_indent(b);
    
    c_out_write(b, "Task* task;\n");
    if (n->as.func.param_len > 0) {
      c_out_write(b, "%.*s_args* args = dr_alloc(sizeof(%.*s_args));\n",
                  (int)n->as.func.name.len, n->as.func.name.start,
                  (int)n->as.func.name.len, n->as.func.name.start);
      for (size_t i = 0; i < n->as.func.param_len; i++) {
        Node *p = n->as.func.params[i];
        c_out_write(b, "args->%.*s = %.*s;\n",
                    (int)p->as.var_decl.name.len, p->as.var_decl.name.start,
                    (int)p->as.var_decl.name.len, p->as.var_decl.name.start);
      }
      c_out_write(b, "task = dr_task_create(%.*s_worker, args);\n", 
                  (int)n->as.func.name.len, n->as.func.name.start);
    } else {
      c_out_write(b, "task = dr_task_create(%.*s_worker, NULL);\n", 
                  (int)n->as.func.name.len, n->as.func.name.start);
    }
    c_out_write(b, "%.*s_current_task = task;\n", 
                (int)n->as.func.name.len, n->as.func.name.start);
    c_out_write(b, "return task;\n");
    
    c_out_dedent(b);
    c_out_write(b, "}\n");
  } else {
    // Regular function - emit the function body
    CGCtx ctx = {0};
    ctx.ret_type = n->as.func.ret_type;
    ctx.is_sync_function = 1;
    ctx.function_body = n->as.func.body;
    cgctx_scope_enter(&ctx);
    for (size_t i = 0; i < n->as.func.param_len; i++) {
      Node *p = n->as.func.params[i];
      cgctx_push_decl(&ctx, p);
    }
    cg_emit_stmt(&ctx, b, n->as.func.body, src_file);
    cgctx_scope_leave(&ctx);
    free(ctx.vars);
  }
  c_out_newline(b);
}

void emit_func(COut *b, Node *n, const char *src_file, bool external) {
  emit_func_impl(b, (Slice){NULL, 0}, n, src_file, external);
}

void emit_method(COut *b, Slice class_name, Node *n, const char *src_file) {
  // implicit this parameter
  // copy params with extra first param
  c_out_write(b, "/* Dream method %.*s.%.*s", (int)class_name.len,
              class_name.start, (int)n->as.func.name.len,
              n->as.func.name.start);
  if (n->as.func.param_len > 0) {
    c_out_write(b, " with %zu parameter%s", n->as.func.param_len, 
                n->as.func.param_len == 1 ? "" : "s");
  }
  if (n->as.func.is_async) {
    c_out_write(b, " (async)");
  }
  c_out_write(b, " at line %zu */\n", n->pos.line);
  c_out_write(b, "static %s %.*s_%.*s(struct %.*s *this",
              type_to_c(n->as.func.ret_type), (int)class_name.len,
              class_name.start, (int)n->as.func.name.len, n->as.func.name.start,
              (int)class_name.len, class_name.start);
  for (size_t i = 0; i < n->as.func.param_len; i++) {
    Node *p = n->as.func.params[i];
    c_out_write(b, ", ");
    emit_type_with_pointer(b, p->as.var_decl.type, p->as.var_decl.type_name,
                           p->as.var_decl.is_pointer);
    c_out_write(b, " %.*s",
                (int)p->as.var_decl.name.len, p->as.var_decl.name.start);
  }
  c_out_write(b, ") ");
  CGCtx ctx = {0};
  ctx.ret_type = n->as.func.ret_type;
  ctx.is_sync_function = !n->as.func.is_async;
  ctx.function_body = n->as.func.body;
  cgctx_scope_enter(&ctx);
  cgctx_push(&ctx, "this", 4, TK_IDENT, class_name);
  for (size_t i = 0; i < n->as.func.param_len; i++) {
    Node *p = n->as.func.params[i];
    cgctx_push_decl(&ctx, p);
  }
  cg_emit_stmt(&ctx, b, n->as.func.body, src_file);
  cgctx_scope_leave(&ctx);
  free(ctx.vars);
  c_out_newline(b);
}

static void emit_scope_cleanup(CGCtx *ctx, COut *b, size_t start) {
  for (size_t i = ctx->len; i-- > start;) {
    VarBinding *v = &ctx->vars[i];
    // String arrays are C storage and explicit pointers borrow their target.
    // Neither binding owns a scalar string allocation or its borrowed elements.
    if ((v->type == TK_KW_STRING && v->indirections == 0) ||
        (v->type == TK_IDENT && cg_is_class_type(v->type_name))) {
      c_out_write(b, "dr_release(%.*s);\n", (int)v->len, v->start);
    }
  }
}

static int return_releases_binding(const VarBinding *v) {
  // Depth 1 holds borrowed parameters and this. Class containers stay separate.
  return v->owns_string ||
         (v->depth > 1 && v->indirections == 0 && v->type == TK_IDENT &&
          cg_is_class_type(v->type_name));
}

static int has_return_cleanup(CGCtx *ctx) {
  for (size_t i = 0; i < ctx->len; i++)
    if (return_releases_binding(&ctx->vars[i])) return 1;
  return 0;
}

static void emit_return_cleanup(CGCtx *ctx, COut *b,
                                 const VarBinding *preserved) {
  for (size_t i = ctx->len; i-- > 0;) {
    VarBinding *v = &ctx->vars[i];
    if (v != preserved && return_releases_binding(v))
      c_out_write(b, "dr_release(%.*s);\n", (int)v->len, v->start);
  }
}

static const VarBinding *owned_string_result(CGCtx *ctx, Node *expr) {
  if (ctx->ret_type != TK_KW_STRING || !expr || expr->kind != ND_IDENT)
    return NULL;
  const VarBinding *v = cgctx_binding(ctx, expr->as.ident.start,
                                    expr->as.ident.len);
  // A direct owned local already has independent storage. Preserve that one
  // allocation for the caller; do not infer ownership through other expressions.
  return v && v->depth > 1 && v->type == TK_KW_STRING &&
         v->indirections == 0 && v->owns_string ? v : NULL;
}

static int names_binding(Slice name, const VarBinding *v) {
  return name.len == v->len && memcmp(name.start, v->start, v->len) == 0;
}

static int parameter_unchanged(const Node *n, const VarBinding *v) {
  if (!n) return 1;
  // Scan the entire body, including later statements and loop updates. A
  // parameter rebound to local storage no longer proves a caller-backed result.
  // Same-name declarations conservatively disqualify rather than infer aliases.
  switch (n->kind) {
  case ND_BINOP:
    switch (n->as.bin.op) {
    case TK_EQ: case TK_PLUSEQ: case TK_MINUSEQ: case TK_STAREQ:
    case TK_SLASHEQ: case TK_PERCENTEQ: case TK_ANDEQ: case TK_OREQ:
    case TK_XOREQ: case TK_LSHIFTEQ: case TK_RSHIFTEQ: case TK_QMARKQMARKEQ:
      if (n->as.bin.lhs->kind == ND_IDENT &&
          names_binding(n->as.bin.lhs->as.ident, v)) return 0;
      break;
    default: break;
    }
    return parameter_unchanged(n->as.bin.lhs, v) &&
           parameter_unchanged(n->as.bin.rhs, v);
  case ND_UNARY: case ND_POST_UNARY:
    if ((n->as.unary.op == TK_PLUSPLUS || n->as.unary.op == TK_MINUSMINUS) &&
        n->as.unary.expr->kind == ND_IDENT &&
        names_binding(n->as.unary.expr->as.ident, v)) return 0;
    return parameter_unchanged(n->as.unary.expr, v);
  case ND_VAR_DECL:
    return !names_binding(n->as.var_decl.name, v) &&
           parameter_unchanged(n->as.var_decl.init, v);
  case ND_BLOCK:
    for (size_t i = 0; i < n->as.block.len; i++)
      if (!parameter_unchanged(n->as.block.items[i], v)) return 0;
    return 1;
  case ND_IF:
    return parameter_unchanged(n->as.if_stmt.cond, v) &&
           parameter_unchanged(n->as.if_stmt.then_br, v) &&
           parameter_unchanged(n->as.if_stmt.else_br, v);
  case ND_COND:
    return parameter_unchanged(n->as.cond.cond, v) &&
           parameter_unchanged(n->as.cond.then_expr, v) &&
           parameter_unchanged(n->as.cond.else_expr, v);
  case ND_WHILE:
    return parameter_unchanged(n->as.while_stmt.cond, v) &&
           parameter_unchanged(n->as.while_stmt.body, v);
  case ND_DO_WHILE:
    return parameter_unchanged(n->as.do_while_stmt.body, v) &&
           parameter_unchanged(n->as.do_while_stmt.cond, v);
  case ND_FOR:
    return parameter_unchanged(n->as.for_stmt.init, v) &&
           parameter_unchanged(n->as.for_stmt.cond, v) &&
           parameter_unchanged(n->as.for_stmt.update, v) &&
           parameter_unchanged(n->as.for_stmt.body, v);
  case ND_SWITCH:
    if (!parameter_unchanged(n->as.switch_stmt.expr, v)) return 0;
    for (size_t i = 0; i < n->as.switch_stmt.len; i++)
      if (!parameter_unchanged(n->as.switch_stmt.cases[i].value, v) ||
          !parameter_unchanged(n->as.switch_stmt.cases[i].body, v)) return 0;
    return 1;
  case ND_TRY:
    return !names_binding(n->as.try_stmt.catch_param, v) &&
           parameter_unchanged(n->as.try_stmt.body, v) &&
           parameter_unchanged(n->as.try_stmt.catch_body, v) &&
           parameter_unchanged(n->as.try_stmt.finally_body, v);
  case ND_CALL:
    if (!parameter_unchanged(n->as.call.callee, v)) return 0;
    for (size_t i = 0; i < n->as.call.len; i++)
      if (!parameter_unchanged(n->as.call.args[i], v)) return 0;
    return 1;
  case ND_NEW:
    for (size_t i = 0; i < n->as.new_expr.arg_len; i++)
      if (!parameter_unchanged(n->as.new_expr.args[i], v)) return 0;
    return 1;
  case ND_INDEX:
    return parameter_unchanged(n->as.index.array, v) &&
           parameter_unchanged(n->as.index.index, v);
  case ND_FIELD: return parameter_unchanged(n->as.field.object, v);
  case ND_RETURN: return parameter_unchanged(n->as.ret.expr, v);
  case ND_EXPR_STMT: return parameter_unchanged(n->as.expr_stmt.expr, v);
  case ND_CONSOLE_CALL: return parameter_unchanged(n->as.console.arg, v);
  case ND_THROW: return parameter_unchanged(n->as.throw_stmt.expr, v);
  case ND_AWAIT: return parameter_unchanged(n->as.await_expr.expr, v);
  case ND_INT: case ND_FLOAT: case ND_CHAR: case ND_STRING: case ND_BOOL:
  case ND_NULL: case ND_IDENT: case ND_BASE: case ND_BREAK: case ND_CONTINUE:
    return 1;
  default: return 0; // Unknown/nested declarations do not establish the proof.
  }
}

static int borrowed_parameter_result(CGCtx *ctx, Node *expr) {
  if (ctx->ret_type != TK_KW_STRING || !ctx->function_body ||
      !expr || expr->kind != ND_IDENT) return 0;
  const VarBinding *v = cgctx_binding(ctx, expr->as.ident.start,
                                    expr->as.ident.len);
  return v && v->depth == 1 && v->type == TK_KW_STRING &&
         v->indirections == 0 && !v->owns_string &&
         parameter_unchanged(ctx->function_body, v);
}

// An unchanged receiver slot alone says nothing about field backing. Keep this
// proof separate from parameter_unchanged: even unrelated writes/calls may act
// through aliases, and class locals can release an unretained receiver alias at
// an already-exited block. Unknown nodes/operators fail closed.
static int member_body_readonly(const Node *n, Slice guarded_name) {
  if (!n) return 1;
  switch (n->kind) {
  case ND_BINOP:
    switch (n->as.bin.op) {
    case TK_PLUS: case TK_MINUS: case TK_STAR: case TK_SLASH: case TK_PERCENT:
    case TK_CARET: case TK_LSHIFT: case TK_RSHIFT: case TK_AND: case TK_OR:
    case TK_LT: case TK_GT: case TK_LTEQ: case TK_GTEQ: case TK_EQEQ: case TK_NEQ:
    case TK_ANDAND: case TK_OROR: case TK_QMARKQMARK:
      return member_body_readonly(n->as.bin.lhs, guarded_name) &&
             member_body_readonly(n->as.bin.rhs, guarded_name);
    default: return 0;
    }
  case ND_UNARY:
    switch (n->as.unary.op) {
    case TK_PLUS: case TK_MINUS: case TK_BANG: case TK_TILDE:
      return member_body_readonly(n->as.unary.expr, guarded_name);
    default: return 0;
    }
  case ND_VAR_DECL:
    switch (n->as.var_decl.type) {
    case TK_KW_INT: case TK_KW_FLOAT: case TK_KW_BOOL: case TK_KW_CHAR:
    case TK_KW_STRING: break;
    default: return 0;
    }
    return !n->as.var_decl.is_pointer && !n->as.var_decl.array_len &&
           !(n->as.var_decl.name.len == guarded_name.len &&
             memcmp(n->as.var_decl.name.start, guarded_name.start, guarded_name.len) == 0) &&
           member_body_readonly(n->as.var_decl.init, guarded_name);
  case ND_BLOCK:
    for (size_t i = 0; i < n->as.block.len; i++)
      if (!member_body_readonly(n->as.block.items[i], guarded_name)) return 0;
    return 1;
  case ND_IF:
    return member_body_readonly(n->as.if_stmt.cond, guarded_name) &&
           member_body_readonly(n->as.if_stmt.then_br, guarded_name) &&
           member_body_readonly(n->as.if_stmt.else_br, guarded_name);
  case ND_COND:
    return member_body_readonly(n->as.cond.cond, guarded_name) &&
           member_body_readonly(n->as.cond.then_expr, guarded_name) &&
           member_body_readonly(n->as.cond.else_expr, guarded_name);
  case ND_WHILE:
    return member_body_readonly(n->as.while_stmt.cond, guarded_name) &&
           member_body_readonly(n->as.while_stmt.body, guarded_name);
  case ND_DO_WHILE:
    return member_body_readonly(n->as.do_while_stmt.body, guarded_name) &&
           member_body_readonly(n->as.do_while_stmt.cond, guarded_name);
  case ND_FOR:
    return member_body_readonly(n->as.for_stmt.init, guarded_name) &&
           member_body_readonly(n->as.for_stmt.cond, guarded_name) &&
           member_body_readonly(n->as.for_stmt.update, guarded_name) &&
           member_body_readonly(n->as.for_stmt.body, guarded_name);
  case ND_SWITCH:
    if (!member_body_readonly(n->as.switch_stmt.expr, guarded_name)) return 0;
    for (size_t i = 0; i < n->as.switch_stmt.len; i++)
      if (!member_body_readonly(n->as.switch_stmt.cases[i].value, guarded_name) ||
          !member_body_readonly(n->as.switch_stmt.cases[i].body, guarded_name)) return 0;
    return 1;
  case ND_INDEX:
    return member_body_readonly(n->as.index.array, guarded_name) &&
           member_body_readonly(n->as.index.index, guarded_name);
  case ND_FIELD: return member_body_readonly(n->as.field.object, guarded_name);
  case ND_RETURN: return member_body_readonly(n->as.ret.expr, guarded_name);
  case ND_EXPR_STMT: return member_body_readonly(n->as.expr_stmt.expr, guarded_name);
  case ND_INT: case ND_FLOAT: case ND_CHAR: case ND_STRING: case ND_BOOL:
  case ND_NULL: case ND_IDENT: case ND_BASE: case ND_BREAK: case ND_CONTINUE:
    return 1;
  default: return 0;
  }
}

static int borrowed_member_result(CGCtx *ctx, Node *expr) {
  if (ctx->ret_type != TK_KW_STRING || !ctx->function_body ||
      !expr || expr->kind != ND_FIELD ||
      expr->as.field.object->kind != ND_IDENT) return 0;
  Slice name = expr->as.field.object->as.ident;
  const VarBinding *receiver = cgctx_binding(ctx, name.start, name.len);
  if (!receiver || receiver->depth != 1 || receiver->type != TK_IDENT ||
      receiver->indirections != 0 || !cg_is_class_type(receiver->type_name)) return 0;
  // Resolve actual declaration/shape, including inherited owner lookup. Normal
  // expression emission supplies that owner's cast; token typing is insufficient.
  Node *field = cg_member(receiver->type_name, expr->as.field.name, NULL);
  return field && field->kind == ND_VAR_DECL &&
         field->as.var_decl.type == TK_KW_STRING &&
         !field->as.var_decl.is_static && !field->as.var_decl.is_pointer &&
         !field->as.var_decl.array_len &&
         member_body_readonly(ctx->function_body,
                              (Slice){receiver->start, receiver->len});
}

// A global slot does not own its backing. Only direct, unshadowed class
// qualifiers and the same whole-body read-only proof establish independence
// from callee locals. Conditional trees qualify each value leaf separately.
static int borrowed_static_member_result(CGCtx *ctx, Node *expr) {
  if (ctx->ret_type != TK_KW_STRING || !ctx->function_body ||
      !expr || expr->kind != ND_FIELD ||
      expr->as.field.object->kind != ND_IDENT) return 0;
  Slice qualifier = expr->as.field.object->as.ident;
  if (cgctx_has_var(ctx, qualifier.start, qualifier.len) ||
      !cg_is_class_type(qualifier)) return 0;
  Node *owner = NULL;
  Node *field = cg_member(qualifier, expr->as.field.name, &owner);
  return owner && owner->kind == ND_CLASS_DECL &&
         field && field->kind == ND_VAR_DECL &&
         field->as.var_decl.is_static &&
         field->as.var_decl.type == TK_KW_STRING &&
         !field->as.var_decl.is_pointer && !field->as.var_decl.array_len &&
         member_body_readonly(ctx->function_body, qualifier);
}

static int is_primitive_scalar_result(TokenKind type) {
  return type == TK_KW_INT || type == TK_KW_FLOAT ||
         type == TK_KW_BOOL || type == TK_KW_CHAR;
}

static int is_static_string_result(const Node *expr) {
  if (!expr) return 0;
  if (expr->kind == ND_STRING) return 1;
  // Only value branches determine result lifetime. Conditions are evaluated
  // before cleanup and may read locals or have side effects.
  return expr->kind == ND_COND &&
         is_static_string_result(expr->as.cond.then_expr) &&
         is_static_string_result(expr->as.cond.else_expr);
}

static int is_scoped_string_result(CGCtx *ctx, Node *expr) {
  if (!expr) return 0;
  if (expr->kind == ND_COND)
    return is_scoped_string_result(ctx, expr->as.cond.then_expr) &&
           is_scoped_string_result(ctx, expr->as.cond.else_expr);
  // Every possible value must survive cleanup. Reuse the whole-body proof:
  // even a write in the condition or a later loop update can rebind a parameter
  // or field to local storage. Member leaves require the separate read-only
  // body proof, including conditions and unselected branches. One unsupported
  // leaf preserves the entire old return.
  return expr->kind == ND_STRING || owned_string_result(ctx, expr) ||
         borrowed_parameter_result(ctx, expr) || borrowed_member_result(ctx, expr) ||
         borrowed_static_member_result(ctx, expr);
}

static int result_mentions_binding(CGCtx *ctx, Node *expr,
                                   const VarBinding *v) {
  if (expr->kind == ND_COND)
    return result_mentions_binding(ctx, expr->as.cond.then_expr, v) ||
           result_mentions_binding(ctx, expr->as.cond.else_expr, v);
  return owned_string_result(ctx, expr) == v;
}

static void emit_conditional_return_cleanup(CGCtx *ctx, COut *b,
                                            Node *expr, const char *temporary) {
  for (size_t i = ctx->len; i-- > 0;) {
    VarBinding *v = &ctx->vars[i];
    if (!return_releases_binding(v)) continue;
    // Independently owned local slots cannot share a live allocation. Compare
    // after evaluating the result: conditions may have changed a slot, and must
    // never be evaluated again. Null comparisons only omit a no-op release.
    if (result_mentions_binding(ctx, expr, v))
      c_out_write(b, "if (%.*s != %s) ", (int)v->len, v->start, temporary);
    c_out_write(b, "dr_release(%.*s);\n", (int)v->len, v->start);
  }
}

static int copies_string_initializer(Node *init) {
  return init && (init->kind == ND_INDEX || init->kind == ND_STRING ||
                  init->kind == ND_IDENT || init->kind == ND_FIELD ||
                  init->kind == ND_BASE || init->kind == ND_CALL ||
                  init->kind == ND_COND ||
                  (init->kind == ND_BINOP &&
                   (init->as.bin.op == TK_EQ || init->as.bin.op == TK_PLUSEQ)));
}

static Node *string_factory_definition(Node *call) {
  if (!call || call->kind != ND_CALL || !call->as.call.callee ||
      call->as.call.callee->kind != ND_IDENT) return NULL;
  Node *fn = cg_function(call->as.call.callee->as.ident);
  if (!fn || fn->as.func.is_async || fn->as.func.ret_type != TK_KW_STRING ||
      fn->as.func.param_len != call->as.call.len) return NULL;
  return fn;
}

/* Return cleanup eligibility alone does not prove ownership. Declarations,
 * complete assignments, standalone calls and direct Console statements can
 * consume a proven direct append. */
static int string_factory_result(Node *fn, int allow_append) {
  Node *body = fn->as.func.body;
  if (!body || body->kind != ND_BLOCK || body->as.block.len != 2) return 0;
  Node *local = body->as.block.items[0], *ret = body->as.block.items[1];
  if (!local || local->kind != ND_VAR_DECL ||
      local->as.var_decl.type != TK_KW_STRING || local->as.var_decl.is_static ||
      local->as.var_decl.is_pointer || local->as.var_decl.array_len ||
      !ret || ret->kind != ND_RETURN || !ret->as.ret.expr) return 0;
  Node *value = ret->as.ret.expr;
  Node *operand_fn = NULL;
  Slice operand_name = {0};
  if (allow_append && value->kind == ND_BINOP && value->as.bin.op == TK_PLUSEQ) {
    Node *operand = value->as.bin.rhs;
    operand_fn = string_factory_definition(operand);
    if (!operand_fn || local->as.var_decl.is_const) return 0;
    operand_name = operand->as.call.callee->as.ident;
    value = value->as.bin.lhs;
  }
  if (value->kind != ND_IDENT) return 0;
  Slice slot = local->as.var_decl.name, result = value->as.ident;
  if (slot.len != result.len || memcmp(slot.start, result.start, slot.len)) return 0;
  if (operand_fn && slot.len == operand_name.len &&
      !memcmp(slot.start, operand_name.start, slot.len)) return 0;
  Node *init = local->as.var_decl.init;
  int source_is_parameter = 0;
  for (size_t i = 0; i < fn->as.func.param_len; i++) {
    Node *p = fn->as.func.params[i];
    if (!p || p->kind != ND_VAR_DECL || p->as.var_decl.is_pointer ||
        p->as.var_decl.array_len || p->as.var_decl.is_static ||
        !(is_primitive_scalar_result(p->as.var_decl.type) ||
          p->as.var_decl.type == TK_KW_STRING)) return 0;
    Slice parameter = p->as.var_decl.name;
    // Same-spelled local/parameter cannot establish either binding's identity.
    if (slot.len == parameter.len &&
        !memcmp(slot.start, parameter.start, slot.len)) return 0;
    if (operand_fn && parameter.len == operand_name.len &&
        !memcmp(parameter.start, operand_name.start, parameter.len)) return 0;
    if (init && init->kind == ND_IDENT &&
        p->as.var_decl.type == TK_KW_STRING &&
        init->as.ident.len == parameter.len &&
        !memcmp(init->as.ident.start, parameter.start, parameter.len))
      source_is_parameter = 1;
  }
  // The append operand must satisfy the original two-statement proof.
  // Disabling append here bounds the check even for recursive calls.
  return (!init || init->kind == ND_STRING || init->kind == ND_NULL ||
          source_is_parameter) &&
         (!operand_fn || string_factory_result(operand_fn, 0));
}

static int adopts_string_call_result(CGCtx *ctx, Node *call, int allow_append) {
  Node *fn = string_factory_definition(call);
  if (!fn) return 0;
  Slice name = call->as.call.callee->as.ident;
  return !cgctx_has_var(ctx, name.start, name.len) &&
         string_factory_result(fn, allow_append);
}

static int adopts_string_call(CGCtx *ctx, Node *call) {
  return adopts_string_call_result(ctx, call, 0);
}

static Node *owned_factory_append_slot(CGCtx *ctx, Node *expr) {
  if (!expr || expr->kind != ND_BINOP || expr->as.bin.op != TK_PLUSEQ ||
      expr->as.bin.lhs->kind != ND_IDENT) return NULL;
  Node *lhs = expr->as.bin.lhs;
  Slice name = lhs->as.ident;
  const VarBinding *slot = cgctx_binding(ctx, name.start, name.len);
  if (slot && slot->owns_string && slot->type == TK_KW_STRING &&
      slot->indirections == 0 && adopts_string_call(ctx, expr->as.bin.rhs))
    return lhs;
  return NULL;
}

static void emit_owned_factory_append(CGCtx *ctx, COut *b, Node *slot, Node *operand) {
  c_out_write(b, "__dream_string_append_owned(&");
  cg_emit_expr(ctx, b, slot);
  c_out_write(b, ", ");
  cg_emit_expr(ctx, b, operand);
  c_out_write(b, ")");
}

static Node *console_wrapper_argument(CGCtx *ctx, Node *call) {
  if (call->kind != ND_CALL || call->as.call.len != 1 ||
      !call->as.call.callee || call->as.call.callee->kind != ND_IDENT) return NULL;
  Slice name = call->as.call.callee->as.ident;
  if (cgctx_has_var(ctx, name.start, name.len)) return NULL;
  Node *fn = cg_function(name);
  if (!fn || fn->as.func.is_async || fn->as.func.ret_type != TK_KW_VOID ||
      fn->as.func.param_len != 1) return NULL;
  Node *param = fn->as.func.params[0];
  if (!param || param->kind != ND_VAR_DECL ||
      param->as.var_decl.type != TK_KW_STRING || param->as.var_decl.is_pointer ||
      param->as.var_decl.array_len || param->as.var_decl.is_static) return NULL;

  // The whole callee body must use the original parameter only for synchronous
  // output. Even an unchanged parameter can escape through another statement.
  Node *body = fn->as.func.body;
  if (!body || body->kind != ND_BLOCK || body->as.block.len != 1) return NULL;
  Node *stmt = body->as.block.items[0];
  if (!stmt || stmt->kind != ND_EXPR_STMT) return NULL;
  Node *console = stmt->as.expr_stmt.expr;
  if (!console || console->kind != ND_CONSOLE_CALL || console->as.console.read)
    return NULL;
  Node *arg = console->as.console.arg;
  Slice slot = param->as.var_decl.name;
  if (!arg || arg->kind != ND_IDENT || arg->as.ident.len != slot.len ||
      memcmp(arg->as.ident.start, slot.start, slot.len)) return NULL;
  return call->as.call.args[0];
}

static int owned_argument_name_in_use(CGCtx *ctx, Slice name) {
  if (cgctx_has_var(ctx, name.start, name.len) || cg_function(name)) return 1;
  // Qualified Dream enum members become unqualified C constants.
  for (size_t i = 0; i < g_program->as.block.len; i++) {
    Node *decl = g_program->as.block.items[i];
    if (decl->kind != ND_ENUM_DECL) continue;
    for (size_t j = 0; j < decl->as.enum_decl.len; j++) {
      Slice member = decl->as.enum_decl.members[j]->as.var_decl.name;
      if (member.len == name.len && !memcmp(member.start, name.start, name.len))
        return 1;
    }
  }
  return 0;
}

static void emit_loop_body(CGCtx *ctx, COut *b, Node *body,
                           const char *src_file) {
  CGControlTarget target = {ctx->len, 1, ctx->control_target};
  ctx->control_target = &target;
  cg_emit_stmt(ctx, b, body, src_file);
  ctx->control_target = target.parent;
}

static void emit_control_jump(CGCtx *ctx, COut *b, int is_continue) {
  CGControlTarget *target = ctx->control_target;
  while (target && is_continue && !target->is_loop) target = target->parent;
  // A compound statement keeps cleanup conditional for an unbraced if body.
  // Do not pop emitter bindings: other branches still need their scope metadata.
  c_out_write(b, "{\n");
  c_out_indent(b);
  if (target) {
    for (size_t i = ctx->len; i-- > target->binding_start;) {
      VarBinding *v = &ctx->vars[i];
      // Match normal class-local cleanup at the same exited scope boundary.
      if (v->owns_string ||
          (v->type == TK_IDENT && cg_is_class_type(v->type_name)))
        c_out_write(b, "dr_release(%.*s);\n", (int)v->len, v->start);
    }
  }
  c_out_write(b, "%s;\n", is_continue ? "continue" : "break");
  c_out_dedent(b);
  c_out_write(b, "}\n");
}

void cg_emit_stmt(CGCtx *ctx, COut *b, Node *n, const char *src_file) {
  // Emit debug line directive for this statement
  switch (n->kind) {
  case ND_VAR_DECL:
    emit_debug_line(b, n, src_file, "variable declaration");
    break;
  case ND_IF:
    emit_debug_line(b, n, src_file, "if statement");
    break;
  case ND_WHILE:
    emit_debug_line(b, n, src_file, "while loop");
    break;
  case ND_DO_WHILE:
    emit_debug_line(b, n, src_file, "do-while loop");
    break;
  case ND_FOR:
    emit_debug_line(b, n, src_file, "for loop");
    break;
  case ND_SWITCH:
    emit_debug_line(b, n, src_file, "switch statement");
    break;
  case ND_BLOCK:
    emit_debug_line(b, n, src_file, "block statement");
    break;
  case ND_RETURN:
    emit_debug_line(b, n, src_file, "return statement");
    break;
  case ND_BREAK:
    emit_debug_line(b, n, src_file, "break statement");
    break;
  case ND_CONTINUE:
    emit_debug_line(b, n, src_file, "continue statement");
    break;
  case ND_EXPR_STMT:
    emit_debug_line(b, n, src_file, "expression statement");
    break;
  case ND_TRY:
    emit_debug_line(b, n, src_file, "try statement");
    break;
  case ND_THROW:
    emit_debug_line(b, n, src_file, "throw statement");
    break;
  default:
    // For other statements, still emit basic line directive
    if (n->pos.line) {
      if (!b->at_line_start)
        c_out_newline(b);
      c_out_write(b, "#line %zu \"%s\"\n", n->pos.line, src_file);
    }
    break;
  }
  
  switch (n->kind) {
  case ND_VAR_DECL:
    if (n->as.var_decl.array_len > 0) {
      if (n->as.var_decl.is_const) {
        c_out_write(b, "const ");
      }
      emit_type_with_pointer(b, n->as.var_decl.type, n->as.var_decl.type_name, n->as.var_decl.is_pointer);
      c_out_write(b, " %.*s[%zu]", (int)n->as.var_decl.name.len,
                  n->as.var_decl.name.start, n->as.var_decl.array_len);
      if (n->as.var_decl.init) {
        c_out_write(b, " = ");
        cg_emit_expr(ctx, b, n->as.var_decl.init);
      }
    } else {
      if (n->as.var_decl.is_const) {
        c_out_write(b, "const ");
      }
      emit_type_with_pointer(b, n->as.var_decl.type, n->as.var_decl.type_name, n->as.var_decl.is_pointer);
      c_out_write(b, " %.*s", (int)n->as.var_decl.name.len,
                  n->as.var_decl.name.start);
      if (n->as.var_decl.init) {
        c_out_write(b, " = ");
        // Literals have static storage; indexed, identifier, member and call
        // values may be borrowed, including conditional and assignment results.
        // Only a proven free-function factory transfers its original allocation.
        // Give all other scalar declarations an independent copy for cleanup.
        int copy_string = n->as.var_decl.type == TK_KW_STRING &&
                          !n->as.var_decl.is_pointer &&
                          copies_string_initializer(n->as.var_decl.init) &&
                          !adopts_string_call_result(ctx, n->as.var_decl.init, 1);
        Node *init = n->as.var_decl.init;
        Node *append_slot = copy_string ? owned_factory_append_slot(ctx, init) : NULL;
        if (copy_string) c_out_write(b, "dream_string_copy(");
        if (append_slot) {
          emit_owned_factory_append(ctx, b, append_slot, init->as.bin.rhs);
        } else {
          cg_emit_value_as(ctx, b, n->as.var_decl.type_name, init);
        }
        if (copy_string) c_out_write(b, ")");
      } else if (n->as.var_decl.type == TK_KW_STRING &&
                 !n->as.var_decl.is_pointer) {
        // An empty scalar slot must be safe to replace and clean up.
        c_out_write(b, " = NULL");
      }
    }
    cgctx_push_decl(ctx, n);
    if (n->as.var_decl.type == TK_KW_STRING &&
        !n->as.var_decl.array_len && !n->as.var_decl.is_pointer) {
      Node *init = n->as.var_decl.init;
      ctx->vars[ctx->len - 1].owns_string =
          !init || copies_string_initializer(init) || init->kind == ND_NULL ||
          (init->kind == ND_CONSOLE_CALL && init->as.console.read) ||
          (init->kind == ND_BINOP && init->as.bin.op == TK_PLUS &&
           cg_is_string_expr(ctx, init));
    }
    c_out_write(b, ";");
    c_out_newline(b);
    break;
  case ND_FUNC:
    emit_func(b, n, src_file, false);
    break;
  case ND_IF:
    c_out_write(b, "if (");
    cg_emit_expr(ctx, b, n->as.if_stmt.cond);
    c_out_write(b, ") ");
    cg_emit_stmt(ctx, b, n->as.if_stmt.then_br, src_file);
    if (n->as.if_stmt.else_br) {
      c_out_write(b, " else ");
      cg_emit_stmt(ctx, b, n->as.if_stmt.else_br, src_file);
    }
    break;
  case ND_WHILE:
    c_out_write(b, "while (");
    cg_emit_expr(ctx, b, n->as.while_stmt.cond);
    c_out_write(b, ") ");
    emit_loop_body(ctx, b, n->as.while_stmt.body, src_file);
    break;
  case ND_DO_WHILE:
    c_out_write(b, "do ");
    emit_loop_body(ctx, b, n->as.do_while_stmt.body, src_file);
    c_out_write(b, " while (");
    cg_emit_expr(ctx, b, n->as.do_while_stmt.cond);
    c_out_write(b, ");");
    c_out_newline(b);
    break;
  case ND_FOR: {
    Node *init = n->as.for_stmt.init;
    int declaration = init && init->kind == ND_VAR_DECL;
    size_t loop_start = ctx->len;
    if (declaration) {
      // Keep the binding alive through condition/body/update and release it
      // after the loop, including zero iterations and a loop-targeting break.
      c_out_write(b, "{\n");
      c_out_indent(b);
      cgctx_scope_enter(ctx);
      cg_emit_stmt(ctx, b, init, src_file);
    }
    c_out_write(b, "for (");
    if (init && !declaration) cg_emit_expr(ctx, b, init);
    c_out_write(b, "; ");
    if (n->as.for_stmt.cond)
      cg_emit_expr(ctx, b, n->as.for_stmt.cond);
    c_out_write(b, "; ");
    if (n->as.for_stmt.update)
      cg_emit_expr(ctx, b, n->as.for_stmt.update);
    c_out_write(b, ") ");
    emit_loop_body(ctx, b, n->as.for_stmt.body, src_file);
    if (declaration) {
      emit_scope_cleanup(ctx, b, loop_start);
      cgctx_scope_leave(ctx);
      c_out_dedent(b);
      c_out_write(b, "}\n");
    }
    break;
  }
  case ND_SWITCH: {
    CGControlTarget target = {ctx->len, 0, ctx->control_target};
    ctx->control_target = &target;
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
      cg_emit_stmt(ctx, b, sc->body, src_file);
    }
    c_out_dedent(b);
    c_out_write(b, "}");
    c_out_newline(b);
    ctx->control_target = target.parent;
    break;
  }
  case ND_BREAK:
    emit_control_jump(ctx, b, 0);
    break;
  case ND_CONTINUE:
    emit_control_jump(ctx, b, 1);
    break;
  case ND_RETURN: {
    const VarBinding *preserved = owned_string_result(ctx, n->as.ret.expr);
    int conditional_scoped = ctx->ret_type == TK_KW_STRING &&
                             n->as.ret.expr && n->as.ret.expr->kind == ND_COND &&
                             !is_static_string_result(n->as.ret.expr) &&
                             is_scoped_string_result(ctx, n->as.ret.expr);
    if (ctx->is_async_worker) {
      // In async worker, set task result instead of returning directly
      if (n->as.ret.expr) {
        switch (ctx->ret_type) {
          case TK_KW_INT:
          case TK_KW_BOOL:
            c_out_write(b, "dr_task_set_int_result(%.*s_current_task, ", 
                        (int)ctx->async_func_name.len, ctx->async_func_name.start);
            cg_emit_expr(ctx, b, n->as.ret.expr);
            c_out_write(b, ");");
            break;
          case TK_KW_FLOAT:
            c_out_write(b, "dr_task_set_float_result(%.*s_current_task, ", 
                        (int)ctx->async_func_name.len, ctx->async_func_name.start);
            cg_emit_expr(ctx, b, n->as.ret.expr);
            c_out_write(b, ");");
            break;
          case TK_KW_STRING:
            c_out_write(b, "dr_task_set_string_result(%.*s_current_task, ", 
                        (int)ctx->async_func_name.len, ctx->async_func_name.start);
            cg_emit_expr(ctx, b, n->as.ret.expr);
            c_out_write(b, ");");
            break;
          default:
            c_out_write(b, "dr_task_set_ptr_result(%.*s_current_task, ", 
                        (int)ctx->async_func_name.len, ctx->async_func_name.start);
            cg_emit_expr(ctx, b, n->as.ret.expr);
            c_out_write(b, ");");
            break;
        }
      }
      c_out_write(b, "\nreturn NULL;");
    } else if (ctx->is_sync_function && !n->as.ret.expr &&
               ctx->ret_type == TK_KW_VOID &&
               ctx->exception_depth == 0) {
      // Keep cleanup inside an unbraced conditional return. Bindings remain
      // available to emit other branches and unreachable fall-through cleanup.
      c_out_write(b, "{\n");
      c_out_indent(b);
      emit_return_cleanup(ctx, b, NULL);
      c_out_write(b, "return;\n");
      c_out_dedent(b);
      c_out_write(b, "}");
    } else if (ctx->is_sync_function && n->as.ret.expr &&
               (is_primitive_scalar_result(ctx->ret_type) || preserved ||
                conditional_scoped || borrowed_parameter_result(ctx, n->as.ret.expr) ||
                borrowed_member_result(ctx, n->as.ret.expr) ||
                borrowed_static_member_result(ctx, n->as.ret.expr) ||
                (ctx->ret_type == TK_KW_STRING &&
                 is_static_string_result(n->as.ret.expr))) &&
               ctx->exception_depth == 0 && has_return_cleanup(ctx)) {
      // Preflight lowered names as well as bindings: a C initializer is already
      // inside its declarator's scope, so the temporary must not shadow a call.
      COut value;
      c_out_init(&value);
      cg_emit_expr(ctx, &value, n->as.ret.expr);
      char temporary[64];
      size_t suffix = 0;
      do {
        snprintf(temporary, sizeof temporary, "__dream_return_%zu", suffix++);
      } while (cgctx_has_var(ctx, temporary, strlen(temporary)) ||
               (value.data && strstr(value.data, temporary)));
      c_out_free(&value);

      // Evaluate exactly once before freeing any storage read by the result.
      // Static/caller-backed results are independent of these locals;
      // owned-local leaves preserve their slot.
      // Keep bindings for other branches; keep unbraced returns conditional.
      c_out_write(b, "{\n");
      c_out_indent(b);
      c_out_write(b, "%s %s = ", type_to_c(ctx->ret_type), temporary);
      cg_emit_expr(ctx, b, n->as.ret.expr);
      c_out_write(b, ";\n");
      if (conditional_scoped)
        emit_conditional_return_cleanup(ctx, b, n->as.ret.expr, temporary);
      else
        emit_return_cleanup(ctx, b, preserved);
      c_out_write(b, "return %s;\n", temporary);
      c_out_dedent(b);
      c_out_write(b, "}");
    } else {
      Node *expr = n->as.ret.expr;
      Node *append_slot = ctx->is_sync_function && ctx->ret_type == TK_KW_STRING &&
                          ctx->exception_depth == 0
                              ? owned_factory_append_slot(ctx, expr) : NULL;
      c_out_write(b, "return");
      if (expr) {
        c_out_write(b, " ");
        if (append_slot && owned_string_result(ctx, append_slot)) {
          // Consume the factory operand and return the local's installed join.
          emit_owned_factory_append(ctx, b, append_slot, expr->as.bin.rhs);
        } else {
          cg_emit_expr(ctx, b, expr);
        }
      } else if (ctx->ret_type != TK_KW_VOID) {
        c_out_write(b, " 0");
      }
      c_out_write(b, ";");
    }
    c_out_newline(b);
    break;
  }
  case ND_THROW:
    if (n->as.throw_stmt.expr) {
      // Throw with expression (use expression as message)
      c_out_write(b, "{\n");
      c_out_indent(b);
      c_out_write(b, "char *exc_msg = ");
      cg_emit_expr(ctx, b, n->as.throw_stmt.expr);
      c_out_write(b, ";\n");
      c_out_write(b, "dream_exception_throw(DREAM_EXC_GENERIC, exc_msg, __FILE__, __LINE__);\n");
      c_out_dedent(b);
      c_out_write(b, "}\n");
    } else {
      // Throw without expression (generic exception)
      c_out_write(b, "dream_exception_throw(DREAM_EXC_GENERIC, \"An exception occurred\", __FILE__, __LINE__);\n");
    }
    break;
  case ND_BLOCK: {
    c_out_write(b, "{");
    c_out_newline(b);
    c_out_indent(b);
    size_t block_start = ctx->len;
    cgctx_scope_enter(ctx);
    for (size_t i = 0; i < n->as.block.len; i++)
      cg_emit_stmt(ctx, b, n->as.block.items[i], src_file);
    emit_scope_cleanup(ctx, b, block_start);
    cgctx_scope_leave(ctx);
    c_out_dedent(b);
    c_out_write(b, "}");
    c_out_newline(b);
    break;
  }
  case ND_EXPR_STMT: {
    Node *expr = n->as.expr_stmt.expr;
    Node *append_slot = owned_factory_append_slot(ctx, expr);
    Node *owned_arg = append_slot ? expr->as.bin.rhs : NULL;
    if (expr->kind == ND_BINOP && expr->as.bin.op == TK_EQ &&
        expr->as.bin.lhs->kind == ND_IDENT) {
      Slice name = expr->as.bin.lhs->as.ident;
      const VarBinding *slot = cgctx_binding(ctx, name.start, name.len);
      if (slot && slot->owns_string && slot->type == TK_KW_STRING &&
          slot->indirections == 0 &&
          adopts_string_call_result(ctx, expr->as.bin.rhs, 1)) {
        // A statement-local temporary avoids a new global helper namespace.
        // Finish the RHS before reading/releasing the current destination: an
        // argument may itself replace that destination or snapshot its backing.
        char temporary[64];
        size_t suffix = 0;
        do {
          snprintf(temporary, sizeof temporary, "__dream_owned_%zu", suffix++);
        } while (cgctx_has_var(ctx, temporary, strlen(temporary)) ||
                 cg_function((Slice){temporary, strlen(temporary)}));
        c_out_write(b, "{ char *%s = (char *)", temporary);
        cg_emit_expr(ctx, b, expr->as.bin.rhs);
        c_out_write(b, "; dr_release(");
        cg_emit_expr(ctx, b, expr->as.bin.lhs);
        c_out_write(b, "); ");
        cg_emit_expr(ctx, b, expr->as.bin.lhs);
        c_out_write(b, " = %s; }", temporary);
        c_out_newline(b);
        break;
      }
    }
    const char *console_writer = NULL;
    if (expr->kind == ND_CONSOLE_CALL && !expr->as.console.read) {
      console_writer = expr->as.console.newline ? "dr_console_writeln" : "dr_console_write";
      // The runtime prints synchronously and does not store the pointer.
      if (adopts_string_call_result(ctx, expr->as.console.arg, 1))
        owned_arg = expr->as.console.arg;
    } else if (!append_slot) {
      Node *arg = console_wrapper_argument(ctx, expr);
      Node *wrapped_append_slot = owned_factory_append_slot(ctx, arg);
      if (wrapped_append_slot) {
        // Pass the destination-owned join after consuming the factory operand.
        cg_emit_expr(ctx, b, expr->as.call.callee);
        c_out_write(b, "(");
        emit_owned_factory_append(ctx, b, wrapped_append_slot, arg->as.bin.rhs);
        c_out_write(b, ");");
        c_out_newline(b);
        break;
      }
      if (adopts_string_call(ctx, arg))
        owned_arg = arg;
    }
    if (owned_arg) {
      // C puts a declaration's name in scope inside its own initializer.
      // A name without underscores also avoids type_member names.
      char temporary[64];
      size_t suffix = 0;
      do {
        snprintf(temporary, sizeof temporary, "dreamOwnedArgument%zu", suffix++);
      } while (owned_argument_name_in_use(ctx, (Slice){temporary, strlen(temporary)}));
      c_out_write(b, "{ const char *%s = ", temporary);
      cg_emit_expr(ctx, b, owned_arg);
      c_out_write(b, "; ");
      if (console_writer)
        c_out_write(b, "%s", console_writer);
      else if (append_slot)
        c_out_write(b, "__dream_string_append");
      else
        cg_emit_expr(ctx, b, expr->as.call.callee);
      c_out_write(b, "(");
      if (append_slot) {
        // The helper reads the post-RHS slot, copies both operands, and
        // installs the new allocation before the RHS reference is released.
        c_out_write(b, "&");
        cg_emit_expr(ctx, b, append_slot);
        c_out_write(b, ", ");
      }
      c_out_write(b, "%s); dr_release((void *)%s); }", temporary, temporary);
      c_out_newline(b);
    } else if (expr->kind == ND_CONSOLE_CALL) {
      if (expr->as.console.read) {
        c_out_write(b, "dream_readline();");
        c_out_newline(b);
      } else if (cg_is_string_expr(ctx, expr->as.console.arg)) {
        Node *arg = expr->as.console.arg;
        Node *printed_append_slot = owned_factory_append_slot(ctx, arg);
        c_out_write(b, "%s(", console_writer);
        if (printed_append_slot)
          emit_owned_factory_append(ctx, b, printed_append_slot, arg->as.bin.rhs);
        else
          cg_emit_expr(ctx, b, arg);
        c_out_write(b, ");");
        c_out_newline(b);
      } else {
        c_out_write(b, "printf(\"");
        c_out_write(b, "%s", cg_fmt_for_arg(ctx, expr->as.console.arg));
        if (expr->as.console.newline)
          c_out_write(b, "\\n");
        c_out_write(b, "\", ");
        cg_emit_expr(ctx, b, expr->as.console.arg);
        c_out_write(b, ");");
        c_out_newline(b);
      }
    } else if (adopts_string_call_result(ctx, expr, 1)) {
      // The discarded result has no destination to own its transferred reference.
      c_out_write(b, "dr_release((void *)");
      cg_emit_expr(ctx, b, expr);
      c_out_write(b, ");");
      c_out_newline(b);
    } else {
      cg_emit_expr(ctx, b, expr);
      c_out_write(b, ";");
      c_out_newline(b);
    }
    break;
  }
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
  case ND_TRY: {
    int has_finally = n->as.try_stmt.finally_body != NULL;
    ctx->exception_depth++;
    
    c_out_write(b, "{\n");
    c_out_indent(b);
    
    // Push exception context
    c_out_write(b, "jmp_buf* exc_buf = dream_exception_push(%d);\n", has_finally);
    c_out_write(b, "if (exc_buf && setjmp(*exc_buf) == 0) {\n");
    c_out_indent(b);
    
    // Try block
    cg_emit_stmt(ctx, b, n->as.try_stmt.body, src_file);
    
    c_out_dedent(b);
    c_out_write(b, "} else {\n");
    c_out_indent(b);
    
    // Mark that we're entering a catch block
    c_out_write(b, "dream_exception_enter_catch();\n");
    
    // Catch block
    if (n->as.try_stmt.catch_body) {
      // If we have catch parameter, declare it
      if (n->as.try_stmt.catch_param.len > 0) {
        // Declare exception variable in catch scope
        cgctx_scope_enter(ctx);
        if (n->as.try_stmt.catch_type.len > 0) {
          // Custom exception type
          c_out_write(b, "/* TODO: Custom exception type %.*s */\n",
                     (int)n->as.try_stmt.catch_type.len, n->as.try_stmt.catch_type.start);
        }
        c_out_write(b, "DreamException *%.*s = dream_exception_current();\n",
                   (int)n->as.try_stmt.catch_param.len, n->as.try_stmt.catch_param.start);
        
        cg_emit_stmt(ctx, b, n->as.try_stmt.catch_body, src_file);
        cgctx_scope_leave(ctx);
      } else {
        cg_emit_stmt(ctx, b, n->as.try_stmt.catch_body, src_file);
      }
    }
    
    // Mark that we're exiting the catch block
    c_out_write(b, "dream_exception_exit_catch();\n");
    
    c_out_dedent(b);
    c_out_write(b, "}\n");
    
    // Finally block
    if (n->as.try_stmt.finally_body) {
      c_out_write(b, "/* Finally block */\n");
      cg_emit_stmt(ctx, b, n->as.try_stmt.finally_body, src_file);
      c_out_write(b, "dream_exception_finally_executed();\n");
    }
    
    // Pop exception context
    c_out_write(b, "dream_exception_pop();\n");
    ctx->exception_depth--;
    
    c_out_dedent(b);
    c_out_write(b, "}\n");
    break;
  }
  case ND_ENUM_DECL:
    // Add enum name to context so VkResult.Success works
    cgctx_push(ctx, n->as.enum_decl.name.start, n->as.enum_decl.name.len, 
               TK_KW_ENUM, (Slice){NULL, 0});
    break;
  default:
    break;
  }
}
