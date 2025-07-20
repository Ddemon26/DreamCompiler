#ifndef WARNINGS_H
#define WARNINGS_H

#include "parser.h"

/**
 * @brief Analyzes the AST for potential warnings and adds them to the diagnostic vector.
 * 
 * @param p Pointer to the parser structure
 * @param root Root node of the AST to analyze
 */
void analyze_warnings(Parser *p, Node *root);

/**
 * @brief Checks for unused variables in a scope.
 */
void check_unused_variables(Parser *p, Node *scope);

/**
 * @brief Checks for unreachable code patterns.
 */
void check_unreachable_code(Parser *p, Node *stmt);

/**
 * @brief Checks for suspicious expressions that might be mistakes.
 */
void check_suspicious_expressions(Parser *p, Node *expr);

#endif