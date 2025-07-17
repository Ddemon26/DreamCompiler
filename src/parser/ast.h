#ifndef AST_H
#define AST_H
#include <stddef.h>
#include "../lexer/lexer.h"

typedef struct {
    char *ptr;
    size_t len;
    size_t cap;
} Arena;

void arena_init(Arena *a);
void *arena_alloc(Arena *a, size_t size);

typedef enum {
    ND_INT,
    ND_FLOAT,
    ND_CHAR,
    ND_STRING,
    ND_BOOL,
    ND_IDENT,
    ND_BINOP,
    ND_VAR_DECL,
    ND_IF,
    ND_WHILE,
    ND_BREAK,
    ND_BLOCK,
    ND_EXPR_STMT,
    ND_CONSOLE_CALL,
    ND_ERROR
} NodeKind;

typedef struct Node Node;

typedef struct {
    const char *start;
    size_t len;
} Slice;

Node *node_new(Arena *a, NodeKind kind);

struct Node {
    NodeKind kind;
    union {
        Slice lit;                    // ND_* literal nodes
        Slice ident;                  // ND_IDENT
        struct {                      // ND_BINOP
            TokenKind op;
            Node *lhs;
            Node *rhs;
        } bin;
        struct {                      // ND_VAR_DECL
            TokenKind type;
            Slice name;
            Node *init;
        } var_decl;
        struct {                      // ND_IF
            Node *cond;
            Node *then_br;
            Node *else_br; // may be NULL
        } if_stmt;
        struct {                      // ND_WHILE
            Node *cond;
            Node *body;
        } while_stmt;
        struct {                      // ND_BLOCK
            Node **items;
            size_t len;
        } block;
        struct {                      // ND_EXPR_STMT
            Node *expr;
        } expr_stmt;
        struct {                      // ND_CONSOLE_CALL
            Node *arg;
            int newline;
        } console;
    } as;
};

#endif
