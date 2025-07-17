#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../codegen/codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../opt/pipeline.h"
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

int main(int argc, char *argv[]) {
    bool opt1 = false;
    bool emit_c = true;
    bool emit_obj = false;
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
    }

    run_pipeline(NULL, opt1);

    if (emit_c) {
        codegen_emit_c(NULL, stdout);
    } else if (emit_obj) {
        codegen_emit_obj(NULL, "a.o");
    }

    return 0;
}
