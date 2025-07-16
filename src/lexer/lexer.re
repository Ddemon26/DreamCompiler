#include "lexer.h"
#include <string.h>

#define YYCTYPE char
#define YYCURSOR lx->cursor
#define YYLIMIT  lx->limit
#define YYMARKER lx->marker
#define YYCTXMARKER lx->ctx
#define YYPEEK() (*YYCURSOR)
#define YYSKIP() (++YYCURSOR)
#define YYBACKUP() (YYMARKER = YYCURSOR)
#define YYRESTORE() (YYCURSOR = YYMARKER)
#define YYLESSTHAN(n) (YYLIMIT - YYCURSOR < (n))
#define YYFILL(n) return make_token(lx, TK_EOF, YYCURSOR, 0)
#define YYGETCONDITION() ((int)lx->state)
#define YYSETCONDITION(s) (lx->state = (StartCondition)(s))

static void advance_pos(Lexer *lx, const char *start, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (start[i] == '\n') {
            lx->pos.line++;
            lx->pos.column = 1;
        } else {
            lx->pos.column++;
        }
    }
}

static Token make_token(Lexer *lx, TokenKind kind, const char *start, size_t len) {
    Token t = { kind, start, len, lx->pos };
    advance_pos(lx, start, len);
    return t;
}

static Token lex_raw(Lexer *lx);
Token lexer_next(Lexer *lx) {
    if (lx->has_peek) { lx->has_peek = false; return lx->lookahead; }
    return lex_raw(lx);
}
Token lexer_peek(Lexer *lx) {
    if (!lx->has_peek) { lx->lookahead = lex_raw(lx); lx->has_peek = true; }
    return lx->lookahead;
}

void lexer_init(Lexer *lx, const char *src) {
    lx->src = src;
    lx->cursor = src;
    lx->marker = src;
    lx->ctx = src;
    lx->limit = src + strlen(src);
    lx->state = SC_NORMAL;
    lx->pos.line = 1;
    lx->pos.column = 1;
    lx->has_peek = false;
}

static const char *token_names[] = {
#define TOKEN(k, r) #k,
#include "tokens.def"
};
#undef TOKEN
const char *token_kind_name(TokenKind k) { return token_names[k]; }

static Token lex_raw(Lexer *lx) {
    const char *tok_start;
    /*!re2c
        re2c:api = custom;
        re2c:define:YYCTYPE = "char";
        re2c:define:YYCURSOR = "lx->cursor";
        re2c:define:YYMARKER = "lx->marker";
        re2c:define:YYLIMIT  = "lx->limit";
        re2c:define:YYCTXMARKER = "lx->ctx";
        /* use default YYGETCONDITION/YYSETCONDITION macros */
        re2c:yyfill:enable = 0;
    */
    for (;;) {
        tok_start = lx->cursor;
        /*!re2c
        <SC_NORMAL> "/*" { YYSETCONDITION(SC_COMMENT); continue; }
        <SC_COMMENT> "*/" { YYSETCONDITION(SC_NORMAL); tok_start = lx->cursor; continue; }
        <SC_COMMENT> "\n" { lx->pos.line++; lx->pos.column = 1; tok_start = lx->cursor; continue; }
        <SC_COMMENT> [^] { tok_start = lx->cursor; continue; }
        <SC_NORMAL> "\n" { advance_pos(lx, tok_start, 1); tok_start = lx->cursor; continue; }
        <SC_NORMAL> [ \t\r]+ { advance_pos(lx, tok_start, lx->cursor - tok_start); tok_start = lx->cursor; continue; }
        <SC_NORMAL> "//" [^\n]* { advance_pos(lx, tok_start, lx->cursor - tok_start); tok_start = lx->cursor; continue; }
        <SC_NORMAL> "\"" { YYSETCONDITION(SC_STRING); tok_start = lx->cursor; continue; }
        <SC_STRING> "\"" { YYSETCONDITION(SC_NORMAL); return make_token(lx, TK_STRING_LITERAL, tok_start, lx->cursor - tok_start - 1); }
        <SC_STRING> "\\" . { continue; }
        <SC_STRING> "\n" { advance_pos(lx, tok_start, 1); tok_start = lx->cursor; continue; }
        <SC_STRING> [^] { continue; }
        <SC_NORMAL> "[0-9]+" { return make_token(lx, TK_INT_LITERAL, tok_start, lx->cursor - tok_start); }
        <SC_NORMAL> "[0-9]+\.[0-9]+" { return make_token(lx, TK_FLOAT_LITERAL, tok_start, lx->cursor - tok_start); }
        <SC_NORMAL> "'([^'\\n]|\\\\.)'" { return make_token(lx, TK_CHAR_LITERAL, tok_start, lx->cursor - tok_start); }
        <SC_NORMAL> "[a-zA-Z_][a-zA-Z0-9_]*" {
            size_t len = lx->cursor - tok_start;
#define TOKEN(k,r) if (len == sizeof(r)-1 && memcmp(tok_start, r, len) == 0) return make_token(lx, TK_##k, tok_start, len);
#include "tokens.def"
#undef TOKEN
            return make_token(lx, TK_IDENT, tok_start, len);
        }
        <SC_NORMAL> "++" { return make_token(lx, TK_PLUSPLUS, tok_start, 2); }
        <SC_NORMAL> "--" { return make_token(lx, TK_MINUSMINUS, tok_start, 2); }
        <SC_NORMAL> "+=" { return make_token(lx, TK_PLUSEQ, tok_start, 2); }
        <SC_NORMAL> "-=" { return make_token(lx, TK_MINUSEQ, tok_start, 2); }
        <SC_NORMAL> "*=" { return make_token(lx, TK_STAREQ, tok_start, 2); }
        <SC_NORMAL> "/=" { return make_token(lx, TK_SLASHEQ, tok_start, 2); }
        <SC_NORMAL> "%=" { return make_token(lx, TK_PERCENTEQ, tok_start, 2); }
        <SC_NORMAL> "+" { return make_token(lx, TK_PLUS, tok_start, 1); }
        <SC_NORMAL> "-" { return make_token(lx, TK_MINUS, tok_start, 1); }
        <SC_NORMAL> "*" { return make_token(lx, TK_STAR, tok_start, 1); }
        <SC_NORMAL> "/" { return make_token(lx, TK_SLASH, tok_start, 1); }
        <SC_NORMAL> "%" { return make_token(lx, TK_PERCENT, tok_start, 1); }
        <SC_NORMAL> "^" { return make_token(lx, TK_CARET, tok_start, 1); }
        <SC_NORMAL> "<<" { return make_token(lx, TK_LSHIFT, tok_start, 2); }
        <SC_NORMAL> ">>" { return make_token(lx, TK_RSHIFT, tok_start, 2); }
        <SC_NORMAL> "<=" { return make_token(lx, TK_LTEQ, tok_start, 2); }
        <SC_NORMAL> ">=" { return make_token(lx, TK_GTEQ, tok_start, 2); }
        <SC_NORMAL> "==" { return make_token(lx, TK_EQEQ, tok_start, 2); }
        <SC_NORMAL> "!=" { return make_token(lx, TK_NEQ, tok_start, 2); }
        <SC_NORMAL> "<" { return make_token(lx, TK_LT, tok_start, 1); }
        <SC_NORMAL> ">" { return make_token(lx, TK_GT, tok_start, 1); }
        <SC_NORMAL> "&&" { return make_token(lx, TK_ANDAND, tok_start, 2); }
        <SC_NORMAL> "||" { return make_token(lx, TK_OROR, tok_start, 2); }
        <SC_NORMAL> "&" { return make_token(lx, TK_AND, tok_start, 1); }
        <SC_NORMAL> "|" { return make_token(lx, TK_OR, tok_start, 1); }
        <SC_NORMAL> "~" { return make_token(lx, TK_TILDE, tok_start, 1); }
        <SC_NORMAL> "!" { return make_token(lx, TK_BANG, tok_start, 1); }
        <SC_NORMAL> "=" { return make_token(lx, TK_EQ, tok_start, 1); }
        <SC_NORMAL> "?" { return make_token(lx, TK_QUESTION, tok_start, 1); }
        <SC_NORMAL> ":" { return make_token(lx, TK_COLON, tok_start, 1); }
        <SC_NORMAL> ";" { return make_token(lx, TK_SEMICOLON, tok_start, 1); }
        <SC_NORMAL> "," { return make_token(lx, TK_COMMA, tok_start, 1); }
        <SC_NORMAL> "." { return make_token(lx, TK_DOT, tok_start, 1); }
        <SC_NORMAL> "(" { return make_token(lx, TK_LPAREN, tok_start, 1); }
        <SC_NORMAL> ")" { return make_token(lx, TK_RPAREN, tok_start, 1); }
        <SC_NORMAL> "{" { return make_token(lx, TK_LBRACE, tok_start, 1); }
        <SC_NORMAL> "}" { return make_token(lx, TK_RBRACE, tok_start, 1); }
        <SC_NORMAL> "[" { return make_token(lx, TK_LBRACKET, tok_start, 1); }
        <SC_NORMAL> "]" { return make_token(lx, TK_RBRACKET, tok_start, 1); }
        <SC_NORMAL> [^] { return make_token(lx, TK_ERROR, tok_start, 1); }
        */
    }
}
