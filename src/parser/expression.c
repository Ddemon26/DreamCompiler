#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Precedence climbing expression parser. Each binary operator is mapped
   to a precedence and associativity for single-pass parsing. */

typedef struct {
  int prec;
  char assoc; /* 'L' or 'R' */
} OpInfo;

static int is_binary_op(TokenType type) {
  switch (type) {
  case TOKEN_PLUS:
  case TOKEN_MINUS:
  case TOKEN_STAR:
  case TOKEN_SLASH:
  case TOKEN_PERCENT:
  case TOKEN_CARET:
  case TOKEN_SHL:
  case TOKEN_SHR:
  case TOKEN_LT:
  case TOKEN_GT:
  case TOKEN_LE:
  case TOKEN_GE:
  case TOKEN_EQEQ:
  case TOKEN_NEQ:
  case TOKEN_AND:
  case TOKEN_OR:
  case TOKEN_BITAND:
  case TOKEN_BITOR:
    return 1;
  default:
    return 0;
  }
}

static OpInfo get_op_info(TokenType type) {
  switch (type) {
  case TOKEN_OR:
    return (OpInfo){1, 'L'};
  case TOKEN_AND:
    return (OpInfo){2, 'L'};
  case TOKEN_BITOR:
    return (OpInfo){3, 'L'};
  case TOKEN_CARET: /* bitwise XOR */
    return (OpInfo){4, 'L'};
  case TOKEN_BITAND:
    return (OpInfo){5, 'L'};
  case TOKEN_EQEQ:
  case TOKEN_NEQ:
    return (OpInfo){6, 'L'};
  case TOKEN_LT:
  case TOKEN_GT:
  case TOKEN_LE:
  case TOKEN_GE:
    return (OpInfo){7, 'L'};
  case TOKEN_SHL:
  case TOKEN_SHR:
    return (OpInfo){8, 'L'};
  case TOKEN_PLUS:
  case TOKEN_MINUS:
    return (OpInfo){9, 'L'};
  case TOKEN_STAR:
  case TOKEN_SLASH:
  case TOKEN_PERCENT:
    return (OpInfo){10, 'L'};
  default:
    return (OpInfo){0, 'L'};
  }
}

static Node *compute_expr(Lexer *lexer, Token *token, int min_prec);
static Node *compute_atom(Lexer *lexer, Token *token);

/* Parse literals, identifiers, function calls and unary operators. */
static Node *compute_atom(Lexer *lexer, Token *token) {
  int unary_minus = 0;
  int unary_not = 0;
  int unary_bitnot = 0;
  int prefix_inc = 0;
  int prefix_dec = 0;
  if (token->type == TOKEN_MINUS || token->type == TOKEN_NOT ||
      token->type == TOKEN_BITNOT ||
      token->type == TOKEN_PLUSPLUS || token->type == TOKEN_MINUSMINUS) {
    if (token->type == TOKEN_MINUS)
      unary_minus = 1;
    else if (token->type == TOKEN_NOT)
      unary_not = 1;
    else if (token->type == TOKEN_BITNOT)
      unary_bitnot = 1;
    else if (token->type == TOKEN_PLUSPLUS)
      prefix_inc = 1;
    else
      prefix_dec = 1;
    free(token->value);
    *token = next_token(lexer);
  }

  Node *left = NULL;
  if (prefix_inc || prefix_dec) {
    if (token->type != TOKEN_IDENTIFIER) {
      fprintf(stderr, "Expected identifier after %s\n",
              prefix_inc ? "++" : "--");
      exit(1);
    }
    char *name = token->value;
    *token = next_token(lexer);
    left = create_node(prefix_inc ? NODE_PRE_INC : NODE_PRE_DEC, name, NULL, NULL,
                       NULL);
    free(name);
  } else if (token->type == TOKEN_LPAREN) {
    free(token->value);
    *token = next_token(lexer);
    left = compute_expr(lexer, token, 0);
    if (token->type != TOKEN_RPAREN) {
      fprintf(stderr, "Expected )\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
  } else {
    if (token->type != TOKEN_IDENTIFIER && token->type != TOKEN_NUMBER &&
        token->type != TOKEN_STRING && token->type != TOKEN_CHAR) {
      fprintf(stderr, "Expected identifier, number, string or char\n");
      exit(1);
    }
    NodeType left_type = NODE_IDENTIFIER;
    if (token->type == TOKEN_NUMBER)
      left_type = NODE_NUMBER;
    else if (token->type == TOKEN_STRING)
      left_type = NODE_STRING;
    else if (token->type == TOKEN_CHAR)
      left_type = NODE_CHAR;
    char *name = token->value;
    *token = next_token(lexer);
    if (left_type == NODE_IDENTIFIER && token->type == TOKEN_LPAREN) {
      free(token->value);
      *token = next_token(lexer);
      Node *args = NULL;
      Node **cur = &args;
      if (token->type != TOKEN_RPAREN) {
        while (1) {
          Node *arg_expr = compute_expr(lexer, token, 0);
          Node *blk = create_node(NODE_BLOCK, NULL, arg_expr, NULL, NULL);
          *cur = blk;
          cur = &blk->right;
          if (token->type == TOKEN_COMMA) {
            free(token->value);
            *token = next_token(lexer);
            continue;
          }
          break;
        }
        if (token->type != TOKEN_RPAREN) {
          fprintf(stderr, "Expected ) after call\n");
          exit(1);
        }
      }
      free(token->value);
      *token = next_token(lexer);
      left = create_node(NODE_FUNC_CALL, name, args, NULL, NULL);
      free(name);
    } else {
      left = create_node(left_type, name, NULL, NULL, NULL);
      free(name);
    }
  }

  while (token->type == TOKEN_DOT || token->type == TOKEN_LBRACKET) {
    if (token->type == TOKEN_DOT) {
      free(token->value);
      *token = next_token(lexer);
      if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected field name after .\n");
        exit(1);
      }
      char *fname = token->value;
      *token = next_token(lexer);
      left = create_node(NODE_MEMBER, fname, left, NULL, NULL);
    } else {
      free(token->value);
      *token = next_token(lexer);
      Node *index = parse_expression(lexer, token);
      if (token->type != TOKEN_RBRACKET) {
        fprintf(stderr, "Expected ] after index\n");
        exit(1);
      }
      free(token->value);
      *token = next_token(lexer);
      left = create_node(NODE_INDEX, NULL, left, index, NULL);
    }
  }

  if (left->type == NODE_IDENTIFIER &&
      (token->type == TOKEN_PLUSPLUS || token->type == TOKEN_MINUSMINUS)) {
    int inc = token->type == TOKEN_PLUSPLUS;
    free(token->value);
    *token = next_token(lexer);
    Node *tmp = left;
    left = create_node(inc ? NODE_POST_INC : NODE_POST_DEC, tmp->value, NULL,
                       NULL, NULL);
    free_node(tmp);
  }
  if (unary_minus)
    left = create_node(NODE_UNARY_OP, "-", left, NULL, NULL);
  if (unary_not)
    left = create_node(NODE_UNARY_OP, "!", left, NULL, NULL);
  if (unary_bitnot)
    left = create_node(NODE_UNARY_OP, "~", left, NULL, NULL);
  return left;
}

/* Parse binary operator chains with precedence climbing. */
static Node *compute_expr(Lexer *lexer, Token *token, int min_prec) {
  Node *lhs = compute_atom(lexer, token);
  while (is_binary_op(token->type)) {
    OpInfo info = get_op_info(token->type);
    if (info.prec < min_prec)
      break;
    if (lhs->type == NODE_STRING && token->type != TOKEN_PLUS)
      break;
    int next_min = info.assoc == 'L' ? info.prec + 1 : info.prec;
    char *op = token->value;
    *token = next_token(lexer);
    Node *rhs = compute_expr(lexer, token, next_min);
    lhs = create_node(NODE_BINARY_OP, op, lhs, rhs, NULL);
    free(op);
  }
  return lhs;
}

Node *parse_expression(Lexer *lexer, Token *token) {
  Node *expr = compute_expr(lexer, token, 0);
  if (token->type == TOKEN_QUESTION) {
    free(token->value);
    *token = next_token(lexer);
    Node *true_expr = parse_expression(lexer, token);
    if (token->type != TOKEN_COLON) {
      fprintf(stderr, "Expected : in ternary expression\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *false_expr = parse_expression(lexer, token);
    expr = create_node(NODE_TERNARY, NULL, expr, true_expr, false_expr);
  }
  return expr;
}
