#ifndef DREAM_H
#define DREAM_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_STRING,
    TOKEN_PLUS, TOKEN_MINUS,
    TOKEN_SEMICOLON, TOKEN_EQUALS, TOKEN_CONSOLE, TOKEN_DOT,
    TOKEN_WRITELINE, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_IF, TOKEN_WHILE,
    TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_EOF, TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

typedef enum {
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_WRITELINE,
    NODE_BINARY_OP,
    NODE_IF,
    NODE_WHILE,
    NODE_IDENTIFIER,
    NODE_NUMBER,
    NODE_STRING
} NodeType;

typedef struct Node {
    NodeType type;
    char *value;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    char *source;
    int pos;
    int line;
} Lexer;

typedef struct {
    FILE *output;
} Compiler;

Token next_token(Lexer *lexer);
Node *create_node(NodeType type, char *value, Node *left, Node *right);
Node *parse_expression(Lexer *lexer, Token *token);
Node *parse_statement(Lexer *lexer, Token *token);
void gen_c_expr(FILE *out, Node *expr);
void generate_c(Compiler *compiler, Node *node);
void free_node(Node *n);

#endif
