#ifndef CODEGEN_H
#define CODEGEN_H

#include "../ast/node.h"
#include <stdio.h>

typedef struct {
  FILE *output;
  char **string_vars;
  int string_var_count;
  char **bool_vars;
  int bool_var_count;
  char **float_vars;
  int float_var_count;
  char **char_vars;
  int char_var_count;
  char **string_funcs;
  int string_func_count;
  char **bool_funcs;
  int bool_func_count;
  char **float_funcs;
  int float_func_count;
  char **char_funcs;
  int char_func_count;
} Compiler;

void gen_c_expr(Compiler *compiler, FILE *out, Node *expr);
void gen_c_expr_unwrapped(Compiler *compiler, FILE *out, Node *expr);
void generate_c(Compiler *compiler, Node *node);
void generate_c_function(Compiler *compiler, Node *node);
void generate_c_class(Compiler *compiler, Node *node);

#endif
