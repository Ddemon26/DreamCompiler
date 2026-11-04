#ifndef ANALYSIS_H
#define ANALYSIS_H
#include "../parser/parser.h"
#include "scope.h"

typedef struct SemAnalyzer {
  Arena *arena;
  DiagnosticVec diags;
  Scope *scope;
  TokenKind current_ret;
  Node *program;
  Node *current_class;
  const char *source_file;
  const char *source_text;
} SemAnalyzer;

void sem_analyzer_init(SemAnalyzer *s, Arena *a);
void sem_analyzer_free(SemAnalyzer *s);
void sem_analyze_program(SemAnalyzer *s, Node *root);

/** Lookups use the inheritance graph validated by sem_analyze_program. */
Node *sem_find_type(Node *program, Slice name);
Node *sem_find_member(Node *type, Slice name, Node **owner);
bool sem_is_subclass(Node *derived, Node *base);

#endif
