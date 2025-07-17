#ifndef CODEGEN_H
#define CODEGEN_H

#include "../parser/ast.h"
#include <stdio.h>

/**
 * @brief Emits C code for the given AST root node to the specified output file.
 *
 * @param root Pointer to the root node of the AST.
 * @param out Pointer to the output file.
 */
void codegen_emit_c(Node *root, FILE *out);
/**
 * @brief Emits an object file for the given AST root node.
 *
 * @param root Pointer to the root node of the AST.
 * @param path Path to the output object file.
 */
void codegen_emit_obj(Node *root, const char *path);

#endif

