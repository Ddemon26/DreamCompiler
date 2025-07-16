#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../../codegen/c_emit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

int main(int argc, char *argv[]) {
  int emit_c = 1;
  const char *input = NULL;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--emit-c") == 0)
      emit_c = 1;
    else if (strcmp(argv[i], "--emit-obj") == 0)
      emit_c = 0;
    else
      input = argv[i];
  }
  if (!input) {
    fprintf(stderr, "Usage: %s [--emit-c|--emit-obj] <input.dr>\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(input, "r");
  if (!file) {
    fprintf(stderr, "Cannot open file %s\n", argv[1]);
    return 1;
  }
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *source = malloc(size + 1);
  fread(source, 1, size, file);
  source[size] = '\0';
  fclose(file);
  Lexer lexer = {source, 0, 1};
  if ((unsigned char)source[0] == 0xEF && (unsigned char)source[1] == 0xBB &&
      (unsigned char)source[2] == 0xBF) {
    lexer.pos = 3;
  }
#ifdef _WIN32
  _mkdir("build");
  _mkdir("build/bin");
#else
  mkdir("build", 0755);
  mkdir("build/bin", 0755);
#endif
  Token token = next_token(&lexer);
  Node *program = NULL;
  Node **current = &program;
  while (token.type != TOKEN_EOF) {
    Node *stmt = parse_statement(&lexer, &token);
    Node *block = create_node(NODE_BLOCK, NULL, stmt, NULL, NULL);
    *current = block;
    current = &block->right;
    free(token.value);
    token = next_token(&lexer);
  }

  FILE *out = fopen("build/bin/dream.c", "w");
  if (!out) {
    fprintf(stderr, "Failed to open output file\n");
    return 1;
  }
  emit_c_program(program, out);
  fclose(out);
  free_node(program);
  free(source);
  free(token.value);
  if (emit_c) {
    const char *cc = getenv("CC");
    if (!cc)
      cc = "gcc";
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "%s build/bin/dream.c -o build/bin/dream.exe", cc);
    system(cmd);
  } else {
    fprintf(stderr, "Object emission not implemented yet\n");
  }
  return 0;
}
