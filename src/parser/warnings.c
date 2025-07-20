#include "warnings.h"
#include "ast.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Recursively walks through AST nodes to check for warnings.
 */
static void analyze_node(Parser *p, Node *node) {
    if (!node) return;
    
    switch (node->kind) {
        case ND_BINOP:
            // Check for assignment in condition (= instead of ==)
            if (node->as.bin.op == TK_EQ) {
                check_suspicious_expressions(p, node);
            }
            analyze_node(p, node->as.bin.lhs);
            analyze_node(p, node->as.bin.rhs);
            break;
        case ND_BLOCK:
            check_unused_variables(p, node);
            check_unreachable_code(p, node);
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
    
    // Suppress unused parameter warning
    (void)p;
    
    // TODO: Implement symbol table tracking for unused variables
    // This would require integration with the semantic analyzer
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
                .sev = DIAG_WARNING
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
    
    // Check for assignment in what looks like a comparison context
    if (expr->kind == ND_BINOP && expr->as.bin.op == TK_EQ) {
        // This is a simple heuristic - a real implementation would need more context
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
            .sev = DIAG_WARNING
        };
    }
}