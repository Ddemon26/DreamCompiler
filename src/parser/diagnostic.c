#include "diagnostic.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief ANSI escape code for red text color.
 */
static const char *RED = "\x1b[31m";
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
        const char *line_start = find_line_start(src, d.pos);
        const char *line_end = find_line_end(line_start);
        fprintf(stderr, "%serror:%s %s\n", RED, RESET, d.msg);
        fwrite(line_start, 1, line_end - line_start, stderr);
        fputc('\n', stderr);
        for (size_t c = 1; c < d.pos.column; ++c) fputc(' ', stderr);
        fprintf(stderr, "%s^%s\n", RED, RESET);
    }
}
