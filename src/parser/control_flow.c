#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *parse_control_flow(Lexer *lexer, Token *token) {
  if (token->type == TOKEN_IF) {
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
        if (token->type == TOKEN_SEMICOLON) {
          free(token->value);
          *token = next_token(lexer);
        }
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
  } else if (token->type == TOKEN_SWITCH) {
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_LPAREN) {
      fprintf(stderr, "Expected ( after switch\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *expr = parse_expression(lexer, token);
    if (token->type != TOKEN_RPAREN) {
      fprintf(stderr, "Expected ) after switch expression\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_LBRACE) {
      fprintf(stderr, "Expected { after switch\n");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *cases = NULL;
    Node **cur_case = &cases;
    Node *default_body = NULL;
    while (token->type != TOKEN_RBRACE) {
      if (token->type == TOKEN_CASE) {
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_NUMBER) {
          fprintf(stderr, "Expected number after case\n");
          exit(1);
        }
        char *val = token->value;
        *token = next_token(lexer);
        if (token->type != TOKEN_COLON) {
          fprintf(stderr, "Expected : after case value\n");
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
            fprintf(stderr, "Expected } after case block\n");
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
        Node *case_node = create_node(NODE_CASE, val, body, NULL, NULL);
        *cur_case = case_node;
        cur_case = &case_node->right;
      } else if (token->type == TOKEN_DEFAULT) {
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_COLON) {
          fprintf(stderr, "Expected : after default\n");
          exit(1);
        }
        free(token->value);
        *token = next_token(lexer);
        if (token->type == TOKEN_LBRACE) {
          free(token->value);
          *token = next_token(lexer);
          default_body = parse_block(lexer, token);
          if (token->type != TOKEN_RBRACE) {
            fprintf(stderr, "Expected } after default block\n");
            exit(1);
          }
          free(token->value);
          *token = next_token(lexer);
        } else {
          default_body = parse_statement(lexer, token);
          if (token->type != TOKEN_SEMICOLON) {
            fprintf(stderr, "Expected semicolon\n");
            exit(1);
          }
          free(token->value);
          *token = next_token(lexer);
        }
      } else {
        fprintf(stderr, "Expected case or default in switch\n");
        exit(1);
      }
    }
    free(token->value);
    *token = next_token(lexer);
    return create_node(NODE_SWITCH, NULL, expr, cases, default_body);
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
  return NULL;
}
