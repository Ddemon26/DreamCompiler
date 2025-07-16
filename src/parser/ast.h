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
    ND_STRING,
    ND_IDENT,
    ND_BINOP,
    ND_VAR_DECL,
    ND_BLOCK,
    ND_EXPR_STMT,
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
        Slice lit;                    // ND_INT, ND_STRING
        Slice ident;                  // ND_IDENT
        struct {                      // ND_BINOP
            TokenKind op;
            Node *lhs;
            Node *rhs;
        } bin;
        struct {                      // ND_VAR_DECL
            Slice name;
            Node *init;
        } var_decl;
        struct {                      // ND_BLOCK
            Node **items;
            size_t len;
        } block;
        struct {                      // ND_EXPR_STMT
            Node *expr;
        } expr_stmt;
    } as;
};

#endif
