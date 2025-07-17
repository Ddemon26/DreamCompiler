#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H
#include "parser.h"

/**
 * @brief Prints diagnostic messages for a given source code and diagnostic vector.
 *
 * @param src Pointer to the source code.
 * @param vec Pointer to the vector containing diagnostic information.
 */
void print_diagnostics(const char *src, DiagnosticVec *vec);

#endif
