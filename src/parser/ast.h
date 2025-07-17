#ifndef AST_H
#define AST_H
#include "../lexer/lexer.h"
#include <stddef.h>

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
  ND_UNARY,
  ND_POST_UNARY,
  ND_BINOP,
  ND_COND,
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
  ND_ERROR
} NodeKind;

typedef struct Node Node;
typedef struct {
  const char *start;
  size_t len;
} Slice;

Node *node_new(Arena *a, NodeKind kind);

typedef struct {
  int is_default;
  Node *value; // NULL if default
  Node *body;
} SwitchCase;

struct Node {
  NodeKind kind;
  union {
    Slice lit;   // ND_* literal nodes
    Slice ident; // ND_IDENT
    struct {     // ND_UNARY and ND_POST_UNARY
      TokenKind op;
      Node *expr;
    } unary;
    struct { // ND_BINOP
      TokenKind op;
      Node *lhs;
      Node *rhs;
    } bin;
    struct { // ND_COND
      Node *cond;
      Node *then_expr;
      Node *else_expr;
    } cond;
    struct { // ND_VAR_DECL
      TokenKind type;
      Slice name;
      Node *init;
    } var_decl;
    struct { // ND_IF
      Node *cond;
      Node *then_br;
      Node *else_br; // may be NULL
    } if_stmt;
    struct { // ND_WHILE
      Node *cond;
      Node *body;
    } while_stmt;
    struct { // ND_DO_WHILE
      Node *body;
      Node *cond;
    } do_while_stmt;
    struct {        // ND_FOR
      Node *init;   // may be NULL
      Node *cond;   // may be NULL
      Node *update; // may be NULL
      Node *body;
    } for_stmt;
    struct { // ND_BLOCK
      Node **items;
      size_t len;
    } block;
    struct {      // ND_RETURN
      Node *expr; // may be NULL
    } ret;
    struct { // ND_EXPR_STMT
      Node *expr;
    } expr_stmt;
    struct { // ND_CONSOLE_CALL
      Node *arg;
      int newline;
    } console;
    struct { // ND_SWITCH
      Node *expr;
      SwitchCase *cases;
      size_t len;
    } switch_stmt;
    /* ND_CONTINUE has no fields */
  } as;
};

#endif
