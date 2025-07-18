#include "cse.h"
#include "../ir/ir.h"
#include <stdlib.h>

typedef struct CSEntry CSEntry;
struct CSEntry {
    IROp op;
    int a, b;
    int value;
    CSEntry *next;
};

static int is_commutative(IROp op) {
    switch(op) {
        case IR_ADD: case IR_MUL: case IR_AND: case IR_OR: case IR_XOR:
        case IR_EQ: case IR_NE:
            return 1;
        default: return 0;
    }
}

static CSEntry *find(CSEntry *h, IROp op, int a, int b) {
    for(CSEntry *e=h; e; e=e->next)
        if(e->op==op && e->a==a && e->b==b) return e;
    return NULL;
}

static void insert(CSEntry **h, IROp op, int a, int b, int val) {
    CSEntry *e = malloc(sizeof(CSEntry));
    e->op=op; e->a=a; e->b=b; e->value=val; e->next=*h; *h=e;
}

bool cse(CFG *cfg) {
    if(!cfg) return false;
    bool changed=false;
    CSEntry *map=NULL;
    for(size_t bi=0; bi<cfg->nblocks; bi++) {
        BasicBlock *b = cfg->blocks[bi];
        for(size_t i=0;i<b->ninstrs;i++) {
            IRInstr *ins=b->instrs[i];
            if(ins->op>=IR_ADD && ins->op<=IR_NE) {
                int a = ins->a.id;
                int bval = ins->b.id;
                if(is_commutative(ins->op) && a>bval){int t=a;a=bval;bval=t;}
                CSEntry *e=find(map,ins->op,a,bval);
                if(e) {
                    ins->op=IR_MOV;
                    ins->a.id=e->value;
                    ins->b.id=0;
                    changed=true;
                } else {
                    insert(&map,ins->op,a,bval,ins->dst.id);
                }
            }
        }
    }
    while(map){CSEntry*n=map->next; free(map); map=n;}
    return changed;
}
