#include "parser.h"

/**
 * @brief Advances the parser to the next token.
 *
 * @param p Pointer to the parser structure.
 */
static void next(Parser *p) { p->tok = lexer_next(&p->lx); }

/**
 * @brief Synchronizes the parser by skipping tokens until a safe recovery point.
 * Attempts to find statement boundaries or block boundaries to continue parsing.
 *
 * @param p Pointer to the parser structure.
 */
void parser_sync(Parser *p) {
    // Count braces to avoid getting lost in nested structures
    int brace_depth = 0;
    int paren_depth = 0;
    
    while (p->tok.kind != TK_EOF) {
        switch (p->tok.kind) {
            case TK_LBRACE:
                brace_depth++;
                break;
            case TK_RBRACE:
                if (brace_depth > 0) {
                    brace_depth--;
                } else {
                    // Found a closing brace at the current level - good recovery point
                    return;
                }
                break;
            case TK_LPAREN:
                paren_depth++;
                break;
            case TK_RPAREN:
                if (paren_depth > 0) {
                    paren_depth--;
                }
                break;
            case TK_SEMICOLON:
                if (brace_depth == 0 && paren_depth == 0) {
                    // Found a semicolon at the top level - consume and return
                    next(p);
                    return;
                }
                break;
            case TK_KW_FUNC:   // Function keyword - good recovery point
            case TK_KW_CLASS:  // Class keyword - good recovery point  
            case TK_KW_STRUCT: // Struct keyword - good recovery point
            case TK_KW_MODULE: // Module keyword - good recovery point
                if (brace_depth == 0 && paren_depth == 0) {
                    // Found a new declaration at top level
                    return;
                }
                break;
            case TK_KW_IF:     // Control flow statements
            case TK_KW_WHILE:
            case TK_KW_FOR:
            case TK_KW_DO:
            case TK_KW_SWITCH:
            case TK_KW_TRY:
            case TK_KW_RETURN:
            case TK_KW_BREAK:
            case TK_KW_CONTINUE:
                if (brace_depth == 0 && paren_depth == 0) {
                    // Found a new statement at top level
                    return;
                }
                break;
            case TK_KW_INT:    // Type keywords - good recovery points for declarations
            case TK_KW_FLOAT:
            case TK_KW_BOOL:
            case TK_KW_CHAR:
            case TK_KW_STRING:
            case TK_KW_VOID:
                if (brace_depth == 0 && paren_depth == 0) {
                    // Found a type declaration
                    return;
                }
                break;
        }
        next(p);
    }
}

/**
 * @brief Enhanced synchronization for expression parsing.
 * Finds safe points to resume expression parsing.
 *
 * @param p Pointer to the parser structure.
 */
void parser_sync_expr(Parser *p) {
    int paren_depth = 0;
    int bracket_depth = 0;
    
    while (p->tok.kind != TK_EOF) {
        switch (p->tok.kind) {
            case TK_LPAREN:
                paren_depth++;
                break;
            case TK_RPAREN:
                if (paren_depth > 0) {
                    paren_depth--;
                } else {
                    // Unmatched closing paren - good stopping point
                    return;
                }
                break;
            case TK_LBRACKET:
                bracket_depth++;
                break;
            case TK_RBRACKET:
                if (bracket_depth > 0) {
                    bracket_depth--;
                } else {
                    // Unmatched closing bracket
                    return;
                }
                break;
            case TK_SEMICOLON:
            case TK_COMMA:
                if (paren_depth == 0 && bracket_depth == 0) {
                    return;
                }
                break;
            case TK_LBRACE:
            case TK_RBRACE:
                // Block boundaries
                return;
        }
        next(p);
    }
}
