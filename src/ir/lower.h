#ifndef LOWER_H
#define LOWER_H
#include "../cfg/cfg.h"
#include "../parser/ast.h"

CFG *ir_lower_program(Node *root, int *nvars);
void cfg_free(CFG *cfg);

#endif
