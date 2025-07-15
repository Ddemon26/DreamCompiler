#include "codegen.h"
#include <stdio.h>

void gen_c_expr(FILE *out, Node *expr) {
  if (!expr)
    return;
  if (expr->type == NODE_BINARY_OP) {
    fputc('(', out);
    gen_c_expr(out, expr->left);
    fprintf(out, " %s ", expr->value);
    gen_c_expr(out, expr->right);
    fputc(')', out);
  } else if (expr->type == NODE_UNARY_OP) {
    fputc('(', out);
    fprintf(out, "%s", expr->value);
    gen_c_expr(out, expr->left);
    fputc(')', out);
  } else if (expr->type == NODE_IDENTIFIER ||
             (expr->type == NODE_VAR_DECL && expr->left == NULL &&
              expr->right == NULL)) {
    fprintf(out, "%s", expr->value);
  } else if (expr->type == NODE_NUMBER) {
    fprintf(out, "%s", expr->value);
  } else if (expr->type == NODE_STRING) {
    fprintf(out, "\"%s\"", expr->value);
  }
}

void generate_c(Compiler *compiler, Node *node) {
  FILE *out = compiler->output;
  if (node->type == NODE_VAR_DECL) {
    fprintf(out, "    long %s", node->value);
    if (node->left) {
      fprintf(out, " = ");
      gen_c_expr(out, node->left);
    }
    fprintf(out, ";\n");
  } else if (node->type == NODE_ASSIGN) {
    fprintf(out, "    %s = ", node->value);
    gen_c_expr(out, node->left);
    fprintf(out, ";\n");
  } else if (node->type == NODE_WRITELINE) {
    if (node->left && node->left->type == NODE_STRING) {
      fprintf(out, "    printf(\"%%s\\n\", ");
    } else {
      fprintf(out, "    printf(\"%%ld\\n\", ");
    }
    gen_c_expr(out, node->left);
    fprintf(out, ");\n");
  } else if (node->type == NODE_IF) {
    fprintf(out, "    if (");
    gen_c_expr(out, node->left);
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
    gen_c_expr(out, node->left);
    fprintf(out, ") {\n");
    generate_c(compiler, node->right);
    fprintf(out, "    }\n");
  } else if (node->type == NODE_DO_WHILE) {
    fprintf(out, "    do {\n");
    generate_c(compiler, node->right);
    fprintf(out, "    } while (");
    gen_c_expr(out, node->left);
    fprintf(out, ");\n");
  } else if (node->type == NODE_BREAK) {
    fprintf(out, "    break;\n");
  } else if (node->type == NODE_CONTINUE) {
    fprintf(out, "    continue;\n");
  } else if (node->type == NODE_BLOCK) {
    Node *cur = node;
    while (cur) {
      generate_c(compiler, cur->left);
      cur = cur->right;
    }
  }
}
