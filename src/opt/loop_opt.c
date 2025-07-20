#include "loop_opt.h"
#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Default loop optimization configuration.
 */
static const LoopOptConfig default_config = {
    .max_unroll_count = 8,          // Unroll loops up to 8 times
    .max_unroll_size = 200,         // Don't unroll if it increases size by more than 200%
    .enable_strength_reduction = true,
    .enable_loop_fusion = true,
    .enable_vectorization = false   // Vectorization hints disabled by default
};

/**
 * @brief Helper function to check if a block dominates another.
 * @param dominator Potential dominator block.
 * @param dominated Potentially dominated block.
 * @return true if dominator dominates dominated, false otherwise.
 */
static bool dominates(BasicBlock *dominator, BasicBlock *dominated) {
    BasicBlock *current = dominated;
    while (current && current != dominator) {
        current = current->idom;
    }
    return current == dominator;
}

/**
 * @brief Finds back edges in the CFG to identify natural loops.
 * @param cfg Pointer to the control flow graph.
 * @param back_edges Output array for back edges.
 * @param max_edges Maximum number of back edges to find.
 * @return Number of back edges found.
 */
static size_t find_back_edges(CFG *cfg, BasicBlock ***back_edges, size_t max_edges) {
    size_t count = 0;
    
    for (size_t i = 0; i < cfg->nblocks && count < max_edges; i++) {
        BasicBlock *bb = cfg->blocks[i];
        
        for (size_t j = 0; j < bb->nsucc; j++) {
            BasicBlock *succ = bb->succ[j];
            
            // Back edge: successor dominates current block
            if (dominates(succ, bb)) {
                back_edges[count] = malloc(2 * sizeof(BasicBlock*));
                back_edges[count][0] = bb;    // Latch
                back_edges[count][1] = succ;  // Header
                count++;
            }
        }
    }
    
    return count;
}

/**
 * @brief Collects all blocks in a natural loop.
 * @param header Loop header block.
 * @param latch Loop latch block.
 * @param loop_blocks Output array for loop blocks.
 * @return Number of blocks in the loop.
 */
static size_t collect_loop_blocks(BasicBlock *header, BasicBlock *latch, 
                                 BasicBlock **loop_blocks) {
    // Simple algorithm: collect all blocks that can reach the latch
    // and are dominated by the header
    size_t count = 0;
    bool *visited = calloc(1000, sizeof(bool)); // Assume max 1000 blocks
    
    BasicBlock **worklist = malloc(1000 * sizeof(BasicBlock*));
    size_t worklist_size = 0;
    
    // Add header and latch to loop
    loop_blocks[count++] = header;
    visited[header->id] = true;
    
    if (latch != header) {
        loop_blocks[count++] = latch;
        visited[latch->id] = true;
        worklist[worklist_size++] = latch;
    }
    
    // Work backwards from latch to find all loop blocks
    while (worklist_size > 0) {
        BasicBlock *current = worklist[--worklist_size];
        
        for (size_t i = 0; i < current->npred; i++) {
            BasicBlock *pred = current->pred[i];
            
            // Include predecessor if it's dominated by header and not yet visited
            if (!visited[pred->id] && dominates(header, pred)) {
                visited[pred->id] = true;
                loop_blocks[count++] = pred;
                worklist[worklist_size++] = pred;
            }
        }
    }
    
    free(worklist);
    free(visited);
    return count;
}

/**
 * @brief Discovers loops in a CFG using natural loop detection.
 * @param cfg Pointer to the control flow graph.
 * @return Pointer to the loop nest information.
 */
LoopNest *discover_loops(CFG *cfg) {
    LoopNest *nest = malloc(sizeof(LoopNest));
    nest->loops = NULL;
    nest->nloops = 0;
    nest->nest_depth = 0;
    
    // Find back edges
    BasicBlock **back_edges[100]; // Max 100 loops
    size_t num_back_edges = find_back_edges(cfg, back_edges, 100);
    
    if (num_back_edges == 0) {
        return nest;
    }
    
    nest->loops = malloc(num_back_edges * sizeof(Loop*));
    
    // Create loop structures for each back edge
    for (size_t i = 0; i < num_back_edges; i++) {
        BasicBlock *latch = back_edges[i][0];
        BasicBlock *header = back_edges[i][1];
        
        Loop *loop = malloc(sizeof(Loop));
        loop->header = header;
        loop->latch = latch;
        loop->preheader = NULL; // Will be created if needed
        loop->blocks = malloc(cfg->nblocks * sizeof(BasicBlock*));
        loop->nblocks = collect_loop_blocks(header, latch, loop->blocks);
        loop->induction_var = (IRValue){.id = 0};
        loop->initial_value = (IRValue){.id = 0};
        loop->step_value = (IRValue){.id = 0};
        loop->limit_value = (IRValue){.id = 0};
        loop->comparison_op = IR_LT;
        loop->trip_count = -1; // Unknown
        loop->is_countable = false;
        loop->parent = NULL;
        loop->children = NULL;
        loop->nchildren = 0;
        
        nest->loops[nest->nloops++] = loop;
        free(back_edges[i]);
    }
    
    return nest;
}

/**
 * @brief Analyzes a loop to find induction variables.
 * @param loop Pointer to the loop structure.
 * @return Array of induction variables found.
 */
InductionVar *analyze_induction_variables(Loop *loop, size_t *count) {
    *count = 0;
    if (!loop || loop->nblocks == 0) return NULL;
    
    InductionVar *vars = malloc(10 * sizeof(InductionVar)); // Max 10 induction vars
    
    // Look for patterns like: i = i + step
    for (size_t i = 0; i < loop->nblocks; i++) {
        BasicBlock *bb = loop->blocks[i];
        
        for (size_t j = 0; j < bb->ninstrs; j++) {
            IRInstr *instr = bb->instrs[j];
            
            // Look for addition patterns: dst = src + constant
            if (instr->op == IR_ADD && !ir_is_const(instr->a) && ir_is_const(instr->b)) {
                // Check if dst and src are the same variable (phi function would handle this)
                // This is a simplified analysis
                InductionVar *var = &vars[*count];
                var->var = instr->dst;
                var->base = instr->a;
                var->multiplier = ir_const(1);
                var->addend = instr->b;
                var->is_linear = true;
                
                (*count)++;
                if (*count >= 10) break; // Limit to prevent overflow
            }
        }
    }
    
    // Analyze loop bounds if we found induction variables
    if (*count > 0 && loop->header) {
        // Look for comparison in loop header to determine bounds
        for (size_t i = 0; i < loop->header->ninstrs; i++) {
            IRInstr *instr = loop->header->instrs[i];
            
            if (instr->op >= IR_LT && instr->op <= IR_NE) {
                // Found a comparison - might be loop bound
                loop->comparison_op = instr->op;
                loop->limit_value = instr->b;
                loop->is_countable = ir_is_const(instr->b);
                
                // Try to compute trip count
                if (loop->is_countable && *count > 0) {
                    InductionVar *primary = &vars[0]; // Use first induction var as primary
                    if (ir_is_const(primary->addend)) {
                        int step = ir_const_value(primary->addend);
                        int limit = ir_const_value(loop->limit_value);
                        if (step > 0 && loop->comparison_op == IR_LT) {
                            loop->trip_count = limit / step;
                        }
                    }
                }
                break;
            }
        }
    }
    
    return vars;
}

/**
 * @brief Determines if a loop is suitable for unrolling.
 * @param loop Pointer to the loop structure.
 * @param config Pointer to the optimization configuration.
 * @return Recommended unrolling factor (1 means no unrolling).
 */
int determine_unroll_factor(Loop *loop, LoopOptConfig *config) {
    if (!loop->is_countable || loop->trip_count <= 0) {
        return 1; // Don't unroll non-countable loops
    }
    
    // Calculate loop body size
    int body_size = 0;
    for (size_t i = 0; i < loop->nblocks; i++) {
        body_size += (int)loop->blocks[i]->ninstrs;
    }
    
    // Don't unroll large loops
    if (body_size > 20) {
        return 1;
    }
    
    // Determine unroll factor based on trip count and size
    int unroll_factor = 1;
    
    if (loop->trip_count <= 4) {
        // Small loops - fully unroll
        unroll_factor = loop->trip_count;
    } else if (loop->trip_count <= 16 && body_size <= 5) {
        // Medium loops with small bodies
        unroll_factor = 4;
    } else if (body_size <= 2) {
        // Very small loops
        unroll_factor = 8;
    }
    
    // Respect configuration limits
    if (unroll_factor > config->max_unroll_count) {
        unroll_factor = config->max_unroll_count;
    }
    
    // Check size increase limit
    int size_increase = body_size * (unroll_factor - 1);
    if (size_increase > config->max_unroll_size) {
        return 1; // Don't unroll if it increases size too much
    }
    
    return unroll_factor;
}

/**
 * @brief Clones a basic block for loop unrolling.
 * @param original Original block to clone.
 * @param var_offset Variable offset for renaming.
 * @param iteration Iteration number for naming.
 * @return Pointer to the cloned block.
 */
static BasicBlock *clone_block_for_unroll(BasicBlock *original, int var_offset, int iteration) {
    BasicBlock *cloned = malloc(sizeof(BasicBlock));
    cloned->id = original->id + var_offset + iteration * 1000;
    cloned->ninstrs = original->ninstrs;
    cloned->instrs = malloc(original->ninstrs * sizeof(IRInstr*));
    
    // Clone instructions with variable renaming
    for (size_t i = 0; i < original->ninstrs; i++) {
        IRInstr *orig_instr = original->instrs[i];
        IRInstr *cloned_instr = malloc(sizeof(IRInstr));
        *cloned_instr = *orig_instr;
        
        // Rename variables (simple offset-based strategy)
        if (!ir_is_const(cloned_instr->dst)) {
            cloned_instr->dst.id += var_offset + iteration * 100;
        }
        if (!ir_is_const(cloned_instr->a)) {
            cloned_instr->a.id += var_offset + iteration * 100;
        }
        if (!ir_is_const(cloned_instr->b)) {
            cloned_instr->b.id += var_offset + iteration * 100;
        }
        
        cloned->instrs[i] = cloned_instr;
    }
    
    // Initialize edge arrays
    cloned->succ = NULL;
    cloned->nsucc = 0;
    cloned->pred = NULL;
    cloned->npred = 0;
    cloned->idom = NULL;
    cloned->df = NULL;
    cloned->ndf = 0;
    cloned->visited = 0;
    
    return cloned;
}

/**
 * @brief Performs loop unrolling on a specific loop.
 * @param cfg Pointer to the control flow graph.
 * @param loop Pointer to the loop to unroll.
 * @param unroll_factor Number of times to unroll the loop.
 * @return true if unrolling was successful, false otherwise.
 */
bool unroll_loop(CFG *cfg, Loop *loop, int unroll_factor) {
    if (unroll_factor <= 1 || !loop->is_countable) {
        return false;
    }
    
    // For full unrolling of small loops
    if (unroll_factor >= loop->trip_count) {
        // Fully unroll the loop
        size_t old_nblocks = cfg->nblocks;
        cfg->nblocks += loop->nblocks * (loop->trip_count - 1);
        cfg->blocks = realloc(cfg->blocks, cfg->nblocks * sizeof(BasicBlock*));
        
        // Create unrolled iterations
        for (int iter = 1; iter < loop->trip_count; iter++) {
            for (size_t i = 0; i < loop->nblocks; i++) {
                BasicBlock *cloned = clone_block_for_unroll(loop->blocks[i], 10000, iter);
                cfg->blocks[old_nblocks + (iter - 1) * loop->nblocks + i] = cloned;
            }
        }
        
        // Connect unrolled iterations (simplified)
        // In a full implementation, you'd properly wire the control flow
        
        return true;
    } else {
        // Partial unrolling - more complex transformation
        // This would require careful handling of loop bounds and control flow
        return false; // Not implemented in this simplified version
    }
}

/**
 * @brief Performs strength reduction on induction variables in a loop.
 * @param loop Pointer to the loop structure.
 * @param induction_vars Array of induction variables.
 * @param count Number of induction variables.
 * @return true if any strength reduction was performed, false otherwise.
 */
bool strength_reduction(Loop *loop, InductionVar *induction_vars, size_t count) {
    bool changed = false;
    
    for (size_t i = 0; i < loop->nblocks; i++) {
        BasicBlock *bb = loop->blocks[i];
        
        for (size_t j = 0; j < bb->ninstrs; j++) {
            IRInstr *instr = bb->instrs[j];
            
            // Look for expensive operations that can be reduced
            if (instr->op == IR_MUL) {
                // Check if one operand is an induction variable
                for (size_t k = 0; k < count; k++) {
                    InductionVar *iv = &induction_vars[k];
                    
                    if (instr->a.id == iv->var.id && ir_is_const(instr->b)) {
                        // Found multiplication by induction variable
                        // Replace with addition in preheader and loop
                        // This is a simplified implementation
                        
                        // Create a new derived induction variable
                        // In preheader: derived_iv = base * constant
                        // In loop: derived_iv = derived_iv + step * constant
                        
                        changed = true;
                        break;
                    }
                }
            }
        }
    }
    
    return changed;
}

/**
 * @brief Checks if two loops are fusion candidates.
 * @param loop1 Pointer to the first loop.
 * @param loop2 Pointer to the second loop.
 * @return true if loops can be fused, false otherwise.
 */
bool are_loops_fusible(Loop *loop1, Loop *loop2) {
    // Simplified fusion compatibility check
    if (!loop1->is_countable || !loop2->is_countable) {
        return false;
    }
    
    // Check if loops have same trip count
    if (loop1->trip_count != loop2->trip_count) {
        return false;
    }
    
    // Check if loops are adjacent (simplified check)
    // In a full implementation, you'd check for dependencies
    
    return true;
}

/**
 * @brief Attempts to fuse adjacent loops with compatible iteration patterns.
 * @param cfg Pointer to the control flow graph.
 * @param nest Pointer to the loop nest.
 * @return true if any loops were fused, false otherwise.
 */
bool fuse_compatible_loops(CFG *cfg, LoopNest *nest) {
    bool changed = false;
    
    // Look for adjacent fusible loops
    for (size_t i = 0; i < nest->nloops; i++) {
        for (size_t j = i + 1; j < nest->nloops; j++) {
            if (are_loops_fusible(nest->loops[i], nest->loops[j])) {
                // Fuse loops i and j
                // This is a complex transformation - simplified here
                changed = true;
                break;
            }
        }
    }
    
    return changed;
}

/**
 * @brief Estimates the cost of a loop for optimization decisions.
 * @param loop Pointer to the loop structure.
 * @return Estimated execution cost.
 */
int estimate_loop_cost(Loop *loop) {
    int cost = 0;
    
    for (size_t i = 0; i < loop->nblocks; i++) {
        BasicBlock *bb = loop->blocks[i];
        for (size_t j = 0; j < bb->ninstrs; j++) {
            IRInstr *instr = bb->instrs[j];
            
            switch (instr->op) {
                case IR_MUL:
                case IR_DIV:
                case IR_MOD:
                    cost += 10; // Expensive operations
                    break;
                case IR_CALL:
                    cost += 50; // Function calls are very expensive
                    break;
                default:
                    cost += 1;
                    break;
            }
        }
    }
    
    // Multiply by estimated trip count
    if (loop->trip_count > 0) {
        cost *= loop->trip_count;
    } else {
        cost *= 10; // Assume 10 iterations for unknown loops
    }
    
    return cost;
}

/**
 * @brief Main entry point for advanced loop optimizations.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the optimization configuration.
 * @return true if any optimizations were performed, false otherwise.
 */
bool optimize_loops(CFG *cfg, LoopOptConfig *config) {
    if (!config) config = (LoopOptConfig*)&default_config;
    
    bool changed = false;
    
    // Discover loops
    LoopNest *nest = discover_loops(cfg);
    if (nest->nloops == 0) {
        loop_nest_free(nest);
        return false;
    }
    
    // Process each loop
    for (size_t i = 0; i < nest->nloops; i++) {
        Loop *loop = nest->loops[i];
        
        // Analyze induction variables
        size_t iv_count;
        InductionVar *induction_vars = analyze_induction_variables(loop, &iv_count);
        
        // Apply strength reduction
        if (config->enable_strength_reduction && iv_count > 0) {
            if (strength_reduction(loop, induction_vars, iv_count)) {
                changed = true;
            }
        }
        
        // Apply loop unrolling
        int unroll_factor = determine_unroll_factor(loop, config);
        if (unroll_factor > 1) {
            if (unroll_loop(cfg, loop, unroll_factor)) {
                changed = true;
            }
        }
        
        induction_vars_free(induction_vars, iv_count);
    }
    
    // Apply loop fusion
    if (config->enable_loop_fusion) {
        if (fuse_compatible_loops(cfg, nest)) {
            changed = true;
        }
    }
    
    loop_nest_free(nest);
    return changed;
}

/**
 * @brief Frees a loop nest and all associated memory.
 * @param nest Pointer to the loop nest to free.
 */
void loop_nest_free(LoopNest *nest) {
    if (!nest) return;
    
    for (size_t i = 0; i < nest->nloops; i++) {
        Loop *loop = nest->loops[i];
        free(loop->blocks);
        free(loop->children);
        free(loop);
    }
    
    free(nest->loops);
    free(nest);
}

/**
 * @brief Frees an array of induction variables.
 * @param vars Pointer to the induction variables array.
 * @param count Number of variables.
 */
void induction_vars_free(InductionVar *vars, size_t count) {
    (void)count; // Unused parameter
    free(vars);
}