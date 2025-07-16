#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *parse_declaration(Lexer *lexer, Token *token) {
  if (token->type == TOKEN_INT || token->type == TOKEN_STRING_TYPE ||
      token->type == TOKEN_BOOL_TYPE || token->type == TOKEN_FLOAT_TYPE ||
      token->type == TOKEN_CHAR_TYPE) {
    TokenType decl_type = token->type;
    int is_string = decl_type == TOKEN_STRING_TYPE;
    int is_bool = decl_type == TOKEN_BOOL_TYPE;
    int is_float = decl_type == TOKEN_FLOAT_TYPE;
    int is_char = decl_type == TOKEN_CHAR_TYPE;
    free(token->value);
    *token = next_token(lexer);

    if (token->type != TOKEN_IDENTIFIER) {
      if (is_string)
        fprintf(stderr, "Expected identifier after string\n");
      else if (decl_type == TOKEN_BOOL_TYPE)
        fprintf(stderr, "Expected identifier after bool\n");
      else if (decl_type == TOKEN_FLOAT_TYPE)
        fprintf(stderr, "Expected identifier after float\n");
      else if (decl_type == TOKEN_CHAR_TYPE)
        fprintf(stderr, "Expected identifier after char\n");
      else
        fprintf(stderr, "Expected identifier after int\n");
      exit(1);
    }

    char *var_name = token->value;
    *token = next_token(lexer);
    int is_array = 0;
    Node *array_size = NULL;
    if ((decl_type == TOKEN_INT || decl_type == TOKEN_FLOAT_TYPE ||
         decl_type == TOKEN_BOOL_TYPE || decl_type == TOKEN_CHAR_TYPE ||
         decl_type == TOKEN_STRING_TYPE) &&
        token->type == TOKEN_LBRACKET) {
      is_array = 1;
      free(token->value);
      *token = next_token(lexer);
      array_size = parse_expression(lexer, token);
      if (token->type != TOKEN_RBRACKET) {
        fprintf(stderr, "Expected ] after array size\n");
        exit(1);
      }
      free(token->value);
      *token = next_token(lexer);
    }
    Node *init = NULL;
    if (is_array && token->type == TOKEN_EQUALS) {
      fprintf(stderr, "Array initialization not supported\n");
      exit(1);
    }
    if (!is_array && token->type == TOKEN_EQUALS) {
      free(token->value);
      *token = next_token(lexer);
      init = parse_expression(lexer, token);
    }

    NodeType ntype;
    if (is_array) {
      if (is_float)
        ntype = NODE_FLOAT_ARRAY_DECL;
      else if (is_bool)
        ntype = NODE_BOOL_ARRAY_DECL;
      else if (is_char)
        ntype = NODE_CHAR_ARRAY_DECL;
      else if (is_string)
        ntype = NODE_STR_ARRAY_DECL;
      else
        ntype = NODE_ARRAY_DECL;
    } else {
      ntype = is_string
                 ? NODE_STR_DECL
                 : is_bool  ? NODE_BOOL_DECL
                 : is_float ? NODE_FLOAT_DECL
                 : is_char  ? NODE_CHAR_DECL
                              : NODE_VAR_DECL;
    }
    Node *first_decl =
        create_node(ntype, var_name, is_array ? array_size : init, NULL, NULL);

    Node *head = NULL;
    Node **cur = NULL;

    if (token->type == TOKEN_COMMA) {
      if (is_array) {
        fprintf(stderr, "Array declarations cannot be grouped\n");
        exit(1);
      }
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
        int is_char_param = 0;
        if (token->type == TOKEN_INT || token->type == TOKEN_STRING_TYPE ||
            token->type == TOKEN_BOOL_TYPE || token->type == TOKEN_FLOAT_TYPE ||
            token->type == TOKEN_CHAR_TYPE) {
          is_string = token->type == TOKEN_STRING_TYPE;
          is_bool_param = token->type == TOKEN_BOOL_TYPE;
          is_float_param = token->type == TOKEN_FLOAT_TYPE;
          is_char_param = token->type == TOKEN_CHAR_TYPE;
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
        else if (is_char_param)
          decl_type_node = NODE_CHAR_DECL;
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
          field->type != NODE_BOOL_DECL && field->type != NODE_FLOAT_DECL &&
          field->type != NODE_CHAR_DECL && field->type != NODE_ARRAY_DECL &&
          field->type != NODE_FLOAT_ARRAY_DECL &&
          field->type != NODE_BOOL_ARRAY_DECL &&
          field->type != NODE_CHAR_ARRAY_DECL &&
          field->type != NODE_STR_ARRAY_DECL) {
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
  }
  return NULL;
}
