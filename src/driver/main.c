#include "../codegen/codegen.h"
#include "../codegen/module.h"
#include "../ir/lower.h"
#include "../lexer/lexer.h"
#include "../opt/pipeline.h"
#include "../parser/diagnostic.h"
#include "../parser/parser.h"
#include "../parser/warnings.h"
#include "../sem/analysis.h"
#include "../ssa/ssa.h"
#include "../util/console_debug.h"
#include "../util/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#ifdef _WIN32
#include <windows.h>
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
  bool dev_mode = false;    /**< Flag for development mode (no compilation). */
  bool multi_file = false;  /**< Flag for multi-file compilation mode. */
  const char *input = NULL; /**< Path to the input file. */
  
  // Warning configuration options
  bool warnings_as_errors = false;
  bool disable_warnings = false;

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
    if (strcmp(argv[i], "--dev") == 0 || strcmp(argv[i], "--no-compile") == 0) {
      dev_mode = true;
      continue;
    }
    if (strcmp(argv[i], "-Werror") == 0 || strcmp(argv[i], "--warnings-as-errors") == 0) {
      warnings_as_errors = true;
      continue;
    }
    if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--no-warnings") == 0) {
      disable_warnings = true;
      continue;
    }
    if (strcmp(argv[i], "--multi-file") == 0) {
      multi_file = true;
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
  
  // Apply warning configuration from command line
  p.warn_config.warnings_as_errors = warnings_as_errors;
  p.warn_config.disable_all_warnings = disable_warnings;
  
  Node *root = parse_program(&p);
  
  // Run warning analysis on the parsed AST
  analyze_warnings(&p, root);
  
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
    dr_mkdir("build/libs");
    
    // Copy runtime headers to build/libs for distribution
    const char *headers[][2] = {
      {"io/console.h", "console.h"},
      {"memory/memory.h", "memory.h"},
      {"extensions/custom.h", "custom.h"},
      {"system/task.h", "task.h"},
      {"exceptions/exception.h", "exception.h"}
    };
    for (size_t i = 0; i < sizeof(headers)/sizeof(headers[0]); i++) {
      char src_path[256], dst_path[256];
      snprintf(src_path, sizeof(src_path), "src%cruntime%c%s", DR_PATH_SEP, DR_PATH_SEP, headers[i][0]);
      snprintf(dst_path, sizeof(dst_path), "build%clibs%c%s", DR_PATH_SEP, DR_PATH_SEP, headers[i][1]);
      
      FILE *src = fopen(src_path, "r");
      FILE *dst = fopen(dst_path, "w");
      if (src && dst) {
        char buffer[4096];
        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
          fwrite(buffer, 1, bytes, dst);
        }
        fclose(src);
        fclose(dst);
      } else {
        if (src) fclose(src);
        if (dst) fclose(dst);
        fprintf(stderr, "Failed to copy header %s\n", headers[i]);
      }
    }
    
    FILE *out = fopen("build/bin/dream.c", "w");
    if (!out) {
      perror("fopen");
      return 1;
    }
    codegen_emit_c(root, out, input);
    fclose(out);
    
    // In dev mode, just generate C code and exit
    if (dev_mode) {
      printf("C code generated successfully at build/bin/dream.c\n");
      return 0;
    }
    
    const char *cc = getenv("CC");
    if (!cc) {
#ifdef _WIN32
      cc = "zig cc";  // Use zig cc on Windows to avoid bash issues
#else
      cc = "gcc";
#endif
    }
    char cmd[256];
    int res;
    const char *optflag =
        opt_level >= 3 ? "-O3" : (opt_level >= 2 ? "-O2" : "");

    // Use pre-built runtime library instead of compiling individual files
    // The zig build system already creates zig-out/lib/dreamrt.lib

    // Link with pre-built runtime library and sanitizer runtime
    snprintf(cmd, sizeof(cmd),
             "%s -g %s -Isrc/runtime -Isrc/runtime/memory -Isrc/runtime/io -Isrc/runtime/extensions -Isrc/runtime/system -Isrc/runtime/exceptions build/bin/dream.c -Lzig-out/lib -ldreamrt -fsanitize=undefined -o %s",
             cc, optflag, DR_EXE_NAME);
    res = system(cmd);
    if (res != 0) {
      fprintf(stderr, "failed to run: %s\n", cmd);
      return 1;
    }
  } else if (emit_obj) {
    codegen_emit_obj(root, "a.o", input);
  }

  free(src);
  free(p.diags.data);
  sem_analyzer_free(&sem);
  free(arena.ptr);
  return 0;
}
