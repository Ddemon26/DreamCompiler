#include "parser.h"
#include <stdlib.h>
#include <string.h>

static void diag_push(Parser *p, Pos pos, const char *msg) {
  if (p->diags.len + 1 > p->diags.cap) {
    p->diags.cap = p->diags.cap ? p->diags.cap * 2 : 4;
    p->diags.data = realloc(p->diags.data, p->diags.cap * sizeof(Diagnostic));
  }
  p->diags.data[p->diags.len++] = (Diagnostic){pos, msg};
}

static void next(Parser *p) { p->tok = lexer_next(&p->lx); }

void parser_init(Parser *p, Arena *a, const char *src) {
  lexer_init(&p->lx, src);
  p->arena = a;
  p->diags.data = NULL;
  p->diags.len = 0;
  p->diags.cap = 0;
  next(p);
}

static Node *parse_expr_prec(Parser *p, int min_prec);
static Node *parse_expr(Parser *p);
static Node *parse_stmt(Parser *p);

static bool is_type_token(TokenKind k) {
  switch (k) {
  case TK_KW_INT:
  case TK_KW_FLOAT:
  case TK_KW_BOOL:
  case TK_KW_CHAR:
  case TK_KW_STRING:
    return true;
  default:
    return false;
  }
}

static Node *parse_if(Parser *p) {
  next(p); // consume 'if'
  if (p->tok.kind != TK_LPAREN) {
    diag_push(p, p->tok.pos, "expected '('");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node *cond = parse_expr_prec(p, 0);
  if (p->tok.kind != TK_RPAREN) {
    diag_push(p, p->tok.pos, "expected ')'");
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

static Node *parse_while(Parser *p) {
  next(p); // consume 'while'
  if (p->tok.kind != TK_LPAREN) {
    diag_push(p, p->tok.pos, "expected '('");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node *cond = parse_expr_prec(p, 0);
  if (p->tok.kind != TK_RPAREN) {
    diag_push(p, p->tok.pos, "expected ')'");
  } else {
    next(p);
  }
  Node *body = parse_stmt(p);
  Node *n = node_new(p->arena, ND_WHILE);
  n->as.while_stmt.cond = cond;
  n->as.while_stmt.body = body;
  return n;
}

static Node *parse_break(Parser *p) {
  next(p); // consume 'break'
  if (p->tok.kind == TK_SEMICOLON) {
    next(p);
  } else {
    diag_push(p, p->tok.pos, "expected ';'");
  }
  return node_new(p->arena, ND_BREAK);
}

static Node *parse_continue(Parser *p) {
  next(p); // consume 'continue'
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, "expected ';'");
  return node_new(p->arena, ND_CONTINUE);
}

static Node *parse_return(Parser *p) {
  next(p); // consume 'return'
  Node *expr = NULL;
  if (p->tok.kind != TK_SEMICOLON) {
    expr = parse_expr(p);
  }
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, "expected ';'");
  Node *n = node_new(p->arena, ND_RETURN);
  n->as.ret.expr = expr;
  return n;
}

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
      diag_push(p, p->tok.pos, "expected '.'");
      return node_new(p->arena, ND_ERROR);
    }
    next(p); /* consume '.' */
    int newline = 0;
    if (p->tok.kind == TK_KW_WRITELINE) {
      newline = 1;
    } else if (p->tok.kind == TK_KW_WRITE) {
      newline = 0;
    } else {
      diag_push(p, p->tok.pos, "expected Write or WriteLine");
      return node_new(p->arena, ND_ERROR);
    }
    next(p);
    if (p->tok.kind != TK_LPAREN) {
      diag_push(p, p->tok.pos, "expected '('");
      return node_new(p->arena, ND_ERROR);
    }
    next(p);
    Node *arg = parse_expr_prec(p, 0);
    if (p->tok.kind == TK_RPAREN)
      next(p);
    else
      diag_push(p, p->tok.pos, "expected ')'");
    n = node_new(p->arena, ND_CONSOLE_CALL);
    n->as.console.arg = arg;
    n->as.console.newline = newline;
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
      diag_push(p, t.pos, "malformed char literal");
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
      diag_push(p, p->tok.pos, "expected ')'");
    return n;
  }
  default:
    diag_push(p, t.pos, "unexpected token in expression");
    next(p);
    n = node_new(p->arena, ND_ERROR);
    return n;
  }
}

static Node *parse_var_decl(Parser *p) {
  TokenKind type_tok = p->tok.kind;
  next(p); // consume type
  if (p->tok.kind != TK_IDENT) {
    diag_push(p, p->tok.pos, "expected identifier");
    return node_new(p->arena, ND_ERROR);
  }
  Node *n = node_new(p->arena, ND_VAR_DECL);
  n->as.var_decl.type = type_tok;
  n->as.var_decl.name.start = p->tok.start;
  n->as.var_decl.name.len = p->tok.len;
  next(p);
  if (p->tok.kind == TK_EQ) {
    next(p);
    n->as.var_decl.init = parse_expr(p);
  } else {
    diag_push(p, p->tok.pos, "expected '='");
    n->as.var_decl.init = node_new(p->arena, ND_ERROR);
  }
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, "expected ';'");
  return n;
}
static int precedence(TokenKind k) {
  switch (k) {
  case TK_EQ:
    return 1;
  case TK_EQEQ:
  case TK_NEQ:
  case TK_LT:
  case TK_GT:
  case TK_LTEQ:
  case TK_GTEQ:
    return 2;
  case TK_PLUS:
  case TK_MINUS:
    return 3;
  case TK_STAR:
  case TK_SLASH:
  case TK_PERCENT:
    return 4;
  default:
    return -1;
  }
}

static int right_assoc(TokenKind k) { return k == TK_EQ; }

static Node *parse_expr_prec(Parser *p, int min_prec) {
  Node *lhs = parse_primary(p);
  for (;;) {
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

static Node *parse_expr(Parser *p) { return parse_expr_prec(p, 0); }

static void nodevec_push(Node ***data, size_t *len, size_t *cap, Node *n) {
  if (*len + 1 > *cap) {
    *cap = *cap ? *cap * 2 : 4;
    *data = realloc(*data, *cap * sizeof(Node *));
  }
  (*data)[(*len)++] = n;
}

static Node *parse_stmt(Parser *p) {
  if (p->tok.kind == TK_KW_IF) {
    return parse_if(p);
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
  if (is_type_token(p->tok.kind)) {
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
      diag_push(p, p->tok.pos, "expected '}'");
    Node *blk = node_new(p->arena, ND_BLOCK);
    blk->as.block.items = items;
    blk->as.block.len = len;
    return blk;
  }
  Node *expr = parse_expr(p);
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  else
    diag_push(p, p->tok.pos, "expected ';'");
  Node *st = node_new(p->arena, ND_EXPR_STMT);
  st->as.expr_stmt.expr = expr;
  return st;
}

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
