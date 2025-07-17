#ifndef C_EMIT_H
#define C_EMIT_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Indentation aware string builder */
typedef struct {
    char *data;
    size_t len;
    size_t cap;
    int indent;
    int indent_width;
    int at_line_start;
} COut;

void c_out_init(COut *out);
void c_out_free(COut *out);
void c_out_indent(COut *out);
void c_out_dedent(COut *out);
void c_out_write(COut *out, const char *fmt, ...);
void c_out_newline(COut *out);
void c_out_dump(FILE *f, COut *out);

/* Generic name mangling */
char *c_mangle(const char *base, const char **types, size_t ntypes);

typedef struct CDecl CDecl;
struct CDecl {
    const char *name;
    const char **deps;
    size_t ndeps;
};

void cdecl_toposort(CDecl *decls, size_t ndecls, size_t *order);

#ifdef __cplusplus
}
#endif

#endif
