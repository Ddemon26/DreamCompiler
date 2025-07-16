#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_INT_LITERAL,
    TOKEN_STRING_LITERAL,
    // keywords
    TOKEN_IF,
    TOKEN_WHILE,
    TOKEN_FUNC,
    TOKEN_VAR,
    // operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_BANG_EQUAL,
    // punctuation
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_UNKNOWN
} TokenKind;

typedef struct {
    TokenKind kind;
    size_t start;
    size_t length;
} Token;

typedef struct {
    const char *src;
    size_t len;
    size_t pos;
} Lexer;

void init_lexer(Lexer *lx, const char *src, size_t len);
Token next_token(Lexer *lx);

#endif // TOKEN_H
