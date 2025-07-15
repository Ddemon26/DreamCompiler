#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *parse_expression(Lexer *lexer, Token *token) {
  int unary_minus = 0;
  int unary_not = 0;
  if (token->type == TOKEN_MINUS || token->type == TOKEN_NOT) {
    if (token->type == TOKEN_MINUS)
      unary_minus = 1;
    else
      unary_not = 1;
    free(token->value);
    *token = next_token(lexer);
  }
  Node *left = NULL;
  if (token->type == TOKEN_LPAREN) {
    free(token->value);
    *token = next_token(lexer);
    left = parse_expression(lexer, token);
    if (token->type != TOKEN_RPAREN) {
      fprintf(stderr, "Expected )\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
  } else {
    if (token->type != TOKEN_IDENTIFIER && token->type != TOKEN_NUMBER &&
        token->type != TOKEN_STRING) {
      fprintf(stderr, "Expected identifier, number or string\n");
      exit(1);
    }
    NodeType left_type = NODE_IDENTIFIER;
    if (token->type == TOKEN_NUMBER)
      left_type = NODE_NUMBER;
    else if (token->type == TOKEN_STRING)
      left_type = NODE_STRING;
    char *name = token->value;
    *token = next_token(lexer);
    if (left_type == NODE_IDENTIFIER && token->type == TOKEN_LPAREN) {
      free(token->value);
      *token = next_token(lexer);
      Node *args = NULL;
      Node **cur = &args;
      if (token->type != TOKEN_RPAREN) {
        while (1) {
          Node *arg_expr = parse_expression(lexer, token);
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
  if (unary_minus)
    left = create_node(NODE_UNARY_OP, "-", left, NULL, NULL);
  if (unary_not)
    left = create_node(NODE_UNARY_OP, "!", left, NULL, NULL);
  if ((token->type == TOKEN_PLUS || token->type == TOKEN_MINUS ||
       token->type == TOKEN_STAR || token->type == TOKEN_SLASH ||
       token->type == TOKEN_PERCENT || token->type == TOKEN_LT ||
       token->type == TOKEN_GT || token->type == TOKEN_LE ||
       token->type == TOKEN_GE || token->type == TOKEN_EQEQ ||
       token->type == TOKEN_NEQ || token->type == TOKEN_AND ||
       token->type == TOKEN_OR) &&
      left->type != NODE_STRING) {
    char *op = token->value;
    *token = next_token(lexer);
    Node *right = parse_expression(lexer, token);
    return create_node(NODE_BINARY_OP, op, left, right, NULL);
  }
  return left;
}
