#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_string_var(Compiler *compiler, const char *name) {
  for (int i = 0; i < compiler->string_var_count; i++) {
    if (strcmp(compiler->string_vars[i], name) == 0)
      return 1;
  }
  return 0;
}

static void gen_c_expr_internal(Compiler *compiler, FILE *out, Node *expr,
                                int top) {
  if (!expr)
    return;
  if (expr->type == NODE_BINARY_OP) {
    if (!top)
      fputc('(', out);
    gen_c_expr_internal(compiler, out, expr->left, 0);
    fprintf(out, " %s ", expr->value);
    gen_c_expr_internal(compiler, out, expr->right, 0);
    if (!top)
      fputc(')', out);
  } else if (expr->type == NODE_UNARY_OP) {
    if (!top)
      fputc('(', out);
    fprintf(out, "%s", expr->value);
    gen_c_expr_internal(compiler, out, expr->left, 0);
    if (!top)
      fputc(')', out);
  } else if (expr->type == NODE_IDENTIFIER ||
             (expr->type == NODE_VAR_DECL && expr->left == NULL &&
              expr->right == NULL)) {
    fprintf(out, "%s", expr->value);
  } else if (expr->type == NODE_NUMBER) {
    fprintf(out, "%s", expr->value);
  } else if (expr->type == NODE_STRING) {
    fprintf(out, "\"%s\"", expr->value);
  } else if (expr->type == NODE_FUNC_CALL) {
    fprintf(out, "%s()", expr->value);
  }
}

void gen_c_expr(Compiler *compiler, FILE *out, Node *expr) {
  gen_c_expr_internal(compiler, out, expr, 1);
}

void generate_c_function(Compiler *compiler, Node *node) {
  FILE *out = compiler->output;
  fprintf(out, "long %s() {\n", node->value);
  generate_c(compiler, node->left);
  fprintf(out, "    return 0;\n");
  fprintf(out, "}\n");
}

void generate_c(Compiler *compiler, Node *node) {
  FILE *out = compiler->output;
  if (node->type == NODE_VAR_DECL || node->type == NODE_STR_DECL) {
    if (node->type == NODE_STR_DECL) {
      compiler->string_vars = realloc(compiler->string_vars, sizeof(char *) * (compiler->string_var_count + 1));
      compiler->string_vars[compiler->string_var_count++] = strdup(node->value);
    }
    fprintf(out, "    %s %s", node->type == NODE_STR_DECL ? "const char*" : "long", node->value);
    if (node->left) {
      fprintf(out, " = ");
      gen_c_expr(compiler, out, node->left);
    }
    fprintf(out, ";\n");
  } else if (node->type == NODE_ASSIGN) {
    fprintf(out, "    %s = ", node->value);
    gen_c_expr(compiler, out, node->left);
    fprintf(out, ";\n");
  } else if (node->type == NODE_WRITELINE) {
    int is_str = 0;
    if (node->left) {
      if (node->left->type == NODE_STRING)
        is_str = 1;
      else if (node->left->type == NODE_IDENTIFIER)
        is_str = is_string_var(compiler, node->left->value);
    }
    if (is_str) {
      fprintf(out, "    printf(\"%%s\\n\", ");
      gen_c_expr(compiler, out, node->left);
      fprintf(out, ");\n");
    } else {
      fprintf(out, "    printf(\"%%ld\\n\", (long)");
      gen_c_expr(compiler, out, node->left);
      fprintf(out, ");\n");
    }
  } else if (node->type == NODE_IF) {
    fprintf(out, "    if (");
    gen_c_expr(compiler, out, node->left);
    fprintf(out, ") {\n");
    generate_c(compiler, node->right);
    fprintf(out, "    }");
    if (node->else_branch) {
      fprintf(out, " else {\n");
      generate_c(compiler, node->else_branch);
      fprintf(out, "    }");
    }
    fprintf(out, "\n");
  } else if (node->type == NODE_WHILE) {
    fprintf(out, "    while (");
    gen_c_expr(compiler, out, node->left);
    fprintf(out, ") {\n");
    generate_c(compiler, node->right);
    fprintf(out, "    }\n");
  } else if (node->type == NODE_DO_WHILE) {
    fprintf(out, "    do {\n");
    generate_c(compiler, node->right);
    fprintf(out, "    } while (");
    gen_c_expr(compiler, out, node->left);
    fprintf(out, ");\n");
  } else if (node->type == NODE_BREAK) {
    fprintf(out, "    break;\n");
  } else if (node->type == NODE_CONTINUE) {
    fprintf(out, "    continue;\n");
  } else if (node->type == NODE_RETURN) {
    if (node->left) {
      fprintf(out, "    return ");
      gen_c_expr(compiler, out, node->left);
      fprintf(out, ";\n");
    } else {
      fprintf(out, "    return 0;\n");
    }
  } else if (node->type == NODE_BLOCK) {
    Node *cur = node;
    while (cur) {
      generate_c(compiler, cur->left);
      cur = cur->right;
    }
  }
}
