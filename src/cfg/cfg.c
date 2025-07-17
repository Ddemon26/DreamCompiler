#include "cfg.h"
#include <stdlib.h>

/**
 * @brief Creates a new control flow graph (CFG).
 *
 * Allocates and initializes a new CFG structure.
 *
 * @return A pointer to the newly created CFG.
 */
CFG *cfg_new(void) {
    CFG *cfg = calloc(1, sizeof(CFG));
    return cfg;
}

/**
 * @brief Adds a new basic block to the control flow graph (CFG).
 *
 * Allocates and initializes a new basic block, assigns it a unique ID,
 * and appends it to the list of blocks in the CFG. If the CFG has no
 * entry block, the new block is set as the entry.
 *
 * @param cfg The control flow graph to add the block to.
 * @return A pointer to the newly created basic block.
 */
BasicBlock *cfg_add_block(CFG *cfg) {
    BasicBlock *bb = calloc(1, sizeof(BasicBlock));
    bb->id = cfg->nblocks;
    cfg->blocks = realloc(cfg->blocks, sizeof(BasicBlock*) * (cfg->nblocks + 1));
    cfg->blocks[cfg->nblocks++] = bb;
    if (!cfg->entry) cfg->entry = bb;
    return bb;
}

/**
 * @brief Adds a directed edge between two basic blocks in the control flow graph (CFG).
 *
 * Updates the successor list of the source block and the predecessor list
 * of the destination block to reflect the new edge.
 *
 * @param from The source basic block.
 * @param to The destination basic block.
 */
void cfg_add_edge(BasicBlock *from, BasicBlock *to) {
    from->succ = realloc(from->succ, sizeof(BasicBlock*) * (from->nsucc + 1));
    from->succ[from->nsucc++] = to;
    to->pred = realloc(to->pred, sizeof(BasicBlock*) * (to->npred + 1));
    to->pred[to->npred++] = from;
}

/**
 * @brief Represents a dynamic array of integers.
 *
 * A structure that holds a pointer to an array of integers
 * and its current length.
 */
typedef struct { int *data; size_t len; } IntVec;

/**
 * @brief Appends an integer to a dynamic array.
 *
 * Resizes the array if necessary and adds the given integer
 * to the end of the array.
 *
 * @param v The dynamic array to append to.
 * @param x The integer to append.
 */
static void ivec_push(IntVec *v, int x) {
    v->data = realloc(v->data, sizeof(int)*(v->len+1));
    v->data[v->len++] = x;
}

/**
 * @brief Computes the dominance frontier for each basic block in the control flow graph (CFG).
 *
 * Iterates through all basic blocks in the CFG, excluding the entry block,
 * and calculates their dominance frontier by traversing their predecessors
 * and updating the dominance frontier of intermediate blocks.
 *
 * @param cfg The control flow graph for which to compute the dominance frontier.
 */
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

/**
 * @brief Performs a depth-first search (DFS) on the control flow graph (CFG).
 *
 * Traverses the CFG starting from the given basic block, assigning
 * depth-first numbers and updating parent and semi-dominator arrays.
 *
 * @param b The starting basic block for the DFS.
 * @param vertex An array to store the order of visited basic blocks.
 * @param parent An array to store the parent of each basic block in the DFS tree.
 * @param semi An array to store the semi-dominator of each basic block.
 * @param idx A pointer to the current DFS index.
 */
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

/**
 * @brief Represents a union-find data structure for dominance computation.
 *
 * Stores the ancestor and label of a node in the union-find structure.
 */
struct UF { int ancestor; int label; };

/**
 * @brief Evaluates the label of a node in the union-find structure.
 *
 * Traverses the ancestor chain of the given node, performing path compression
 * and updating the label to the smallest value encountered.
 *
 * @param uf The union-find data structure.
 * @param v The index of the node to evaluate.
 * @return The label of the node after evaluation.
 */
static int eval(struct UF *uf, int v) {
    if (uf[v].ancestor == 0) return uf[v].label;
    int compress = eval(uf, uf[v].ancestor);
    if (uf[uf[v].ancestor].label < uf[v].label)
        uf[v].label = uf[uf[v].ancestor].label;
    uf[v].ancestor = compress;
    return uf[v].label;
}

/**
 * @brief Links a child node to a parent node in the union-find structure.
 *
 * Updates the ancestor of the child node to point to the parent node.
 *
 * @param uf The union-find data structure.
 * @param parent The parent node.
 * @param child The child node.
 */
static void link(struct UF *uf, int parent, int child) {
    uf[child].ancestor = parent;
}

/**
 * @brief Computes the dominator tree for the control flow graph (CFG).
 *
 * Uses a semi-dominators based algorithm to calculate the immediate dominators
 * for each basic block in the CFG and updates the dominance frontier.
 *
 * @param cfg The control flow graph for which to compute the dominator tree.
 */
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
