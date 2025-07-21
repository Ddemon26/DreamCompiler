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
  case ND_CALL:
    // Handle method calls that might return strings
    if (n->as.call.callee && n->as.call.callee->kind == ND_FIELD) {
      Node *fld = n->as.call.callee;
      // Check if method name suggests it returns a string
      if (fld->as.field.name.len >= 7 && 
          strncmp(fld->as.field.name.start, "getName", 7) == 0) {
        return 1;
      }
      // Add other common string-returning method patterns
      if (fld->as.field.name.len >= 8 && 
          strncmp(fld->as.field.name.start, "toString", 8) == 0) {
        return 1;
      }
    }
    return 0;
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
      if (cg_is_class_type(ty)) {
        // For class field access, check if this might be an inherited field
        // that needs to go through the base member
        // TODO: Implement proper inheritance field resolution
        // For now, try direct access first, base access will be handled elsewhere
        c_out_write(b, "->%.*s", (int)n->as.field.name.len,
                    n->as.field.name.start);
      } else {
        c_out_write(b, ".%.*s", (int)n->as.field.name.len,
                    n->as.field.name.start);
      }
    }
    break;
  }
  case ND_CALL:
    if (n->as.call.callee && n->as.call.callee->kind == ND_FIELD) {
      Node *fld = n->as.call.callee;
      
      // Check if this is a module-qualified call (module.function)
      if (fld->as.field.object && fld->as.field.object->kind == ND_IDENT) {
        int is_var = cgctx_has_var(ctx, fld->as.field.object->as.ident.start,
                                   fld->as.field.object->as.ident.len);
        if (!is_var) {
          // This appears to be a module-qualified call like math_utils.add()
          // Convert to C function call: module_function()
          c_out_write(b, "%.*s_%.*s(", 
                      (int)fld->as.field.object->as.ident.len, fld->as.field.object->as.ident.start,
                      (int)fld->as.field.name.len, fld->as.field.name.start);
          
          // Emit arguments
          for (size_t i = 0; i < n->as.call.len; i++) {
            if (i) c_out_write(b, ", ");
            cg_emit_expr(ctx, b, n->as.call.args[i]);
          }
          c_out_write(b, ")");
          break;
        }
      }
      
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
  case ND_BASE:
    // Access base class member: since inheritance is flattened, access directly
    // Changed from this->base.member_name to this->member_name 
    c_out_write(b, "this->%.*s", (int)n->as.base.name.len, n->as.base.name.start);
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
