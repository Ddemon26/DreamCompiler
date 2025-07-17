#ifndef CONSOLE_DEBUG_H
#define CONSOLE_DEBUG_H
#include <stdio.h>
#include <stdarg.h>

typedef struct ConsoleAPI {
    void (*WriteLine)(const char *fmt, ...);
    void (*Write)(const char *fmt, ...);
} ConsoleAPI;

#ifndef DREAM_RELEASE
static inline void console_writeline_impl(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
}

static inline void console_write_impl(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
#else
static inline void console_writeline_impl(const char *fmt, ...) {
    (void)fmt;
}
static inline void console_write_impl(const char *fmt, ...) {
    (void)fmt;
}
#endif

static const ConsoleAPI Console = {
    .WriteLine = console_writeline_impl,
    .Write = console_write_impl,
};

#endif // CONSOLE_DEBUG_H

