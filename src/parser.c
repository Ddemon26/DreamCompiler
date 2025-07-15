#include "dream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *create_node(NodeType type, char *value, Node *left, Node *right,
                  Node *else_branch) {
  Node *node = malloc(sizeof(Node));
  node->type = type;
  node->value = value ? strdup(value) : NULL;
  node->left = left;
  node->right = right;
  node->else_branch = else_branch;
  return node;
}

Node *parse_expression(Lexer *lexer, Token *token) {
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
  Node *left = create_node(left_type, token->value, NULL, NULL, NULL);
  *token = next_token(lexer);
  if ((token->type == TOKEN_PLUS || token->type == TOKEN_MINUS ||
       token->type == TOKEN_STAR || token->type == TOKEN_SLASH ||
       token->type == TOKEN_PERCENT || token->type == TOKEN_LT ||
       token->type == TOKEN_GT || token->type == TOKEN_LE ||
       token->type == TOKEN_GE || token->type == TOKEN_EQEQ ||
       token->type == TOKEN_NEQ) &&
      left_type != NODE_STRING) {
    char *op = token->value;
    *token = next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER && token->type != TOKEN_NUMBER) {
      fprintf(stderr, "Expected identifier or number after %s\n", op);
      exit(1);
    }
    Node *right = create_node(token->type == TOKEN_IDENTIFIER ? NODE_IDENTIFIER
                                                              : NODE_NUMBER,
                              token->value, NULL, NULL, NULL);
    *token = next_token(lexer);
    return create_node(NODE_BINARY_OP, op, left, right, NULL);
  }
  return left;
}

Node *parse_statement(Lexer *lexer, Token *token) {
  if (token->type == TOKEN_INT) {
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
      fprintf(stderr, "Expected identifier after int\n");
      exit(1);
    }
    char *var_name = token->value;
    *token = next_token(lexer);
    Node *init = NULL;
    if (token->type == TOKEN_EQUALS) {
      free(token->value);
      *token = next_token(lexer);
      init = parse_expression(lexer, token);
    }
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon\n");
      exit(1);
    }
    return create_node(NODE_VAR_DECL, var_name, init, NULL, NULL);
  } else if (token->type == TOKEN_IDENTIFIER) {
    char *var_name = token->value;
    *token = next_token(lexer);
    if (token->type == TOKEN_EQUALS) {
      free(token->value);
      *token = next_token(lexer);
      Node *expr = parse_expression(lexer, token);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
      return create_node(NODE_ASSIGN, var_name, expr, NULL, NULL);
    }
  } else if (token->type == TOKEN_CONSOLE) {
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_DOT) {
      fprintf(stderr, "Expected dot after Console\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_WRITELINE) {
      fprintf(stderr, "Expected WriteLine\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_LPAREN) {
      fprintf(stderr, "Expected (\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER && token->type != TOKEN_NUMBER &&
        token->type != TOKEN_STRING) {
      fprintf(stderr, "Expected identifier, number or string\n");
      exit(1);
    }
    NodeType arg_type = NODE_IDENTIFIER;
    if (token->type == TOKEN_NUMBER)
      arg_type = NODE_NUMBER;
    else if (token->type == TOKEN_STRING)
      arg_type = NODE_STRING;
    Node *arg = create_node(arg_type, token->value, NULL, NULL, NULL);
    *token = next_token(lexer);
    if (token->type != TOKEN_RPAREN) {
      fprintf(stderr, "Expected )\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon\n");
      exit(1);
    }
    return create_node(NODE_WRITELINE, NULL, arg, NULL, NULL);
  } else if (token->type == TOKEN_IF) {
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_LPAREN) {
      fprintf(stderr, "Expected ( after if\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *cond = parse_expression(lexer, token);
    if (token->type != TOKEN_RPAREN) {
      fprintf(stderr, "Expected ) after condition\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *body = NULL;
    Node *else_body = NULL;
    if (token->type == TOKEN_LBRACE) {
      free(token->value);
      *token = next_token(lexer);
      body = parse_statement(lexer, token);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
      free(token->value);
      *token = next_token(lexer);
      if (token->type != TOKEN_RBRACE) {
        fprintf(stderr, "Expected } after if body\n");
        exit(1);
      }
      Token end_token = *token;
      int saved_pos = lexer->pos;
      *token = next_token(lexer);
      if (token->type != TOKEN_ELSE) {
        free(token->value);
        *token = end_token;
        lexer->pos = saved_pos;
      } else {
        free(end_token.value);
      }
    } else {
      body = parse_statement(lexer, token);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
      Token end_token = *token;
      int saved_pos = lexer->pos;
      *token = next_token(lexer);
      if (token->type != TOKEN_ELSE) {
        free(token->value);
        *token = end_token;
        lexer->pos = saved_pos;
      } else {
        free(end_token.value);
      }
    }
    if (token->type == TOKEN_ELSE) {
      free(token->value);
      *token = next_token(lexer);
      if (token->type == TOKEN_LBRACE) {
        free(token->value);
        *token = next_token(lexer);
        else_body = parse_statement(lexer, token);
        if (token->type != TOKEN_SEMICOLON) {
          fprintf(stderr, "Expected semicolon\n");
          exit(1);
        }
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_RBRACE) {
          fprintf(stderr, "Expected } after else body\n");
          exit(1);
        }
        free(token->value);
        *token = next_token(lexer);
      } else {
        else_body = parse_statement(lexer, token);
        if (token->type != TOKEN_SEMICOLON) {
          fprintf(stderr, "Expected semicolon\n");
          exit(1);
        }
        free(token->value);
        *token = next_token(lexer);
      }
    }
    return create_node(NODE_IF, NULL, cond, body, else_body);
  } else if (token->type == TOKEN_WHILE) {
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_LPAREN) {
      fprintf(stderr, "Expected ( after while\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *cond = parse_expression(lexer, token);
    if (token->type != TOKEN_RPAREN) {
      fprintf(stderr, "Expected ) after condition\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *body = NULL;
    if (token->type == TOKEN_LBRACE) {
      free(token->value);
      *token = next_token(lexer);
      body = parse_statement(lexer, token);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
      free(token->value);
      *token = next_token(lexer);
      if (token->type != TOKEN_RBRACE) {
        fprintf(stderr, "Expected } after while body\n");
        exit(1);
      }
    } else {
      body = parse_statement(lexer, token);
    }
    return create_node(NODE_WHILE, NULL, cond, body, NULL);
  }
  fprintf(stderr, "Invalid statement\n");
  exit(1);
}

void free_node(Node *n) {
  if (!n)
    return;
  if (n->value)
    free(n->value);
  free_node(n->left);
  free_node(n->right);
  free_node(n->else_branch);
  free(n);
}
