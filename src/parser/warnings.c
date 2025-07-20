#include "warnings.h"
#include "ast.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Recursively walks through AST nodes to check for warnings.
 */
static void analyze_node(Parser *p, Node *node) {
    if (!node || p->warn_config.disable_all_warnings) return;
    
    switch (node->kind) {
        case ND_BINOP:
            // Check for assignment in condition (= instead of ==)
            if (node->as.bin.op == TK_EQ && p->warn_config.warn_suspicious_expr) {
                check_suspicious_expressions(p, node);
            }
            analyze_node(p, node->as.bin.lhs);
            analyze_node(p, node->as.bin.rhs);
            break;
        case ND_BLOCK:
            if (p->warn_config.warn_unused_vars) {
                check_unused_variables(p, node);
            }
            if (p->warn_config.warn_unreachable_code) {
                check_unreachable_code(p, node);
            }
            // Recurse into statements
            for (size_t i = 0; i < node->as.block.len; i++) {
                analyze_node(p, node->as.block.items[i]);
            }
            break;
        case ND_IF:
            analyze_node(p, node->as.if_stmt.cond);
            analyze_node(p, node->as.if_stmt.then_br);
            analyze_node(p, node->as.if_stmt.else_br);
            break;
        case ND_WHILE:
            analyze_node(p, node->as.while_stmt.cond);
            analyze_node(p, node->as.while_stmt.body);
            break;
        case ND_FOR:
            analyze_node(p, node->as.for_stmt.init);
            analyze_node(p, node->as.for_stmt.cond);
            analyze_node(p, node->as.for_stmt.update);
            analyze_node(p, node->as.for_stmt.body);
            break;
        case ND_RETURN:
            // Check if there's code after return
            break;
        default:
            // For other node types, recursively analyze children based on node structure
            break;
    }
}

void analyze_warnings(Parser *p, Node *root) {
    if (!root) return;
    analyze_node(p, root);
}

void check_unused_variables(Parser *p, Node *scope) {
    // This is a simplified check - in a real implementation, you'd track variable declarations and usage
    // For now, we'll add a placeholder warning system
    if (!scope || scope->kind != ND_BLOCK) return;
    
    // Basic heuristic: look for variable declarations that might be unused
    // This is a simple pattern-based check, not full semantic analysis
    for (size_t i = 0; i < scope->as.block.len; i++) {
        Node *stmt = scope->as.block.items[i];
        if (!stmt || stmt->kind != ND_VAR_DECL) continue;
        
        // Simple heuristic: if a variable is declared but we don't see it referenced
        // later in the same block, it might be unused (this is very basic)
        bool possibly_unused = true;
        
        // Look for usage in subsequent statements (very basic check)
        for (size_t j = i + 1; j < scope->as.block.len && possibly_unused; j++) {
            Node *later_stmt = scope->as.block.items[j];
            if (later_stmt && later_stmt->kind == ND_IDENT) {
                // This is a very simplified check - real implementation would need proper symbol analysis
                possibly_unused = false;
            }
        }
        
        // For now, only warn about variables with specific patterns to avoid false positives
        if (stmt->as.var_decl.name.len > 0) {
            const char *name = stmt->as.var_decl.name.start;
            size_t len = stmt->as.var_decl.name.len;
            
            // Only warn about variables starting with 'temp' or 'unused' for safety
            if ((len >= 4 && strncmp(name, "temp", 4) == 0) || 
                (len >= 6 && strncmp(name, "unused", 6) == 0)) {
                if (p->diags.len + 1 > p->diags.cap) {
                    p->diags.cap = p->diags.cap ? p->diags.cap * 2 : 4;
                    p->diags.data = realloc(p->diags.data, p->diags.cap * sizeof(Diagnostic));
                }
                p->diags.data[p->diags.len++] = (Diagnostic){
                    .pos = stmt->pos,
                    .end_pos = stmt->pos,
                    .start = name,
                    .len = len,
                    .msg = "variable might be unused",
                    .hint = "consider removing unused variables or prefixing with '_' to indicate intentional non-use",
                    .sev = p->warn_config.warnings_as_errors ? DIAG_ERROR : DIAG_WARNING
                };
            }
        }
    }
}

void check_unreachable_code(Parser *p, Node *stmt) {
    if (!stmt || stmt->kind != ND_BLOCK) return;
    
    // Check for statements after return/break/continue
    bool found_terminator = false;
    for (size_t i = 0; i < stmt->as.block.len; i++) {
        Node *current = stmt->as.block.items[i];
        if (!current) continue;
        
        if (found_terminator) {
            // Found code after a terminator - this is unreachable
            if (p->diags.len + 1 > p->diags.cap) {
                p->diags.cap = p->diags.cap ? p->diags.cap * 2 : 4;
                p->diags.data = realloc(p->diags.data, p->diags.cap * sizeof(Diagnostic));
            }
            p->diags.data[p->diags.len++] = (Diagnostic){
                .pos = current->pos,
                .end_pos = current->pos,
                .start = NULL,
                .len = 0,
                .msg = "unreachable code detected",
                .hint = "this code will never be executed due to the preceding return/break/continue",
                .sev = p->warn_config.warnings_as_errors ? DIAG_ERROR : DIAG_WARNING
            };
            break; // Only warn about the first unreachable statement
        }
        
        if (current->kind == ND_RETURN || current->kind == ND_BREAK || current->kind == ND_CONTINUE) {
            found_terminator = true;
        }
    }
}

void check_suspicious_expressions(Parser *p, Node *expr) {
    if (!expr) return;
    
    switch (expr->kind) {
        case ND_BINOP:
            // Check for assignment in what looks like a comparison context
            if (expr->as.bin.op == TK_EQ) {
                if (p->diags.len + 1 > p->diags.cap) {
                    p->diags.cap = p->diags.cap ? p->diags.cap * 2 : 4;
                    p->diags.data = realloc(p->diags.data, p->diags.cap * sizeof(Diagnostic));
                }
                p->diags.data[p->diags.len++] = (Diagnostic){
                    .pos = expr->pos,
                    .end_pos = expr->pos,
                    .start = NULL,
                    .len = 0,
                    .msg = "assignment in expression - did you mean '==' for comparison?",
                    .hint = "use '==' for equality comparison, '=' for assignment",
                    .sev = p->warn_config.warnings_as_errors ? DIAG_ERROR : DIAG_WARNING
                };
            }
            // Check for potential operator precedence issues
            else if (expr->as.bin.op == TK_ANDAND || expr->as.bin.op == TK_OROR) {
                // Warn about mixed && and || without parentheses (simplified check)
                if (expr->as.bin.lhs && expr->as.bin.lhs->kind == ND_BINOP) {
                    TokenKind lhs_op = expr->as.bin.lhs->as.bin.op;
                    if ((expr->as.bin.op == TK_ANDAND && lhs_op == TK_OROR) ||
                        (expr->as.bin.op == TK_OROR && lhs_op == TK_ANDAND)) {
                        if (p->diags.len + 1 > p->diags.cap) {
                            p->diags.cap = p->diags.cap ? p->diags.cap * 2 : 4;
                            p->diags.data = realloc(p->diags.data, p->diags.cap * sizeof(Diagnostic));
                        }
                        p->diags.data[p->diags.len++] = (Diagnostic){
                            .pos = expr->pos,
                            .end_pos = expr->pos,
                            .start = NULL,
                            .len = 0,
                            .msg = "mixed logical operators may be confusing",
                            .hint = "consider adding parentheses to clarify operator precedence",
                            .sev = p->warn_config.warnings_as_errors ? DIAG_ERROR : DIAG_WARNING
                        };
                    }
                }
            }
            break;
        
        case ND_INT:
            // Check for suspicious integer literals (e.g., very large numbers)
            if (expr->as.lit.len > 10) { // More than 10 digits might be suspicious
                if (p->diags.len + 1 > p->diags.cap) {
                    p->diags.cap = p->diags.cap ? p->diags.cap * 2 : 4;
                    p->diags.data = realloc(p->diags.data, p->diags.cap * sizeof(Diagnostic));
                }
                p->diags.data[p->diags.len++] = (Diagnostic){
                    .pos = expr->pos,
                    .end_pos = expr->pos,
                    .start = expr->as.lit.start,
                    .len = expr->as.lit.len,
                    .msg = "very large integer literal",
                    .hint = "consider using scientific notation or named constants for large numbers",
                    .sev = p->warn_config.warnings_as_errors ? DIAG_ERROR : DIAG_WARNING
                };
            }
            break;
            
        default:
            break;
    }
}