/*
 * DreamCompiler C API Header for Go Testing
 * This header provides the C API interface for Go tests via cgo
 */

#ifndef DREAM_H
#define DREAM_H

#include <stddef.h>
#include <stdbool.h>

// Forward declarations
typedef struct Lexer Lexer;
typedef struct Parser Parser;
typedef struct Arena Arena;
typedef struct Node Node;
typedef struct Type Type;

// Token definitions
typedef enum {
    TK_EOF = 0,
    TK_INT,
    TK_FLOAT,
    TK_CHAR,
    TK_STRING,
    TK_BOOL,
    TK_IDENT,
    TK_PLUS,
    TK_MINUS,
    TK_STAR,
    TK_SLASH,
    TK_PERCENT,
    TK_ASSIGN,
    TK_EQ,
    TK_NE,
    TK_LT,
    TK_LE,
    TK_GT,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_SEMICOLON,
    TK_COMMA,
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_RBRACE,
    TK_LBRACKET,
    TK_RBRACKET,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_RETURN,
    TK_BREAK,
    TK_CONTINUE,
    TK_FUNCTION,
    TK_VAR,
    TK_LET,
    TK_CONST,
    TK_CLASS,
    TK_STRUCT,
    TK_NEW,
    TK_THIS,
    TK_SUPER,
    TK_STATIC,
    TK_PUBLIC,
    TK_PRIVATE,
    TK_PROTECTED,
    TK_TRY,
    TK_CATCH,
    TK_FINALLY,
    TK_THROW,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_DO,
    TK_CONSOLE,
    TK_WRITELINE,
    TK_WRITE,
    TK_READLINE,
    TK_READ,
    TK_DOT,
    TK_ARROW,
    TK_INCREMENT,
    TK_DECREMENT,
    TK_PLUS_ASSIGN,
    TK_MINUS_ASSIGN,
    TK_STAR_ASSIGN,
    TK_SLASH_ASSIGN,
    TK_PERCENT_ASSIGN,
    TK_QUESTION,
    TK_COLON,
    TK_BITWISE_AND,
    TK_BITWISE_OR,
    TK_BITWISE_XOR,
    TK_BITWISE_NOT,
    TK_LSHIFT,
    TK_RSHIFT,
    TK_LOGICAL_AND,
    TK_LOGICAL_OR,
    TK_NEWLINE,
    TK_WHITESPACE,
    TK_COMMENT,
    TK_BLOCK_COMMENT,
    TK_ERROR
} TokenKind;

typedef struct {
    size_t line;
    size_t column;
} Pos;

typedef struct {
    TokenKind kind;
    const char *start;
    size_t len;
    Pos pos;
} Token;

// Node kinds
typedef enum {
    ND_INT,
    ND_FLOAT,
    ND_CHAR,
    ND_STRING,
    ND_BOOL,
    ND_IDENT,
    ND_UNARY,
    ND_POST_UNARY,
    ND_BINOP,
    ND_COND,
    ND_INDEX,
    ND_FIELD,
    ND_VAR_DECL,
    ND_IF,
    ND_WHILE,
    ND_DO_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_RETURN,
    ND_BLOCK,
    ND_EXPR_STMT,
    ND_SWITCH,
    ND_CONSOLE_CALL,
    ND_CALL,
    ND_FUNC,
    ND_NEW,
    ND_STRUCT_DECL,
    ND_CLASS_DECL,
    ND_TRY,
    ND_THROW,
    ND_ERROR
} NodeKind;

// Type kinds
typedef enum {
    TY_VAR,
    TY_INT,
    TY_FLOAT,
    TY_BOOL,
    TY_CHAR,
    TY_STRING,
    TY_FUNC,
    TY_ERROR
} TypeKind;

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