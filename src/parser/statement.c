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
  if (token->type == TOKEN_INT || token->type == TOKEN_STRING_TYPE ||
      token->type == TOKEN_BOOL_TYPE || token->type == TOKEN_FLOAT_TYPE) {
    TokenType decl_type = token->type;
    int is_string = decl_type == TOKEN_STRING_TYPE;
    int is_bool = decl_type == TOKEN_BOOL_TYPE;
    int is_float = decl_type == TOKEN_FLOAT_TYPE;
    free(token->value);
    *token = next_token(lexer);

    if (token->type != TOKEN_IDENTIFIER) {
      if (is_string)
        fprintf(stderr, "Expected identifier after string\n");
      else if (decl_type == TOKEN_BOOL_TYPE)
        fprintf(stderr, "Expected identifier after bool\n");
      else if (decl_type == TOKEN_FLOAT_TYPE)
        fprintf(stderr, "Expected identifier after float\n");
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

    NodeType ntype = is_string
                         ? NODE_STR_DECL
                         : is_bool  ? NODE_BOOL_DECL
                         : is_float ? NODE_FLOAT_DECL
                                     : NODE_VAR_DECL;
    Node *first_decl = create_node(ntype, var_name, init, NULL, NULL);

    Node *head = NULL;
    Node **cur = NULL;

    if (token->type == TOKEN_COMMA) {
      head = create_node(NODE_BLOCK, NULL, first_decl, NULL, NULL);
      cur = &head->right;
      while (token->type == TOKEN_COMMA) {
        free(token->value);
        *token = next_token(lexer);
        if (token->type != TOKEN_IDENTIFIER) {
          fprintf(stderr, "Expected identifier after ,\n");
          exit(1);
        }
        char *name2 = token->value;
        *token = next_token(lexer);
        Node *init2 = NULL;
        if (token->type == TOKEN_EQUALS) {
          free(token->value);
          *token = next_token(lexer);
          init2 = parse_expression(lexer, token);
        }
        Node *decl2 = create_node(ntype, name2, init2, NULL, NULL);
        Node *blk = create_node(NODE_BLOCK, NULL, decl2, NULL, NULL);
        *cur = blk;
        cur = &blk->right;
      }
    }

    if (token->type != TOKEN_SEMICOLON) {
      fprintf(stderr, "Expected semicolon\n");
      exit(1);
    }

    if (head)
      return head;
    return first_decl;
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
    Node *params = NULL;
    Node **cur_param = &params;
    if (token->type != TOKEN_RPAREN) {
      while (1) {
        int is_string = 0;
        int is_bool_param = 0;
        int is_float_param = 0;
        if (token->type == TOKEN_INT || token->type == TOKEN_STRING_TYPE ||
            token->type == TOKEN_BOOL_TYPE || token->type == TOKEN_FLOAT_TYPE) {
          is_string = token->type == TOKEN_STRING_TYPE;
          is_bool_param = token->type == TOKEN_BOOL_TYPE;
          is_float_param = token->type == TOKEN_FLOAT_TYPE;
          free(token->value);
          *token = next_token(lexer);
        } else {
          fprintf(stderr, "Expected parameter type\n");
          exit(1);
        }
        if (token->type != TOKEN_IDENTIFIER) {
          fprintf(stderr, "Expected parameter name\n");
          exit(1);
        }
        char *pname = token->value;
        *token = next_token(lexer);
        NodeType decl_type_node = NODE_VAR_DECL;
        if (is_string)
          decl_type_node = NODE_STR_DECL;
        else if (is_bool_param)
          decl_type_node = NODE_BOOL_DECL;
        else if (is_float_param)
          decl_type_node = NODE_FLOAT_DECL;
        Node *decl = create_node(decl_type_node,
                                 pname, NULL, NULL, NULL);
        free(pname);
        Node *blk = create_node(NODE_BLOCK, NULL, decl, NULL, NULL);
        *cur_param = blk;
        cur_param = &blk->right;
        if (token->type == TOKEN_COMMA) {
          free(token->value);
          *token = next_token(lexer);
          continue;
        }
        break;
      }
      if (token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Expected ) after function params\n");
        exit(1);
      }
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
      return create_node(NODE_FUNC_DEF, func_name, body, params, NULL);
  } else if (token->type == TOKEN_CLASS || token->type == TOKEN_STRUCT) {
    int is_struct = token->type == TOKEN_STRUCT;
    free(token->value);
    *token = next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
      fprintf(stderr, "Expected identifier after %s\n",
              is_struct ? "struct" : "class");
      exit(1);
    }
    char *class_name = token->value;
    *token = next_token(lexer);
    if (token->type != TOKEN_LBRACE) {
      fprintf(stderr, "Expected { after %s name\n",
              is_struct ? "struct" : "class");
      exit(1);
    }
    free(token->value);
    *token = next_token(lexer);
    Node *fields = NULL;
    Node **cur_field = &fields;
    while (token->type != TOKEN_RBRACE) {
      Node *field = parse_statement(lexer, token);
      if (field->type != NODE_VAR_DECL && field->type != NODE_STR_DECL &&
          field->type != NODE_BOOL_DECL && field->type != NODE_FLOAT_DECL) {
        fprintf(stderr, "Only variable declarations allowed in %s\n",
                is_struct ? "struct" : "class");
        exit(1);
      }
      if (token->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "Expected semicolon\n");
        exit(1);
      }
      Node *blk = create_node(NODE_BLOCK, NULL, field, NULL, NULL);
      *cur_field = blk;
      cur_field = &blk->right;
      free(token->value);
      *token = next_token(lexer);
    }
    return create_node(NODE_CLASS_DEF, class_name, fields, NULL, NULL);
  } else if (token->type == TOKEN_IDENTIFIER) {
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
    while (token->type == TOKEN_DOT) {
      free(token->value);
      *token = next_token(lexer);
      if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected field name after .\n");
        exit(1);
      }
      char *fname = token->value;
      *token = next_token(lexer);
      target = create_node(NODE_MEMBER, fname, target, NULL, NULL);
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
  fprintf(stderr, "Invalid statement\n");
  exit(1);
}

