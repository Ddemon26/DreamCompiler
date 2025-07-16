#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../codegen/codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../opt/pipeline.h"
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

int main(int argc, char *argv[]) {
    bool opt1 = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-O1") == 0 || strcmp(argv[i], "--O1") == 0)
            opt1 = true;
    }
    run_pipeline(NULL, opt1);
    return 0;
}
