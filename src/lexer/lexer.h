#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum {
  TOKEN_INT,
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_PERCENT,
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_LE,
  TOKEN_GE,
  TOKEN_EQEQ,
  TOKEN_NEQ,
  TOKEN_NOT,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_SEMICOLON,
  TOKEN_EQUALS,
  TOKEN_CONSOLE,
  TOKEN_DOT,
  TOKEN_WRITELINE,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_DO,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_EOF,
  TOKEN_UNKNOWN
} TokenType;

typedef struct {
  TokenType type;
  char *value;
} Token;

typedef struct {
  char *source;
  int pos;
  int line;
} Lexer;

Token next_token(Lexer *lexer);

#endif
