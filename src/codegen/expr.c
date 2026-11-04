#include "expr.h"
#include "stmt.h"
#include "../sem/analysis.h"
#include <string.h>

static Node *expr_member(CGCtx *ctx, Node *n, Node **owner);

static Node *base_type(CGCtx *ctx) {
  Node *type = cg_type_decl(cgctx_lookup_name(ctx, "this", 4));
  return type ? type->as.type_decl.base_decl : NULL;
}

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
  case ND_FIELD:
  case ND_BASE: {
    Node *member = expr_member(ctx, n, NULL);
    if (member && member->kind == ND_VAR_DECL) return member->as.var_decl.type_name;
    return (Slice){0};
  }
  default:
    return (Slice){NULL, 0};
  }
}

static Node *expr_member(CGCtx *ctx, Node *n, Node **owner) {
  if (n->kind == ND_CALL) {
    if (n->as.call.callee->kind == ND_IDENT) return cg_function(n->as.call.callee->as.ident);
    return expr_member(ctx, n->as.call.callee, owner);
  }
  if (n->kind == ND_FIELD) return cg_member(expr_type(ctx, n->as.field.object), n->as.field.name, owner);
  if (n->kind == ND_BASE) return sem_find_member(base_type(ctx), n->as.base.name, owner);
  if (n->kind == ND_INDEX) return expr_member(ctx, n->as.index.array, owner);
  return NULL;
}

static TokenKind member_type(CGCtx *ctx, Node *n) {
  Node *member = expr_member(ctx, n, NULL);
  if (!member) return TK_ERROR;
  return member->kind == ND_FUNC ? member->as.func.ret_type : member->as.var_decl.type;
}

static int is_type_reference(CGCtx *ctx, Node *n) {
  return n && n->kind == ND_IDENT && !cgctx_has_var(ctx, n->as.ident.start, n->as.ident.len) && cg_is_known_type(n->as.ident);
}

static void emit_receiver(CGCtx *ctx, COut *b, Node *object, Slice type, Node *owner) {
  if (owner && owner->kind == ND_CLASS_DECL) {
    Slice name = owner->as.type_decl.name;
    c_out_write(b, "((struct %.*s *)", (int)name.len, name.start);
    if (object) cg_emit_expr(ctx, b, object);
    else c_out_write(b, "this");
    c_out_write(b, ")");
  } else if (cg_is_class_type(type)) {
    cg_emit_expr(ctx, b, object);
  } else {
    c_out_write(b, "&(");
    cg_emit_expr(ctx, b, object);
    c_out_write(b, ")");
  }
}

void cg_emit_value_as(CGCtx *ctx, COut *b, Slice target_type, Node *n) {
  Node *target = cg_type_decl(target_type);
  Node *source = cg_type_decl(expr_type(ctx, n));
  if (target && target->kind == ND_CLASS_DECL && source != target && sem_is_subclass(source, target))
    c_out_write(b, "(struct %.*s *)", (int)target_type.len, target_type.start);
  cg_emit_expr(ctx, b, n);
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
    return "?\?=";
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
  if (cg_is_float_expr(ctx, arg)) return "%f";
  if (member_type(ctx, arg) == TK_KW_CHAR) return "%c";
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
  case ND_CALL:
  case ND_FIELD:
  case ND_BASE:
    return member_type(ctx, n) == TK_KW_STRING;
  case ND_BINOP:
    // Handle string concatenation operations (+ operator with strings)
    if (n->as.bin.op == TK_PLUS) {
      int lhs_is_string = cg_is_string_expr(ctx, n->as.bin.lhs);
      int rhs_is_string = cg_is_string_expr(ctx, n->as.bin.rhs);
      int lhs_is_int = cg_is_int_expr(ctx, n->as.bin.lhs);
      int rhs_is_int = cg_is_int_expr(ctx, n->as.bin.rhs);
      int lhs_is_float = cg_is_float_expr(ctx, n->as.bin.lhs);
      int rhs_is_float = cg_is_float_expr(ctx, n->as.bin.rhs);
      
      // Any concatenation with at least one string results in a string
      return (lhs_is_string && rhs_is_string) ||
             (lhs_is_string && rhs_is_int) ||
             (lhs_is_int && rhs_is_string) ||
             (lhs_is_string && rhs_is_float) ||
             (lhs_is_float && rhs_is_string);
    }
    return 0;
  case ND_INDEX:
    if (member_type(ctx, n) == TK_KW_STRING) return 1;
    // Array access - check if the array is a string array
    if (n->as.index.array->kind == ND_IDENT) {
      TokenKind array_type = cgctx_lookup(ctx, n->as.index.array->as.ident.start, 
                                          n->as.index.array->as.ident.len);
      return array_type == TK_KW_STRING;
    }
    return 0;
  default:
    return 0;
  }
}

int cg_is_int_expr(CGCtx *ctx, Node *n) {
  switch (n->kind) {
  case ND_INT:
    return 1;
  case ND_IDENT:
    return cgctx_lookup(ctx, n->as.ident.start, n->as.ident.len) == TK_KW_INT;
  case ND_FIELD: case ND_BASE: case ND_CALL: case ND_INDEX:
    return member_type(ctx, n) == TK_KW_INT;
  default:
    return 0;
  }
}

int cg_is_float_expr(CGCtx *ctx, Node *n) {
  switch (n->kind) {
  case ND_FLOAT:
    return 1;
  case ND_IDENT:
    return cgctx_lookup(ctx, n->as.ident.start, n->as.ident.len) == TK_KW_FLOAT;
  case ND_FIELD: case ND_BASE: case ND_CALL: case ND_INDEX:
    return member_type(ctx, n) == TK_KW_FLOAT;
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
  case ND_NULL:
    if (n->kind == ND_NULL) {
      c_out_write(b, "((void*)0)");
    } else if (n->kind == ND_BOOL) {
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
    if (n->as.bin.op == TK_PLUS) {
      int lhs_is_string = cg_is_string_expr(ctx, n->as.bin.lhs);
      int rhs_is_string = cg_is_string_expr(ctx, n->as.bin.rhs);
      int lhs_is_int = cg_is_int_expr(ctx, n->as.bin.lhs);
      int rhs_is_int = cg_is_int_expr(ctx, n->as.bin.rhs);
      int lhs_is_float = cg_is_float_expr(ctx, n->as.bin.lhs);
      int rhs_is_float = cg_is_float_expr(ctx, n->as.bin.rhs);
      
      // Handle string concatenation with mixed types
      if (lhs_is_string && rhs_is_string) {
        // String + String - use existing dream_concat
        c_out_write(b, "dream_concat(");
        cg_emit_expr(ctx, b, n->as.bin.lhs);
        c_out_write(b, ", ");
        cg_emit_expr(ctx, b, n->as.bin.rhs);
        c_out_write(b, ")");
      } else if (lhs_is_string && rhs_is_int) {
        // String + Int
        c_out_write(b, "dream_concat_string_int(");
        cg_emit_expr(ctx, b, n->as.bin.lhs);
        c_out_write(b, ", ");
        cg_emit_expr(ctx, b, n->as.bin.rhs);
        c_out_write(b, ")");
      } else if (lhs_is_int && rhs_is_string) {
        // Int + String
        c_out_write(b, "dream_concat_int_string(");
        cg_emit_expr(ctx, b, n->as.bin.lhs);
        c_out_write(b, ", ");
        cg_emit_expr(ctx, b, n->as.bin.rhs);
        c_out_write(b, ")");
      } else if (lhs_is_string && rhs_is_float) {
        // String + Float
        c_out_write(b, "dream_concat_string_float(");
        cg_emit_expr(ctx, b, n->as.bin.lhs);
        c_out_write(b, ", ");
        cg_emit_expr(ctx, b, n->as.bin.rhs);
        c_out_write(b, ")");
      } else if (lhs_is_float && rhs_is_string) {
        // Float + String
        c_out_write(b, "dream_concat_float_string(");
        cg_emit_expr(ctx, b, n->as.bin.lhs);
        c_out_write(b, ", ");
        cg_emit_expr(ctx, b, n->as.bin.rhs);
        c_out_write(b, ")");
      } else {
        // Regular arithmetic addition
        cg_emit_expr(ctx, b, n->as.bin.lhs);
        c_out_write(b, " %s ", op_text(n->as.bin.op));
        cg_emit_expr(ctx, b, n->as.bin.rhs);
      }
    } else {
      // Non-plus operations
      cg_emit_expr(ctx, b, n->as.bin.lhs);
      c_out_write(b, " %s ", op_text(n->as.bin.op));
      if (n->as.bin.op == TK_EQ) cg_emit_value_as(ctx, b, expr_type(ctx, n->as.bin.lhs), n->as.bin.rhs);
      else cg_emit_expr(ctx, b, n->as.bin.rhs);
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
    // Check for enum member access like VkResult.Success
    if (n->as.field.object->kind == ND_IDENT) {
      // Look up the object type to see if it's an enum
      TokenKind obj_type = cgctx_lookup(ctx, n->as.field.object->as.ident.start,
                                       n->as.field.object->as.ident.len);
      if (obj_type == TK_KW_ENUM) {
        // This is enum member access - just emit the member name
        c_out_write(b, "%.*s", (int)n->as.field.name.len, n->as.field.name.start);
        break;
      }
    }
    
    Slice ty = expr_type(ctx, n->as.field.object);
    Node *owner = NULL;
    Node *member = cg_member(ty, n->as.field.name, &owner);
    if (owner && member && member->kind == ND_VAR_DECL && member->as.var_decl.is_static) ty = owner->as.type_decl.name;
    if (is_type_reference(ctx, n->as.field.object) || (member && member->kind == ND_VAR_DECL && member->as.var_decl.is_static)) {
      c_out_write(b, "%.*s_%.*s", (int)ty.len, ty.start,
                  (int)n->as.field.name.len, n->as.field.name.start);
    } else {
      if (cg_is_class_type(ty)) {
        emit_receiver(ctx, b, n->as.field.object, ty, owner);
        c_out_write(b, "->%.*s", (int)n->as.field.name.len,
                    n->as.field.name.start);
      } else {
        cg_emit_expr(ctx, b, n->as.field.object);
        c_out_write(b, ".%.*s", (int)n->as.field.name.len,
                    n->as.field.name.start);
      }
    }
    break;
  }
  case ND_CALL:
    if (n->as.call.callee && (n->as.call.callee->kind == ND_FIELD || n->as.call.callee->kind == ND_BASE)) {
      Node *fld = n->as.call.callee;
      int is_base = fld->kind == ND_BASE;
      Node *object = is_base ? NULL : fld->as.field.object;
      Node *base = is_base ? base_type(ctx) : NULL;
      Slice name = is_base ? fld->as.base.name : fld->as.field.name;
      Slice ty = is_base ? (base ? base->as.type_decl.name : (Slice){0}) : expr_type(ctx, object);
      Node *owner = NULL;
      Node *method = cg_member(ty, name, &owner);
      int has_receiver = method && method->kind == ND_FUNC ? !method->as.func.is_static : !is_type_reference(ctx, object);
      // Preserve module-qualified calls until the multi-file backend is completed.
      if (!ty.len && object && object->kind == ND_IDENT && !cgctx_has_var(ctx, object->as.ident.start, object->as.ident.len)) {
        ty = object->as.ident;
        has_receiver = 0;
      }
      if (ty.len) {
        Slice declaring_type = owner ? owner->as.type_decl.name : ty;
        c_out_write(b, "%.*s_%.*s(", (int)declaring_type.len, declaring_type.start, (int)name.len, name.start);
        if (has_receiver) emit_receiver(ctx, b, object, ty, owner);
        for (size_t i = 0; i < n->as.call.len; i++) {
          if (has_receiver || i)
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
  case ND_BASE:
    {
      Node *owner = NULL;
      Node *base = base_type(ctx);
      Node *member = sem_find_member(base, n->as.base.name, &owner);
      if (member && member->kind == ND_VAR_DECL && member->as.var_decl.is_static) {
        Slice name = owner->as.type_decl.name;
        c_out_write(b, "%.*s_%.*s", (int)name.len, name.start, (int)n->as.base.name.len, n->as.base.name.start);
      } else {
        emit_receiver(ctx, b, NULL, base ? base->as.type_decl.name : (Slice){0}, owner);
        c_out_write(b, "->%.*s", (int)n->as.base.name.len, n->as.base.name.start);
      }
    }
    break;
  case ND_AWAIT:
    // Await expression: dr_task_await(expr)
    c_out_write(b, "dr_task_await(");
    cg_emit_expr(ctx, b, n->as.await_expr.expr);
    c_out_write(b, ")");
    break;
  default:
    c_out_write(b, "0");
    break;
  }
}
