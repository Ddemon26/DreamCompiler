#include "parser.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Adds a diagnostic message to the parser's diagnostic list.
 *
 * @param p Pointer to the parser structure.
 * @param pos Position in the source code where the diagnostic occurred.
 * @param msg Diagnostic message to be added.
 */
static void diag_push(Parser *p, Pos pos, DiagSeverity sev, const char *msg) {
  if (p->diags.len + 1 > p->diags.cap) {
    p->diags.cap = p->diags.cap ? p->diags.cap * 2 : 4;
    p->diags.data = realloc(p->diags.data, p->diags.cap * sizeof(Diagnostic));
  }
  p->diags.data[p->diags.len++] = (Diagnostic){pos, msg, sev};
}

static void diag_pushf(Parser *p, Pos pos, DiagSeverity sev, const char *fmt,
                       ...) {
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);
  char *buf = arena_alloc(p->arena, n + 1);
  va_start(ap, fmt);
  vsnprintf(buf, n + 1, fmt, ap);
  va_end(ap);
  diag_push(p, pos, sev, buf);
}

/**
 * @brief Advances the parser to the next token.
 *
 * @param p Pointer to the parser structure.
 */
static void next(Parser *p) { p->tok = lexer_next(&p->lx); }

/**
 * @brief Initializes the parser with the given source code and memory arena.
 *
 * @param p Pointer to the parser structure.
 * @param a Pointer to the memory arena used for allocations.
 * @param src Pointer to the source code to be parsed.
 */
void parser_init(Parser *p, Arena *a, const char *src) {
  lexer_init(&p->lx, src);
  p->arena = a;
  p->diags.data = NULL;
  p->diags.len = 0;
  p->diags.cap = 0;
  p->types = NULL;
  p->type_len = 0;
  p->type_cap = 0;
  next(p);
}

/**
 * @brief Parses an expression with a given minimum precedence.
 *
 * @param p Pointer to the parser structure.
 * @param min_prec Minimum precedence for parsing the expression.
 * @return Pointer to the parsed expression node.
 */
static Node *parse_expr_prec(Parser *p, int min_prec);

/**
 * @brief Parses an expression.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed expression node.
 */
static Node *parse_expr(Parser *p);

/**
 * @brief Parses a statement.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed statement node.
 */
static Node *parse_stmt(Parser *p);

/**
 * @brief Parses a unary expression.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed unary expression node.
 */
static Node *parse_unary(Parser *p);

/**
 * @brief Parses a do-while loop.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed do-while loop node.
 */
static Node *parse_do_while(Parser *p);

/**
 * @brief Parses a for loop.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed for loop node.
 */
static Node *parse_for(Parser *p);

/**
 * @brief Parses a switch statement.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed switch statement node.
 */
static Node *parse_switch(Parser *p);
static Node *parse_var_decl(Parser *p);

/**
 * @brief Adds a node to a dynamic array of nodes.
 *
 * @param data Pointer to the array of node pointers.
 * @param len Pointer to the current length of the array.
 * @param cap Pointer to the current capacity of the array.
 * @param n Pointer to the node to be added.
 */
static void nodevec_push(Node ***data, size_t *len, size_t *cap, Node *n);
static void typevec_push(Parser *p, Slice name);
static bool typevec_contains(Parser *p, Token tok);

/**
 * @brief Checks if the given token kind represents a type keyword.
 *
 * @param k Token kind to check.
 * @return true if the token kind is a type keyword, false otherwise.
 */
static bool is_type_token(TokenKind k) {
  switch (k) {
  case TK_KW_INT:
  case TK_KW_FLOAT:
  case TK_KW_BOOL:
  case TK_KW_CHAR:
  case TK_KW_STRING:
  case TK_KW_VAR:
  case TK_KW_STRUCT:
  case TK_KW_CLASS:
    return true;
  default:
    return false;
  }
}

/**
 * @brief Infers a variable type from an initializer expression.
 */
static TokenKind infer_var_type(Node *expr) {
  if (!expr)
    return TK_KW_INT;
  switch (expr->kind) {
  case ND_INT:
    return TK_KW_INT;
  case ND_FLOAT:
    return TK_KW_FLOAT;
  case ND_CHAR:
    return TK_KW_CHAR;
  case ND_STRING:
    return TK_KW_STRING;
  case ND_BOOL:
    return TK_KW_BOOL;
  case ND_UNARY:
    return infer_var_type(expr->as.unary.expr);
  case ND_BINOP:
    return infer_var_type(expr->as.bin.lhs);
  case ND_COND:
    return infer_var_type(expr->as.cond.then_expr);
  default:
    return TK_KW_INT;
  }
}

/**
 * @brief Parses an if statement.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed if statement node.
 */
static Node *parse_if(Parser *p) {
  next(p); // consume 'if'
  if (p->tok.kind != TK_LPAREN) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '('");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node *cond = parse_expr_prec(p, 0);
  if (p->tok.kind != TK_RPAREN) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
  } else {
    next(p);
  }
  Node *then_br = parse_stmt(p);
  Node *else_br = NULL;
  if (p->tok.kind == TK_KW_ELSE) {
    next(p);
    else_br = parse_stmt(p);
  }
  Node *n = node_new(p->arena, ND_IF);
  n->as.if_stmt.cond = cond;
  n->as.if_stmt.then_br = then_br;
  n->as.if_stmt.else_br = else_br;
  return n;
}

/**
 * @brief Parses a while loop.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed while loop node.
 */
static Node *parse_while(Parser *p) {
  next(p); // consume 'while'
  if (p->tok.kind != TK_LPAREN) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '('");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node *cond = parse_expr_prec(p, 0);
  if (p->tok.kind != TK_RPAREN) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
  } else {
    next(p);
  }
  Node *body = parse_stmt(p);
  Node *n = node_new(p->arena, ND_WHILE);
  n->as.while_stmt.cond = cond;
  n->as.while_stmt.body = body;
  return n;
}

/**
 * @brief Parses a do-while loop.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed do-while loop node.
 */
static Node *parse_do_while(Parser *p) {
  next(p); // consume 'do'
  Node *body = parse_stmt(p);
  if (p->tok.kind != TK_KW_WHILE) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected 'while'");
    return node_new(p->arena, ND_ERROR);
  }
  next(p); // consume 'while'
  if (p->tok.kind != TK_LPAREN) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '('");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node *cond = parse_expr_prec(p, 0);
  if (p->tok.kind != TK_RPAREN)
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
  else
    next(p);
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
  Node *n = node_new(p->arena, ND_DO_WHILE);
  n->as.do_while_stmt.body = body;
  n->as.do_while_stmt.cond = cond;
  return n;
}

/**
 * @brief Parses a for loop.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed for loop node.
 */
static Node *parse_for(Parser *p) {
  next(p); // consume 'for'
  if (p->tok.kind != TK_LPAREN) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '('");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node *init = NULL;
  if (p->tok.kind != TK_SEMICOLON) {
    if (is_type_token(p->tok.kind)) {
      TokenKind type_tok = p->tok.kind;
      next(p);
      if (p->tok.kind != TK_IDENT) {
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected identifier");
        return node_new(p->arena, ND_ERROR);
      }
      init = node_new(p->arena, ND_VAR_DECL);
      init->as.var_decl.type = type_tok;
      init->as.var_decl.name.start = p->tok.start;
      init->as.var_decl.name.len = p->tok.len;
      next(p);
      if (p->tok.kind == TK_EQ) {
        next(p);
        init->as.var_decl.init = parse_expr(p);
      } else {
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected '='");
        init->as.var_decl.init = node_new(p->arena, ND_ERROR);
      }
      if (type_tok == TK_KW_VAR)
        init->as.var_decl.type = infer_var_type(init->as.var_decl.init);
    } else {
      init = parse_expr(p);
    }
  }
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
  Node *cond = NULL;
  if (p->tok.kind != TK_SEMICOLON)
    cond = parse_expr(p);
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
  Node *update = NULL;
  if (p->tok.kind != TK_RPAREN)
    update = parse_expr(p);
  if (p->tok.kind == TK_RPAREN)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
  Node *body = parse_stmt(p);
  Node *n = node_new(p->arena, ND_FOR);
  n->as.for_stmt.init = init;
  n->as.for_stmt.cond = cond;
  n->as.for_stmt.update = update;
  n->as.for_stmt.body = body;
  return n;
}

/**
 * @brief Parses a switch statement.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed switch statement node.
 */
static Node *parse_switch(Parser *p) {
  next(p); // consume 'switch'
  if (p->tok.kind != TK_LPAREN) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '('");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node *expr = parse_expr_prec(p, 0);
  if (p->tok.kind == TK_RPAREN)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
  if (p->tok.kind != TK_LBRACE) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '{'");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  SwitchCase *cases = NULL;
  size_t len = 0, cap = 0;
  while (p->tok.kind != TK_RBRACE && p->tok.kind != TK_EOF) {
    int is_default = 0;
    Node *val = NULL;
    if (p->tok.kind == TK_KW_CASE) {
      next(p);
      val = parse_expr(p);
    } else if (p->tok.kind == TK_KW_DEFAULT) {
      next(p);
      is_default = 1;
    } else {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected 'case' or 'default'");
      break;
    }
    if (p->tok.kind == TK_COLON)
      next(p);
    else
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected ':'");
    Node *body = parse_stmt(p);
    if (len + 1 > cap) {
      cap = cap ? cap * 2 : 4;
      cases = realloc(cases, cap * sizeof(SwitchCase));
    }
    cases[len++] =
        (SwitchCase){.is_default = is_default, .value = val, .body = body};
  }
  if (p->tok.kind == TK_RBRACE)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '}'");
  Node *n = node_new(p->arena, ND_SWITCH);
  n->as.switch_stmt.expr = expr;
  n->as.switch_stmt.cases = cases;
  n->as.switch_stmt.len = len;
  return n;
}

/**
 * @brief Parses a break statement.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed break statement node.
 */
static Node *parse_break(Parser *p) {
  next(p); // consume 'break'
  if (p->tok.kind == TK_SEMICOLON) {
    next(p);
  } else {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
  }
  return node_new(p->arena, ND_BREAK);
}

/**
 * @brief Parses a continue statement.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed continue statement node.
 */
static Node *parse_continue(Parser *p) {
  next(p); // consume 'continue'
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
  return node_new(p->arena, ND_CONTINUE);
}

/**
 * @brief Parses a return statement.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed return statement node.
 */
static Node *parse_return(Parser *p) {
  next(p); // consume 'return'
  Node *expr = NULL;
  if (p->tok.kind != TK_SEMICOLON) {
    expr = parse_expr(p);
  }
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
  Node *n = node_new(p->arena, ND_RETURN);
  n->as.ret.expr = expr;
  return n;
}

static Node *parse_throw(Parser *p) {
  next(p); // consume 'throw'
  Node *expr = NULL;
  if (p->tok.kind != TK_SEMICOLON)
    expr = parse_expr(p);
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
  Node *n = node_new(p->arena, ND_THROW);
  n->as.throw_stmt.expr = expr;
  return n;
}

static Node *parse_try(Parser *p) {
  next(p); // consume 'try'
  Node *body = parse_stmt(p);
  Node *catch_body = NULL;
  Node *finally_body = NULL;
  if (p->tok.kind == TK_KW_CATCH) {
    next(p);
    catch_body = parse_stmt(p);
  }
  if (p->tok.kind == TK_KW_FINALLY) {
    next(p);
    finally_body = parse_stmt(p);
  }
  Node *n = node_new(p->arena, ND_TRY);
  n->as.try_stmt.body = body;
  n->as.try_stmt.catch_body = catch_body;
  n->as.try_stmt.finally_body = finally_body;
  return n;
}

/**
 * @brief Parses a function definition.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed function node.
 */
static Node *parse_func(Parser *p) {
  Pos start_pos = p->tok.pos;
  next(p); // consume 'func'
  TokenKind ret_type = TK_KW_VOID;
  if (is_type_token(p->tok.kind) || p->tok.kind == TK_KW_VOID) {
    ret_type = p->tok.kind;
    next(p);
  }
  if (p->tok.kind != TK_IDENT) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected identifier");
    return node_new(p->arena, ND_ERROR);
  }
  Slice name = {p->tok.start, p->tok.len};
  next(p);
  if (p->tok.kind != TK_LPAREN) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '('");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node **params = NULL;
  size_t len = 0, cap = 0;
  if (p->tok.kind != TK_RPAREN) {
    for (;;) {
      if (!is_type_token(p->tok.kind) && p->tok.kind != TK_KW_VOID) {
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected parameter type");
        return node_new(p->arena, ND_ERROR);
      }
      TokenKind pt = p->tok.kind;
      next(p);
      if (p->tok.kind != TK_IDENT) {
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected parameter name");
        return node_new(p->arena, ND_ERROR);
      }
      Node *vd = node_new(p->arena, ND_VAR_DECL);
      vd->as.var_decl.type = pt;
      vd->as.var_decl.name.start = p->tok.start;
      vd->as.var_decl.name.len = p->tok.len;
      vd->as.var_decl.init = NULL;
      vd->as.var_decl.array_len = 0;
      next(p);
      nodevec_push(&params, &len, &cap, vd);
      if (p->tok.kind == TK_COMMA) {
        next(p);
        continue;
      }
      break;
    }
  }
  if (p->tok.kind == TK_RPAREN)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
  Node *body = parse_stmt(p);
  Node *fn = node_new(p->arena, ND_FUNC);
  fn->pos = start_pos;
  fn->as.func.ret_type = ret_type;
  fn->as.func.name = name;
  fn->as.func.params = params;
  fn->as.func.param_len = len;
  fn->as.func.body = body;
  fn->as.func.is_static = 0;
  return fn;
}

static Node *parse_type_decl(Parser *p, NodeKind kind) {
  Pos start_pos = p->tok.pos;
  next(p); // consume 'class' or 'struct'
  if (p->tok.kind != TK_IDENT) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected identifier");
    return node_new(p->arena, ND_ERROR);
  }
  Slice name = {p->tok.start, p->tok.len};
  next(p);
  if (p->tok.kind != TK_LBRACE) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '{'");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node **members = NULL;
  size_t len = 0, cap = 0;
  while (p->tok.kind != TK_RBRACE && p->tok.kind != TK_EOF) {
    if (p->tok.kind == TK_SEMICOLON) {
      next(p);
      continue;
    }
    int is_static = 0;
    if (p->tok.kind == TK_KW_STATIC) {
      is_static = 1;
      next(p);
    }
    Node *m = NULL;
    if (p->tok.kind == TK_KW_FUNC) {
      m = parse_func(p);
      if (m->kind == ND_FUNC)
        m->as.func.is_static = is_static;
    } else if (is_type_token(p->tok.kind)) {
      m = parse_var_decl(p);
      if (m->kind == ND_VAR_DECL)
        m->as.var_decl.is_static = is_static;
    } else {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected member declaration");
      m = parse_stmt(p);
    }
    nodevec_push(&members, &len, &cap, m);
  }
  if (p->tok.kind == TK_RBRACE)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '}'");
  Node *n = node_new(p->arena, kind);
  n->pos = start_pos;
  n->as.type_decl.name = name;
  n->as.type_decl.members = members;
  n->as.type_decl.len = len;
  typevec_push(p, name);
  return n;
}

/**
 * @brief Parses a primary expression.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed primary expression node.
 */
static Node *parse_primary(Parser *p) {
  Token t = p->tok;
  Node *n;
  switch (t.kind) {
  case TK_INT_LITERAL:
    n = node_new(p->arena, ND_INT);
    n->as.lit.start = t.start;
    n->as.lit.len = t.len;
    next(p);
    return n;
  case TK_FLOAT_LITERAL:
    n = node_new(p->arena, ND_FLOAT);
    n->as.lit.start = t.start;
    n->as.lit.len = t.len;
    next(p);
    return n;
  case TK_CHAR_LITERAL:
    n = node_new(p->arena, ND_CHAR);
    n->as.lit.start = t.start;
    n->as.lit.len = t.len;
    next(p);
    return n;
  case TK_STRING_LITERAL:
    n = node_new(p->arena, ND_STRING);
    n->as.lit.start = t.start;
    n->as.lit.len = t.len;
    next(p);
    return n;
  case TK_KW_CONSOLE: {
    next(p); /* consume 'Console' */
    if (p->tok.kind != TK_DOT) {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected '.'");
      return node_new(p->arena, ND_ERROR);
    }
    next(p); /* consume '.' */
    int newline = 0;
    int read = 0;
    if (p->tok.kind == TK_KW_WRITELINE) {
      newline = 1;
    } else if (p->tok.kind == TK_KW_WRITE) {
      newline = 0;
    } else if (p->tok.kind == TK_KW_READLINE) {
      read = 1;
    } else {
      diag_push(p, p->tok.pos, DIAG_ERROR,
                "expected Write, WriteLine or ReadLine");
      return node_new(p->arena, ND_ERROR);
    }
    next(p);
    if (p->tok.kind != TK_LPAREN) {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected '('");
      return node_new(p->arena, ND_ERROR);
    }
    next(p);
    Node *arg = NULL;
    if (!read)
      arg = parse_expr_prec(p, 0);
    if (p->tok.kind == TK_RPAREN)
      next(p);
    else
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
    n = node_new(p->arena, ND_CONSOLE_CALL);
    n->as.console.arg = arg;
    n->as.console.newline = newline;
    n->as.console.read = read;
    return n;
  }
  case TK_KW_NEW: {
    next(p); /* consume 'new' */
    if (p->tok.kind != TK_IDENT || !typevec_contains(p, p->tok)) {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected type identifier");
      return node_new(p->arena, ND_ERROR);
    }
    Slice type_name = {p->tok.start, p->tok.len};
    next(p);
    if (p->tok.kind != TK_LPAREN) {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected '('");
      return node_new(p->arena, ND_ERROR);
    }
    next(p);
    Node **args = NULL;
    size_t alen = 0, acap = 0;
    if (p->tok.kind != TK_RPAREN) {
      for (;;) {
        Node *arg = parse_expr_prec(p, 0);
        nodevec_push(&args, &alen, &acap, arg);
        if (p->tok.kind == TK_COMMA) {
          next(p);
          continue;
        }
        break;
      }
    }
    if (p->tok.kind == TK_RPAREN)
      next(p);
    else
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
    n = node_new(p->arena, ND_NEW);
    n->as.new_expr.type_name = type_name;
    n->as.new_expr.args = args;
    n->as.new_expr.arg_len = alen;
    return n;
  }
  case TK_KW_TRUE:
  case TK_KW_FALSE:
    n = node_new(p->arena, ND_BOOL);
    n->as.lit.start = t.start;
    n->as.lit.len = t.len;
    next(p);
    return n;
  case TK_ERROR:
    if (t.len == 1 && t.start[0] == '\'') {
      next(p);
      Token mid = p->tok;
      next(p);
      Token end = p->tok;
      if (mid.kind == TK_IDENT && mid.len == 1 && end.kind == TK_ERROR &&
          end.len == 1 && end.start[0] == '\'') {
        n = node_new(p->arena, ND_CHAR);
        n->as.lit.start = mid.start;
        n->as.lit.len = 1;
        next(p);
        return n;
      }
      diag_push(p, t.pos, DIAG_ERROR, "malformed char literal");
      return node_new(p->arena, ND_ERROR);
    }
    // fallthrough
  case TK_IDENT:
    n = node_new(p->arena, ND_IDENT);
    n->as.ident.start = t.start;
    n->as.ident.len = t.len;
    next(p);
    return n;
  case TK_LPAREN: {
    next(p);
    n = parse_expr_prec(p, 0);
    if (p->tok.kind == TK_RPAREN)
      next(p);
    else
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
    return n;
  }
  default:
    diag_push(p, t.pos, DIAG_ERROR, "unexpected token in expression");
    next(p);
    n = node_new(p->arena, ND_ERROR);
    return n;
  }
}

/**
 * @brief Parses a postfix expression.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed postfix expression node.
 */
static Node *parse_postfix(Parser *p) {
  Node *n = parse_primary(p);
  for (;;) {
    if (p->tok.kind == TK_LPAREN) {
      next(p); // consume '('
      Node **args = NULL;
      size_t len = 0, cap = 0;
      if (p->tok.kind != TK_RPAREN) {
        for (;;) {
          Node *arg = parse_expr_prec(p, 0);
          nodevec_push(&args, &len, &cap, arg);
          if (p->tok.kind == TK_COMMA) {
            next(p);
            continue;
          }
          break;
        }
      }
      if (p->tok.kind == TK_RPAREN)
        next(p);
      else
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
      Node *call = node_new(p->arena, ND_CALL);
      call->as.call.callee = n;
      call->as.call.args = args;
      call->as.call.len = len;
      n = call;
    } else if (p->tok.kind == TK_PLUSPLUS || p->tok.kind == TK_MINUSMINUS) {
      TokenKind op = p->tok.kind;
      next(p);
      Node *post = node_new(p->arena, ND_POST_UNARY);
      post->as.unary.op = op;
      post->as.unary.expr = n;
      n = post;
    } else if (p->tok.kind == TK_LBRACKET) {
      next(p);
      Node *idx = parse_expr_prec(p, 0);
      if (p->tok.kind == TK_RBRACKET)
        next(p);
      else
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected ']'");
      Node *idxn = node_new(p->arena, ND_INDEX);
      idxn->as.index.array = n;
      idxn->as.index.index = idx;
      n = idxn;
    } else if (p->tok.kind == TK_DOT) {
      next(p);
      if (p->tok.kind != TK_IDENT) {
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected identifier");
        return node_new(p->arena, ND_ERROR);
      }
      Node *fld = node_new(p->arena, ND_FIELD);
      fld->as.field.object = n;
      fld->as.field.name.start = p->tok.start;
      fld->as.field.name.len = p->tok.len;
      n = fld;
      next(p);
    } else {
      break;
    }
  }
  return n;
}

/**
 * @brief Parses a unary expression.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed unary expression node.
 */
static Node *parse_unary(Parser *p) {
  if (p->tok.kind == TK_MINUS || p->tok.kind == TK_PLUS ||
      p->tok.kind == TK_BANG || p->tok.kind == TK_TILDE ||
      p->tok.kind == TK_PLUSPLUS || p->tok.kind == TK_MINUSMINUS) {
    TokenKind op = p->tok.kind;
    next(p);
    Node *expr = parse_unary(p);
    Node *n = node_new(p->arena, ND_UNARY);
    n->as.unary.op = op;
    n->as.unary.expr = expr;
    return n;
  }
  return parse_postfix(p);
}

static Node *parse_var_decl(Parser *p) {
  Token type_tok = p->tok;
  next(p); // consume type
  Node **items = NULL;
  size_t len = 0, cap = 0;
  for (;;) {
    if (p->tok.kind != TK_IDENT) {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected identifier");
      return node_new(p->arena, ND_ERROR);
    }
    Node *n = node_new(p->arena, ND_VAR_DECL);
    n->as.var_decl.type = type_tok.kind;
    n->as.var_decl.type_name.start = type_tok.start;
    n->as.var_decl.type_name.len = type_tok.len;
    n->as.var_decl.name.start = p->tok.start;
    n->as.var_decl.name.len = p->tok.len;
    n->as.var_decl.array_len = 0;
    n->as.var_decl.is_static = 0;
    next(p);
    if (p->tok.kind == TK_LBRACKET) {
      next(p);
      if (p->tok.kind != TK_INT_LITERAL) {
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected array size");
        n->as.var_decl.array_len = 0;
      } else {
        n->as.var_decl.array_len = strtoul(p->tok.start, NULL, 10);
        next(p);
      }
      if (p->tok.kind == TK_RBRACKET)
        next(p);
      else
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected ']'");
    }
    if (p->tok.kind == TK_EQ) {
      next(p);
      n->as.var_decl.init = parse_expr(p);
    } else {
      n->as.var_decl.init = NULL;
    }
    if (type_tok.kind == TK_KW_VAR) {
      if (!n->as.var_decl.init) {
        diag_push(p, p->tok.pos, DIAG_ERROR,
                  "var declaration requires initializer");
        n->as.var_decl.type = TK_KW_INT;
      } else {
        n->as.var_decl.type = infer_var_type(n->as.var_decl.init);
      }
    }
    nodevec_push(&items, &len, &cap, n);
    if (p->tok.kind == TK_COMMA) {
      next(p);
      continue;
    }
    break;
  }
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
  if (len == 1) {
    Node *single = items[0];
    free(items);
    return single;
  }
  Node *blk = node_new(p->arena, ND_BLOCK);
  blk->as.block.items = items;
  blk->as.block.len = len;
  return blk;
}

/**
 * @brief Determines the precedence of a given token kind.
 *
 * @param k Token kind to evaluate.
 * @return Precedence level of the token kind, or -1 if not applicable.
 */
static int precedence(TokenKind k) {
  switch (k) {
  case TK_EQ:
  case TK_PLUSEQ:
  case TK_MINUSEQ:
  case TK_STAREQ:
  case TK_SLASHEQ:
  case TK_PERCENTEQ:
  case TK_ANDEQ:
  case TK_OREQ:
  case TK_XOREQ:
  case TK_LSHIFTEQ:
  case TK_RSHIFTEQ:
  case TK_QMARKQMARKEQ:
  case TK_QUESTION:
    return 1;
  case TK_OROR:
    return 2;
  case TK_ANDAND:
    return 3;
  case TK_EQEQ:
  case TK_NEQ:
  case TK_LT:
  case TK_GT:
  case TK_LTEQ:
  case TK_GTEQ:
    return 7;
  case TK_LSHIFT:
  case TK_RSHIFT:
    return 8;
  case TK_PLUS:
  case TK_MINUS:
    return 9;
  case TK_STAR:
  case TK_SLASH:
  case TK_PERCENT:
    return 10;
  case TK_OR:
    return 4;
  case TK_CARET:
    return 5;
  case TK_AND:
    return 6;
  default:
    return -1;
  }
}

/**
 * @brief Determines if a token kind is right-associative.
 *
 * @param k Token kind to evaluate.
 * @return 1 if the token kind is right-associative, 0 otherwise.
 */
static int right_assoc(TokenKind k) {
  switch (k) {
  case TK_EQ:
  case TK_PLUSEQ:
  case TK_MINUSEQ:
  case TK_STAREQ:
  case TK_SLASHEQ:
  case TK_PERCENTEQ:
  case TK_ANDEQ:
  case TK_OREQ:
  case TK_XOREQ:
  case TK_LSHIFTEQ:
  case TK_RSHIFTEQ:
  case TK_QMARKQMARKEQ:
  case TK_QUESTION:
    return 1;
  default:
    return 0;
  }
}

/**
 * @brief Parses an expression with a given minimum precedence.
 *
 * @param p Pointer to the parser structure.
 * @param min_prec Minimum precedence for parsing the expression.
 * @return Pointer to the parsed expression node.
 */
static Node *parse_expr_prec(Parser *p, int min_prec) {
  Node *lhs = parse_unary(p);
  for (;;) {
    if (p->tok.kind == TK_QUESTION && min_prec <= 1) {
      next(p);
      Node *then_expr = parse_expr_prec(p, 0);
      if (p->tok.kind != TK_COLON) {
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected ':'");
      } else {
        next(p);
      }
      Node *else_expr = parse_expr_prec(p, 1);
      Node *tern = node_new(p->arena, ND_COND);
      tern->as.cond.cond = lhs;
      tern->as.cond.then_expr = then_expr;
      tern->as.cond.else_expr = else_expr;
      lhs = tern;
      continue;
    }
    int prec = precedence(p->tok.kind);
    if (prec < min_prec)
      break;
    TokenKind op = p->tok.kind;
    next(p);
    int next_prec = right_assoc(op) ? prec : prec + 1;
    Node *rhs = parse_expr_prec(p, next_prec);
    Node *bin = node_new(p->arena, ND_BINOP);
    bin->as.bin.op = op;
    bin->as.bin.lhs = lhs;
    bin->as.bin.rhs = rhs;
    lhs = bin;
  }
  return lhs;
}

/**
 * @brief Parses an expression.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed expression node.
 */
static Node *parse_expr(Parser *p) { return parse_expr_prec(p, 0); }

/**
 * @brief Adds a node to a dynamic array of nodes, resizing the array if
 * necessary.
 *
 * @param data Pointer to the array of node pointers.
 * @param len Pointer to the current length of the array.
 * @param cap Pointer to the current capacity of the array.
 * @param n Pointer to the node to be added.
 */
static void nodevec_push(Node ***data, size_t *len, size_t *cap, Node *n) {
  if (*len + 1 > *cap) {
    *cap = *cap ? *cap * 2 : 4;
    *data = realloc(*data, *cap * sizeof(Node *));
  }
  (*data)[(*len)++] = n;
}

static void typevec_push(Parser *p, Slice name) {
  if (p->type_len + 1 > p->type_cap) {
    p->type_cap = p->type_cap ? p->type_cap * 2 : 4;
    p->types = realloc(p->types, p->type_cap * sizeof(Slice));
  }
  p->types[p->type_len++] = name;
}

static bool typevec_contains(Parser *p, Token tok) {
  for (size_t i = 0; i < p->type_len; i++) {
    Slice s = p->types[i];
    if (s.len == tok.len && strncmp(s.start, tok.start, tok.len) == 0)
      return true;
  }
  return false;
}

/**
 * @brief Parses a statement based on the current token kind.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed statement node.
 */
static Node *parse_stmt(Parser *p) {
  if (p->tok.kind == TK_KW_IF) {
    return parse_if(p);
  }
  if (p->tok.kind == TK_KW_DO) {
    return parse_do_while(p);
  }
  if (p->tok.kind == TK_KW_FOR) {
    return parse_for(p);
  }
  if (p->tok.kind == TK_KW_SWITCH) {
    return parse_switch(p);
  }
  if (p->tok.kind == TK_KW_WHILE) {
    return parse_while(p);
  }
  if (p->tok.kind == TK_KW_BREAK) {
    return parse_break(p);
  }
  if (p->tok.kind == TK_KW_CONTINUE) {
    return parse_continue(p);
  }
  if (p->tok.kind == TK_KW_RETURN) {
    return parse_return(p);
  }
  if (p->tok.kind == TK_KW_TRY) {
    return parse_try(p);
  }
  if (p->tok.kind == TK_KW_THROW) {
    return parse_throw(p);
  }
  if (p->tok.kind == TK_KW_CLASS) {
    return parse_type_decl(p, ND_CLASS_DECL);
  }
  if (p->tok.kind == TK_KW_STRUCT) {
    return parse_type_decl(p, ND_STRUCT_DECL);
  }
  if (p->tok.kind == TK_KW_FUNC) {
    return parse_func(p);
  }
  if (is_type_token(p->tok.kind)) {
    return parse_var_decl(p);
  }
  if (p->tok.kind == TK_IDENT && typevec_contains(p, p->tok)) {
    Token la = lexer_peek(&p->lx);
    if (la.kind == TK_IDENT)
      return parse_var_decl(p);
  }
  if (p->tok.kind == TK_LBRACE) {
    next(p);
    Node **items = NULL;
    size_t len = 0, cap = 0;
    while (p->tok.kind != TK_RBRACE && p->tok.kind != TK_EOF) {
      Node *st = parse_stmt(p);
      nodevec_push(&items, &len, &cap, st);
    }
    if (p->tok.kind == TK_RBRACE)
      next(p);
    else
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected '}'");
    Node *blk = node_new(p->arena, ND_BLOCK);
    blk->as.block.items = items;
    blk->as.block.len = len;
    return blk;
  }
  Node *expr = parse_expr(p);
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
  Node *st = node_new(p->arena, ND_EXPR_STMT);
  st->as.expr_stmt.expr = expr;
  return st;
}

/**
 * @brief Parses the entire program into a block node.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed program block node.
 */
Node *parse_program(Parser *p) {
  Node **items = NULL;
  size_t len = 0, cap = 0;
  while (p->tok.kind != TK_EOF) {
    Node *st = parse_stmt(p);
    nodevec_push(&items, &len, &cap, st);
  }
  Node *blk = node_new(p->arena, ND_BLOCK);
  blk->as.block.items = items;
  blk->as.block.len = len;
  return blk;
}
