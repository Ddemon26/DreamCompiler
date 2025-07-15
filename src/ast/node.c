#include "node.h"
#include <string.h>
#include <stdlib.h>

Node *create_node(NodeType type, char *value, Node *left, Node *right,
                  Node *else_branch) {
  Node *node = malloc(sizeof(Node));
  node->type = type;
  node->value = value ? strdup(value) : NULL;
  node->left = left;
  node->right = right;
  node->else_branch = else_branch;
  return node;
}

void free_node(Node *n) {
  if (!n)
    return;
  if (n->value)
    free(n->value);
  free_node(n->left);
  free_node(n->right);
  free_node(n->else_branch);
  free(n);
}
