#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>
#include <stdbool.h>

typedef struct { size_t line; size_t column; } Pos;

#define TOKEN(k, r) TK_##k,
typedef enum {
#include "tokens.def"
} TokenKind;
#undef TOKEN

typedef struct {
    TokenKind kind;
    const char *start;
    size_t len;
    Pos pos;
} Token;

typedef enum { SC_NORMAL, SC_STRING, SC_COMMENT } StartCondition;

typedef struct {
    const char *src;
    const char *cursor;
    const char *marker;
    const char *limit;
    const char *ctx;
    StartCondition state;
    Pos pos;
    Token lookahead;
    bool has_peek;
} Lexer;

void lexer_init(Lexer *lx, const char *src);
Token lexer_next(Lexer *lx);
Token lexer_peek(Lexer *lx);
const char *token_kind_name(TokenKind k);

#endif
