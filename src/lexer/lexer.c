#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
static char *strndup(const char *src, size_t n) {
  size_t len = strlen(src);
  if (n < len)
    len = n;
  char *dest = malloc(len + 1);
  if (!dest)
    return NULL;
  memcpy(dest, src, len);
  dest[len] = '\0';
  return dest;
}
#endif

Token next_token(Lexer *lexer) {
  while (isspace((unsigned char)lexer->source[lexer->pos]))
    lexer->pos++;

  if (lexer->source[lexer->pos] == '/' &&
      lexer->source[lexer->pos + 1] == '/') {
    while (lexer->source[lexer->pos] && lexer->source[lexer->pos] != '\n')
      lexer->pos++;
    return next_token(lexer);
  }

  if (lexer->source[lexer->pos] == '/' &&
      lexer->source[lexer->pos + 1] == '*') {
    lexer->pos += 2;
    while (lexer->source[lexer->pos] &&
           !(lexer->source[lexer->pos] == '*' &&
             lexer->source[lexer->pos + 1] == '/'))
      lexer->pos++;
    if (lexer->source[lexer->pos])
      lexer->pos += 2;
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
    else if (strcmp(token.value, "string") == 0)
      token.type = TOKEN_STRING_TYPE;
    else if (strcmp(token.value, "Console") == 0)
      token.type = TOKEN_CONSOLE;
    else if (strcmp(token.value, "WriteLine") == 0)
      token.type = TOKEN_WRITELINE;
    else if (strcmp(token.value, "if") == 0)
      token.type = TOKEN_IF;
    else if (strcmp(token.value, "else") == 0)
      token.type = TOKEN_ELSE;
    else if (strcmp(token.value, "while") == 0)
      token.type = TOKEN_WHILE;
    else if (strcmp(token.value, "for") == 0)
      token.type = TOKEN_FOR;
    else if (strcmp(token.value, "do") == 0)
      token.type = TOKEN_DO;
    else if (strcmp(token.value, "break") == 0)
      token.type = TOKEN_BREAK;
    else if (strcmp(token.value, "continue") == 0)
      token.type = TOKEN_CONTINUE;
    else if (strcmp(token.value, "return") == 0)
      token.type = TOKEN_RETURN;
    else if (strcmp(token.value, "func") == 0)
      token.type = TOKEN_FUNC;
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
  case '*':
    token.type = TOKEN_STAR;
    token.value = strdup("*");
    lexer->pos++;
    break;
  case '/':
    token.type = TOKEN_SLASH;
    token.value = strdup("/");
    lexer->pos++;
    break;
  case '%':
    token.type = TOKEN_PERCENT;
    token.value = strdup("%");
    lexer->pos++;
    break;
  case '<':
    if (lexer->source[lexer->pos + 1] == '=') {
      token.type = TOKEN_LE;
      token.value = strdup("<=");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_LT;
      token.value = strdup("<");
      lexer->pos++;
    }
    break;
  case '>':
    if (lexer->source[lexer->pos + 1] == '=') {
      token.type = TOKEN_GE;
      token.value = strdup(">=");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_GT;
      token.value = strdup(">");
      lexer->pos++;
    }
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
    if (lexer->source[lexer->pos + 1] == '=') {
      token.type = TOKEN_EQEQ;
      token.value = strdup("==");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_EQUALS;
      token.value = strdup("=");
      lexer->pos++;
    }
    break;
  case '!':
    if (lexer->source[lexer->pos + 1] == '=') {
      token.type = TOKEN_NEQ;
      token.value = strdup("!=");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_NOT;
      token.value = strdup("!");
      lexer->pos++;
    }
    break;
  case '&':
    if (lexer->source[lexer->pos + 1] == '&') {
      token.type = TOKEN_AND;
      token.value = strdup("&&");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_UNKNOWN;
      token.value = strndup(lexer->source + lexer->pos, 1);
      lexer->pos++;
    }
    break;
  case '|':
    if (lexer->source[lexer->pos + 1] == '|') {
      token.type = TOKEN_OR;
      token.value = strdup("||");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_UNKNOWN;
      token.value = strndup(lexer->source + lexer->pos, 1);
      lexer->pos++;
    }
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
  case ',':
    token.type = TOKEN_COMMA;
    token.value = strdup(",");
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
