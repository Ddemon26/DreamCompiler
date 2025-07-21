#include "custom.h"
#include "../memory/memory.h"

int dr_custom_value(void) { return 42; }

char *dream_int_to_string(int value) {
    // Allocate enough space for largest int + null terminator
    char *result = dr_alloc(32);
    snprintf(result, 32, "%d", value);
    return result;
}

char *dream_float_to_string(float value) {
    // Allocate enough space for float representation + null terminator
    char *result = dr_alloc(64);
    snprintf(result, 64, "%f", value);
    return result;
}

char *dream_concat_string_int(const char *str, int value) {
    char *int_str = dream_int_to_string(value);
    
    size_t str_len = strlen(str);
    size_t int_len = strlen(int_str);
    char *result = dr_alloc(str_len + int_len + 1);
    
    strcpy(result, str);
    strcat(result, int_str);
    
    // Note: int_str is allocated with dr_alloc, will be freed by dr_release_all
    return result;
}

char *dream_concat_int_string(int value, const char *str) {
    char *int_str = dream_int_to_string(value);
    
    size_t int_len = strlen(int_str);
    size_t str_len = strlen(str);
    char *result = dr_alloc(int_len + str_len + 1);
    
    strcpy(result, int_str);
    strcat(result, str);
    
    return result;
}

char *dream_concat_string_float(const char *str, float value) {
    char *float_str = dream_float_to_string(value);
    
    size_t str_len = strlen(str);
    size_t float_len = strlen(float_str);
    char *result = dr_alloc(str_len + float_len + 1);
    
    strcpy(result, str);
    strcat(result, float_str);
    
    return result;
}

char *dream_concat_float_string(float value, const char *str) {
    char *float_str = dream_float_to_string(value);
    
    size_t float_len = strlen(float_str);
    size_t str_len = strlen(str);
    char *result = dr_alloc(float_len + str_len + 1);
    
    strcpy(result, float_str);
    strcat(result, str);
    
    return result;
}
