#include "codegen.h"
#include "../util/platform.h"
#include "c_emit.h"
#include "context.h"
#include "expr.h"
#include "stmt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#ifdef _WIN32
#include <io.h>
#endif

// Check if the AST contains any async functions
static bool has_async_functions(Node *n) {
  if (!n) return false;
  
  if (n->kind == ND_FUNC && n->as.func.is_async) {
    return true;
  }
  
  // Check children based on node type
  switch (n->kind) {
    case ND_BLOCK:
      for (size_t i = 0; i < n->as.block.len; i++) {
        if (has_async_functions(n->as.block.items[i])) {
          return true;
        }
      }
      break;
    case ND_CLASS_DECL:
    case ND_STRUCT_DECL:
      for (size_t i = 0; i < n->as.type_decl.len; i++) {
        if (has_async_functions(n->as.type_decl.members[i])) {
          return true;
        }
      }
      break;
    case ND_ENUM_DECL:
      // Enums don't have async functions
      break;
    case ND_FUNC:
      if (has_async_functions(n->as.func.body)) {
        return true;
      }
      break;
    default:
      break;
  }
  
  return false;
}

void codegen_emit_c(Node *root, FILE *out, const char *src_file) {
  COut builder;
  c_out_init(&builder);

  c_out_write(&builder, "#ifndef DREAM_GENERATED\n#define DREAM_GENERATED\n");
  c_out_write(&builder, "#include <stdio.h>\n");
  c_out_write(&builder, "#include <string.h>\n");
  c_out_write(&builder, "#include <stdlib.h>\n");
  c_out_write(&builder, "#include <setjmp.h>\n");

  // Include runtime headers from build/libs for distribution
  c_out_write(&builder, "#include \"../libs/console.h\"\n");
  c_out_write(&builder, "#include \"../libs/custom.h\"\n");
  c_out_write(&builder, "#include \"../libs/memory.h\"\n");
  c_out_write(&builder, "#include \"../libs/exception.h\"\n");
  
  // Only include task.h when async functions are used
  if (has_async_functions(root)) {
    c_out_write(&builder, "#include \"../libs/task.h\"\n");
  }
  c_out_newline(&builder);

  // Initialize exception handling system
  c_out_write(&builder, "static void dream_init_runtime(void) {\n");
  c_out_write(&builder, "    dream_exception_init();\n");
  c_out_write(&builder, "}\n\n");

  c_out_write(&builder, "static char *dream_readline(void){\n");
  c_out_write(&builder, "    char buf[256];\n");
  c_out_write(&builder, "    if(!fgets(buf,sizeof buf,stdin)) return NULL;\n");
  c_out_write(&builder, "    size_t len=strlen(buf);\n");
  c_out_write(&builder, "    if(len && buf[len-1]=='\\n') len--;\n");
  c_out_write(&builder, "    char *r=dr_alloc(len+1);\n");
  c_out_write(&builder, "    memcpy(r,buf,len);\n");
  c_out_write(&builder, "    r[len]=0;\n    return r;\n}\n\n");

  CGTypeInfo *tinfo = NULL;
  size_t tlen = 0, tcap = 0;
  for (size_t i = 0; i < root->as.block.len; i++) {
    Node *it = root->as.block.items[i];
    if (it->kind == ND_STRUCT_DECL || it->kind == ND_CLASS_DECL) {
      if (tlen + 1 > tcap) {
        tcap = tcap ? tcap * 2 : 4;
        tinfo = realloc(tinfo, tcap * sizeof(CGTypeInfo));
      }
      int has_init = 0;
      for (size_t j = 0; j < it->as.type_decl.len; j++) {
        Node *m = it->as.type_decl.members[j];
        if (m->kind == ND_FUNC && m->as.func.name.len == 4 &&
            strncmp(m->as.func.name.start, "init", 4) == 0)
          has_init = 1;
      }
      tinfo[tlen++] = (CGTypeInfo){it->as.type_decl.name,
                                   it->kind == ND_CLASS_DECL, has_init};
    }
  }
  cg_register_types(tinfo, tlen);

  char src_buf[512];
  size_t i = 0;
  for (; src_file[i] && i < sizeof(src_buf) - 1; i++) {
    src_buf[i] = src_file[i] == '\\' ? '/' : src_file[i];
  }
  src_buf[i] = 0;
  const char *src_norm = src_buf;
  c_out_write(&builder, "#line 1 \"%s\"\n", src_norm);

  for (size_t i = 0; i < root->as.block.len; i++) {
    Node *it = root->as.block.items[i];
    if (it->kind == ND_STRUCT_DECL || it->kind == ND_CLASS_DECL)
      emit_type_decl(&builder, it, src_norm);
    else if (it->kind == ND_ENUM_DECL)
      emit_enum_decl(&builder, it, src_norm);
  }
  for (size_t i = 0; i < root->as.block.len; i++) {
    Node *it = root->as.block.items[i];
    if (it->kind == ND_FUNC)
      emit_func(&builder, it, src_norm);
  }

  Slice main_class = {NULL, 0};
  int has_main = 0; /* 1 global, 2 static method */
  for (size_t i = 0; i < root->as.block.len && !has_main; i++) {
    Node *it = root->as.block.items[i];
    if (it->kind == ND_FUNC && it->as.func.name.len == 4 &&
        strncmp(it->as.func.name.start, "main", 4) == 0)
      has_main = 1;
    else if ((it->kind == ND_CLASS_DECL || it->kind == ND_STRUCT_DECL)) {
      for (size_t j = 0; j < it->as.type_decl.len && !has_main; j++) {
        Node *m = it->as.type_decl.members[j];
        if (m->kind == ND_FUNC && m->as.func.is_static &&
            m->as.func.name.len == 4 &&
            strncmp(m->as.func.name.start, "main", 4) == 0) {
          has_main = 2;
          main_class = it->as.type_decl.name;
        }
      }
    }
  }

  if (has_main != 1) {
    c_out_write(&builder, "int main(void){\n");
    c_out_indent(&builder);
    c_out_write(&builder, "dream_init_runtime();\n");
    if (has_main == 2) {
      c_out_write(&builder, "int r = %.*s_main();\n", (int)main_class.len,
                  main_class.start);
      c_out_write(&builder, "dr_release_all();\n");
      c_out_write(&builder, "return r;\n");
    } else {
      CGCtx ctx = {0};
      ctx.ret_type = TK_KW_INT;
      cgctx_scope_enter(&ctx);
      for (size_t i = 0; i < root->as.block.len; i++) {
        Node *it = root->as.block.items[i];
        if (it->kind != ND_FUNC)
          cg_emit_stmt(&ctx, &builder, it, src_norm);
      }
      cgctx_scope_leave(&ctx);
      free(ctx.vars);
      c_out_write(&builder, "dr_release_all();\nreturn 0;\n");
    }
    c_out_dedent(&builder);
    c_out_write(&builder, "}\n");
  }
  c_out_write(&builder, "#endif /* DREAM_GENERATED */\n");

  c_out_dump(out, &builder);
  c_out_free(&builder);
  free(tinfo);
  cg_register_types(NULL, 0);
}

void codegen_emit_obj(Node *root, const char *path, const char *src_file) {
#ifdef _WIN32
  char tmp[L_tmpnam] = {0};
  if (tmpnam_s(tmp, L_tmpnam) != 0) {
    perror("tmpnam_s");
    return;
  }
  strcat(tmp, ".c");
  FILE *f = fopen(tmp, "w");
  if (!f) {
    perror("fopen");
    return;
  }
#else
  char tmp[] = "/tmp/dreamXXXXXX.c";
  int fd = mkstemps(tmp, 2);
  if (fd == -1) {
    perror("mkstemps");
    return;
  }
  FILE *f = fdopen(fd, "w");
  if (!f) {
    perror("fdopen");
    close(fd);
    dr_unlink(tmp);
    return;
  }
#endif
  codegen_emit_c(root, f, src_file);
  fclose(f);
  char cmd[512];
  snprintf(cmd, sizeof(cmd), "zig cc -std=c11 -g -c \"%s\" -o \"%s\"", tmp,
           path);
  int res = system(cmd);
  if (res != 0) {
    fprintf(stderr, "failed to run: %s\n", cmd);
    dr_unlink(tmp);
    return;
  }
  dr_unlink(tmp);
}
