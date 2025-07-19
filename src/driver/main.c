#include "../codegen/codegen.h"
#include "../ir/lower.h"
#include "../lexer/lexer.h"
#include "../opt/pipeline.h"
#include "../parser/diagnostic.h"
#include "../parser/parser.h"
#include "../sem/analysis.h"
#include "../ssa/ssa.h"
#include "../util/console_debug.h"
#include "../util/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#ifdef _WIN32
#define DR_EXE_NAME "dream.exe"
#else
#define DR_EXE_NAME "dream"
#endif

/**
 * @brief Reads the contents of a file into a dynamically allocated buffer.
 *
 * Opens the specified file in binary mode, reads its contents, and ensures
 * the buffer is null-terminated. If the file is empty or does not end with
 * a newline, one is appended.
 *
 * @param path Path to the file to be read.
 * @return char* Pointer to the dynamically allocated buffer containing the
 * file contents, or NULL if the file could not be read.
 */
static void normalize_newlines(char *buf) {
  char *r = buf, *w = buf;
  while (*r) {
    if (r[0] == '\r' && r[1] == '\n') {
      *w++ = '\n';
      r += 2;
    } else {
      *w++ = *r++;
    }
  }
  *w = 0;
}

static char *read_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buf = malloc(len + 2);
  if (!buf) {
    fclose(f);
    return NULL;
  }
  fread(buf, 1, len, f);
  if (len == 0 || buf[len - 1] != '\n') {
    buf[len++] = '\n';
  }
  buf[len] = 0;
  fclose(f);
  return buf;
}

/**
 * @brief Entry point for the compiler program.
 *
 * Parses command-line arguments, reads the input file, and processes it
 * through various stages such as parsing, diagnostics, and code generation.
 * Supports options for optimization and output format.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Exit status of the program.
 */
int main(int argc, char *argv[]) {
  int opt_level = 0;        /**< Optimization level (0-3). */
  bool emit_c = true;       /**< Flag for emitting C code. */
  bool emit_obj = false;    /**< Flag for emitting object code. */
  const char *input = NULL; /**< Path to the input file. */

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-O1") == 0 || strcmp(argv[i], "--O1") == 0) {
      opt_level = 1;
      continue;
    }
    if (strcmp(argv[i], "-O2") == 0 || strcmp(argv[i], "--O2") == 0) {
      opt_level = 2;
      continue;
    }
    if (strcmp(argv[i], "-O3") == 0 || strcmp(argv[i], "--O3") == 0) {
      opt_level = 3;
      continue;
    }
    if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
      diag_verbose = true;
      continue;
    }
    if (strcmp(argv[i], "--emit-c") == 0) {
      emit_c = true;
      emit_obj = false;
      continue;
    }
    if (strcmp(argv[i], "--emit-obj") == 0) {
      emit_obj = true;
      emit_c = false;
      continue;
    }
    input = argv[i];
  }

  if (!input) {
    fprintf(stderr, "usage: %s [options] file\n", argv[0]);
    return 1;
  }

  char *src = read_file(input);
  if (!src) {
    perror("read_file");
    return 1;
  }
  normalize_newlines(src);

  Console.WriteLine("compiling %s", input);

  Arena arena;
  arena_init(&arena);
  Parser p;
  parser_init(&p, &arena, src);
  Node *root = parse_program(&p);
  print_diagnostics(src, &p.diags);

  SemAnalyzer sem;
  sem_analyzer_init(&sem, &arena);
  sem_analyze_program(&sem, root);
  print_diagnostics(src, &sem.diags);

  int nvars = 0;
  CFG *cfg = ir_lower_program(root, &nvars);
  cfg_compute_dominators(cfg);
  /* SSA construction disabled for now while control-flow lowering evolves */
  run_pipeline(cfg, opt_level);
  cfg_free(cfg);

  if (emit_c) {
    dr_mkdir("build");
    dr_mkdir("build/bin");
    FILE *out = fopen("build/bin/dream.c", "w");
    if (!out) {
      perror("fopen");
      return 1;
    }
    codegen_emit_c(root, out, input);
    fclose(out);
    const char *cc = getenv("CC");
    if (!cc)
      cc = "zig cc";
    char cmd[256];
    int res;
    const char *optflag =
        opt_level >= 3 ? "-O3" : (opt_level >= 2 ? "-O2" : "");

    DIR *rt = opendir("src/runtime");
    if (!rt) {
      perror("opendir src/runtime");
      return 1;
    }

    char objs[16][64];
    size_t obj_count = 0;
    struct dirent *ent;
    while ((ent = readdir(rt))) {
      size_t len = strlen(ent->d_name);
      if (len > 2 && strcmp(ent->d_name + len - 2, ".c") == 0) {
        char obj[64];
        snprintf(obj, sizeof(obj), "%.*s.o", (int)len - 2, ent->d_name);
        snprintf(cmd, sizeof(cmd),
                 "%s -g %s -c \"src%cruntime%c%s\" -o \"build%c%s\"",
                 cc, optflag, DR_PATH_SEP, DR_PATH_SEP, ent->d_name, DR_PATH_SEP, obj);
        res = system(cmd);
        if (res != 0) {
          fprintf(stderr, "failed to run: %s\n", cmd);
          closedir(rt);
          return 1;
        }
        strncpy(objs[obj_count++], obj, sizeof(objs[0]) - 1);
      }
    }
    closedir(rt);

#ifdef _WIN32
    char link_cmd[512];
    snprintf(link_cmd, sizeof(link_cmd),
             "%s -g %s -Isrc/runtime \"build%cbin%cdream.c\"",
             cc, optflag, DR_PATH_SEP, DR_PATH_SEP);
    for (size_t i = 0; i < obj_count; i++) {
      strncat(link_cmd, " \"build", sizeof(link_cmd) - strlen(link_cmd) - 1);
      char sep[2] = {DR_PATH_SEP, 0};
      strncat(link_cmd, sep, sizeof(link_cmd) - strlen(link_cmd) - 1);
      strncat(link_cmd, objs[i], sizeof(link_cmd) - strlen(link_cmd) - 1);
      strncat(link_cmd, "\"", sizeof(link_cmd) - strlen(link_cmd) - 1);
    }
    strncat(link_cmd, " -o \"" DR_EXE_NAME "\"", sizeof(link_cmd) - strlen(link_cmd) - 1);
    res = system(link_cmd);
    if (res != 0) {
      fprintf(stderr, "failed to run: %s\n", link_cmd);
      return 1;
    }
#else
    char archive_cmd[512] = "ar rcs build";
    size_t pos = strlen(archive_cmd);
    archive_cmd[pos++] = DR_PATH_SEP;
    strcpy(archive_cmd + pos, "libdruntime.a");
    for (size_t i = 0; i < obj_count; i++) {
      strncat(archive_cmd, " build", sizeof(archive_cmd) - strlen(archive_cmd) - 1);
      char sep[2] = {DR_PATH_SEP, 0};
      strncat(archive_cmd, sep, sizeof(archive_cmd) - strlen(archive_cmd) - 1);
      strncat(archive_cmd, objs[i], sizeof(archive_cmd) - strlen(archive_cmd) - 1);
    }
    res = system(archive_cmd);
    if (res != 0) {
      fprintf(stderr, "failed to run: %s\n", archive_cmd);
      return 1;
    }
    snprintf(cmd, sizeof(cmd),
             "%s -g %s -Isrc/runtime \"build%cbin%cdream.c\" -Lbuild -ldruntime -o \"%s\"",
             cc, optflag, DR_PATH_SEP, DR_PATH_SEP, DR_EXE_NAME);
    res = system(cmd);
    if (res != 0) {
      fprintf(stderr, "failed to run: %s\n", cmd);
      return 1;
    }
#endif
  } else if (emit_obj) {
    codegen_emit_obj(root, "a.o", input);
  }

  free(src);
  free(p.diags.data);
  sem_analyzer_free(&sem);
  free(arena.ptr);
  return 0;
}
