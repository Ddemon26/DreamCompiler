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
  case TK_KW_BASE: {
    next(p); /* consume 'base' */
    if (p->tok.kind != TK_DOT) {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected '.' after 'base'");
      return node_new(p->arena, ND_ERROR);
    }
    next(p); /* consume '.' */
    if (p->tok.kind != TK_IDENT) {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected identifier after 'base.'");
      return node_new(p->arena, ND_ERROR);
    }
    n = node_new(p->arena, ND_BASE);
    n->as.base.name = (Slice){p->tok.start, p->tok.len};
    next(p);
    return n;
  }
  case TK_KW_TRUE:
  case TK_KW_FALSE:
    n = node_new(p->arena, ND_BOOL);
    n->as.lit.start = t.start;
    n->as.lit.len = t.len;
    next(p);
    return n;
  case TK_KW_NULL:
    n = node_new(p->arena, ND_NULL);
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
  case TK_KW_AWAIT: {
    next(p); /* consume 'await' */
    Node *expr = parse_expr_prec(p, 0);
    n = node_new(p->arena, ND_AWAIT);
    n->as.await_expr.expr = expr;
    return n;
  }
  default:
    diag_push_unexpected_token(p, "expression");
    // Try to recover by skipping to a safe expression boundary
    parser_sync_expr(p);
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
  // Handle const modifier
  int is_const = 0;
  if (p->tok.kind == TK_KW_CONST) {
    is_const = 1;
    next(p); // consume 'const'
  }
  
  Token type_tok = p->tok;
  next(p); // consume type
  
  // Check for pointer syntax: Type*
  int is_pointer = 0;
  if (p->tok.kind == TK_STAR) {
    is_pointer = 1;
    next(p); // consume '*'
  }
  
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
    n->as.var_decl.is_pointer = is_pointer;
    n->as.var_decl.is_public = 0; // Default to private
    n->as.var_decl.is_const = is_const;
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
    // Check for const without initializer
    if (is_const && !n->as.var_decl.init) {
      diag_push(p, p->tok.pos, DIAG_ERROR,
                "const declaration requires initializer");
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
