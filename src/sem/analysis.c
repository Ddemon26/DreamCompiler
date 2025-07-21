#include "analysis.h"
#include "symbol.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void diag_push(SemAnalyzer *s, Pos pos, DiagSeverity sev,
                      const char *msg) {
  if (s->diags.len + 1 > s->diags.cap) {
    s->diags.cap = s->diags.cap ? s->diags.cap * 2 : 4;
    s->diags.data = realloc(s->diags.data, s->diags.cap * sizeof(Diagnostic));
  }
  s->diags.data[s->diags.len++] = (Diagnostic){
    .pos = pos, 
    .end_pos = pos,
    .start = NULL,
    .len = 0,
    .msg = msg, 
    .hint = NULL,
    .sev = sev
  };
}

static void diag_pushf(SemAnalyzer *s, Pos pos, DiagSeverity sev,
                       const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);
  char *buf = arena_alloc(s->arena, n + 1);
  va_start(ap, fmt);
  vsnprintf(buf, n + 1, fmt, ap);
  va_end(ap);
  diag_push(s, pos, sev, buf);
}

static char *slice_to_cstr(SemAnalyzer *s, Slice slice) {
  char *buf = arena_alloc(s->arena, slice.len + 1);
  memcpy(buf, slice.start, slice.len);
  buf[slice.len] = '\0';
  return buf;
}

void sem_analyzer_init(SemAnalyzer *s, Arena *a) {
  s->arena = a;
  s->diags.data = NULL;
  s->diags.len = 0;
  s->diags.cap = 0;
  s->scope = malloc(sizeof(Scope));
  scope_init(s->scope, NULL);
  s->current_ret = TK_KW_VOID;
}

void sem_analyzer_free(SemAnalyzer *s) {
  scope_free(s->scope);
  free(s->scope);
  free(s->diags.data);
}

typedef enum { DECL_VAR, DECL_FUNC } DeclKind;

typedef struct Decl Decl;
struct Decl {
  DeclKind kind;
  union {
    struct {
      TokenKind type;
      int is_const;
    } var;
    struct {
      TokenKind ret_type;
      TokenKind *param_types;
      size_t param_len;
    } func;
  } as;
};

static TokenKind analyze_expr(SemAnalyzer *s, Node *n);
static void analyze_stmt(SemAnalyzer *s, Node *n);

static Decl *decl_new(SemAnalyzer *s, DeclKind kind) {
  (void)s;
  Decl *d = malloc(sizeof(Decl));
  d->kind = kind;
  return d;
}

static TokenKind analyze_ident(SemAnalyzer *s, Node *n) {
  char *name = slice_to_cstr(s, n->as.ident);
  Symbol *sym = sym_intern(name);
  Decl *d = scope_lookup(s->scope, sym);
  if (!d) {
    diag_pushf(s, n->pos, DIAG_ERROR, "undefined variable '%s'", name);
    return TK_KW_INT;
  }
  if (d->kind == DECL_VAR)
    return d->as.var.type;
  return d->as.func.ret_type; /* function as value returns its return type */
}

static TokenKind analyze_call(SemAnalyzer *s, Node *n) {
  if (n->as.call.callee->kind != ND_IDENT)
    return TK_KW_INT;
  char *name = slice_to_cstr(s, n->as.call.callee->as.ident);
  Symbol *sym = sym_intern(name);
  Decl *d = scope_lookup(s->scope, sym);
  if (!d || d->kind != DECL_FUNC) {
    diag_pushf(s, n->pos, DIAG_ERROR, "undefined function '%s'", name);
    for (size_t i = 0; i < n->as.call.len; i++)
      analyze_expr(s, n->as.call.args[i]);
    return TK_KW_INT;
  }
  if (d->as.func.param_len != n->as.call.len) {
    diag_pushf(s, n->pos, DIAG_ERROR,
               "expected %zu arguments but got %zu for function '%s'",
               d->as.func.param_len, n->as.call.len, name);
  }
  for (size_t i = 0; i < n->as.call.len; i++)
    analyze_expr(s, n->as.call.args[i]);
  return d->as.func.ret_type;
}

static TokenKind analyze_expr(SemAnalyzer *s, Node *n) {
  if (!n)
    return TK_KW_INT;
  switch (n->kind) {
  case ND_INT:
    return TK_KW_INT;
  case ND_FLOAT:
    return TK_KW_FLOAT;
  case ND_BOOL:
    return TK_KW_BOOL;
  case ND_CHAR:
    return TK_KW_CHAR;
  case ND_STRING:
    return TK_KW_STRING;
  case ND_NULL:
    return TK_KW_VOID; // null is compatible with pointer types
  case ND_IDENT:
    return analyze_ident(s, n);
  case ND_FIELD:
    // Handle field access, including enum member access
    TokenKind object_type = analyze_expr(s, n->as.field.object);
    if (object_type == TK_KW_ENUM) {
      // This is enum member access like VkResult.Success
      return TK_KW_INT; // enum members are integers
    }
    // Regular field access on objects
    return TK_KW_INT; // Default for now
  case ND_BINOP:
    // Check for assignment to const variable
    if (n->as.bin.op == TK_EQ && n->as.bin.lhs->kind == ND_IDENT) {
      char *name = slice_to_cstr(s, n->as.bin.lhs->as.ident);
      Symbol *sym = sym_intern(name);
      Decl *d = scope_lookup(s->scope, sym);
      if (d && d->kind == DECL_VAR && d->as.var.is_const) {
        diag_pushf(s, n->pos, DIAG_ERROR, "cannot assign to const variable '%s'", name);
      }
    }
    analyze_expr(s, n->as.bin.lhs);
    analyze_expr(s, n->as.bin.rhs);
    return TK_KW_INT;
  case ND_CALL:
    return analyze_call(s, n);
  case ND_COND:
    analyze_expr(s, n->as.cond.cond);
    return analyze_expr(s, n->as.cond.then_expr);
  case ND_NEW:
    /* new expressions return a pointer to the type being constructed */
    for (size_t i = 0; i < n->as.new_expr.arg_len; i++)
      analyze_expr(s, n->as.new_expr.args[i]);
    return TK_IDENT; /* Custom type - actual type name stored in type_name */
  case ND_AWAIT:
    /* await expressions return TaskResult type */
    analyze_expr(s, n->as.await_expr.expr);
    return TK_KW_TASKRESULT;
  case ND_INDEX:
    /* Array access - return element type */
    analyze_expr(s, n->as.index.index); /* Analyze index expression */
    if (n->as.index.array->kind == ND_IDENT) {
      TokenKind array_type = analyze_ident(s, n->as.index.array);
      /* Array element has the same type as the array for primitive types */
      return array_type;
    }
    return TK_KW_INT; /* Default fallback */
  default:
    return TK_KW_INT;
  }
}

static void analyze_stmt(SemAnalyzer *s, Node *n) {
  if (!n)
    return;
  switch (n->kind) {
  case ND_BLOCK: {
    s->scope = scope_push(s->scope);
    for (size_t i = 0; i < n->as.block.len; i++)
      analyze_stmt(s, n->as.block.items[i]);
    s->scope = scope_pop(s->scope);
    break;
  }
  case ND_VAR_DECL: {
    char *name = slice_to_cstr(s, n->as.var_decl.name);
    Symbol *sym = sym_intern(name);
    if (scope_lookup(s->scope, sym)) {
      diag_pushf(s, n->pos, DIAG_ERROR, "redefinition of '%s'", name);
      break;
    }
    Decl *d = decl_new(s, DECL_VAR);
    d->as.var.type = n->as.var_decl.type;
    d->as.var.is_const = n->as.var_decl.is_const;
    scope_bind(s->scope, sym, d);
    if (n->as.var_decl.init) {
      TokenKind it = analyze_expr(s, n->as.var_decl.init);
      if (it != d->as.var.type)
        diag_pushf(s, n->pos, DIAG_ERROR,
                   "cannot assign expression of different type to '%s'", name);
    }
    break;
  }
  case ND_EXPR_STMT:
    analyze_expr(s, n->as.expr_stmt.expr);
    break;
  case ND_IF:
    analyze_expr(s, n->as.if_stmt.cond);
    analyze_stmt(s, n->as.if_stmt.then_br);
    analyze_stmt(s, n->as.if_stmt.else_br);
    break;
  case ND_WHILE:
    analyze_expr(s, n->as.while_stmt.cond);
    analyze_stmt(s, n->as.while_stmt.body);
    break;
  case ND_DO_WHILE:
    analyze_stmt(s, n->as.do_while_stmt.body);
    analyze_expr(s, n->as.do_while_stmt.cond);
    break;
  case ND_FOR:
    if (n->as.for_stmt.init)
      analyze_stmt(s, n->as.for_stmt.init);
    if (n->as.for_stmt.cond)
      analyze_expr(s, n->as.for_stmt.cond);
    if (n->as.for_stmt.update)
      analyze_expr(s, n->as.for_stmt.update);
    analyze_stmt(s, n->as.for_stmt.body);
    break;
  case ND_RETURN:
    if (n->as.ret.expr)
      analyze_expr(s, n->as.ret.expr);
    break;
  case ND_FUNC: {
    char *name = slice_to_cstr(s, n->as.func.name);
    Symbol *sym = sym_intern(name);
    if (scope_lookup(s->scope, sym)) {
      diag_pushf(s, n->pos, DIAG_ERROR, "redefinition of function '%s'", name);
      break;
    }
    Decl *d = decl_new(s, DECL_FUNC);
    // For async functions, the semantic return type is Task*
    d->as.func.ret_type = n->as.func.is_async ? TK_KW_TASK : n->as.func.ret_type;
    d->as.func.param_len = n->as.func.param_len;
    d->as.func.param_types = malloc(sizeof(TokenKind) * d->as.func.param_len);
    for (size_t i = 0; i < d->as.func.param_len; i++) {
      Node *param = n->as.func.params[i];
      d->as.func.param_types[i] = param->as.var_decl.type;
    }
    scope_bind(s->scope, sym, d);
    s->scope = scope_push(s->scope);
    for (size_t i = 0; i < n->as.func.param_len; i++) {
      Node *param = n->as.func.params[i];
      char *pname = slice_to_cstr(s, param->as.var_decl.name);
      Symbol *ps = sym_intern(pname);
      Decl *pd = decl_new(s, DECL_VAR);
      pd->as.var.type = param->as.var_decl.type;
      pd->as.var.is_const = param->as.var_decl.is_const;
      scope_bind(s->scope, ps, pd);
    }
    TokenKind prev = s->current_ret;
    s->current_ret = n->as.func.ret_type;
    analyze_stmt(s, n->as.func.body);
    s->current_ret = prev;
    s->scope = scope_pop(s->scope);
    break;
  }
  case ND_ENUM_DECL: {
    // Add enum type to symbol table
    char *enum_name = slice_to_cstr(s, n->as.enum_decl.name);
    Symbol *enum_sym = sym_intern(enum_name);
    if (scope_lookup(s->scope, enum_sym)) {
      diag_pushf(s, n->pos, DIAG_ERROR, "redefinition of enum '%s'", enum_name);
      break;
    }
    
    // Create enum type declaration
    Decl *enum_decl = decl_new(s, DECL_VAR); // Use DECL_VAR for now
    enum_decl->as.var.type = TK_KW_ENUM;
    scope_bind(s->scope, enum_sym, enum_decl);
    
    // Add enum members to symbol table
    int current_value = 0;
    for (size_t i = 0; i < n->as.enum_decl.len; i++) {
      Node *member = n->as.enum_decl.members[i];
      char *member_name = slice_to_cstr(s, member->as.var_decl.name);
      Symbol *member_sym = sym_intern(member_name);
      
      if (scope_lookup(s->scope, member_sym)) {
        diag_pushf(s, member->pos, DIAG_ERROR, "redefinition of '%s'", member_name);
        continue;
      }
      
      // If member has explicit value, analyze it
      if (member->as.var_decl.init) {
        analyze_expr(s, member->as.var_decl.init);
        // TODO: Extract constant value for current_value
      }
      
      // Create member declaration
      Decl *member_decl = decl_new(s, DECL_VAR);
      member_decl->as.var.type = TK_KW_INT; // enum members are integers
      scope_bind(s->scope, member_sym, member_decl);
      
      current_value++;
    }
    break;
  }
  default:
    break;
  }
}

void sem_analyze_program(SemAnalyzer *s, Node *root) {
  for (size_t i = 0; i < root->as.block.len; i++)
    analyze_stmt(s, root->as.block.items[i]);
}
