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
    else if (strcmp(token.value, "bool") == 0)
      token.type = TOKEN_BOOL_TYPE;
    else if (strcmp(token.value, "float") == 0)
      token.type = TOKEN_FLOAT_TYPE;
    else if (strcmp(token.value, "true") == 0) {
      free(token.value);
      token.value = strdup("1");
      token.type = TOKEN_NUMBER;
    } else if (strcmp(token.value, "false") == 0) {
      free(token.value);
      token.value = strdup("0");
      token.type = TOKEN_NUMBER;
    }
    else if (strcmp(token.value, "Console") == 0)
      token.type = TOKEN_CONSOLE;
    else if (strcmp(token.value, "WriteLine") == 0)
      token.type = TOKEN_WRITELINE;
    else if (strcmp(token.value, "Write") == 0)
      token.type = TOKEN_WRITE;
    else if (strcmp(token.value, "if") == 0)
      token.type = TOKEN_IF;
    else if (strcmp(token.value, "else") == 0)
      token.type = TOKEN_ELSE;
    else if (strcmp(token.value, "while") == 0)
      token.type = TOKEN_WHILE;
    else if (strcmp(token.value, "for") == 0)
      token.type = TOKEN_FOR;
    else if (strcmp(token.value, "switch") == 0)
      token.type = TOKEN_SWITCH;
    else if (strcmp(token.value, "case") == 0)
      token.type = TOKEN_CASE;
    else if (strcmp(token.value, "default") == 0)
      token.type = TOKEN_DEFAULT;
    else if (strcmp(token.value, "do") == 0)
      token.type = TOKEN_DO;
    else if (strcmp(token.value, "break") == 0)
      token.type = TOKEN_BREAK;
    else if (strcmp(token.value, "continue") == 0)
      token.type = TOKEN_CONTINUE;
    else if (strcmp(token.value, "return") == 0)
      token.type = TOKEN_RETURN;
    else if (strcmp(token.value, "class") == 0)
      token.type = TOKEN_CLASS;
    else if (strcmp(token.value, "func") == 0)
      token.type = TOKEN_FUNC;
    else
      token.type = TOKEN_IDENTIFIER;
    return token;
  }

  if (lexer->source[lexer->pos] == '"') {
    lexer->pos++; // skip opening quote
    char *buf = NULL;
    int len = 0;
    int cap = 0;
    while (lexer->source[lexer->pos] && lexer->source[lexer->pos] != '"') {
      char c = lexer->source[lexer->pos++];
      if (c == '\\' && lexer->source[lexer->pos]) {
        char next = lexer->source[lexer->pos++];
        switch (next) {
        case 'n': c = '\n'; break;
        case 't': c = '\t'; break;
        case '\\': c = '\\'; break;
        case '"': c = '"'; break;
        default: c = next; break;
        }
      }
      if (len + 1 >= cap) {
        cap = cap ? cap * 2 : 16;
        buf = realloc(buf, cap);
      }
      buf[len++] = c;
    }
    if (lexer->source[lexer->pos] == '"')
      lexer->pos++; // skip closing quote
    if (buf) {
      buf = realloc(buf, len + 1);
      buf[len] = '\0';
    }
    token.value = buf ? buf : strdup("");
    token.type = TOKEN_STRING;
    return token;
  }

  if (isdigit((unsigned char)lexer->source[lexer->pos])) {
    int start = lexer->pos;
    while (isdigit((unsigned char)lexer->source[lexer->pos]))
      lexer->pos++;
    if (lexer->source[lexer->pos] == '.' &&
        isdigit((unsigned char)lexer->source[lexer->pos + 1])) {
      lexer->pos++; // skip '.'
      while (isdigit((unsigned char)lexer->source[lexer->pos]))
        lexer->pos++;
    }
    int len = lexer->pos - start;
    token.value = strndup(lexer->source + start, len);
    token.type = TOKEN_NUMBER;
    return token;
  }

  switch (lexer->source[lexer->pos]) {
  case '+':
    if (lexer->source[lexer->pos + 1] == '+') {
      token.type = TOKEN_PLUSPLUS;
      token.value = strdup("++");
      lexer->pos += 2;
    } else if (lexer->source[lexer->pos + 1] == '=') {
      token.type = TOKEN_PLUSEQ;
      token.value = strdup("+=");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_PLUS;
      token.value = strdup("+");
      lexer->pos++;
    }
    break;
  case '*':
    if (lexer->source[lexer->pos + 1] == '=') {
      token.type = TOKEN_STAREQ;
      token.value = strdup("*=");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_STAR;
      token.value = strdup("*");
      lexer->pos++;
    }
    break;
  case '/':
    if (lexer->source[lexer->pos + 1] == '=') {
      token.type = TOKEN_SLASHEQ;
      token.value = strdup("/=");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_SLASH;
      token.value = strdup("/");
      lexer->pos++;
    }
    break;
  case '%':
    if (lexer->source[lexer->pos + 1] == '=') {
      token.type = TOKEN_MODEQ;
      token.value = strdup("%=");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_PERCENT;
      token.value = strdup("%");
      lexer->pos++;
    }
    break;
  case '<':
    if (lexer->source[lexer->pos + 1] == '<') {
      token.type = TOKEN_SHL;
      token.value = strdup("<<");
      lexer->pos += 2;
    } else if (lexer->source[lexer->pos + 1] == '=') {
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
    if (lexer->source[lexer->pos + 1] == '>') {
      token.type = TOKEN_SHR;
      token.value = strdup(">>");
      lexer->pos += 2;
    } else if (lexer->source[lexer->pos + 1] == '=') {
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
    if (lexer->source[lexer->pos + 1] == '-') {
      token.type = TOKEN_MINUSMINUS;
      token.value = strdup("--");
      lexer->pos += 2;
    } else if (lexer->source[lexer->pos + 1] == '=') {
      token.type = TOKEN_MINUSEQ;
      token.value = strdup("-=");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_MINUS;
      token.value = strdup("-");
      lexer->pos++;
    }
    break;
  case ';':
    token.type = TOKEN_SEMICOLON;
    token.value = strdup(";");
    lexer->pos++;
    break;
  case ':':
    token.type = TOKEN_COLON;
    token.value = strdup(":");
    lexer->pos++;
    break;
  case '?':
    token.type = TOKEN_QUESTION;
    token.value = strdup("?");
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
      token.type = TOKEN_BITAND;
      token.value = strdup("&");
      lexer->pos++;
    }
    break;
  case '|':
    if (lexer->source[lexer->pos + 1] == '|') {
      token.type = TOKEN_OR;
      token.value = strdup("||");
      lexer->pos += 2;
    } else {
      token.type = TOKEN_BITOR;
      token.value = strdup("|");
      lexer->pos++;
    }
    break;
  case '^':
    token.type = TOKEN_CARET;
    token.value = strdup("^");
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
