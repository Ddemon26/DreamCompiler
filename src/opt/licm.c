#include "licm.h"
#include <stdlib.h>
#include <stdbool.h>

static bool dominates(BasicBlock *dom, BasicBlock *b) {
    while (b && b != dom)
        b = b->idom;
    return b == dom;
}

static bool in_loop(BasicBlock **loop, size_t n, BasicBlock *b) {
    for (size_t i = 0; i < n; i++)
        if (loop[i] == b) return true;
    return false;
}

static void collect_loop(BasicBlock *header, BasicBlock *latch, BasicBlock ***out, size_t *n) {
    BasicBlock **stack = NULL;
    size_t sp = 0;
    *out = NULL;
    *n = 0;
    stack = realloc(stack, sizeof(BasicBlock*));
    stack[sp++] = latch;
    while (sp) {
        BasicBlock *x = stack[--sp];
        if (in_loop(*out, *n, x)) continue;
        *out = realloc(*out, sizeof(BasicBlock*) * (*n + 1));
        (*out)[(*n)++] = x;
        for (size_t i = 0; i < x->npred; i++)
            if (x->pred[i] != header)
                stack = realloc(stack, sizeof(BasicBlock*) * (sp + 1)), stack[sp++] = x->pred[i];
    }
    free(stack);
    *out = realloc(*out, sizeof(BasicBlock*) * (*n + 1));
    (*out)[(*n)++] = header;
}

static BasicBlock *find_preheader(BasicBlock *header, BasicBlock **loop, size_t n) {
    BasicBlock *pre = NULL;
    for (size_t i = 0; i < header->npred; i++) {
        BasicBlock *p = header->pred[i];
        if (!in_loop(loop, n, p)) {
            if (pre) return NULL; // not unique
            pre = p;
        }
    }
    return pre;
}

static bool is_pure(IRInstr *ins) {
    switch (ins->op) {
    case IR_BIN:
    case IR_MOV:
        return true;
    default:
        return false;
    }
}

static bool may_alias(IRInstr *a, IRInstr *b) {
    (void)a; (void)b;
    return true; // placeholder alias analysis
}

static bool value_in_set(int v, int *vals, size_t n) {
    for (size_t i = 0; i < n; i++)
        if (vals[i] == v) return true;
    return false;
}

static void hoist_loop(BasicBlock **loop, size_t n, BasicBlock *pre) {
    int *defined = NULL;
    size_t ndef = 0;
    for (size_t i = 0; i < n; i++) {
        BasicBlock *b = loop[i];
        for (size_t j = 0; j < b->ninstrs; j++) {
            IRInstr *ins = b->instrs[j];
            defined = realloc(defined, sizeof(int) * (ndef + 1));
            defined[ndef++] = ins->dst.id;
        }
    }
    int *hoisted = NULL;
    size_t nhoisted = 0;
    for (size_t i = 0; i < n; i++) {
        BasicBlock *b = loop[i];
        for (size_t j = 0; j < b->ninstrs; j++) {
            IRInstr *ins = b->instrs[j];
            if (!is_pure(ins)) continue;
            if (value_in_set(ins->a.id, defined, ndef) && !value_in_set(ins->a.id, hoisted, nhoisted))
                continue;
            if (value_in_set(ins->b.id, defined, ndef) && !value_in_set(ins->b.id, hoisted, nhoisted))
                continue;
            // alias check for stores would go here
            (void)may_alias;
            pre->instrs = realloc(pre->instrs, sizeof(IRInstr*) * (pre->ninstrs + 1));
            pre->instrs[pre->ninstrs++] = ins;
            ins->op = IR_NOP;
            hoisted = realloc(hoisted, sizeof(int) * (nhoisted + 1));
            hoisted[nhoisted++] = ins->dst.id;
        }
    }
    free(defined);
    free(hoisted);
}

void licm(CFG *cfg) {
    if (!cfg) return;
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *b = cfg->blocks[i];
        for (size_t s = 0; s < b->nsucc; s++) {
            BasicBlock *t = b->succ[s];
            if (dominates(t, b)) {
                BasicBlock **loop = NULL;
                size_t n = 0;
                collect_loop(t, b, &loop, &n);
                BasicBlock *pre = find_preheader(t, loop, n);
                if (pre) hoist_loop(loop, n, pre);
                free(loop);
            }
        }
    }
}
