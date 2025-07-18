#ifndef ANALYSIS_H
#define ANALYSIS_H
#include "../parser/parser.h"
#include "scope.h"

typedef struct SemAnalyzer {
  Arena *arena;
  DiagnosticVec diags;
  Scope *scope;
  TokenKind current_ret;
} SemAnalyzer;

void sem_analyzer_init(SemAnalyzer *s, Arena *a);
void sem_analyzer_free(SemAnalyzer *s);
void sem_analyze_program(SemAnalyzer *s, Node *root);

#endif
