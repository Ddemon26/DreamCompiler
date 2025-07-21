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
  case TK_KW_TASK:
  case TK_KW_TASKRESULT:
  case TK_KW_VAR:
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
