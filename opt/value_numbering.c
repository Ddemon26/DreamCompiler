#include "value_numbering.h"
#include <stdlib.h>

// Simple local value numbering within each basic block.
typedef struct VNEntry VNEntry;
struct VNEntry {
    IROp op;
    int a, b;
    int value;
    VNEntry *next;
};

static VNEntry *vn_find(VNEntry *head, IROp op, int a, int b) {
    for (VNEntry *e = head; e; e = e->next)
        if (e->op == op && e->a == a && e->b == b)
            return e;
    return NULL;
}

static void vn_insert(VNEntry **head, IROp op, int a, int b, int value) {
    VNEntry *e = malloc(sizeof(VNEntry));
    e->op = op;
    e->a = a;
    e->b = b;
    e->value = value;
    e->next = *head;
    *head = e;
}

static void vn_free(VNEntry *head) {
    while (head) {
        VNEntry *n = head->next;
        free(head);
        head = n;
    }
}

static void value_number_block(BasicBlock *bb) {
    VNEntry *map = NULL;
    for (size_t i = 0; i < bb->ninstrs; i++) {
        IRInstr *ins = bb->instrs[i];
        if (!ins) continue;
        if (ins->op == IR_BIN) {
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

void value_numbering(CFG *cfg) {
    if (!cfg) return;
    for (size_t i = 0; i < cfg->nblocks; i++) {
        value_number_block(cfg->blocks[i]);
    }
}
