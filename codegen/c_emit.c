#include "c_emit.h"
#include "../src/codegen/codegen.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void cemit_init(CEmitter *e, FILE *out) {
    e->out = out;
    e->indent = 0;
}

void cemit_indent(CEmitter *e) { e->indent += 4; }
void cemit_dedent(CEmitter *e) { if (e->indent >= 4) e->indent -= 4; }

static void cemit_write_indent(CEmitter *e) {
    for (int i = 0; i < e->indent; ++i)
        fputc(' ', e->out);
}

void cemit_line(CEmitter *e, const char *fmt, ...) {
    cemit_write_indent(e);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(e->out, fmt, ap);
    va_end(ap);
}

char *mangle_specialisation(const char *base, const char **types, int count) {
    size_t len = strlen(base);
    for (int i = 0; i < count; ++i)
        len += 2 + strlen(types[i]);
    char *out = malloc(len + 1);
    strcpy(out, base);
    for (int i = 0; i < count; ++i) {
        strcat(out, "__");
        strcat(out, types[i]);
    }
    return out;
}

/* collect all function calls in a node */
static void collect_calls(Node *n, char ***arr, int *len, int *cap) {
    if (!n) return;
    if (n->type == NODE_FUNC_CALL && n->value) {
        if (*len >= *cap) {
            *cap = *cap ? *cap * 2 : 4;
            *arr = realloc(*arr, *cap * sizeof(char *));
        }
        (*arr)[(*len)++] = n->value;
    }
    collect_calls(n->left, arr, len, cap);
    collect_calls(n->right, arr, len, cap);
    collect_calls(n->else_branch, arr, len, cap);
}

typedef struct {
    Node *node;
    char **deps;
    int dep_count;
    int emitted;
    int visiting;
} FuncInfo;

static FuncInfo *find_func(FuncInfo *fns, int count, const char *name) {
    for (int i = 0; i < count; ++i)
        if (strcmp(fns[i].node->value, name) == 0)
            return &fns[i];
    return NULL;
}

static void emit_func_sorted(Compiler *c, FuncInfo *fn, FuncInfo *all, int count) {
    if (!fn || fn->emitted) return;
    if (fn->visiting) return; /* cycle */
    fn->visiting = 1;
    for (int i = 0; i < fn->dep_count; ++i) {
        FuncInfo *dep = find_func(all, count, fn->deps[i]);
        if (dep) emit_func_sorted(c, dep, all, count);
    }
    generate_c_function(c, fn->node);
    fn->emitted = 1;
    fn->visiting = 0;
}

void emit_c_program(Node *program, FILE *out) {
    CEmitter em;
    cemit_init(&em, out);
    Compiler c = {out, NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0};

    cemit_line(&em, "#include <stdio.h>\n");
    cemit_line(&em, "#include <stdlib.h>\n");
    cemit_line(&em, "#include <string.h>\n");
    cemit_line(&em, "static char* dream_concat(const char* a, const char* b){\n");
    cemit_indent(&em);
    cemit_line(&em, "size_t len = strlen(a) + strlen(b);\n");
    cemit_line(&em, "char* s = malloc(len + 1);\n");
    cemit_line(&em, "strcpy(s, a);\n");
    cemit_line(&em, "strcat(s, b);\n");
    cemit_line(&em, "return s;\n");
    cemit_dedent(&em);
    cemit_line(&em, "}\n");

    /* classes first */
    for (Node *cur = program; cur; cur = cur->right) {
        if (cur->left && cur->left->type == NODE_CLASS_DEF)
            generate_c_class(&c, cur->left);
    }

    /* gather function definitions */
    int fn_count = 0;
    for (Node *cur = program; cur; cur = cur->right)
        if (cur->left && cur->left->type == NODE_FUNC_DEF)
            fn_count++;
    FuncInfo *fns = calloc(fn_count, sizeof(FuncInfo));
    int idx = 0;
    for (Node *cur = program; cur; cur = cur->right) {
        if (cur->left && cur->left->type == NODE_FUNC_DEF) {
            FuncInfo *fi = &fns[idx++];
            fi->node = cur->left;
            fi->deps = NULL;
            fi->dep_count = 0;
            fi->emitted = 0;
            fi->visiting = 0;
            collect_calls(cur->left->left, &fi->deps, &fi->dep_count, &(int){0});
        }
    }
    /* emit functions in sorted order */
    for (int i = 0; i < fn_count; ++i)
        emit_func_sorted(&c, &fns[i], fns, fn_count);

    cemit_line(&em, "int main(){\n");
    cemit_indent(&em);
    for (Node *cur = program; cur; cur = cur->right) {
        if (cur->left && cur->left->type != NODE_FUNC_DEF && cur->left->type != NODE_CLASS_DEF)
            generate_c(&c, cur->left);
    }
    cemit_line(&em, "return 0;\n");
    cemit_dedent(&em);
    cemit_line(&em, "}\n");

    for (int i = 0; i < fn_count; ++i)
        free(fns[i].deps);
    free(fns);
}
