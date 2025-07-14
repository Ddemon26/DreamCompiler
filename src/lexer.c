#include "dream.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
static char *strndup(const char *src, size_t n)
{
    size_t len = strlen(src);
    if (n < len) len = n;
    char *dest = malloc(len + 1);
    if (!dest) return NULL;
    memcpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}
#endif

Token next_token(Lexer *lexer)
{
    while (isspace((unsigned char)lexer->source[lexer->pos]))
        lexer->pos++;

    if (lexer->source[lexer->pos] == '/' && lexer->source[lexer->pos + 1] == '/') {
        while (lexer->source[lexer->pos] && lexer->source[lexer->pos] != '\n')
            lexer->pos++;
        return next_token(lexer);
    }

    Token token = {TOKEN_UNKNOWN, NULL};
    if (lexer->source[lexer->pos] == '\0') {
        token.type = TOKEN_EOF;
        return token;
    }

    if (isalpha((unsigned char)lexer->source[lexer->pos])) {
        int start = lexer->pos;
        while (isalnum((unsigned char)lexer->source[lexer->pos]))
            lexer->pos++;
        int len = lexer->pos - start;
        token.value = strndup(lexer->source + start, len);
        if (strcmp(token.value, "int") == 0)
            token.type = TOKEN_INT;
        else if (strcmp(token.value, "Console") == 0)
            token.type = TOKEN_CONSOLE;
        else if (strcmp(token.value, "WriteLine") == 0)
            token.type = TOKEN_WRITELINE;
        else if (strcmp(token.value, "if") == 0)
            token.type = TOKEN_IF;
        else if (strcmp(token.value, "while") == 0)
            token.type = TOKEN_WHILE;
        else
            token.type = TOKEN_IDENTIFIER;
        return token;
    }

    if (lexer->source[lexer->pos] == '"') {
        lexer->pos++; // skip opening quote
        int start = lexer->pos;
        while (lexer->source[lexer->pos] && lexer->source[lexer->pos] != '"')
            lexer->pos++;
        int len = lexer->pos - start;
        token.value = strndup(lexer->source + start, len);
        token.type = TOKEN_STRING;
        if (lexer->source[lexer->pos] == '"')
            lexer->pos++; // skip closing quote
        return token;
    }

    if (isdigit((unsigned char)lexer->source[lexer->pos])) {
        int start = lexer->pos;
        while (isdigit((unsigned char)lexer->source[lexer->pos]))
            lexer->pos++;
        int len = lexer->pos - start;
        token.value = strndup(lexer->source + start, len);
        token.type = TOKEN_NUMBER;
        return token;
    }

    switch (lexer->source[lexer->pos]) {
    case '+':
        token.type = TOKEN_PLUS;
        token.value = strdup("+");
        lexer->pos++;
        break;
    case '-':
        token.type = TOKEN_MINUS;
        token.value = strdup("-");
        lexer->pos++;
        break;
    case ';':
        token.type = TOKEN_SEMICOLON;
        token.value = strdup(";");
        lexer->pos++;
        break;
    case '=':
        token.type = TOKEN_EQUALS;
        token.value = strdup("=");
        lexer->pos++;
        break;
    case '.':
        token.type = TOKEN_DOT;
        token.value = strdup(".");
        lexer->pos++;
        break;
    case '(':
        token.type = TOKEN_LPAREN;
        token.value = strdup("(");
        lexer->pos++;
        break;
    case ')':
        token.type = TOKEN_RPAREN;
        token.value = strdup(")");
        lexer->pos++;
        break;
    case '{':
        token.type = TOKEN_LBRACE;
        token.value = strdup("{");
        lexer->pos++;
        break;
    case '}':
        token.type = TOKEN_RBRACE;
        token.value = strdup("}");
        lexer->pos++;
        break;
    default:
        token.type = TOKEN_UNKNOWN;
        token.value = strndup(lexer->source + lexer->pos, 1);
        lexer->pos++;
        break;
    }
    return token;
}

