#include "diagnostic.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool diag_verbose = false;

/**
 * @brief ANSI escape code for red text color.
 */
static const char *RED = "\x1b[31m";
static const char *YELLOW = "\x1b[33m";
/**
 * @brief ANSI escape code to reset text formatting.
 */
static const char *RESET = "\x1b[0m";

/**
 * @brief Finds the start of the specified line in the source text.
 *
 * @param src Pointer to the source text.
 * @param pos Position structure containing the line number.
 * @return Pointer to the start of the specified line.
 */
static const char *find_line_start(const char *src, Pos pos) {
    const char *p = src;
    size_t line = 1;
    while (*p && line < pos.line) {
        if (*p == '\n') line++;
        p++;
    }
    return p;
}

/**
 * @brief Finds the end of a line in the source text.
 *
 * @param p Pointer to the current position in the source text.
 * @return Pointer to the end of the line or the null terminator.
 */
static const char *find_line_end(const char *p) {
    while (*p && *p != '\n') p++;
    return p;
}

/**
 * @brief Prints diagnostic messages for a given source code and diagnostic vector.
 *
 * @param src Pointer to the source code.
 * @param vec Pointer to the vector containing diagnostic information.
 */
void print_diagnostics(const char *src, DiagnosticVec *vec) {
    for (size_t i = 0; i < vec->len; ++i) {
        Diagnostic d = vec->data[i];
        const char *color = d.sev == DIAG_ERROR ? RED : YELLOW;
        const char *label = d.sev == DIAG_ERROR ? "error" : "warning";
        
        // Print main diagnostic message
        fprintf(stderr, "%s%zu:%zu: %s:%s %s\n", color, d.pos.line, d.pos.column, label, RESET, d.msg);
        
        // Always show context line and caret (not just in verbose mode)
        const char *line_start = find_line_start(src, d.pos);
        const char *line_end = find_line_end(line_start);
        
        // Print the source line
        fwrite(line_start, 1, line_end - line_start, stderr);
        fputc('\n', stderr);
        
        // Print caret/underline indicator
        for (size_t c = 1; c < d.pos.column; ++c) fputc(' ', stderr);
        fprintf(stderr, "%s", color);
        
        // If we have span info, underline the whole token
        if (d.len > 1 && d.end_pos.column > d.pos.column) {
            for (size_t c = d.pos.column; c < d.end_pos.column && c <= d.pos.column + d.len; ++c) {
                fputc('~', stderr);
            }
        } else {
            fputc('^', stderr);
        }
        fprintf(stderr, "%s\n", RESET);
        
        // Print hint if available
        if (d.hint) {
            fprintf(stderr, "%shelp:%s %s\n", YELLOW, RESET, d.hint);
        }
        
        fputc('\n', stderr); // Extra line for readability
    }
}
