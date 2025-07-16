#include "token.h"
#include "dfa_table.h"
#include <ctype.h>
#include <string.h>

void init_lexer(Lexer *lx, const char *src, size_t len) {
    lx->src = src;
    lx->len = len;
    lx->pos = 0;
}

static int keyword_lookup(const char *start, size_t len) {
    static const struct { const char *kw; TokenKind kind; } table[] = {
        {"func", TOKEN_FUNC},
        {"if", TOKEN_IF},
        {"var", TOKEN_VAR},
        {"while", TOKEN_WHILE},
    };
    int lo = 0;
    int hi = (int)(sizeof(table) / sizeof(table[0])) - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        int cmp = strncmp(start, table[mid].kw, len);
        if (cmp == 0 && table[mid].kw[len] == '\0') return table[mid].kind;
        if (cmp < 0) hi = mid - 1; else lo = mid + 1;
    }
    return TOKEN_IDENTIFIER;
}

Token next_token(Lexer *lx) {
    const char *src = lx->src;
    size_t len = lx->len;
    size_t pos = lx->pos;

    while (pos < len && isspace((unsigned char)src[pos])) pos++;
    if (pos >= len) {
        lx->pos = pos;
        return (Token){TOKEN_EOF, pos, 0};
    }

    size_t start = pos;
    uint16_t state = DFA_START_STATE;
    int16_t last_accept = -1;
    size_t last_pos = pos;

    while (pos < len) {
        unsigned char c = (unsigned char)src[pos];
        uint16_t next = DFA_STATES[state][c];
        if (next == DFA_INVALID_STATE) break;
        state = next;
        pos++;
        if (DFA_ACCEPTS[state] != -1) {
            last_accept = DFA_ACCEPTS[state];
            last_pos = pos;
        }
    }

    if (last_accept == -1) {
        lx->pos = pos + 1;
        return (Token){TOKEN_UNKNOWN, start, 1};
    }

    lx->pos = last_pos;
    TokenKind kind = (TokenKind)last_accept;
    size_t tok_len = last_pos - start;
    if (kind == TOKEN_IDENTIFIER) {
        kind = keyword_lookup(src + start, tok_len);
    }
    return (Token){kind, start, tok_len};
}
