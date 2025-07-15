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

static Node *parse_for_increment(Lexer *lexer, Token *token) {
  if (token->type != TOKEN_IDENTIFIER) {
    fprintf(stderr, "Invalid for increment\n");
    exit(1);
  }
  char *var_name = token->value;
  *token = next_token(lexer);
  if (token->type != TOKEN_EQUALS) {
    fprintf(stderr, "Expected = in for increment\n");
    exit(1);
  }
  free(token->value);
  *token = next_token(lexer);
  Node *expr = parse_expression(lexer, token);
  return create_node(NODE_ASSIGN, var_name, expr, NULL, NULL);
}

Node *parse_statement(Lexer *lexer, Token *token) {
  if (token->type == TOKEN_INT || token->type == TOKEN_STRING_TYPE) {
    int is_string = token->type == TOKEN_STRING_TYPE;
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
      if (is_string)
        fprintf(stderr, "Expected identifier after string\n");
      else
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
    if (is_string)
      return create_node(NODE_STR_DECL, var_name, init, NULL, NULL);
    return create_node(NODE_VAR_DECL, var_name, init, NULL, NULL);
  } else if (token->type == TOKEN_FUNC) {
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
      fprintf(stderr, "Expected identifier after func\n");
      exit(1);
    }
    char *func_name = token->value;
    *token = next_token(lexer);
    if (token->type != TOKEN_LPAREN) {
      fprintf(stderr, "Expected ( after function name\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_RPAREN) {
      fprintf(stderr, "Expected ) after function params\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_LBRACE) {
      fprintf(stderr, "Expected { before function body\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *body = parse_block(lexer, token);
    if (token->type != TOKEN_RBRACE) {
      fprintf(stderr, "Expected } after function body\n");
      exit(1);
    }
    return create_node(NODE_FUNC_DEF, func_name, body, NULL, NULL);
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
    } else if (token->type == TOKEN_LPAREN) {
      free(token->value);
      *token = next_token(lexer);
      if (token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Expected ) after call\n");
        exit(1);
      }
      free(token->value);
      *token = next_token(lexer);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
      return create_node(NODE_FUNC_CALL, var_name, NULL, NULL, NULL);
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
    Node *arg = parse_expression(lexer, token);
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
  } else if (token->type == TOKEN_FOR) {
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_LPAREN) {
      fprintf(stderr, "Expected ( after for\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *init = parse_statement(lexer, token);
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon after for init\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *cond = parse_expression(lexer, token);
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon after for condition\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *post = parse_for_increment(lexer, token);
    if (token->type != TOKEN_RPAREN) {
      fprintf(stderr, "Expected ) after for clauses\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *body = NULL;
    int has_block = 0;
    if (token->type == TOKEN_LBRACE) {
      has_block = 1;
      free(token->value);
      *token = next_token(lexer);
      body = parse_block(lexer, token);
      if (token->type != TOKEN_RBRACE) {
        fprintf(stderr, "Expected } after for body\n");
        exit(1);
      }
    } else {
      body = parse_statement(lexer, token);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
    }
    Node *post_block = create_node(NODE_BLOCK, NULL, post, NULL, NULL);
    Node *body_block = NULL;
    if (has_block) {
      if (body) {
        Node *tail = body;
        while (tail->right)
          tail = tail->right;
        tail->right = post_block;
        body_block = body;
      } else {
        body_block = post_block;
      }
    } else {
      body_block = create_node(NODE_BLOCK, NULL, body, post_block, NULL);
    }
    Node *while_node = create_node(NODE_WHILE, NULL, cond, body_block, NULL);
    Node *head = create_node(NODE_BLOCK, NULL, init, NULL, NULL);
    head->right = create_node(NODE_BLOCK, NULL, while_node, NULL, NULL);
    return head;
  } else if (token->type == TOKEN_DO) {
    free(token->value);
    *token = next_token(lexer);
    Node *body = NULL;
    if (token->type == TOKEN_LBRACE) {
      free(token->value);
      *token = next_token(lexer);
      body = parse_block(lexer, token);
      if (token->type != TOKEN_RBRACE) {
        fprintf(stderr, "Expected } after do body\n");
        exit(1);
      }
      free(token->value);
      *token = next_token(lexer);
    } else {
      body = parse_statement(lexer, token);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
      free(token->value);
      *token = next_token(lexer);
    }
    if (token->type != TOKEN_WHILE) {
      fprintf(stderr, "Expected while after do body\n");
      exit(1);
    }
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
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon after do-while\n");
      exit(1);
    }
    return create_node(NODE_DO_WHILE, NULL, cond, body, NULL);
  } else if (token->type == TOKEN_BREAK) {
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon\n");
      exit(1);
    }
    return create_node(NODE_BREAK, NULL, NULL, NULL, NULL);
  } else if (token->type == TOKEN_CONTINUE) {
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon\n");
      exit(1);
    }
    return create_node(NODE_CONTINUE, NULL, NULL, NULL, NULL);
  } else if (token->type == TOKEN_RETURN) {
    free(token->value);
    *token = next_token(lexer);
    Node *expr = NULL;
    if (token->type != TOKEN_SEMICOLON) {
      expr = parse_expression(lexer, token);
    }
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon\n");
      exit(1);
    }
    return create_node(NODE_RETURN, NULL, expr, NULL, NULL);
  }
  fprintf(stderr, "Invalid statement\n");
  exit(1);
}

