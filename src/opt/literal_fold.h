#ifndef LITERAL_FOLD_H
#define LITERAL_FOLD_H
#include "../parser/ast.h"
#include <stdbool.h>

/* Fold eligible maximal binary expressions through isolated SCCP IR.
 * False means an internal error was diagnosed; do not open output files. */
bool fold_literal_arithmetic(Arena *arena, Node *root);
#endif
