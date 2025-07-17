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
#define yycSC_NORMAL SC_NORMAL
#define yycSC_STRING SC_STRING
#define yycSC_COMMENT SC_COMMENT

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

static inline StartCondition lexer_get_state(Lexer *lx) { return lx->state; }
static inline void lexer_set_state(Lexer *lx, StartCondition s) { lx->state = s; }

void lexer_init(Lexer *lx, const char *src);
Token lexer_next(Lexer *lx);
Token lexer_peek(Lexer *lx);
const char *token_kind_name(TokenKind k);

#endif
