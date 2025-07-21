/**
 * @brief Parses an if statement.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed if statement node.
 */
static Node *parse_if(Parser *p) {
  next(p); // consume 'if'
  
  // Handle missing opening parenthesis
  if (p->tok.kind != TK_LPAREN) {
    diag_push_with_hint(p, p->tok.pos, DIAG_ERROR, "expected '(' after 'if'", 
                       "if statements require a condition in parentheses");
    // Try to continue parsing anyway - maybe they just forgot the paren
    parser_sync_expr(p);
  } else {
    next(p);
  }
  
  Node *cond = parse_expr_prec(p, 0);
  if (!cond) {
    cond = node_new(p->arena, ND_ERROR);
  }
  
  // Handle missing closing parenthesis
  if (p->tok.kind != TK_RPAREN) {
    diag_push_with_hint(p, p->tok.pos, DIAG_ERROR, "expected ')' to close if condition", 
                       "make sure to close the opening '(' with a matching ')'");
    // Continue without consuming the token - it might be the start of the statement
  } else {
    next(p);
  }
  
  Node *then_br = parse_stmt(p);
  if (!then_br) {
    then_br = node_new(p->arena, ND_ERROR);
  }
  
  Node *else_br = NULL;
  if (p->tok.kind == TK_KW_ELSE) {
    next(p);
    else_br = parse_stmt(p);
    if (!else_br) {
      else_br = node_new(p->arena, ND_ERROR);
    }
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
      init->as.var_decl.is_const = 0; // For loop variables are not const by default
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
    Token start_tok = p->tok;
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
      next(p);
      continue;
    }
    if (p->tok.kind == TK_COLON)
      next(p);
    else
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected ':'");
    
    // Parse multiple statements until next case/default/end
    Node **stmts = NULL;
    size_t stmt_len = 0, stmt_cap = 0;
    while (p->tok.kind != TK_KW_CASE && 
           p->tok.kind != TK_KW_DEFAULT && 
           p->tok.kind != TK_RBRACE && 
           p->tok.kind != TK_EOF) {
      Node *stmt = parse_stmt(p);
      if (stmt_len + 1 > stmt_cap) {
        stmt_cap = stmt_cap ? stmt_cap * 2 : 4;
        stmts = realloc(stmts, stmt_cap * sizeof(Node*));
      }
      stmts[stmt_len++] = stmt;
    }
    
    // Create a block node containing all the statements
    Node *body = node_new(p->arena, ND_BLOCK);
    body->as.block.items = stmts;
    body->as.block.len = stmt_len;
    
    // Safeguard: ensure we advanced past the starting token
    if (p->tok.kind == start_tok.kind && p->tok.start == start_tok.start) {
      next(p);
    }
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
  Slice catch_param = {NULL, 0};
  Slice catch_type = {NULL, 0};
  
  if (p->tok.kind == TK_KW_CATCH) {
    next(p); // consume 'catch'
    
    // Parse optional catch parameter: catch (Exception e)
    if (p->tok.kind == TK_LPAREN) {
      next(p); // consume '('
      
      // Parse exception type
      if (p->tok.kind == TK_IDENT) {
        catch_type.start = p->tok.start;
        catch_type.len = p->tok.len;
        next(p);
        
        // Parse parameter name
        if (p->tok.kind == TK_IDENT) {
          catch_param.start = p->tok.start;
          catch_param.len = p->tok.len;
          next(p);
        }
      }
      
      if (p->tok.kind == TK_RPAREN) {
        next(p); // consume ')'
      } else {
        diag_push(p, p->tok.pos, DIAG_ERROR, "expected ')'");
      }
    }
    
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
  n->as.try_stmt.catch_param = catch_param;
  n->as.try_stmt.catch_type = catch_type;
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
      
      // Check for pointer syntax in parameters: Type*
      int param_is_pointer = 0;
      if (p->tok.kind == TK_STAR) {
        param_is_pointer = 1;
        next(p); // consume '*'
      }
      
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
      vd->as.var_decl.is_pointer = param_is_pointer;
      vd->as.var_decl.is_const = 0; // Function parameters are not const by default
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
  fn->as.func.is_async = 0;
  fn->as.func.is_public = 0; // Default to private
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
  
  // Check for optional inheritance (only for classes, not structs)
  Slice base_name = {NULL, 0};
  if (kind == ND_CLASS_DECL && p->tok.kind == TK_COLON) {
    next(p); // consume ':'
    if (p->tok.kind != TK_IDENT) {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected base class identifier");
      return node_new(p->arena, ND_ERROR);
    }
    base_name = (Slice){p->tok.start, p->tok.len};
    next(p);
  }
  
  if (p->tok.kind != TK_LBRACE) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '{'");
    return node_new(p->arena, ND_ERROR);
  }
  next(p);
  Node **members = NULL;
  size_t len = 0, cap = 0;
  while (p->tok.kind != TK_RBRACE && p->tok.kind != TK_EOF) {
    Token start_tok = p->tok;
    if (p->tok.kind == TK_SEMICOLON) {
      next(p);
      continue;
    }
    int is_public = 0; // Default to private
    if (p->tok.kind == TK_KW_PUBLIC) {
      is_public = 1;
      next(p);
    } else if (p->tok.kind == TK_KW_PRIVATE) {
      is_public = 0; // Explicitly private (same as default)
      next(p);
    }
    int is_static = 0;
    if (p->tok.kind == TK_KW_STATIC) {
      is_static = 1;
      next(p);
    }
    Node *m = NULL;
    if (p->tok.kind == TK_KW_FUNC) {
      m = parse_func(p);
      if (m->kind == ND_FUNC) {
        m->as.func.is_static = is_static;
        m->as.func.is_public = is_public;
      }
    } else if (is_type_token(p->tok.kind) || (p->tok.kind == TK_IDENT && typevec_contains(p, p->tok))) {
      m = parse_var_decl(p);
      if (m->kind == ND_VAR_DECL) {
        m->as.var_decl.is_static = is_static;
        m->as.var_decl.is_public = is_public;
      }
    } else {
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected member declaration");
      next(p);
      continue;
    }
    // Safeguard: ensure we advanced past the starting token
    if (p->tok.kind == start_tok.kind && p->tok.start == start_tok.start) {
      next(p);
    }
    nodevec_push(&members, &len, &cap, m);
  }
  if (p->tok.kind == TK_RBRACE)
    next(p);
  else
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected '}'");
  // Consume optional semicolon after struct/class declaration
  if (p->tok.kind == TK_SEMICOLON)
    next(p);
  Node *n = node_new(p->arena, kind);
  n->pos = start_pos;
  n->as.type_decl.name = name;
  n->as.type_decl.base_name = base_name;
  n->as.type_decl.members = members;
  n->as.type_decl.len = len;
  typevec_push(p, name);
  return n;
}

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
 * @brief Parses a module directive: module identifier ;
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed module node.
 */
static Node *parse_module(Parser *p) {
  next(p); // consume 'module'
  
  if (p->tok.kind != TK_IDENT) {
    diag_push_unexpected_token(p, "module name");
    return node_new(p->arena, ND_ERROR);
  }
  
  Node *n = node_new(p->arena, ND_MODULE);
  n->as.module.name = (Slice){p->tok.start, p->tok.len};
  next(p); // consume identifier
  
  if (p->tok.kind != TK_SEMICOLON) {
    diag_push_unexpected_token(p, "';'");
    return node_new(p->arena, ND_ERROR);
  }
  next(p); // consume ';'
  
  return n;
}

/**
 * @brief Parses an import directive: import module_path ;
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed import node.
 */
static Node *parse_import(Parser *p) {
  next(p); // consume 'import'
  
  if (p->tok.kind != TK_IDENT) {
    diag_push_unexpected_token(p, "module path");
    return node_new(p->arena, ND_ERROR);
  }
  
  Node *n = node_new(p->arena, ND_IMPORT);
  n->as.import.path = (Slice){p->tok.start, p->tok.len};
  next(p); // consume identifier
  
  if (p->tok.kind != TK_SEMICOLON) {
    diag_push_unexpected_token(p, "';'");
    return node_new(p->arena, ND_ERROR);
  }
  next(p); // consume ';'
  
  return n;
}

/**
 * @brief Parses an export declaration: export declaration
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed export node.
 */
static Node *parse_export(Parser *p) {
  next(p); // consume 'export'
  
  // Parse the declaration being exported
  Node *decl = parse_stmt(p);
  if (!decl) {
    diag_push(p, p->tok.pos, DIAG_ERROR, "expected declaration after 'export'");
    return node_new(p->arena, ND_ERROR);
  }
  
  Node *n = node_new(p->arena, ND_EXPORT);
  n->as.export.decl = decl;
  
  return n;
}

/**
 * @brief Parses a statement based on the current token kind.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed statement node.
 */
static Node *parse_stmt(Parser *p) {
  Pos start_pos = p->tok.pos;
  Node *n = NULL;
  if (p->tok.kind == TK_KW_IF) {
    n = parse_if(p);
  } else if (p->tok.kind == TK_KW_DO) {
    n = parse_do_while(p);
  } else if (p->tok.kind == TK_KW_FOR) {
    n = parse_for(p);
  } else if (p->tok.kind == TK_KW_SWITCH) {
    n = parse_switch(p);
  } else if (p->tok.kind == TK_KW_WHILE) {
    n = parse_while(p);
  } else if (p->tok.kind == TK_KW_BREAK) {
    n = parse_break(p);
  } else if (p->tok.kind == TK_KW_CONTINUE) {
    n = parse_continue(p);
  } else if (p->tok.kind == TK_KW_RETURN) {
    n = parse_return(p);
  } else if (p->tok.kind == TK_KW_TRY) {
    n = parse_try(p);
  } else if (p->tok.kind == TK_KW_THROW) {
    n = parse_throw(p);
  } else if (p->tok.kind == TK_KW_MODULE) {
    n = parse_module(p);
  } else if (p->tok.kind == TK_KW_IMPORT) {
    n = parse_import(p);
  } else if (p->tok.kind == TK_KW_EXPORT) {
    n = parse_export(p);
  } else if (p->tok.kind == TK_KW_CLASS) {
    n = parse_type_decl(p, ND_CLASS_DECL);
  } else if (p->tok.kind == TK_KW_STRUCT) {
    n = parse_type_decl(p, ND_STRUCT_DECL);
  } else if (p->tok.kind == TK_KW_FUNC) {
    n = parse_func(p);
  } else if (p->tok.kind == TK_KW_ASYNC) {
    // Look ahead to see if this is an async function
    Token la = lexer_peek(&p->lx);
    if (la.kind == TK_KW_FUNC) {
      next(p); // consume 'async'
      n = parse_func(p);
      n->as.func.is_async = 1;
    } else {
      // This should be an error - async without func
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected 'func' after 'async'");
      next(p);
      n = node_new(p->arena, ND_ERROR);
    }
  } else if (p->tok.kind == TK_KW_CONST) {
    n = parse_var_decl(p);
  } else if (is_type_token(p->tok.kind)) {
    n = parse_var_decl(p);
  } else if (p->tok.kind == TK_IDENT && typevec_contains(p, p->tok)) {
    Token la = lexer_peek(&p->lx);
    if (la.kind == TK_IDENT)
      n = parse_var_decl(p);
  }
  if (!n && p->tok.kind == TK_LBRACE) {
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
    n = blk;
  }
  if (!n) {
    Node *expr = parse_expr(p);
    if (p->tok.kind == TK_SEMICOLON)
      next(p);
    else
      diag_push(p, p->tok.pos, DIAG_ERROR, "expected ';'");
    Node *st = node_new(p->arena, ND_EXPR_STMT);
    st->as.expr_stmt.expr = expr;
    n = st;
  }
  n->pos = start_pos;
  return n;
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
