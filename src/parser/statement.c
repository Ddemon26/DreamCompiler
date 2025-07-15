#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Node *parse_block(Lexer *lexer, Token *token) {
  Node *head = NULL;
  Node **current = &head;
  while (token->type != TOKEN_RBRACE && token->type != TOKEN_EOF) {
    Node *stmt = parse_statement(lexer, token);
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon\n");
      exit(1);
    }
    Node *block_node = create_node(NODE_BLOCK, NULL, stmt, NULL, NULL);
    *current = block_node;
    current = &block_node->right;
    free(token->value);
    *token = next_token(lexer);
  }
  return head;
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
      body = parse_block(lexer, token);
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
        else_body = parse_block(lexer, token);
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
      body = parse_block(lexer, token);
      if (token->type != TOKEN_RBRACE) {
        fprintf(stderr, "Expected } after while body\n");
        exit(1);
      }
    } else {
      body = parse_statement(lexer, token);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
    }
    return create_node(NODE_WHILE, NULL, cond, body, NULL);
  }
  fprintf(stderr, "Invalid statement\n");
  exit(1);
}

