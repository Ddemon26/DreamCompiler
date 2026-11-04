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
    .sev = sev,
    .source_file = s->source_file,
    .source_text = s->source_text
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
  s->current_ret = TK_EOF; /* Top-level entry statements retain their return behavior. */
  s->program = NULL;
  s->current_class = NULL;
  s->source_file = NULL;
  s->source_text = NULL;
}

void sem_analyzer_free(SemAnalyzer *s) {
  scope_free(s->scope);
  free(s->scope);
  free(s->diags.data);
}

typedef enum { DECL_VAR, DECL_FUNC, DECL_TYPE } DeclKind;

typedef struct Decl Decl;
struct Decl {
  DeclKind kind;
  union {
    struct {
      TokenKind type;
      int is_const;
      Slice type_name;
    } var;
    struct {
      TokenKind ret_type;
      TokenKind *param_types;
      size_t param_len;
      Node *definition;
    } func;
  } as;
};

static TokenKind analyze_expr(SemAnalyzer *s, Node *n);
static void analyze_stmt(SemAnalyzer *s, Node *n);

static const char *return_type_name(TokenKind type) {
  switch (type) {
  case TK_KW_INT: return "int";
  case TK_KW_FLOAT: return "float";
  case TK_KW_BOOL: return "bool";
  case TK_KW_CHAR: return "char";
  case TK_KW_STRING: return "string";
  case TK_KW_VOID: return "void";
  default: return "non-primitive";
  }
}

static bool primitive_return_type(TokenKind type) {
  return type == TK_KW_INT || type == TK_KW_FLOAT || type == TK_KW_BOOL ||
         type == TK_KW_CHAR || type == TK_KW_STRING;
}

static Decl *decl_new(SemAnalyzer *s, DeclKind kind) {
  Decl *d = arena_alloc(s->arena, sizeof(Decl));
  d->kind = kind;
  return d;
}

static void source_context(SemAnalyzer *s, Node *node) {
  s->source_file = node->source_file;
  s->source_text = node->source_text;
}

static Decl *declare_function(SemAnalyzer *s, Node *node) {
  char *name = slice_to_cstr(s, node->as.func.name);
  Symbol *sym = sym_intern(name);
  Decl *known = scope_lookup(s->scope, sym);
  if (known) {
    if (known->kind == DECL_FUNC && known->as.func.definition == node) return known;
    diag_pushf(s, node->pos, DIAG_ERROR, "redefinition of function '%s'", name);
    return NULL;
  }
  Decl *function = decl_new(s, DECL_FUNC);
  function->as.func.definition = node;
  function->as.func.ret_type = node->as.func.is_async ? TK_KW_TASK : node->as.func.ret_type;
  function->as.func.param_len = node->as.func.param_len;
  function->as.func.param_types = arena_alloc(s->arena, sizeof(TokenKind) * node->as.func.param_len);
  for (size_t i = 0; i < node->as.func.param_len; ++i)
    function->as.func.param_types[i] = node->as.func.params[i]->as.var_decl.type;
  scope_bind(s->scope, sym, function);
  return function;
}

static bool slice_equal(Slice a, Slice b) {
  return a.len == b.len && (a.len == 0 || memcmp(a.start, b.start, a.len) == 0);
}

Node *sem_find_type(Node *program, Slice name) {
  if (!program || !name.len) return NULL;
  for (size_t i = 0; i < program->as.block.len; ++i) {
    Node *n = program->as.block.items[i];
    if ((n->kind == ND_CLASS_DECL || n->kind == ND_STRUCT_DECL) &&
        slice_equal(n->as.type_decl.name, name)) return n;
  }
  return NULL;
}

Node *sem_find_member(Node *type, Slice name, Node **owner) {
  if (owner) *owner = NULL;
  for (; type; type = type->as.type_decl.base_decl) {
    for (size_t i = 0; i < type->as.type_decl.len; ++i) {
      Node *member = type->as.type_decl.members[i];
      Slice member_name = member->kind == ND_VAR_DECL ? member->as.var_decl.name :
                          member->kind == ND_FUNC ? member->as.func.name : (Slice){0};
      if (member_name.len && slice_equal(member_name, name)) {
        if (owner) *owner = type;
        return member;
      }
    }
  }
  return NULL;
}

bool sem_is_subclass(Node *derived, Node *base) {
  if (!base) return false;
  for (; derived; derived = derived->as.type_decl.base_decl) {
    if (derived == base) return true;
  }
  return false;
}

static bool resolve_inheritance(SemAnalyzer *s, Node *type) {
  if (type->as.type_decl.inheritance_state == 2) return true;
  if (type->as.type_decl.inheritance_state == 3) return false;
  Slice name = type->as.type_decl.name;
  if (type->as.type_decl.inheritance_state == 1) {
    diag_pushf(s, type->pos, DIAG_ERROR, "inheritance cycle involving '%.*s'", (int)name.len, name.start);
    type->as.type_decl.inheritance_state = 3;
    return false;
  }
  type->as.type_decl.inheritance_state = 1;
  Slice base_name = type->as.type_decl.base_name;
  if (base_name.len) {
    Node *base = sem_find_type(s->program, base_name);
    if (!base || base->kind != ND_CLASS_DECL) {
      diag_pushf(s, type->pos, DIAG_ERROR, "unknown or invalid base class '%.*s'", (int)base_name.len, base_name.start);
      type->as.type_decl.inheritance_state = 3;
      return false;
    }
    if (!resolve_inheritance(s, base)) {
      type->as.type_decl.inheritance_state = 3;
      return false;
    }
    type->as.type_decl.base_decl = base;
  }
  for (size_t i = 0; i < type->as.type_decl.len; ++i) {
    Node *member = type->as.type_decl.members[i];
    Slice member_name = member->kind == ND_VAR_DECL ? member->as.var_decl.name : member->as.func.name;
    for (size_t j = 0; j < i; ++j) {
      Node *previous = type->as.type_decl.members[j];
      Slice previous_name = previous->kind == ND_VAR_DECL ? previous->as.var_decl.name : previous->as.func.name;
      if (slice_equal(member_name, previous_name)) {
        diag_pushf(s, member->pos, DIAG_ERROR, "duplicate member '%.*s'", (int)member_name.len, member_name.start);
        type->as.type_decl.inheritance_state = 3;
        return false;
      }
    }
  }
  type->as.type_decl.inheritance_state = 2;
  return true;
}

static Node *expr_class(SemAnalyzer *s, Node *n) {
  if (!n) return NULL;
  if (n->kind == ND_NEW) return sem_find_type(s->program, n->as.new_expr.type_name);
  if (n->kind == ND_IDENT) {
    Decl *d = scope_lookup(s->scope, sym_intern(slice_to_cstr(s, n->as.ident)));
    if (d && d->kind == DECL_VAR) return sem_find_type(s->program, d->as.var.type_name);
    if (d && d->kind == DECL_TYPE) return sem_find_type(s->program, n->as.ident);
  }
  if (n->kind == ND_FIELD) {
    Node *member = sem_find_member(expr_class(s, n->as.field.object), n->as.field.name, NULL);
    if (member && member->kind == ND_VAR_DECL) return sem_find_type(s->program, member->as.var_decl.type_name);
  }
  if (n->kind == ND_BASE && s->current_class) {
    Node *member = sem_find_member(s->current_class->as.type_decl.base_decl, n->as.base.name, NULL);
    if (member && member->kind == ND_VAR_DECL) return sem_find_type(s->program, member->as.var_decl.type_name);
  }
  return NULL;
}

static void check_class_assignment(SemAnalyzer *s, Node *target, Node *value) {
  Node *source = expr_class(s, value);
  if (target && source && !sem_is_subclass(source, target)) {
    diag_pushf(s, value->pos, DIAG_ERROR, "cannot assign '%.*s' to '%.*s'",
               (int)source->as.type_decl.name.len, source->as.type_decl.name.start,
               (int)target->as.type_decl.name.len, target->as.type_decl.name.start);
  }
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
  if (d->kind == DECL_TYPE)
    return TK_IDENT;
  return d->as.func.ret_type; /* function as value returns its return type */
}

static TokenKind analyze_call(SemAnalyzer *s, Node *n) {
  if (n->as.call.callee->kind == ND_FIELD || n->as.call.callee->kind == ND_BASE) {
    Node *field = n->as.call.callee;
    bool is_base = field->kind == ND_BASE;
    Slice member_name = is_base ? field->as.base.name : field->as.field.name;
    Node *type = is_base ? (s->current_class ? s->current_class->as.type_decl.base_decl : NULL) : expr_class(s, field->as.field.object);
    if (is_base && !type) {
      diag_push(s, n->pos, DIAG_ERROR, "base access requires a base class and an instance method");
      return TK_KW_INT;
    }
    if (type) {
      Node *method = sem_find_member(type, member_name, NULL);
      if (!method || method->kind != ND_FUNC) {
        diag_pushf(s, n->pos, DIAG_ERROR, "unknown method '%.*s'", (int)member_name.len, member_name.start);
        return TK_KW_INT;
      }
      if (!is_base && field->as.field.object->kind == ND_IDENT && !method->as.func.is_static) {
        Decl *object = scope_lookup(s->scope, sym_intern(slice_to_cstr(s, field->as.field.object->as.ident)));
        if (object && object->kind == DECL_TYPE) diag_push(s, n->pos, DIAG_ERROR, "instance method requires an object");
      }
      if (method->as.func.param_len != n->as.call.len)
        diag_push(s, n->pos, DIAG_ERROR, "incorrect number of method arguments");
      for (size_t i = 0; i < n->as.call.len; ++i) analyze_expr(s, n->as.call.args[i]);
      return method->as.func.ret_type;
    }
  }
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
  for (size_t i = 0; i < n->as.call.len; i++) {
    TokenKind actual = analyze_expr(s, n->as.call.args[i]);
    if (i < d->as.func.param_len && actual != d->as.func.param_types[i])
      diag_pushf(s, n->pos, DIAG_ERROR, "argument %zu has incompatible type for function '%s'", i + 1, name);
  }
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
  case ND_BASE: {
    Node *base = s->current_class ? s->current_class->as.type_decl.base_decl : NULL;
    if (!base) {
      diag_push(s, n->pos, DIAG_ERROR, "base access requires a base class and an instance method");
      return TK_KW_INT;
    }
    Node *member = sem_find_member(base, n->as.base.name, NULL);
    if (!member || member->kind != ND_VAR_DECL) {
      diag_pushf(s, n->pos, DIAG_ERROR, "unknown base field '%.*s'", (int)n->as.base.name.len, n->as.base.name.start);
      return TK_KW_INT;
    }
    return member->as.var_decl.type;
  }
  case ND_FIELD: {
    // Handle field access, including enum member access
    TokenKind object_type = analyze_expr(s, n->as.field.object);
    if (object_type == TK_KW_ENUM) {
      // This is enum member access like VkResult.Success
      return TK_KW_INT; // enum members are integers
    }
    Node *type = expr_class(s, n->as.field.object);
    if (type) {
      Node *member = sem_find_member(type, n->as.field.name, NULL);
      if (member && member->kind == ND_VAR_DECL) return member->as.var_decl.type;
      diag_pushf(s, n->pos, DIAG_ERROR, "unknown field '%.*s'", (int)n->as.field.name.len, n->as.field.name.start);
    }
    return TK_KW_INT; // Default for now
  }
  case ND_BINOP: {
    if (n->as.bin.op == TK_EQ) check_class_assignment(s, expr_class(s, n->as.bin.lhs), n->as.bin.rhs);
    // Check for assignment to const variable
    if (n->as.bin.op == TK_EQ && n->as.bin.lhs->kind == ND_IDENT) {
      char *name = slice_to_cstr(s, n->as.bin.lhs->as.ident);
      Symbol *sym = sym_intern(name);
      Decl *d = scope_lookup(s->scope, sym);
      if (d && d->kind == DECL_VAR && d->as.var.is_const) {
        diag_pushf(s, n->pos, DIAG_ERROR, "cannot assign to const variable '%s'", name);
      }
    }
    TokenKind lhs = analyze_expr(s, n->as.bin.lhs);
    TokenKind rhs = analyze_expr(s, n->as.bin.rhs);
    switch (n->as.bin.op) {
    case TK_EQ:
    case TK_PLUSEQ: case TK_MINUSEQ: case TK_STAREQ: case TK_SLASHEQ:
    case TK_PERCENTEQ: case TK_ANDEQ: case TK_OREQ: case TK_XOREQ:
    case TK_LSHIFTEQ: case TK_RSHIFTEQ:
      return lhs;
    case TK_EQEQ: case TK_NEQ: case TK_LT: case TK_LTEQ:
    case TK_GT: case TK_GTEQ: case TK_ANDAND: case TK_OROR:
      return TK_KW_BOOL;
    case TK_PLUS:
      if (lhs == TK_KW_STRING || rhs == TK_KW_STRING) return TK_KW_STRING;
      /* fall through */
    case TK_MINUS: case TK_STAR: case TK_SLASH:
      return lhs == TK_KW_FLOAT || rhs == TK_KW_FLOAT ? TK_KW_FLOAT : TK_KW_INT;
    default:
      return TK_KW_INT;
    }
  }
  case ND_CONSOLE_CALL:
    if (n->as.console.read) return TK_KW_STRING;
    analyze_expr(s, n->as.console.arg);
    return TK_KW_VOID;
  case ND_CALL:
    return analyze_call(s, n);
  case ND_UNARY:
  case ND_POST_UNARY: {
    TokenKind operand = analyze_expr(s, n->as.unary.expr);
    if (n->as.unary.op == TK_BANG) return TK_KW_BOOL;
    if (n->as.unary.op == TK_TILDE) return TK_KW_INT;
    if ((n->as.unary.op == TK_PLUS || n->as.unary.op == TK_MINUS) &&
        (operand == TK_KW_CHAR || operand == TK_KW_BOOL)) return TK_KW_INT;
    return operand;
  }
  case ND_COND: {
    analyze_expr(s, n->as.cond.cond);
    TokenKind then_type = analyze_expr(s, n->as.cond.then_expr);
    TokenKind else_type = analyze_expr(s, n->as.cond.else_expr);
    if (then_type == else_type) return then_type;
    /* Match arithmetic promotion; returning an int alone never widens implicitly. */
    if ((then_type == TK_KW_INT && else_type == TK_KW_FLOAT) ||
        (then_type == TK_KW_FLOAT && else_type == TK_KW_INT)) return TK_KW_FLOAT;
    diag_push(s, n->pos, DIAG_ERROR, "conditional branches have incompatible types");
    return then_type;
  }
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
    d->as.var.type_name = n->as.var_decl.type_name;
    scope_bind(s->scope, sym, d);
    if (n->as.var_decl.init) {
      check_class_assignment(s, sem_find_type(s->program, n->as.var_decl.type_name), n->as.var_decl.init);
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
  case ND_SWITCH:
    analyze_expr(s, n->as.switch_stmt.expr);
    for (size_t i = 0; i < n->as.switch_stmt.len; ++i) {
      analyze_expr(s, n->as.switch_stmt.cases[i].value);
      analyze_stmt(s, n->as.switch_stmt.cases[i].body);
    }
    break;
  case ND_TRY:
    analyze_stmt(s, n->as.try_stmt.body);
    s->scope = scope_push(s->scope);
    if (n->as.try_stmt.catch_param.len) {
      Decl *exception = decl_new(s, DECL_VAR);
      exception->as.var.type = TK_IDENT;
      exception->as.var.type_name = n->as.try_stmt.catch_type;
      scope_bind(s->scope, sym_intern(slice_to_cstr(s, n->as.try_stmt.catch_param)), exception);
    }
    analyze_stmt(s, n->as.try_stmt.catch_body);
    s->scope = scope_pop(s->scope);
    analyze_stmt(s, n->as.try_stmt.finally_body);
    break;
  case ND_RETURN: {
    TokenKind actual = n->as.ret.expr ? analyze_expr(s, n->as.ret.expr) : TK_KW_VOID;
    if (s->current_ret == TK_KW_VOID && n->as.ret.expr) {
      diag_push(s, n->pos, DIAG_ERROR, "void function cannot return an expression");
    } else if (primitive_return_type(s->current_ret)) {
      if (!n->as.ret.expr)
        diag_push(s, n->pos, DIAG_ERROR, "return requires a value");
      else if (actual != s->current_ret)
        diag_pushf(s, n->pos, DIAG_ERROR,
                   "return expression has incompatible type: expected '%s', got '%s'",
                   return_type_name(s->current_ret), return_type_name(actual));
    }
    break;
  }
  case ND_CLASS_DECL:
  case ND_STRUCT_DECL: {
    char *name = slice_to_cstr(s, n->as.type_decl.name);
    Symbol *sym = sym_intern(name);
    if (scope_lookup(s->scope, sym)) {
      diag_pushf(s, n->pos, DIAG_ERROR, "redefinition of type '%s'", name);
      break;
    }
    scope_bind(s->scope, sym, decl_new(s, DECL_TYPE));
    break;
  }
  case ND_FUNC: {
    if (!declare_function(s, n)) break;
    s->scope = scope_push(s->scope);
    for (size_t i = 0; i < n->as.func.param_len; i++) {
      Node *param = n->as.func.params[i];
      char *pname = slice_to_cstr(s, param->as.var_decl.name);
      Symbol *ps = sym_intern(pname);
      Decl *pd = decl_new(s, DECL_VAR);
      pd->as.var.type = param->as.var_decl.type;
      pd->as.var.is_const = param->as.var_decl.is_const;
      pd->as.var.type_name = param->as.var_decl.type_name;
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
  s->program = root;
  for (size_t i = 0; i < root->as.block.len; ++i) {
    Node *n = root->as.block.items[i];
    source_context(s, n);
    if (n->kind == ND_CLASS_DECL || n->kind == ND_STRUCT_DECL) {
      n->as.type_decl.base_decl = NULL;
      n->as.type_decl.inheritance_state = 0;
      analyze_stmt(s, n);
    }
  }
  bool valid = true;
  for (size_t i = 0; i < root->as.block.len; ++i) {
    Node *n = root->as.block.items[i];
    source_context(s, n);
    if (n->kind == ND_CLASS_DECL || n->kind == ND_STRUCT_DECL)
      valid = resolve_inheritance(s, n) && valid;
  }
  if (!valid) return;
  for (size_t i = 0; i < root->as.block.len; ++i) {
    Node *n = root->as.block.items[i];
    source_context(s, n);
    if (n->kind == ND_FUNC) declare_function(s, n);
  }
  for (size_t i = 0; i < root->as.block.len; ++i) {
    Node *n = root->as.block.items[i];
    source_context(s, n);
    if (n->kind != ND_CLASS_DECL && n->kind != ND_STRUCT_DECL) analyze_stmt(s, n);
  }
  // Methods have their own parameter scope and resolve fields through this/base.
  for (size_t i = 0; i < root->as.block.len; ++i) {
    Node *type = root->as.block.items[i];
    source_context(s, type);
    if (type->kind != ND_CLASS_DECL && type->kind != ND_STRUCT_DECL) continue;
    for (size_t j = 0; j < type->as.type_decl.len; ++j) {
      Node *method = type->as.type_decl.members[j];
      if (method->kind != ND_FUNC) continue;
      s->scope = scope_push(s->scope);
      s->current_class = method->as.func.is_static ? NULL : type;
      if (!method->as.func.is_static) {
        Decl *this_decl = decl_new(s, DECL_VAR);
        this_decl->as.var.type = TK_IDENT;
        this_decl->as.var.type_name = type->as.type_decl.name;
        scope_bind(s->scope, sym_intern("this"), this_decl);
      }
      for (size_t k = 0; k < method->as.func.param_len; ++k) {
        Node *param = method->as.func.params[k];
        Decl *d = decl_new(s, DECL_VAR);
        d->as.var.type = param->as.var_decl.type;
        d->as.var.type_name = param->as.var_decl.type_name;
        scope_bind(s->scope, sym_intern(slice_to_cstr(s, param->as.var_decl.name)), d);
      }
      TokenKind prev = s->current_ret;
      s->current_ret = method->as.func.ret_type;
      analyze_stmt(s, method->as.func.body);
      s->current_ret = prev;
      s->scope = scope_pop(s->scope);
      s->current_class = NULL;
    }
  }
}
