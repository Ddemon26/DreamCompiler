#ifndef C_EMIT_H
#define C_EMIT_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure for an indentation-aware string builder.
 */
typedef struct {
    char *data; /**< Pointer to the string data. */
    size_t len; /**< Current length of the string. */
    size_t cap; /**< Capacity of the string buffer. */
    int indent; /**< Current indentation level. */
    int indent_width; /**< Width of each indentation level. */
    int at_line_start; /**< Flag indicating if the builder is at the start of a line. */
} COut;

/**
 * @brief Initializes a COut string builder.
 * @param out Pointer to the COut structure to initialize.
 */
void c_out_init(COut *out);

/**
 * @brief Frees resources used by a COut string builder.
 * @param out Pointer to the COut structure to free.
 */
void c_out_free(COut *out);
/**
 * @brief Increases the indentation level of a COut string builder.
 * @param out Pointer to the COut structure.
 */
void c_out_indent(COut *out);

/**
 * @brief Decreases the indentation level of a COut string builder.
 * @param out Pointer to the COut structure.
 */
void c_out_dedent(COut *out);

/**
 * @brief Writes formatted text to a COut string builder.
 * @param out Pointer to the COut structure.
 * @param fmt Format string.
 * @param ... Additional arguments for the format string.
 */
void c_out_write(COut *out, const char *fmt, ...);

/**
 * @brief Writes a newline to a COut string builder.
 * @param out Pointer to the COut structure.
 */
void c_out_newline(COut *out);

/**
 * @brief Dumps the content of a COut string builder to a file.
 * @param f Pointer to the output file.
 * @param out Pointer to the COut structure.
 */
void c_out_dump(FILE *f, COut *out);

/**
 * @brief Performs generic name mangling.
 * @param base Base name to mangle.
 * @param types Array of type strings.
 * @param ntypes Number of types in the array.
 * @return Pointer to the mangled name.
 */
char *c_mangle(const char *base, const char **types, size_t ntypes);

/**
 * @brief Structure representing a C declaration.
 */
typedef struct CDecl CDecl;
struct CDecl {
    const char *name;
    const char **deps;
    size_t ndeps;
};

/**
 * @brief Performs topological sorting of C declarations.
 * @param decls Array of C declarations.
 * @param ndecls Number of declarations in the array.
 * @param order Output array for the sorted order.
 */
void cdecl_toposort(CDecl *decls, size_t ndecls, size_t *order);

#ifdef __cplusplus
}
#endif

#endif
