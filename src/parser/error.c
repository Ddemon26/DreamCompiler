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
            case TK_SEMICOLON:
                if (brace_depth == 0) {
                    // Found a semicolon at the top level - consume and return
                    next(p);
                    return;
                }
                break;
            case TK_KW_FUNC:   // Function keyword - good recovery point
            case TK_KW_CLASS:  // Class keyword - good recovery point  
            case TK_KW_IF:     // Control flow statements
            case TK_KW_WHILE:
            case TK_KW_FOR:
            case TK_KW_DO:
            case TK_KW_SWITCH:
                if (brace_depth == 0) {
                    // Found a new declaration/statement at top level
                    return;
                }
                break;
        }
        next(p);
    }
}
