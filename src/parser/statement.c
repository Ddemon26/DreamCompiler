#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *parse_block(Lexer *lexer, Token *token) {
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

Node *parse_for_increment(Lexer *lexer, Token *token) {
  if (token->type == TOKEN_PLUSPLUS || token->type == TOKEN_MINUSMINUS) {
    int inc = token->type == TOKEN_PLUSPLUS;
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
      fprintf(stderr, "Expected identifier after %s\n", inc ? "++" : "--");
      exit(1);
    }
    char *name = token->value;
    *token = next_token(lexer);
    return create_node(inc ? NODE_PRE_INC : NODE_PRE_DEC, name, NULL, NULL, NULL);
  }
  if (token->type != TOKEN_IDENTIFIER) {
    fprintf(stderr, "Invalid for increment\n");
    exit(1);
  }
  char *var_name = token->value;
  *token = next_token(lexer);
  if (token->type == TOKEN_PLUSPLUS || token->type == TOKEN_MINUSMINUS) {
    int inc = token->type == TOKEN_PLUSPLUS;
    free(token->value);
    *token = next_token(lexer);
    return create_node(inc ? NODE_POST_INC : NODE_POST_DEC, var_name, NULL, NULL, NULL);
  }
  if (token->type != TOKEN_EQUALS && token->type != TOKEN_PLUSEQ &&
      token->type != TOKEN_MINUSEQ && token->type != TOKEN_STAREQ &&
      token->type != TOKEN_SLASHEQ && token->type != TOKEN_MODEQ) {
    fprintf(stderr, "Expected = or compound operator in for increment\n");
    exit(1);
  }
  TokenType assign_type = token->type;
  free(token->value);
  *token = next_token(lexer);
  Node *expr = parse_expression(lexer, token);
  if (assign_type == TOKEN_EQUALS)
    return create_node(NODE_ASSIGN, var_name, expr, NULL, NULL);
  const char *op = assign_type == TOKEN_PLUSEQ   ? "+" :
                   assign_type == TOKEN_MINUSEQ  ? "-" :
                   assign_type == TOKEN_STAREQ   ? "*" :
                   assign_type == TOKEN_SLASHEQ  ? "/" : "%";
  Node *left_expr = create_node(NODE_IDENTIFIER, var_name, NULL, NULL, NULL);
  Node *binary = create_node(NODE_BINARY_OP, (char *)op, left_expr, expr, NULL);
  return create_node(NODE_ASSIGN, var_name, binary, NULL, NULL);
}

Node *parse_statement(Lexer *lexer, Token *token) {
  if (token->type == TOKEN_PLUSPLUS || token->type == TOKEN_MINUSMINUS) {
    int inc = token->type == TOKEN_PLUSPLUS;
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
      fprintf(stderr, "Expected identifier after %s\n", inc ? "++" : "--");
      exit(1);
    }
    char *name = token->value;
    *token = next_token(lexer);
    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon\n");
      exit(1);
    }
    return create_node(inc ? NODE_PRE_INC : NODE_PRE_DEC, name, NULL, NULL, NULL);
  }

  Node *decl = parse_declaration(lexer, token);
  if (decl)
    return decl;

  if (token->type == TOKEN_IDENTIFIER) {
    /* Check for object declaration: <ClassName> <var>; */
    char *type_name = token->value;
    Token save_token = *token;
    int saved_pos = lexer->pos;
    *token = next_token(lexer);
    if (token->type == TOKEN_IDENTIFIER) {
      char *obj_name = token->value;
      Token lookahead = next_token(lexer);
      if (lookahead.type == TOKEN_SEMICOLON) {
        /* leave semicolon for caller */
        *token = lookahead;
        Node *type_node = create_node(NODE_IDENTIFIER, type_name, NULL, NULL, NULL);
        Node *decl = create_node(NODE_OBJ_DECL, obj_name, NULL, type_node, NULL);
        return decl;
      }
      free(lookahead.value);
    }
    lexer->pos = saved_pos;
    free(token->value);
    *token = save_token;
    char *var_name = token->value;
    *token = next_token(lexer);
    Node *target = create_node(NODE_IDENTIFIER, var_name, NULL, NULL, NULL);
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
        target = create_node(NODE_MEMBER, fname, target, NULL, NULL);
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
        target = create_node(NODE_INDEX, NULL, target, index, NULL);
      }
    }
    if ((token->type == TOKEN_PLUSPLUS || token->type == TOKEN_MINUSMINUS) &&
        target->type == NODE_IDENTIFIER) {
      int inc = token->type == TOKEN_PLUSPLUS;
      free(token->value);
      *token = next_token(lexer);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
      return create_node(inc ? NODE_POST_INC : NODE_POST_DEC, var_name, NULL, NULL, NULL);
    }
    if (token->type == TOKEN_EQUALS || token->type == TOKEN_PLUSEQ ||
        token->type == TOKEN_MINUSEQ || token->type == TOKEN_STAREQ ||
        token->type == TOKEN_SLASHEQ || token->type == TOKEN_MODEQ) {
      TokenType assign_type = token->type;
      free(token->value);
      *token = next_token(lexer);
      Node *expr = parse_expression(lexer, token);
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
      if (assign_type == TOKEN_EQUALS) {
        if (target->type == NODE_IDENTIFIER)
          return create_node(NODE_ASSIGN, var_name, expr, NULL, NULL);
        return create_node(NODE_MEMBER_ASSIGN, NULL, target, expr, NULL);
      }
      const char *op = assign_type == TOKEN_PLUSEQ   ? "+" :
                       assign_type == TOKEN_MINUSEQ  ? "-" :
                       assign_type == TOKEN_STAREQ   ? "*" :
                       assign_type == TOKEN_SLASHEQ  ? "/" : "%";
      Node *left_expr = NULL;
      if (target->type == NODE_IDENTIFIER)
        left_expr = create_node(NODE_IDENTIFIER, var_name, NULL, NULL, NULL);
      else
        left_expr = target;
      Node *binary = create_node(NODE_BINARY_OP, (char *)op, left_expr, expr,
                                 NULL);
      if (target->type == NODE_IDENTIFIER)
        return create_node(NODE_ASSIGN, var_name, binary, NULL, NULL);
      return create_node(NODE_MEMBER_ASSIGN, NULL, left_expr, binary, NULL);
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
    if (token->type != TOKEN_WRITELINE && token->type != TOKEN_WRITE) {
      fprintf(stderr, "Expected WriteLine or Write\n");
      exit(1);
    }
    int no_newline = token->type == TOKEN_WRITE;
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
    return create_node(no_newline ? NODE_WRITE : NODE_WRITELINE, NULL, arg, NULL, NULL);
  }

  Node *ctrl = parse_control_flow(lexer, token);
  if (ctrl)
    return ctrl;

  fprintf(stderr, "Invalid statement\n");
  exit(1);
}
