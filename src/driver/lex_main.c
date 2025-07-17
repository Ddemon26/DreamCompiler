#include "../lexer/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Entry point for the lexer driver program.
 *
 * Reads a file specified as a command-line argument, tokenizes its content
 * using the lexer, and outputs the tokens in JSON format.
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
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen"); return 1; }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(len + 1);
    if (!buf) return 1;
    fread(buf, 1, len, f);
    buf[len] = 0;
    fclose(f);

    Lexer lx;
    lexer_init(&lx, buf);
    printf("[\n");
    int first = 1;
    Token t;
    while ((t = lexer_next(&lx)).kind != TK_EOF) {
        if (!first) printf(",\n");
        first = 0;
        printf("  {\"kind\":\"%s\",\"lexeme\":\"", token_kind_name(t.kind));
        for (size_t i=0;i<t.len;i++) {
            char c=t.start[i];
            if (c=='\\' || c=='\"') putchar('\\');
            if (c=='\n') { printf("\\n"); continue; }
            else if(c=='\r'){ printf("\\r"); continue; }
            else putchar(c);
        }
        printf("\",\"line\":%zu,\"col\":%zu}", t.pos.line, t.pos.column);
    }
    printf("\n]\n");
    return 0;
}
