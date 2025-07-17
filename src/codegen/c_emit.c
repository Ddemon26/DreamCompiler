#include "c_emit.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/**
 * @brief Copies a string from the source to the destination.
 *
 * Provides a local implementation of `stpcpy` to ensure compatibility
 * across platforms where `stpcpy` is not available.
 *
 * @param dest Pointer to the destination buffer.
 * @param src Pointer to the source string.
 * @return Pointer to the null terminator in the destination buffer.
 */
static inline char *dream_stpcpy(char *dest, const char *src) {
    while ((*dest = *src) != '\0') {
        ++dest;
        ++src;
    }
    return dest;
}

/**
 * @brief Expands the capacity of the output buffer if needed.
 *
 * Ensures the buffer has enough capacity to accommodate the required size.
 *
 * @param out Pointer to the output buffer structure.
 * @param needed Additional size needed in the buffer.
 */
static void grow(COut *out, size_t needed) {
    if (out->len + needed <= out->cap) return;
    size_t new_cap = out->cap ? out->cap * 2 : 1024;
    while (new_cap < out->len + needed) new_cap *= 2;
    out->data = realloc(out->data, new_cap);
    out->cap = new_cap;
}

/**
 * @brief Initializes the output buffer structure.
 *
 * Sets up the initial state of the output buffer, including
 * data pointer, length, capacity, indentation level, and line start flag.
 *
 * @param out Pointer to the output buffer structure to initialize.
 */
void c_out_init(COut *out) {
    out->data = NULL;
    out->len = 0;
    out->cap = 0;
    out->indent = 0;
    out->indent_width = 4;
    out->at_line_start = 1;
}

/**
 * @brief Frees the memory allocated for the output buffer.
 *
 * Releases the memory used by the data pointer in the output buffer structure.
 *
 * @param out Pointer to the output buffer structure to free.
 */
void c_out_free(COut *out) { free(out->data); }

/**
 * @brief Increases the indentation level of the output buffer.
 *
 * Increments the current indentation level for the output buffer.
 *
 * @param out Pointer to the output buffer structure.
 */
void c_out_indent(COut *out) { out->indent++; }

/**
 * @brief Decreases the indentation level of the output buffer.
 *
 * Decrements the current indentation level if it is greater than zero.
 *
 * @param out Pointer to the output buffer structure.
 */
void c_out_dedent(COut *out) { if (out->indent > 0) out->indent--; }

/**
 * @brief Adds a newline to the output buffer.
 *
 * Appends a newline character to the buffer and adjusts the state
 * to indicate the start of a new line.
 *
 * @param out Pointer to the output buffer structure.
 */
void c_out_newline(COut *out) {
    grow(out, 2 + out->indent * out->indent_width);
    out->data[out->len++] = '\n';
    out->at_line_start = 1;
}

/**
 * @brief Writes formatted data to the output buffer.
 *
 * Formats the input string and appends it to the output buffer,
 * applying indentation if at the start of a new line.
 *
 * @param out Pointer to the output buffer structure.
 * @param fmt Format string for the data to write.
 * @param ... Additional arguments for the format string.
 */
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

/**
 * @brief Writes the contents of the output buffer to a file.
 *
 * Outputs the data stored in the buffer to the specified file stream.
 *
 * @param f Pointer to the file stream where the data will be written.
 * @param out Pointer to the output buffer structure containing the data.
 */
void c_out_dump(FILE *f, COut *out) {
    fwrite(out->data, 1, out->len, f);
}

/**
 * @brief Finds the index of a declaration by name.
 *
 * Searches through the list of declarations to find the one
 * that matches the specified name.
 *
 * @param decls Pointer to the array of declarations.
 * @param ndecls Number of declarations in the array.
 * @param name Name of the declaration to find.
 * @return Index of the matching declaration, or (size_t)-1 if not found.
 */
static size_t find_decl(CDecl *decls, size_t ndecls, const char *name) {
    for (size_t i = 0; i < ndecls; i++) {
        if (strcmp(decls[i].name, name) == 0) return i;
    }
    return (size_t)-1;
}

/**
 * @brief Visits a declaration and processes its dependencies.
 *
 * Performs a depth-first traversal of the dependency graph,
 * marking the current declaration as visited and adding it
 * to the topological order.
 *
 * @param idx Index of the current declaration.
 * @param decls Pointer to the array of declarations.
 * @param ndecls Number of declarations in the array.
 * @param perm Array indicating permanently marked declarations.
 * @param temp Array indicating temporarily marked declarations.
 * @param order Array to store the topological order.
 * @param pos Pointer to the current position in the order array.
 */
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

/**
 * @brief Performs a topological sort on a list of declarations.
 *
 * Sorts the declarations in topological order based on their dependencies.
 *
 * @param decls Pointer to the array of declarations.
 * @param ndecls Number of declarations in the array.
 * @param order Array to store the topological order of declarations.
 */
void cdecl_toposort(CDecl *decls, size_t ndecls, size_t *order) {
    int *perm = calloc(ndecls, sizeof(int));
    int *temp = calloc(ndecls, sizeof(int));
    size_t pos = 0;
    for (size_t i = 0; i < ndecls; i++)
        visit(i, decls, ndecls, perm, temp, order, &pos);
    free(perm);
    free(temp);
}

/**
 * @brief Generates a mangled name based on a base name and a list of types.
 *
 * Constructs a unique identifier by appending type information to the base name,
 * replacing non-alphanumeric characters in the types with underscores.
 *
 * @param base Base name for the mangled identifier.
 * @param types Array of type strings to append to the base name.
 * @param ntypes Number of type strings in the array.
 * @return Pointer to the newly allocated mangled name string.
 */
char *c_mangle(const char *base, const char **types, size_t ntypes) {
    size_t len = strlen(base);
    for (size_t i = 0; i < ntypes; i++) len += 2 + strlen(types[i]);
    char *res = malloc(len + 1);
    char *p = dream_stpcpy(res, base);
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

