#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../codegen/codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input.dr>\n", argv[0]);
    return 1;
  }
  FILE *file = fopen(argv[1], "r");
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
  Compiler compiler = {fopen("build/bin/dream.c", "w"), NULL, 0, NULL, 0, NULL, 0, NULL, 0,
                        NULL, 0, NULL, 0, NULL, 0, NULL, 0};
  if (!compiler.output) {
    fprintf(stderr, "Failed to open output file\n");
    return 1;
  }
  fprintf(compiler.output, "#include <stdio.h>\n");
  fprintf(compiler.output, "#include <stdlib.h>\n");
  fprintf(compiler.output, "#include <string.h>\n");
  fprintf(compiler.output,
          "static char* dream_concat(const char* a, const char* b) {\n"
          "  size_t len = strlen(a) + strlen(b);\n"
          "  char* s = malloc(len + 1);\n"
          "  strcpy(s, a);\n"
          "  strcat(s, b);\n"
          "  return s;\n"
          "}\n");
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

  Node *cur = program;
  while (cur) {
    if (cur->left->type == NODE_CLASS_DEF)
      generate_c_class(&compiler, cur->left);
    cur = cur->right;
  }
  cur = program;
  while (cur) {
    if (cur->left->type == NODE_FUNC_DEF)
      generate_c_function(&compiler, cur->left);
    cur = cur->right;
  }

  fprintf(compiler.output, "int main() {\n");
  cur = program;
  while (cur) {
    if (cur->left->type != NODE_FUNC_DEF)
      generate_c(&compiler, cur->left);
    cur = cur->right;
  }
  fprintf(compiler.output, "    return 0;\n");
  fprintf(compiler.output, "}\n");
  free_node(program);
  for (int i = 0; i < compiler.string_var_count; i++)
    free(compiler.string_vars[i]);
  free(compiler.string_vars);
  for (int i = 0; i < compiler.bool_var_count; i++)
    free(compiler.bool_vars[i]);
  free(compiler.bool_vars);
  for (int i = 0; i < compiler.float_var_count; i++)
    free(compiler.float_vars[i]);
  free(compiler.float_vars);
  for (int i = 0; i < compiler.char_var_count; i++)
    free(compiler.char_vars[i]);
  free(compiler.char_vars);
  for (int i = 0; i < compiler.string_func_count; i++)
    free(compiler.string_funcs[i]);
  free(compiler.string_funcs);
  for (int i = 0; i < compiler.bool_func_count; i++)
    free(compiler.bool_funcs[i]);
  free(compiler.bool_funcs);
  for (int i = 0; i < compiler.float_func_count; i++)
    free(compiler.float_funcs[i]);
  free(compiler.float_funcs);
  for (int i = 0; i < compiler.char_func_count; i++)
    free(compiler.char_funcs[i]);
  free(compiler.char_funcs);
  fclose(compiler.output);
  free(source);
  free(token.value);
  const char *cc = getenv("CC");
  if (!cc)
    cc = "gcc";
  char cmd[256];
  snprintf(cmd, sizeof(cmd), "%s build/bin/dream.c -o build/bin/dream.exe", cc);
  system(cmd);
  return 0;
}
