#ifndef NODE_H
#define NODE_H

#include <stdlib.h>

typedef enum {
  NODE_VAR_DECL,
  NODE_STR_DECL,
  NODE_ASSIGN,
  NODE_WRITELINE,
  NODE_BINARY_OP,
  NODE_UNARY_OP,
  NODE_PRE_INC,
  NODE_PRE_DEC,
  NODE_POST_INC,
  NODE_POST_DEC,
  NODE_IF,
  NODE_WHILE,
  NODE_DO_WHILE,
  NODE_BLOCK,
  NODE_IDENTIFIER,
  NODE_NUMBER,
  NODE_STRING,
  NODE_BREAK,
  NODE_CONTINUE,
  NODE_RETURN,
  NODE_FUNC_DEF,
  NODE_FUNC_CALL
} NodeType;

typedef struct Node {
  NodeType type;
  char *value;
  struct Node *left;
  struct Node *right;
  struct Node *else_branch;
} Node;

Node *create_node(NodeType type, char *value, Node *left, Node *right,
                  Node *else_branch);
void free_node(Node *n);

#endif
