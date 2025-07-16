#ifndef C_EMIT_H
#define C_EMIT_H

#include "../src/ast/node.h"
#include <stdio.h>

typedef struct {
    FILE *out;
    int indent;
} CEmitter;

void cemit_init(CEmitter *e, FILE *out);
void cemit_indent(CEmitter *e);
void cemit_dedent(CEmitter *e);
void cemit_line(CEmitter *e, const char *fmt, ...);

char *mangle_specialisation(const char *base, const char **types, int count);

void emit_c_program(Node *program, FILE *out);

#endif
