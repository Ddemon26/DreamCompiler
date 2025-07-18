#include "value_numbering.h"
#include <stdlib.h>

/**
 * @brief Represents an entry in the value numbering table.
 *
 * This structure is used for local value numbering within a basic block.
 */
typedef struct VNEntry VNEntry;
struct VNEntry {
  IROp op;
  int a, b;
  int value;
  VNEntry *next;
};

/**
 * @brief Searches for an entry in the value numbering table.
 *
 * This function iterates through the linked list of value numbering entries
 * to find a match for the given operation and operands.
 *
 * @param head Pointer to the head of the value numbering table.
 * @param op The operation to search for.
 * @param a The first operand of the operation.
 * @param b The second operand of the operation.
 * @return Pointer to the matching VNEntry, or NULL if no match is found.
 */
static VNEntry *vn_find(VNEntry *head, IROp op, int a, int b) {
  for (VNEntry *e = head; e; e = e->next)
    if (e->op == op && e->a == a && e->b == b)
      return e;
  return NULL;
}

/**
 * @brief Inserts a new entry into the value numbering table.
 *
 * This function allocates memory for a new VNEntry, initializes it with the
 * provided operation, operands, and value, and inserts it at the head of the
 * table.
 *
 * @param head Pointer to the head of the value numbering table.
 * @param op The operation associated with the entry.
 * @param a The first operand of the operation.
 * @param b The second operand of the operation.
 * @param value The computed value for the operation.
 */
static void vn_insert(VNEntry **head, IROp op, int a, int b, int value) {
  VNEntry *e = malloc(sizeof(VNEntry));
  e->op = op;
  e->a = a;
  e->b = b;
  e->value = value;
  e->next = *head;
  *head = e;
}

/**
 * @brief Frees all entries in the value numbering table.
 *
 * This function deallocates memory for each entry in the linked list
 * of value numbering entries.
 *
 * @param head Pointer to the head of the value numbering table.
 */
static void vn_free(VNEntry *head) {
  while (head) {
    VNEntry *n = head->next;
    free(head);
    head = n;
  }
}

/**
 * @brief Performs value numbering on a basic block.
 *
 * This function processes each instruction in the basic block, performing
 * local value numbering to eliminate redundant computations.
 *
 * @param bb Pointer to the basic block to process.
 */
static void value_number_block(BasicBlock *bb) {
  VNEntry *map = NULL;
  for (size_t i = 0; i < bb->ninstrs; i++) {
    IRInstr *ins = bb->instrs[i];
    if (!ins)
      continue;
    if (ins->op >= IR_ADD && ins->op <= IR_NE) {
      VNEntry *e = vn_find(map, ins->op, ins->a.id, ins->b.id);
      if (e) {
        ins->op = IR_MOV;
        ins->a.id = e->value;
        ins->b.id = 0;
      } else {
        vn_insert(&map, ins->op, ins->a.id, ins->b.id, ins->dst.id);
      }
    }
  }
  vn_free(map);
}

/**
 * @brief Performs value numbering on the control flow graph.
 *
 * This function iterates through all basic blocks in the control flow graph
 * and applies value numbering to each block.
 *
 * @param cfg Pointer to the control flow graph to process.
 */
void value_numbering(CFG *cfg) {
  if (!cfg)
    return;
  for (size_t i = 0; i < cfg->nblocks; i++) {
    value_number_block(cfg->blocks[i]);
  }
}
