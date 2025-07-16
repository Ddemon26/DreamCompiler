#include "parser.h"

static void next(Parser *p) { p->tok = lexer_next(&p->lx); }

void parser_sync(Parser *p) {
    while (p->tok.kind != TK_SEMICOLON && p->tok.kind != TK_RBRACE &&
           p->tok.kind != TK_EOF)
        next(p);
    if (p->tok.kind == TK_SEMICOLON)
        next(p);
}
