#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../parser/diagnostic.h"
#include <stdio.h>
#include <stdlib.h>

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
    if (argc < 2) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        return 1;
    }
    char *src = read_file(argv[1]);
    if (!src) {
        perror("read_file");
        return 1;
    }
    Arena arena;
    arena_init(&arena);
    Parser p;
    parser_init(&p, &arena, src);
    Node *root = parse_program(&p);
    (void)root;
    print_diagnostics(src, &p.diags);
    free(src);
    free(p.diags.data);
    free(arena.ptr);
    return 0;
}
