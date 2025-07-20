/*
 * DreamCompiler C API Header for Go Testing
 * This header provides the C API interface for Go tests via cgo
 */

#ifndef DREAM_H
#define DREAM_H

#include <stddef.h>
#include <stdbool.h>

// Use definitions from the main compiler headers
#include "../../src/lexer/lexer.h"
#include "../../src/parser/ast.h"
#include "../../src/parser/parser.h"
#include "../../src/sem/type.h"

// NodeKind and TypeKind are provided by the main compiler headers

// API Functions

// Lexer API
Lexer* lexer_create(const char* source);
void lexer_destroy(Lexer* lexer);
Token lexer_next_token(Lexer* lexer);
Token lexer_peek_token(Lexer* lexer);
const char* token_kind_name(TokenKind kind);

// Parser API  
Parser* parser_create(const char* source);
void parser_destroy(Parser* parser);
Node* parser_parse_program(Parser* parser);
Node* parser_parse_expression(Parser* parser);
Node* parser_parse_statement(Parser* parser);
bool parser_has_errors(Parser* parser);
size_t parser_error_count(Parser* parser);
const char* parser_get_error(Parser* parser, size_t index);

// AST API
NodeKind node_get_kind(Node* node);
size_t node_count_children(Node* node);
Node* node_get_child(Node* node, size_t index);
const char* node_get_text(Node* node);
Pos node_get_position(Node* node);

// Type API
Type* type_create(TypeKind kind);
void type_destroy(Type* type);
TypeKind type_get_kind(Type* type);
bool type_equals(Type* a, Type* b);
const char* type_to_string(Type* type);

// Memory management
Arena* arena_create(void);
void arena_destroy(Arena* arena);
void* arena_allocate(Arena* arena, size_t size);

// Utility functions
int dream_compile_string(const char* source, char** output, char** errors);
int dream_run_string(const char* source, char** output, char** errors);
void dream_free_string(char* str);

#endif // DREAM_H