#include "src/lexer/lexer.h"
#include <stdio.h>
#include <string.h>

int main() {
    Lexer lx;
    lexer_init(&lx, "const int x = 42;");
    
    Token tok;
    while ((tok = lexer_next(&lx)).kind != TK_EOF) {
        printf("Token: %s, Value: '%.*s'\n", 
               token_kind_name(tok.kind), 
               (int)tok.len, 
               tok.start);
    }
    
    return 0;
}