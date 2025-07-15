#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../ast/node.h"

Node *parse_expression(Lexer *lexer, Token *token);
Node *parse_statement(Lexer *lexer, Token *token);

#endif
