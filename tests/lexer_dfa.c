#include "src/lexer/lexer.h"
#include <assert.h>
#include <stdio.h>

int main() {
    const char *src = "";
    size_t pos = 0;
    Token tok = next_token(src, &pos);
    assert(tok.type == TOKEN_EOF);
    printf("ok\n");
    return 0;
}
