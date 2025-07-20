#include "pipeline.h"
#include "sccp.h"
#include "dce.h"
#include "value_numbering.h"
#include "licm.h"
#include "copy_prop.h"
#include "cse.h"
#include "peephole.h"
#include "inline.h"
#include "loop_opt.h"
#include <stdbool.h>
#include <stdlib.h>
/**
 * @brief Executes a series of optimization passes on a control flow graph (CFG).
 *
 * This function runs a pipeline of optimizations, including Sparse Conditional
 * Constant Propagation (SCCP), Dead Code Elimination (DCE), Value Numbering,
 * and Loop-Invariant Code Motion (LICM). Higher optimization levels repeat
 * the passes until no further changes occur.
 *
 * @param cfg Pointer to the control flow graph to optimize.
 * @param opt_level Optimization level controlling how aggressively the
 * passes are applied.
 */
static void mark_reachable(CFG *cfg, BasicBlock *b) {
    if (!b || b->visited) return;
    b->visited = 1;
    for (size_t i = 0; i < b->nsucc; i++)
        mark_reachable(cfg, b->succ[i]);
}

static bool remove_unreachable(CFG *cfg) {
    for (size_t i = 0; i < cfg->nblocks; i++)
        cfg->blocks[i]->visited = 0;
    mark_reachable(cfg, cfg->entry);
    bool changed = false;
    size_t w = 0;
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *b = cfg->blocks[i];
        if (!b->visited) {
            free(b->instrs);
            free(b->succ);
            free(b->pred);
            free(b->df);
            changed = true;
            continue;
        }
        cfg->blocks[w++] = b;
    }
    cfg->nblocks = w;
    return changed;
}

void run_pipeline(CFG *cfg, int opt_level) {
    if (!cfg || opt_level <= 0) return;
    bool changed;
    do {
        changed = false;
        changed |= sccp(cfg);
        changed |= remove_unreachable(cfg);
        changed |= dce(cfg);
        changed |= copy_propagation(cfg);
        changed |= value_numbering(cfg);
        changed |= cse(cfg);
        
        // Advanced loop optimizations (unrolling, strength reduction, fusion)
        if (opt_level >= 2) {
            LoopOptConfig loop_config = {
                .max_unroll_count = opt_level >= 3 ? 8 : 4,
                .max_unroll_size = opt_level >= 3 ? 200 : 100,
                .enable_strength_reduction = true,
                .enable_loop_fusion = opt_level >= 3,
                .enable_vectorization = false
            };
            changed |= optimize_loops(cfg, &loop_config);
        }
        
        changed |= licm(cfg);
        changed |= peephole(cfg);
    } while (opt_level >= 2 && changed);
}

void run_pipeline_with_inlining(CFG *cfg, FunctionTable *func_table, int opt_level) {
    if (!cfg || opt_level <= 0) return;
    bool changed;
    
    // Run initial optimization passes to clean up the IR before inlining
    sccp(cfg);
    remove_unreachable(cfg);
    dce(cfg);
    
    do {
        changed = false;
        
        // Run inlining pass early, after basic optimizations but before aggressive ones
        if (func_table && opt_level >= 1) {
            InlineConfig config = {
                .max_inline_cost = opt_level >= 3 ? 150 : 100,
                .max_inline_depth = opt_level >= 3 ? 5 : 3,
                .inline_hot_only = opt_level <= 1,
                .hot_threshold = 3
            };
            changed |= inline_functions(cfg, func_table, &config);
        }
        
        // Standard optimization passes
        changed |= sccp(cfg);
        changed |= remove_unreachable(cfg);
        changed |= dce(cfg);
        changed |= copy_propagation(cfg);
        changed |= value_numbering(cfg);
        changed |= cse(cfg);
        
        // Advanced loop optimizations (unrolling, strength reduction, fusion)
        if (opt_level >= 2) {
            LoopOptConfig loop_config = {
                .max_unroll_count = opt_level >= 3 ? 8 : 4,
                .max_unroll_size = opt_level >= 3 ? 200 : 100,
                .enable_strength_reduction = true,
                .enable_loop_fusion = opt_level >= 3,
                .enable_vectorization = false
            };
            changed |= optimize_loops(cfg, &loop_config);
        }
        
        changed |= licm(cfg);
        changed |= peephole(cfg);
        
    } while (opt_level >= 2 && changed);
}
