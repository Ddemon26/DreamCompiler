#ifndef DR_CUSTOM_H
#define DR_CUSTOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

int dr_custom_value(void);

/**
 * Concatenate a string with an integer
 */
char *dream_concat_string_int(const char *str, int value);

/**
 * Concatenate an integer with a string  
 */
char *dream_concat_int_string(int value, const char *str);

/**
 * Concatenate a string with a float
 */
char *dream_concat_string_float(const char *str, float value);

/**
 * Concatenate a float with a string
 */
char *dream_concat_float_string(float value, const char *str);

/**
 * Concatenate two strings
 */
char *dream_concat(const char *str1, const char *str2);

/**
 * Convert integer to string for concatenation
 */
char *dream_int_to_string(int value);

/**
 * Convert float to string for concatenation
 */
char *dream_float_to_string(float value);

#endif
