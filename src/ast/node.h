#ifndef NODE_H
#define NODE_H

#include <stdlib.h>

typedef enum {
  NODE_VAR_DECL,
  NODE_ASSIGN,
  NODE_WRITELINE,
  NODE_BINARY_OP,
  NODE_UNARY_OP,
  NODE_IF,
  NODE_WHILE,
  NODE_IDENTIFIER,
  NODE_NUMBER,
  NODE_STRING
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
