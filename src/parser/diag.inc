#include "parser.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Adds a diagnostic message to the parser's diagnostic list.
 *
 * @param p Pointer to the parser structure.
 * @param pos Position in the source code where the diagnostic occurred.
 * @param msg Diagnostic message to be added.
 */
static void diag_push(Parser *p, Pos pos, DiagSeverity sev, const char *msg) {
  if (p->diags.len + 1 > p->diags.cap) {
    p->diags.cap = p->diags.cap ? p->diags.cap * 2 : 4;
    p->diags.data = realloc(p->diags.data, p->diags.cap * sizeof(Diagnostic));
  }
  // Use current token for better span info
  Pos end_pos = pos;
  if (p->tok.len > 0) {
    end_pos.column = pos.column + p->tok.len;
  }
  p->diags.data[p->diags.len++] = (Diagnostic){
    .pos = pos, 
    .end_pos = end_pos,
    .start = p->tok.start,
    .len = p->tok.len,
    .msg = msg, 
    .hint = NULL,
    .sev = sev
  };
}

static void diag_pushf(Parser *p, Pos pos, DiagSeverity sev, const char *fmt,
                       ...) {
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);
  char *buf = arena_alloc(p->arena, n + 1);
  va_start(ap, fmt);
  vsnprintf(buf, n + 1, fmt, ap);
  va_end(ap);
  diag_push(p, pos, sev, buf);
}

/**
 * @brief Adds a diagnostic with a hint for fixing the issue.
 */
static void diag_push_with_hint(Parser *p, Pos pos, DiagSeverity sev, 
                               const char *msg, const char *hint) {
  if (p->diags.len + 1 > p->diags.cap) {
    p->diags.cap = p->diags.cap ? p->diags.cap * 2 : 4;
    p->diags.data = realloc(p->diags.data, p->diags.cap * sizeof(Diagnostic));
  }
  Pos end_pos = pos;
  if (p->tok.len > 0) {
    end_pos.column = pos.column + p->tok.len;
  }
  p->diags.data[p->diags.len++] = (Diagnostic){
    .pos = pos, 
    .end_pos = end_pos,
    .start = p->tok.start,
    .len = p->tok.len,
    .msg = msg, 
    .hint = hint,
    .sev = sev
  };
}

/**
 * @brief Adds an error with token context and attempts recovery.
 * This function reports an error and then tries to synchronize the parser.
 */
static void diag_error_and_sync(Parser *p, const char *msg, const char *hint) {
  diag_push_with_hint(p, p->tok.pos, DIAG_ERROR, msg, hint);
  parser_sync(p);
}

/**
 * @brief Creates a formatted error message with token information.
 */
static void diag_push_unexpected_token(Parser *p, const char *expected) {
  char *msg = arena_alloc(p->arena, 256);
  if (p->tok.kind == TK_EOF) {
    snprintf(msg, 256, "expected %s but reached end of file", expected);
  } else if (p->tok.start && p->tok.len > 0) {
    snprintf(msg, 256, "expected %s but found '%.*s'", expected, (int)p->tok.len, p->tok.start);
  } else {
    snprintf(msg, 256, "expected %s but found unexpected token", expected);
  }
  diag_push(p, p->tok.pos, DIAG_ERROR, msg);
}

/**
 * @brief Advances the parser to the next token.
 *
 * @param p Pointer to the parser structure.
 */
static void next(Parser *p) { p->tok = lexer_next(&p->lx); }

/**
 * @brief Initializes the parser with the given source code and memory arena.
 *
 * @param p Pointer to the parser structure.
 * @param a Pointer to the memory arena used for allocations.
 * @param src Pointer to the source code to be parsed.
 */
