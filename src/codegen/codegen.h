#ifndef CODEGEN_H
#define CODEGEN_H

#include "../ast/node.h"
#include <stdio.h>

typedef struct {
  FILE *output;
} Compiler;

void gen_c_expr(FILE *out, Node *expr);
void generate_c(Compiler *compiler, Node *node);
void generate_c_function(Compiler *compiler, Node *node);

#endif
