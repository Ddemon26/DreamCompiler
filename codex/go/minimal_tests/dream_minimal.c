/*
 * Minimal DreamCompiler C API Implementation for Go Testing
 */

#include "dream_minimal.h"
#include <string.h>
#include <stdio.h>

// Simple arithmetic test
int dream_test_add(int a, int b) {
    return a + b;
}

// String length test
int dream_test_string_length(const char* str) {
    if (!str) return -1;
    return (int)strlen(str);
}

// Mock compilation test (just checks if source is not NULL)
bool dream_test_compilation(const char* source) {
    if (!source) return false;
    if (strlen(source) == 0) return false;
    
    // Simple check for basic Dream syntax
    if (strstr(source, "Console.WriteLine")) return true;
    if (strstr(source, "int ") || strstr(source, "string ")) return true;
    
    return false;
}

// Version information
const char* dream_get_version(void) {
    return "DreamCompiler v1.0.0";
}