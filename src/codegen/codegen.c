#include "codegen.h"
#include "../util/platform.h"
#include "c_emit.h"
#include "context.h"
#include "expr.h"
#include "stmt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#endif

void codegen_emit_c(Node *root, FILE *out) {
  COut builder;
  c_out_init(&builder);

  c_out_write(&builder, "#ifndef DREAM_GENERATED\n#define DREAM_GENERATED\n");
  c_out_write(&builder, "#include <stdio.h>\n");
  c_out_write(&builder, "#include <string.h>\n");
  c_out_write(&builder, "#include <stdlib.h>\n");
  c_out_write(&builder, "#include <setjmp.h>\n");
  c_out_write(&builder, "#include \"console.h\"\n\n");

  c_out_write(&builder, "static jmp_buf dream_jmp_buf[16];\n");
  c_out_write(&builder, "static int dream_jmp_top = -1;\n");
  c_out_write(
      &builder,
      "static void "
      "dream_throw(void){longjmp(dream_jmp_buf[dream_jmp_top],1);}\n\n");

  c_out_write(&builder,
              "static char *dream_concat(const char *a,const char *b){\n");
  c_out_write(&builder, "    const size_t la=strlen(a);\n");
  c_out_write(&builder, "    const size_t lb=strlen(b);\n");
  c_out_write(&builder, "    char *r=malloc(la+lb+1);\n");
  c_out_write(&builder, "    memcpy(r,a,la);\n");
  c_out_write(&builder, "    memcpy(r+la,b,lb);\n");
  c_out_write(&builder, "    r[la+lb]=0;\n    return r;\n}\n\n");

  c_out_write(&builder, "static char *dream_readline(void){\n");
  c_out_write(&builder, "    static char buf[256];\n");
  c_out_write(&builder, "    if(!fgets(buf,sizeof buf,stdin)) return NULL;\n");
  c_out_write(&builder, "    size_t len=strlen(buf);\n");
  c_out_write(&builder, "    if(len && buf[len-1]=='\\n') buf[len-1]=0;\n");
  c_out_write(&builder, "    return buf;\n}\n\n");

  for (size_t i = 0; i < root->as.block.len; i++) {
    Node *it = root->as.block.items[i];
    if (it->kind == ND_STRUCT_DECL || it->kind == ND_CLASS_DECL)
      emit_type_decl(&builder, it);
  }
  for (size_t i = 0; i < root->as.block.len; i++) {
    Node *it = root->as.block.items[i];
    if (it->kind == ND_FUNC)
      emit_func(&builder, it);
  }

  c_out_write(&builder, "int main(void){\n");
  c_out_indent(&builder);
  CGCtx ctx = {0};
  ctx.ret_type = TK_KW_INT;
  cgctx_scope_enter(&ctx);
  for (size_t i = 0; i < root->as.block.len; i++) {
    Node *it = root->as.block.items[i];
    if (it->kind != ND_FUNC)
      cg_emit_stmt(&ctx, &builder, it);
  }
  cgctx_scope_leave(&ctx);
  free(ctx.vars);
  c_out_write(&builder, "return 0;\n");
  c_out_dedent(&builder);
  c_out_write(&builder, "}\n");
  c_out_write(&builder, "#endif /* DREAM_GENERATED */\n");

  c_out_dump(out, &builder);
  c_out_free(&builder);
}

void codegen_emit_obj(Node *root, const char *path) {
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
  codegen_emit_c(root, f);
  fclose(f);
  char cmd[512];
  snprintf(cmd, sizeof(cmd), "zig cc -std=c11 -c %s -o %s", tmp, path);
  int res = system(cmd);
  if (res != 0)
    fprintf(stderr, "failed to run: %s\n", cmd);
  dr_unlink(tmp);
}
