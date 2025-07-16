#include "lexer.h"
#include "dfa_table.h"
// Manual lexer removed; see tools/lexgen.zig for generator
#include <stddef.h>
#include <stdint.h>

// TODO: implement tokenization via DFA table
// Placeholder next_token implementation for compatibility
Token next_token(const char* src, size_t* pos) {
    Token token = {TOKEN_EOF, 0};
    (void)src;
    (void)pos;
    return token;
}
