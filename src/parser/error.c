#include "parser.h"

/**
 * @brief Advances the parser to the next token.
 *
 * @param p Pointer to the parser structure.
 */
static void next(Parser *p) { p->tok = lexer_next(&p->lx); }

/**
 * @brief Synchronizes the parser by skipping tokens until a semicolon, closing brace, or end of file is encountered.
 *
 * @param p Pointer to the parser structure.
 */
void parser_sync(Parser *p) {
    while (p->tok.kind != TK_SEMICOLON && p->tok.kind != TK_RBRACE &&
           p->tok.kind != TK_EOF)
        next(p);
    if (p->tok.kind == TK_SEMICOLON)
        next(p);
}
