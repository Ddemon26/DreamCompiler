#include "codegen.h"
#include "../../codegen/c_emit.h"
#include <stdio.h>

void codegen_emit_c(Node *root, FILE *out) {
    (void)root;
    COut builder;
    c_out_init(&builder);
    c_out_write(&builder, "// TODO: implement code generation\n");
    c_out_dump(out, &builder);
    c_out_free(&builder);
}

void codegen_emit_obj(Node *root, const char *path) {
    (void)root;
    (void)path;
    fprintf(stderr, "--emit-obj not implemented\n");
}

