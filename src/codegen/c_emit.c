#include "../../codegen/c_emit.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static void grow(COut *out, size_t needed) {
    if (out->len + needed <= out->cap) return;
    size_t new_cap = out->cap ? out->cap * 2 : 1024;
    while (new_cap < out->len + needed) new_cap *= 2;
    out->data = realloc(out->data, new_cap);
    out->cap = new_cap;
}

void c_out_init(COut *out) {
    out->data = NULL;
    out->len = 0;
    out->cap = 0;
    out->indent = 0;
    out->indent_width = 4;
    out->at_line_start = 1;
}

void c_out_free(COut *out) { free(out->data); }

void c_out_indent(COut *out) { out->indent++; }

void c_out_dedent(COut *out) { if (out->indent > 0) out->indent--; }

void c_out_newline(COut *out) {
    grow(out, 2 + out->indent * out->indent_width);
    out->data[out->len++] = '\n';
    out->at_line_start = 1;
}

void c_out_write(COut *out, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char buf[512];
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n < 0) return;
    size_t needed = (size_t)n + 1 + (out->at_line_start ? out->indent * out->indent_width : 0);
    grow(out, needed);
    if (out->at_line_start) {
        for (int i = 0; i < out->indent * out->indent_width; i++)
            out->data[out->len++] = ' ';
        out->at_line_start = 0;
    }
    memcpy(out->data + out->len, buf, (size_t)n);
    out->len += (size_t)n;
    out->data[out->len] = '\0';
}

void c_out_dump(FILE *f, COut *out) {
    fwrite(out->data, 1, out->len, f);
}

static size_t find_decl(CDecl *decls, size_t ndecls, const char *name) {
    for (size_t i = 0; i < ndecls; i++) {
        if (strcmp(decls[i].name, name) == 0) return i;
    }
    return (size_t)-1;
}

static void visit(size_t idx, CDecl *decls, size_t ndecls,
                  int *perm, int *temp, size_t *order, size_t *pos) {
    if (perm[idx]) return;
    if (temp[idx]) return; // ignore cycles
    temp[idx] = 1;
    CDecl *d = &decls[idx];
    for (size_t i = 0; i < d->ndeps; i++) {
        size_t j = find_decl(decls, ndecls, d->deps[i]);
        if (j != (size_t)-1)
            visit(j, decls, ndecls, perm, temp, order, pos);
    }
    perm[idx] = 1;
    order[(*pos)++] = idx;
}

void cdecl_toposort(CDecl *decls, size_t ndecls, size_t *order) {
    int *perm = calloc(ndecls, sizeof(int));
    int *temp = calloc(ndecls, sizeof(int));
    size_t pos = 0;
    for (size_t i = 0; i < ndecls; i++)
        visit(i, decls, ndecls, perm, temp, order, &pos);
    free(perm);
    free(temp);
}

char *c_mangle(const char *base, const char **types, size_t ntypes) {
    size_t len = strlen(base);
    for (size_t i = 0; i < ntypes; i++) len += 2 + strlen(types[i]);
    char *res = malloc(len + 1);
    char *p = stpcpy(res, base);
    for (size_t i = 0; i < ntypes; i++) {
        *p++ = '_'; *p++ = '_';
        const char *t = types[i];
        while (*t) {
            char c = *t++;
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9')) {
                *p++ = c;
            } else {
                *p++ = '_';
            }
        }
    }
    *p = '\0';
    return res;
}

