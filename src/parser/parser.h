#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../ast/node.h"

Node *parse_expression(Lexer *lexer, Token *token);
Node *parse_block(Lexer *lexer, Token *token);
Node *parse_for_increment(Lexer *lexer, Token *token);
Node *parse_declaration(Lexer *lexer, Token *token);
Node *parse_control_flow(Lexer *lexer, Token *token);
Node *parse_statement(Lexer *lexer, Token *token);

#endif
