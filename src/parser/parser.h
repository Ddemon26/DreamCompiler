#ifndef PARSER_H
#define PARSER_H
#include "../lexer/lexer.h"
#include "ast.h"

typedef struct {
    Pos pos;
    const char *msg;
} Diagnostic;

typedef struct {
    Diagnostic *data;
    size_t len;
    size_t cap;
} DiagnosticVec;

typedef struct {
    Lexer lx;
    Token tok;
    Arena *arena;
    DiagnosticVec diags;
} Parser;

void parser_init(Parser *p, Arena *a, const char *src);
Node *parse_program(Parser *p);
void parser_sync(Parser *p);

#endif
