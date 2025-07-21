/*
 * DreamCompiler C API Header for Go Testing
 * This header provides the C API interface for Go tests via cgo
 */

#ifndef DREAM_H
#define DREAM_H

#include <stddef.h>
#include <stdbool.h>

// Forward declarations to avoid header dependency issues
typedef struct Lexer Lexer;
typedef struct Parser Parser;
typedef struct Node Node;
typedef struct Type Type;
typedef struct Arena Arena;

// Position structure
typedef struct { size_t line; size_t column; } Pos;

// Token structure  
typedef struct {
    int kind;           // TokenKind as int to avoid enum issues
    const char *start;
    size_t len;
    Pos pos;
} Token;

// Token constants - matching tokens.def
#define TK_KW_IF 1
#define TK_KW_ELSE 2
#define TK_KW_WHILE 3
#define TK_KW_FOR 4
#define TK_KW_DO 5
#define TK_KW_BREAK 6
#define TK_KW_CONTINUE 7
#define TK_KW_RETURN 8
#define TK_KW_CLASS 9
#define TK_KW_STRUCT 10
#define TK_KW_ENUM 11
#define TK_KW_PUBLIC 12
#define TK_KW_PRIVATE 13
#define TK_KW_STATIC 14
#define TK_KW_NEW 15
#define TK_KW_INT 16
#define TK_KW_STRING 17
#define TK_KW_BOOL 18
#define TK_KW_FLOAT 19
#define TK_KW_CHAR 20
#define TK_KW_VOID 21
#define TK_KW_TASK 22
#define TK_KW_TASKRESULT 23
#define TK_KW_VAR 24
#define TK_KW_LET 25
#define TK_KW_CONST 26
#define TK_KW_TRUE 27
#define TK_KW_FALSE 28
#define TK_KW_NULL 29
#define TK_KW_FUNC 30
#define TK_KW_SWITCH 31
#define TK_KW_CASE 32
#define TK_KW_DEFAULT 33
#define TK_KW_CONSOLE 34
#define TK_KW_WRITELINE 35
#define TK_KW_WRITE 36
#define TK_KW_READLINE 37
#define TK_KW_TRY 38
#define TK_KW_CATCH 39
#define TK_KW_FINALLY 40
#define TK_KW_THROW 41
#define TK_KW_BASE 42
#define TK_KW_USING 43
#define TK_KW_IMPORT 44
#define TK_KW_MODULE 45
#define TK_KW_EXPORT 46
#define TK_KW_ASYNC 47
#define TK_KW_AWAIT 48
#define TK_IDENT 49
#define TK_INT_LITERAL 50
#define TK_FLOAT_LITERAL 51
#define TK_STRING_LITERAL 52
#define TK_CHAR_LITERAL 53
#define TK_PLUSPLUS 54
#define TK_MINUSMINUS 55
#define TK_PLUSEQ 56
#define TK_MINUSEQ 57
#define TK_STAREQ 58
#define TK_SLASHEQ 59
#define TK_PERCENTEQ 60
#define TK_ANDEQ 61
#define TK_OREQ 62
#define TK_XOREQ 63
#define TK_LSHIFTEQ 64
#define TK_RSHIFTEQ 65
#define TK_PLUS 66
#define TK_MINUS 67
#define TK_STAR 68
#define TK_SLASH 69
#define TK_PERCENT 70
#define TK_CARET 71
#define TK_LSHIFT 72
#define TK_RSHIFT 73
#define TK_LTEQ 74
#define TK_GTEQ 75
#define TK_EQEQ 76
#define TK_NEQ 77
#define TK_LT 78
#define TK_GT 79
#define TK_ANDAND 80
#define TK_OROR 81
#define TK_AND 82
#define TK_OR 83
#define TK_ARROW 84
#define TK_FATARROW 85
#define TK_TILDE 86
#define TK_BANG 87
#define TK_EQ 88
#define TK_QUESTION 89
#define TK_QMARKQMARK 90
#define TK_QMARKQMARKEQ 91
#define TK_COLON 92
#define TK_DCOLON 93
#define TK_SEMICOLON 94
#define TK_COMMA 95
#define TK_DOT 96
#define TK_LPAREN 97
#define TK_RPAREN 98
#define TK_LBRACE 99
#define TK_RBRACE 100
#define TK_LBRACKET 101
#define TK_RBRACKET 102
#define TK_EOF 103
#define TK_ERROR 104

// Aliases for backward compatibility and test simplicity
#define TK_IF TK_KW_IF
#define TK_ELSE TK_KW_ELSE
#define TK_WHILE TK_KW_WHILE
#define TK_FOR TK_KW_FOR
#define TK_RETURN TK_KW_RETURN
#define TK_BREAK TK_KW_BREAK
#define TK_CONTINUE TK_KW_CONTINUE
#define TK_INT TK_INT_LITERAL
#define TK_FLOAT TK_FLOAT_LITERAL
#define TK_STRING TK_STRING_LITERAL
#define TK_CHAR TK_CHAR_LITERAL
#define TK_BOOL TK_KW_BOOL
#define TK_TRUE TK_KW_TRUE
#define TK_FALSE TK_KW_FALSE
#define TK_CONSOLE TK_KW_CONSOLE
#define TK_WRITELINE TK_KW_WRITELINE
#define TK_ASSIGN TK_EQ
#define TK_LOGICAL_AND TK_ANDAND
#define TK_LOGICAL_OR TK_OROR
#define TK_NOT TK_BANG

// Node kind constants
#define ND_INT 0
#define ND_FLOAT 1
#define ND_CHAR 2
#define ND_STRING 3
#define ND_BOOL 4
#define ND_NULL 5
#define ND_IDENT 6
#define ND_UNARY 7
#define ND_POST_UNARY 8
#define ND_BINOP 9
#define ND_COND 10
#define ND_INDEX 11
#define ND_CALL 12
#define ND_CONSOLE_CALL 13
#define ND_MEMBER 14
#define ND_BLOCK 15
#define ND_VAR_DECL 16
#define ND_ASSIGN 17
#define ND_IF 18
#define ND_WHILE 19
#define ND_FOR 20
#define ND_BREAK 21
#define ND_CONTINUE 22
#define ND_RETURN 23
#define ND_FUNC 24
#define ND_STRUCT_DECL 25
#define ND_CLASS_DECL 26
#define ND_ENUM_DECL 27
#define ND_SWITCH 28
#define ND_CASE 29
#define ND_ERROR 30

// Type kind constants  
#define TY_VAR 0
#define TY_INT 1
#define TY_FLOAT 2
#define TY_BOOL 3
#define TY_CHAR 4
#define TY_STRING 5
#define TY_FUNC 6
#define TY_ERROR 7

type C API continues...

// API Functions

// Lexer API
Lexer* lexer_create(const char* source);
void lexer_destroy(Lexer* lexer);
Token lexer_next_token(Lexer* lexer);
Token lexer_peek_token(Lexer* lexer);
const char* token_kind_name(int kind);

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
int node_get_kind(Node* node);
size_t node_count_children(Node* node);
Node* node_get_child(Node* node, size_t index);
const char* node_get_text(Node* node);
Pos node_get_position(Node* node);

// Type API
Type* type_create(int kind);
void type_destroy(Type* type);
int type_get_kind(Type* type);
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