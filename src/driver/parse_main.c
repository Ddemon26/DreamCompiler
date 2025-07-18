#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../parser/diagnostic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*-----------------------------------------------------------------------------
 * Symbol JSON Emission Helpers
 *---------------------------------------------------------------------------*/

static void print_symbol(const char *name, Pos pos, const char *kind, int *first) {
    if (!name) return;
    if (!*first) printf(",");
    printf("{\"name\":\"%s\",\"line\":%zu,\"character\":%zu,\"kind\":\"%s\"}",
           name, pos.line, pos.column, kind);
    *first = 0;
}

static void collect_symbols(Node *n, int *first) {
    if (!n) return;
    switch (n->kind) {
    case ND_VAR_DECL: {
        char buf[128];
        size_t len = n->as.var_decl.name.len;
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        memcpy(buf, n->as.var_decl.name.start, len);
        buf[len] = 0;
        print_symbol(buf, n->pos, "var", first);
        break;
    }
    case ND_FUNC: {
        char buf[128];
        size_t len = n->as.func.name.len;
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        memcpy(buf, n->as.func.name.start, len);
        buf[len] = 0;
        print_symbol(buf, n->pos, "func", first);
        collect_symbols(n->as.func.body, first);
        break;
    }
    case ND_STRUCT_DECL: {
        char buf[128];
        size_t len = n->as.type_decl.name.len;
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        memcpy(buf, n->as.type_decl.name.start, len);
        buf[len] = 0;
        print_symbol(buf, n->pos, "struct", first);
        for (size_t i = 0; i < n->as.type_decl.len; ++i)
            collect_symbols(n->as.type_decl.members[i], first);
        break;
    }
    case ND_CLASS_DECL: {
        char buf[128];
        size_t len = n->as.type_decl.name.len;
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        memcpy(buf, n->as.type_decl.name.start, len);
        buf[len] = 0;
        print_symbol(buf, n->pos, "class", first);
        for (size_t i = 0; i < n->as.type_decl.len; ++i)
            collect_symbols(n->as.type_decl.members[i], first);
        break;
    }
    case ND_BLOCK:
        for (size_t i = 0; i < n->as.block.len; ++i)
            collect_symbols(n->as.block.items[i], first);
        break;
    case ND_IF:
        collect_symbols(n->as.if_stmt.then_br, first);
        collect_symbols(n->as.if_stmt.else_br, first);
        break;
    case ND_WHILE:
        collect_symbols(n->as.while_stmt.body, first);
        break;
    case ND_DO_WHILE:
        collect_symbols(n->as.do_while_stmt.body, first);
        break;
    case ND_FOR:
        collect_symbols(n->as.for_stmt.init, first);
        collect_symbols(n->as.for_stmt.cond, first);
        collect_symbols(n->as.for_stmt.update, first);
        collect_symbols(n->as.for_stmt.body, first);
        break;
    case ND_EXPR_STMT:
        collect_symbols(n->as.expr_stmt.expr, first);
        break;
    case ND_SWITCH:
        for (size_t i = 0; i < n->as.switch_stmt.len; ++i)
            collect_symbols(n->as.switch_stmt.cases[i].body, first);
        break;
    case ND_CALL:
        for (size_t i = 0; i < n->as.call.len; ++i)
            collect_symbols(n->as.call.args[i], first);
        break;
    default:
        break;
    }
}

/**
 * @brief Reads the content of a file into a dynamically allocated buffer.
 *
 * @param path Path to the file to be read.
 * @return Pointer to the buffer containing the file content, or NULL on failure.
 */
static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(len + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    fread(buf, 1, len, f);
    buf[len] = 0;
    fclose(f);
    return buf;
}

/**
 * @brief Entry point for the parser driver program.
 *
 * Parses the specified source file, prints diagnostics, and cleans up resources.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return Exit status of the program.
 */
int main(int argc, char **argv) {
    const char *path = NULL;
    bool dump_symbols = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            diag_verbose = true;
            continue;
        }
        if (strcmp(argv[i], "--symbols") == 0) {
            dump_symbols = true;
            continue;
        }
        path = argv[i];
    }
    if (!path) {
        fprintf(stderr, "usage: %s [--verbose] [--symbols] file\n", argv[0]);
        return 1;
    }
    char *src = read_file(path);
    if (!src) {
        perror("read_file");
        return 1;
    }
    Arena arena;
    arena_init(&arena);
    Parser p;
    parser_init(&p, &arena, src);
    Node *root = parse_program(&p);
    if (dump_symbols) {
        int first = 1;
        printf("[");
        collect_symbols(root, &first);
        printf("]\n");
    }
    print_diagnostics(src, &p.diags);
    free(src);
    free(p.diags.data);
    free(arena.ptr);
    return 0;
}
