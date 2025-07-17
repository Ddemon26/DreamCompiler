#ifndef CONSOLE_DEBUG_H
#define CONSOLE_DEBUG_H
#include <stdio.h>
#include <stdarg.h>

/**
 * @brief Represents a console API with methods for writing output.
 *
 * This structure provides function pointers for writing formatted
 * output to the console, either as a single line or without a newline.
 */
typedef struct ConsoleAPI {
    void (*WriteLine)(const char *fmt, ...); /**< Writes a formatted line to the console. */
    void (*Write)(const char *fmt, ...);    /**< Writes formatted output to the console. */
} ConsoleAPI;

#ifndef DREAM_RELEASE
/**
  * @brief Writes a formatted line to the standard error stream.
  *
  * This function formats the given input using the specified format string
  * and writes it to the standard error stream, followed by a newline character.
  *
  * @param fmt The format string.
  * @param ... The values to format.
  */
 static inline void console_writeline_impl(const char *fmt, ...) {
     va_list ap;
     va_start(ap, fmt);
     vfprintf(stderr, fmt, ap);
     va_end(ap);
     fputc('\n', stderr);
 }

/**
  * @brief Writes formatted output to the standard error stream.
  *
  * Formats the given input using the specified format string
  * and writes it to the standard error stream.
  *
  * @param fmt The format string.
  * @param ... The values to format.
  */
 static inline void console_write_impl(const char *fmt, ...) {
     va_list ap;
     va_start(ap, fmt);
     vfprintf(stderr, fmt, ap);
     va_end(ap);
 }
#else
/**
  * @brief No-op implementation of writing a formatted line in release mode.
  *
  * This function does nothing and is used in release mode to disable
  * debug output.
  *
  * @param fmt The format string.
  * @param ... The values to format.
  */
 static inline void console_writeline_impl(const char *fmt, ...) {
     (void)fmt;
 }

 /**
  * @brief No-op implementation of writing formatted output in release mode.
  *
  * This function does nothing and is used in release mode to disable
  * debug output.
  *
  * @param fmt The format string.
  * @param ... The values to format.
  */
 static inline void console_write_impl(const char *fmt, ...) {
     (void)fmt;
 }
#endif

/**
  * @brief Global instance of the ConsoleAPI for writing output.
  *
  * Provides access to methods for writing formatted output to the console,
  * either as a single line or without a newline.
  */
 static const ConsoleAPI Console = {
     .WriteLine = console_writeline_impl,
     .Write = console_write_impl,
 };

 #endif // CONSOLE_DEBUG_H