#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef enum {
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    const char* value;
} Token;

Token next_token(const char* src, size_t* pos);

#endif // LEXER_H
