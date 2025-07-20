#include "regalloc.h"
#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Helper function to check if a variable is in a set.
 * @param var Variable to check.
 * @param set Array of variables.
 * @param count Number of variables in the set.
 * @return true if variable is in the set, false otherwise.
 */
static bool is_in_set(IRValue var, IRValue *set, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (set[i].id == var.id) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Adds a variable to a set if not already present.
 * @param var Variable to add.
 * @param set Pointer to the set array.
 * @param count Pointer to the count.
 * @param capacity Pointer to the capacity.
 */
static void add_to_set(IRValue var, IRValue **set, size_t *count, size_t *capacity) {
    if (ir_is_const(var) || is_in_set(var, *set, *count)) {
        return;
    }
    
    if (*count >= *capacity) {
        *capacity = *capacity == 0 ? 8 : *capacity * 2;
        *set = realloc(*set, *capacity * sizeof(IRValue));
    }
    
    (*set)[(*count)++] = var;
}

/**
 * @brief Computes def and use sets for a basic block.
 * @param bb Pointer to the basic block.
 * @param liveness Pointer to the liveness information to fill.
 */
static void compute_def_use_sets(BasicBlock *bb, LivenessInfo *liveness) {
    size_t def_capacity = 0, use_capacity = 0;
    liveness->def = NULL;
    liveness->def_count = 0;
    liveness->use = NULL;
    liveness->use_count = 0;
    
    for (size_t i = 0; i < bb->ninstrs; i++) {
        IRInstr *instr = bb->instrs[i];
        
        // Add uses (operands read before any definition in this block)
        if (!is_in_set(instr->a, liveness->def, liveness->def_count)) {
            add_to_set(instr->a, &liveness->use, &liveness->use_count, &use_capacity);
        }
        if (!is_in_set(instr->b, liveness->def, liveness->def, liveness->def_count)) {
            add_to_set(instr->b, &liveness->use, &liveness->use_count, &use_capacity);
        }
        
        // Handle call instruction arguments
        if (instr->op == IR_CALL && instr->extra.call.args) {
            for (size_t j = 0; j < instr->extra.call.nargs; j++) {
                if (!is_in_set(instr->extra.call.args[j], liveness->def, liveness->def_count)) {
                    add_to_set(instr->extra.call.args[j], &liveness->use, 
                              &liveness->use_count, &use_capacity);
                }
            }
        }
        
        // Add definition (variable assigned in this block)
        add_to_set(instr->dst, &liveness->def, &liveness->def_count, &def_capacity);
    }
}

/**
 * @brief Performs live variable analysis on a CFG.
 * @param cfg Pointer to the control flow graph.
 * @return Array of liveness information for each basic block.
 */
LivenessInfo *analyze_liveness(CFG *cfg) {
    if (!cfg) return NULL;
    
    LivenessInfo *liveness = calloc(cfg->nblocks, sizeof(LivenessInfo));
    
    // Compute def and use sets for each block
    for (size_t i = 0; i < cfg->nblocks; i++) {
        compute_def_use_sets(cfg->blocks[i], &liveness[i]);
    }
    
    // Initialize live_in and live_out sets
    for (size_t i = 0; i < cfg->nblocks; i++) {
        liveness[i].live_in = NULL;
        liveness[i].live_in_count = 0;
        liveness[i].live_out = NULL;
        liveness[i].live_out_count = 0;
    }
    
    // Iterative dataflow analysis
    bool changed;
    do {
        changed = false;
        
        // Process blocks in reverse order for better convergence
        for (int i = (int)cfg->nblocks - 1; i >= 0; i--) {
            BasicBlock *bb = cfg->blocks[i];
            LivenessInfo *live = &liveness[i];
            
            // Compute new live_out: union of live_in of all successors
            size_t old_live_out_count = live->live_out_count;
            IRValue *old_live_out = malloc(old_live_out_count * sizeof(IRValue));
            memcpy(old_live_out, live->live_out, old_live_out_count * sizeof(IRValue));
            
            free(live->live_out);
            live->live_out = NULL;
            live->live_out_count = 0;
            size_t live_out_capacity = 0;
            
            for (size_t j = 0; j < bb->nsucc; j++) {
                BasicBlock *succ = bb->succ[j];
                // Find successor's liveness info
                for (size_t k = 0; k < cfg->nblocks; k++) {
                    if (cfg->blocks[k] == succ) {
                        LivenessInfo *succ_live = &liveness[k];
                        for (size_t l = 0; l < succ_live->live_in_count; l++) {
                            add_to_set(succ_live->live_in[l], &live->live_out,
                                      &live->live_out_count, &live_out_capacity);
                        }
                        break;
                    }
                }
            }
            
            // Compute new live_in: use ∪ (live_out - def)
            size_t old_live_in_count = live->live_in_count;
            IRValue *old_live_in = malloc(old_live_in_count * sizeof(IRValue));
            memcpy(old_live_in, live->live_in, old_live_in_count * sizeof(IRValue));
            
            free(live->live_in);
            live->live_in = NULL;
            live->live_in_count = 0;
            size_t live_in_capacity = 0;
            
            // Add use set
            for (size_t j = 0; j < live->use_count; j++) {
                add_to_set(live->use[j], &live->live_in, &live->live_in_count, &live_in_capacity);
            }
            
            // Add live_out - def
            for (size_t j = 0; j < live->live_out_count; j++) {
                if (!is_in_set(live->live_out[j], live->def, live->def_count)) {
                    add_to_set(live->live_out[j], &live->live_in, 
                              &live->live_in_count, &live_in_capacity);
                }
            }
            
            // Check if anything changed
            if (live->live_in_count != old_live_in_count || 
                live->live_out_count != old_live_out_count) {
                changed = true;
            } else {
                // Check if contents changed
                for (size_t j = 0; j < live->live_in_count; j++) {
                    if (!is_in_set(live->live_in[j], old_live_in, old_live_in_count)) {
                        changed = true;
                        break;
                    }
                }
                for (size_t j = 0; j < live->live_out_count; j++) {
                    if (!is_in_set(live->live_out[j], old_live_out, old_live_out_count)) {
                        changed = true;
                        break;
                    }
                }
            }
            
            free(old_live_in);
            free(old_live_out);
        }
    } while (changed);
    
    return liveness;
}

/**
 * @brief Builds an interference graph from liveness information.
 * @param cfg Pointer to the control flow graph.
 * @param liveness Array of liveness information.
 * @return Pointer to the interference graph.
 */
InterferenceGraph *build_interference_graph(CFG *cfg, LivenessInfo *liveness) {
    InterferenceGraph *graph = malloc(sizeof(InterferenceGraph));
    graph->variables = NULL;
    graph->nvars = 0;
    graph->edges = NULL;
    graph->nedges = 0;
    graph->colors = NULL;
    graph->spilled = NULL;
    
    // Collect all variables
    size_t var_capacity = 0;
    for (size_t i = 0; i < cfg->nblocks; i++) {
        LivenessInfo *live = &liveness[i];
        for (size_t j = 0; j < live->def_count; j++) {
            add_to_set(live->def[j], &graph->variables, &graph->nvars, &var_capacity);
        }
        for (size_t j = 0; j < live->use_count; j++) {
            add_to_set(live->use[j], &graph->variables, &graph->nvars, &var_capacity);
        }
    }
    
    // Build interference edges
    size_t edge_capacity = 0;
    for (size_t i = 0; i < cfg->nblocks; i++) {
        LivenessInfo *live = &liveness[i];
        
        // Variables that are live at the same time interfere
        for (size_t j = 0; j < live->live_out_count; j++) {
            for (size_t k = j + 1; k < live->live_out_count; k++) {
                if (graph->nedges >= edge_capacity) {
                    edge_capacity = edge_capacity == 0 ? 16 : edge_capacity * 2;
                    graph->edges = realloc(graph->edges, edge_capacity * sizeof(Interference));
                }
                
                graph->edges[graph->nedges].var1 = live->live_out[j];
                graph->edges[graph->nedges].var2 = live->live_out[k];
                graph->edges[graph->nedges].weight = 1.0;
                graph->nedges++;
            }
        }
    }
    
    // Initialize coloring arrays
    graph->colors = malloc(graph->nvars * sizeof(int));
    graph->spilled = calloc(graph->nvars, sizeof(bool));
    for (size_t i = 0; i < graph->nvars; i++) {
        graph->colors[i] = -1; // No color assigned
    }
    
    return graph;
}

/**
 * @brief Performs simplified graph coloring register allocation.
 * @param graph Pointer to the interference graph.
 * @param num_registers Number of available registers.
 * @return true if all variables were allocated registers, false if spilling needed.
 */
bool graph_coloring_allocation(InterferenceGraph *graph, int num_registers) {
    if (!graph || num_registers <= 0) return false;
    
    // Simplified greedy coloring algorithm
    for (size_t i = 0; i < graph->nvars; i++) {
        IRValue var = graph->variables[i];
        bool *used_colors = calloc(num_registers, sizeof(bool));
        
        // Check which colors are used by interfering variables
        for (size_t j = 0; j < graph->nedges; j++) {
            Interference *edge = &graph->edges[j];
            
            if (edge->var1.id == var.id) {
                // Find color of var2
                for (size_t k = 0; k < graph->nvars; k++) {
                    if (graph->variables[k].id == edge->var2.id && graph->colors[k] >= 0) {
                        used_colors[graph->colors[k]] = true;
                        break;
                    }
                }
            } else if (edge->var2.id == var.id) {
                // Find color of var1
                for (size_t k = 0; k < graph->nvars; k++) {
                    if (graph->variables[k].id == edge->var1.id && graph->colors[k] >= 0) {
                        used_colors[graph->colors[k]] = true;
                        break;
                    }
                }
            }
        }
        
        // Find first available color
        int color = -1;
        for (int c = 0; c < num_registers; c++) {
            if (!used_colors[c]) {
                color = c;
                break;
            }
        }
        
        if (color >= 0) {
            graph->colors[i] = color;
        } else {
            graph->spilled[i] = true;
        }
        
        free(used_colors);
    }
    
    // Check if any variables were spilled
    for (size_t i = 0; i < graph->nvars; i++) {
        if (graph->spilled[i]) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Converts SSA form to conventional form for register allocation.
 * @param cfg Pointer to the control flow graph.
 * @return true if conversion was successful, false otherwise.
 */
bool convert_from_ssa(CFG *cfg) {
    bool changed = false;
    
    // Convert phi functions to parallel copies
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *bb = cfg->blocks[i];
        
        for (size_t j = 0; j < bb->ninstrs; j++) {
            IRInstr *instr = bb->instrs[j];
            
            if (instr->op == IR_PHI) {
                // Replace phi with move instructions in predecessors
                // This is a simplified implementation
                instr->op = IR_MOV; // Convert phi to move from first operand
                changed = true;
            }
        }
    }
    
    return changed;
}

/**
 * @brief Prepares the IR for future direct machine code generation.
 * @param cfg Pointer to the control flow graph.
 * @return Pointer to register allocation context.
 */
RegAllocContext *prepare_for_machine_code(CFG *cfg) {
    RegAllocContext *context = malloc(sizeof(RegAllocContext));
    context->cfg = cfg;
    context->num_registers = 8; // Assume 8 general-purpose registers
    
    // Convert from SSA form
    convert_from_ssa(cfg);
    
    // Perform liveness analysis
    context->liveness = analyze_liveness(cfg);
    
    // Build interference graph
    context->graph = build_interference_graph(cfg, context->liveness);
    
    // Attempt register allocation
    if (!graph_coloring_allocation(context->graph, context->num_registers)) {
        // Some variables need to be spilled
        // In a full implementation, we'd insert spill code and retry
    }
    
    context->spill_temps = NULL;
    context->nspill_temps = 0;
    
    return context;
}

/**
 * @brief Frees liveness information.
 * @param liveness Pointer to liveness information array.
 * @param nblocks Number of basic blocks.
 */
void liveness_info_free(LivenessInfo *liveness, size_t nblocks) {
    if (!liveness) return;
    
    for (size_t i = 0; i < nblocks; i++) {
        free(liveness[i].live_in);
        free(liveness[i].live_out);
        free(liveness[i].def);
        free(liveness[i].use);
    }
    
    free(liveness);
}

/**
 * @brief Frees an interference graph.
 * @param graph Pointer to the interference graph.
 */
void interference_graph_free(InterferenceGraph *graph) {
    if (!graph) return;
    
    free(graph->variables);
    free(graph->edges);
    free(graph->colors);
    free(graph->spilled);
    free(graph);
}

/**
 * @brief Frees a register allocation context.
 * @param context Pointer to the register allocation context.
 */
void regalloc_context_free(RegAllocContext *context) {
    if (!context) return;
    
    liveness_info_free(context->liveness, context->cfg->nblocks);
    interference_graph_free(context->graph);
    free(context->spill_temps);
    free(context);
}