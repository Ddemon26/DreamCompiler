/*
 * Minimal DreamCompiler C API Header for Go Testing
 * This header provides only the essential functions needed for basic testing
 */

#ifndef DREAM_MINIMAL_H
#define DREAM_MINIMAL_H

#include <stddef.h>
#include <stdbool.h>

// Simple test functions that don't require full compiler integration
int dream_test_add(int a, int b);
int dream_test_string_length(const char* str);
bool dream_test_compilation(const char* source);
const char* dream_get_version(void);

#endif // DREAM_MINIMAL_H