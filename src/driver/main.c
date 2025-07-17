#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../codegen/codegen.h"
#include "../parser/diagnostic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../opt/pipeline.h"
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

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

int main(int argc, char *argv[]) {
    bool opt1 = false;
    bool emit_c = true;
    bool emit_obj = false;
    const char *input = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-O1") == 0 || strcmp(argv[i], "--O1") == 0) {
            opt1 = true;
            continue;
        }
        if (strcmp(argv[i], "--emit-c") == 0) {
            emit_c = true;
            emit_obj = false;
            continue;
        }
        if (strcmp(argv[i], "--emit-obj") == 0) {
            emit_obj = true;
            emit_c = false;
            continue;
        }
        input = argv[i];
    }

    if (!input) {
        fprintf(stderr, "usage: %s [options] file\n", argv[0]);
        return 1;
    }
    char *src = read_file(input);
    if (!src) {
        perror("read_file");
        return 1;
    }

    Arena arena; arena_init(&arena);
    Parser p; parser_init(&p, &arena, src);
    Node *root = parse_program(&p);
    print_diagnostics(src, &p.diags);

    run_pipeline(NULL, opt1);

    if (emit_c) {
#ifdef _WIN32
        _mkdir("build");
        _mkdir("build/bin");
#else
        mkdir("build", 0755);
        mkdir("build/bin", 0755);
#endif
        FILE *out = fopen("build/bin/dream.c", "w");
        if (!out) {
            perror("fopen");
            return 1;
        }
        codegen_emit_c(root, out);
        fclose(out);
        const char *cc = getenv("CC");
        if (!cc)
            cc = "zig cc";
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "%s build/bin/dream.c -o dream", cc);
        int res = system(cmd);
        if (res != 0)
            fprintf(stderr, "failed to run: %s\n", cmd);
    } else if (emit_obj) {
        codegen_emit_obj(root, "a.o");
    }

    free(src);
    free(p.diags.data);
    free(arena.ptr);
    return 0;
}
