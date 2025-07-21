#include "parser.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diag.inc"
#include "util.inc"

void parser_init(Parser *p, Arena *a, const char *src) {
  lexer_init(&p->lx, src);
  p->arena = a;
  p->diags.data = NULL;
  p->diags.len = 0;
  p->diags.cap = 0;
  p->types = NULL;
  p->type_len = 0;
  p->type_cap = 0;

  p->warn_config.warnings_as_errors = false;
  p->warn_config.disable_all_warnings = false;
  p->warn_config.warn_unused_vars = true;
  p->warn_config.warn_unreachable_code = true;
  p->warn_config.warn_suspicious_expr = true;

  next(p);
}

static Node *parse_expr_prec(Parser *p, int min_prec);
static Node *parse_expr(Parser *p);
static Node *parse_stmt(Parser *p);
static Node *parse_unary(Parser *p);
static Node *parse_do_while(Parser *p);
static Node *parse_for(Parser *p);
static Node *parse_switch(Parser *p);
static Node *parse_var_decl(Parser *p);
static void nodevec_push(Node ***data, size_t *len, size_t *cap, Node *n);
static void typevec_push(Parser *p, Slice name);
static bool typevec_contains(Parser *p, Token tok);

#include "expr.inc"
#include "stmt.inc"
