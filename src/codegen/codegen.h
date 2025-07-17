#ifndef CODEGEN_H
#define CODEGEN_H

#include "../parser/ast.h"
#include <stdio.h>

void codegen_emit_c(Node *root, FILE *out);
void codegen_emit_obj(Node *root, const char *path);

#endif

