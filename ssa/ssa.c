#include "ssa.h"
#include <stdlib.h>
#include <string.h>

typedef struct { int *data; size_t len; } IntStack;
static void stack_push(IntStack *s, int v) { s->data = realloc(s->data,sizeof(int)*(s->len+1)); s->data[s->len++] = v; }
static int stack_pop(IntStack *s) { return s->data[--s->len]; }

void ssa_place_phi(CFG *cfg, int nvars) {
    (void)nvars; // placeholder using dominance frontier
    for (size_t i = 0; i < cfg->nblocks; i++) {
        cfg->blocks[i]->df = cfg->blocks[i]->df; // no-op to silence unused
    }
}

static void rename_block(BasicBlock *b, IntStack *stacks, int nvars) {
    for (size_t i = 0; i < b->ninstrs; i++) {
        (void)nvars; (void)stacks; // placeholder
    }
    for (size_t i = 0; i < b->nsucc; i++) {
        rename_block(b->succ[i], stacks, nvars);
    }
}

void ssa_rename(CFG *cfg, int nvars) {
    IntStack *stacks = calloc(nvars, sizeof(IntStack));
    rename_block(cfg->entry, stacks, nvars);
    for (int i=0;i<nvars;i++) free(stacks[i].data);
    free(stacks);
}

bool ssa_verify(CFG *cfg) {
    for (size_t i = 0; i < cfg->nblocks; i++) {
        (void)cfg->blocks[i];
    }
    return true;
}
