#ifndef PARSER_H
#define PARSER_H
#include "../lexer/lexer.h"
#include "ast.h"

/**
 * @brief Severity levels for diagnostics.
 */
typedef enum {
    DIAG_ERROR,   /**< An error that prevents successful compilation. */
    DIAG_WARNING  /**< A warning that does not stop compilation. */
} DiagSeverity;

/**
 * @brief Represents a diagnostic message with its position, content and severity.
 */
typedef struct {
    Pos pos;
    Pos end_pos;        // End position for better range highlighting
    const char *start;  // Start of token/span for context
    size_t len;         // Length of token/span 
    const char *msg;
    const char *hint;   // Optional hint for fixing the issue
    DiagSeverity sev;
} Diagnostic;

/**
 * @brief A dynamic array of diagnostic messages.
 */
typedef struct {
    Diagnostic *data;
    size_t len;
    size_t cap;
} DiagnosticVec;

/**
 * @brief Warning configuration flags.
 */
typedef struct {
    bool warnings_as_errors;    /**< Treat all warnings as errors. */
    bool disable_all_warnings;  /**< Suppress all warning messages. */
    bool warn_unused_vars;       /**< Warn about unused variables. */
    bool warn_unreachable_code;  /**< Warn about unreachable code. */
    bool warn_suspicious_expr;   /**< Warn about suspicious expressions. */
} WarningConfig;

/**
 * @brief Represents the parser structure, including lexer, current token, memory arena, and diagnostics.
 */
typedef struct {
    Lexer lx;
    Token tok;
    Arena *arena;
    DiagnosticVec diags;
    Slice *types;
    size_t type_len;
    size_t type_cap;
    WarningConfig warn_config;
} Parser;

/**
 * @brief Initializes the parser with a given memory arena and source code.
 *
 * @param p Pointer to the parser structure.
 * @param a Pointer to the memory arena.
 * @param src Source code to be parsed.
 */
void parser_init(Parser *p, Arena *a, const char *src);

/**
 * @brief Parses the entire program into a block node.
 *
 * @param p Pointer to the parser structure.
 * @return Pointer to the parsed program block node.
 */
Node *parse_program(Parser *p);

/**
 * @brief Synchronizes the parser state after an error.
 *
 * @param p Pointer to the parser structure.
 */
void parser_sync(Parser *p);

/**
 * @brief Enhanced synchronization for expression parsing.
 * Finds safe points to resume expression parsing.
 *
 * @param p Pointer to the parser structure.
 */
void parser_sync_expr(Parser *p);

#endif
