#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>
#include <stdbool.h>

/**
 * Represents a position in the source code with line and column numbers.
 */
typedef struct { size_t line; size_t column; } Pos;

#define TOKEN(k, r) TK_##k,
/**
 * Enum for different kinds of tokens.
 */
typedef enum {
#include "tokens.def"
} TokenKind;
#undef TOKEN

/**
 * Represents a token with its kind, starting position, length, and position in the source code.
 */
typedef struct {
    TokenKind kind;
    const char *start;
    size_t len;
    Pos pos;
} Token;

/**
 * Enum for the start conditions of the lexer.
 */
typedef enum { SC_NORMAL, SC_STRING, SC_COMMENT } StartCondition;
#define yycSC_NORMAL SC_NORMAL
#define yycSC_STRING SC_STRING
#define yycSC_COMMENT SC_COMMENT

/**
 * Represents the state of the lexer, including the source, cursor, markers, and current state.
 */
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

/**
 * Gets the current state of the lexer.
 *
 * @param lx The lexer instance.
 * @return The current start condition.
 */
static inline StartCondition lexer_get_state(Lexer *lx) { return lx->state; }

/**
 * Sets the state of the lexer.
 *
 * @param lx The lexer instance.
 * @param s The start condition to set.
 */
static inline void lexer_set_state(Lexer *lx, StartCondition s) { lx->state = s; }

/**
 * Initializes the lexer with the given source code.
 *
 * @param lx The lexer instance.
 * @param src The source code to tokenize.
 */
void lexer_init(Lexer *lx, const char *src);

/**
 * Retrieves the next token from the lexer.
 *
 * @param lx The lexer instance.
 * @return The next token.
 */
Token lexer_next(Lexer *lx);

/**
 * Peeks at the next token without consuming it.
 *
 * @param lx The lexer instance.
 * @return The next token.
 */
Token lexer_peek(Lexer *lx);

/**
 * Gets the name of a token kind.
 *
 * @param k The token kind.
 * @return The name of the token kind.
 */
const char *token_kind_name(TokenKind k);

#endif
