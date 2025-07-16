#include "token.h"
#include "dfa_table.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *src = "var x = 1 + 2 * 3;";
    Lexer lx;
    init_lexer(&lx, src, strlen(src));

    Token t;
    t = next_token(&lx); // var
    assert(t.kind == TOKEN_VAR);
    t = next_token(&lx); // x
    assert(t.kind == TOKEN_IDENTIFIER);
    t = next_token(&lx); // =
    assert(t.kind == TOKEN_EQUAL);
    t = next_token(&lx); // 1
    assert(t.kind == TOKEN_INT_LITERAL);
    t = next_token(&lx); // +
    assert(t.kind == TOKEN_PLUS);
    t = next_token(&lx); // 2
    assert(t.kind == TOKEN_INT_LITERAL);
    t = next_token(&lx); // *
    assert(t.kind == TOKEN_STAR);
    t = next_token(&lx); // 3
    assert(t.kind == TOKEN_INT_LITERAL);
    t = next_token(&lx); // ;
    assert(t.kind == TOKEN_SEMICOLON);
    t = next_token(&lx);
    assert(t.kind == TOKEN_EOF);
    printf("ok\n");
    return 0;
}
