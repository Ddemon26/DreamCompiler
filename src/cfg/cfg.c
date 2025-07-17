#include "cfg.h"
#include <stdlib.h>

CFG *cfg_new(void) {
    CFG *cfg = calloc(1, sizeof(CFG));
    return cfg;
}

BasicBlock *cfg_add_block(CFG *cfg) {
    BasicBlock *bb = calloc(1, sizeof(BasicBlock));
    bb->id = cfg->nblocks;
    cfg->blocks = realloc(cfg->blocks, sizeof(BasicBlock*) * (cfg->nblocks + 1));
    cfg->blocks[cfg->nblocks++] = bb;
    if (!cfg->entry) cfg->entry = bb;
    return bb;
}

void cfg_add_edge(BasicBlock *from, BasicBlock *to) {
    from->succ = realloc(from->succ, sizeof(BasicBlock*) * (from->nsucc + 1));
    from->succ[from->nsucc++] = to;
    to->pred = realloc(to->pred, sizeof(BasicBlock*) * (to->npred + 1));
    to->pred[to->npred++] = from;
}

typedef struct { int *data; size_t len; } IntVec;

static void ivec_push(IntVec *v, int x) {
    v->data = realloc(v->data, sizeof(int)*(v->len+1));
    v->data[v->len++] = x;
}

static void compute_df(CFG *cfg) {
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *b = cfg->blocks[i];
        if (b == cfg->entry) continue;
        for (size_t j = 0; j < b->npred; j++) {
            BasicBlock *p = b->pred[j];
            while (p && p != b->idom) {
                p->df = realloc(p->df, sizeof(BasicBlock*) * (p->ndf + 1));
                p->df[p->ndf++] = b;
                p = p->idom;
            }
        }
    }
}

// Lengauer-Tarjan dominator algorithm
static void dfs(BasicBlock *b, BasicBlock **vertex, int *parent, int *semi, int *idx) {
    if (b->dfnum) return;
    b->dfnum = ++*idx;
    vertex[*idx] = b;
    semi[b->dfnum] = b->dfnum;
    for (size_t i = 0; i < b->nsucc; i++) {
        BasicBlock *s = b->succ[i];
        if (!s->dfnum) {
            dfs(s, vertex, parent, semi, idx);
            parent[s->dfnum] = b->dfnum;
        }
    }
}

struct UF { int ancestor; int label; };

static int eval(struct UF *uf, int v) {
    if (uf[v].ancestor == 0) return uf[v].label;
    int compress = eval(uf, uf[v].ancestor);
    if (uf[uf[v].ancestor].label < uf[v].label)
        uf[v].label = uf[uf[v].ancestor].label;
    uf[v].ancestor = compress;
    return uf[v].label;
}

static void link(struct UF *uf, int parent, int child) {
    uf[child].ancestor = parent;
}

void cfg_compute_dominators(CFG *cfg) {
    size_t n = cfg->nblocks;
    if (n == 0) return;

    BasicBlock **vertex = calloc(n + 1, sizeof(BasicBlock*));
    int *parent = calloc(n + 1, sizeof(int));
    int *semi = calloc(n + 1, sizeof(int));
    int *idom = calloc(n + 1, sizeof(int));
    IntVec *bucket = calloc(n + 1, sizeof(IntVec));
    struct UF *uf = calloc(n + 1, sizeof(struct UF));

    int idx = 0;
    for (size_t i = 0; i < n; i++) cfg->blocks[i]->dfnum = 0;
    dfs(cfg->entry, vertex, parent, semi, &idx);
    for (int i = 1; i <= idx; i++)
        uf[i].label = i;

    for (int i = idx; i >= 2; i--) {
        BasicBlock *w = vertex[i];
        for (size_t j = 0; j < w->npred; j++) {
            BasicBlock *v = w->pred[j];
            if (!v->dfnum) continue;
            int u = eval(uf, v->dfnum);
            if (semi[u] < semi[i]) semi[i] = semi[u];
        }
        ivec_push(&bucket[semi[i]], i);
        link(uf, parent[w->dfnum], i);
        IntVec *bvec = &bucket[parent[w->dfnum]];
        for (size_t k = 0; k < bvec->len; k++) {
            int v = bvec->data[k];
            int u = eval(uf, v);
            idom[v] = (semi[u] < semi[v]) ? u : parent[w->dfnum];
        }
        bvec->len = 0;
    }
    for (int i = 2; i <= idx; i++) {
        if (idom[i] != semi[i]) idom[i] = idom[idom[i]];
        vertex[i]->idom = vertex[idom[i]];
    }
    vertex[1]->idom = NULL;

    free(vertex); free(parent); free(semi); free(idom);
    for (size_t i = 0; i <= n; i++) free(bucket[i].data);
    free(bucket); free(uf);
    compute_df(cfg);
}
