#include "inline.h"
#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Default inlining configuration.
 */
static const InlineConfig default_config = {
    .max_inline_cost = 100,      // Inline functions with cost <= 100
    .max_inline_depth = 3,       // Maximum inlining depth
    .inline_hot_only = false,    // Inline all eligible functions
    .hot_threshold = 5           // Functions called >= 5 times are hot
};

/**
 * @brief Creates a new function table.
 * @return Pointer to the newly created function table.
 */
FunctionTable *function_table_new(void) {
    FunctionTable *table = malloc(sizeof(FunctionTable));
    table->functions = NULL;
    table->nfunctions = 0;
    table->capacity = 0;
    return table;
}

/**
 * @brief Adds a function to the function table.
 * @param table Pointer to the function table.
 * @param func Pointer to the function info to add.
 * @return Function ID assigned to the added function.
 */
int function_table_add(FunctionTable *table, FunctionInfo *func) {
    if (table->nfunctions >= table->capacity) {
        table->capacity = table->capacity == 0 ? 8 : table->capacity * 2;
        table->functions = realloc(table->functions, 
                                  table->capacity * sizeof(FunctionInfo));
    }
    
    func->id = (int)table->nfunctions;
    table->functions[table->nfunctions] = *func;
    return (int)table->nfunctions++;
}

/**
 * @brief Retrieves function info by ID.
 * @param table Pointer to the function table.
 * @param func_id Function identifier.
 * @return Pointer to function info, or NULL if not found.
 */
FunctionInfo *function_table_get(FunctionTable *table, int func_id) {
    if (func_id < 0 || func_id >= (int)table->nfunctions) {
        return NULL;
    }
    return &table->functions[func_id];
}

/**
 * @brief Calculates the inlining cost for a function.
 * @param func Pointer to the function info.
 * @return Estimated cost of inlining this function.
 */
int calculate_inline_cost(FunctionInfo *func) {
    if (!func->cfg) return 1000; // High cost for unknown functions
    
    int cost = 0;
    
    // Count instructions in all basic blocks
    for (size_t i = 0; i < func->cfg->nblocks; i++) {
        BasicBlock *bb = func->cfg->blocks[i];
        for (size_t j = 0; j < bb->ninstrs; j++) {
            IRInstr *instr = bb->instrs[j];
            
            // Assign costs based on instruction type
            switch (instr->op) {
                case IR_CALL:
                    cost += 20; // Function calls are expensive
                    break;
                case IR_MUL:
                case IR_DIV:
                case IR_MOD:
                    cost += 5; // Arithmetic operations have medium cost
                    break;
                case IR_JUMP:
                case IR_CJUMP:
                    cost += 2; // Control flow has low cost
                    break;
                case IR_PHI:
                case IR_MOV:
                    cost += 1; // Simple operations have minimal cost
                    break;
                default:
                    cost += 3; // Default cost for other operations
                    break;
            }
        }
    }
    
    // Penalty for functions with many basic blocks (complex control flow)
    if (func->cfg->nblocks > 5) {
        cost += (int)(func->cfg->nblocks - 5) * 10;
    }
    
    // Bonus for leaf functions (no function calls)
    bool is_leaf = true;
    for (size_t i = 0; i < func->cfg->nblocks && is_leaf; i++) {
        BasicBlock *bb = func->cfg->blocks[i];
        for (size_t j = 0; j < bb->ninstrs; j++) {
            if (bb->instrs[j]->op == IR_CALL) {
                is_leaf = false;
                break;
            }
        }
    }
    if (is_leaf) {
        cost = cost * 8 / 10; // 20% bonus for leaf functions
    }
    
    func->inline_cost = cost;
    return cost;
}

/**
 * @brief Determines if a function should be inlined at a call site.
 * @param func Pointer to the function info.
 * @param config Pointer to the inlining configuration.
 * @param call_depth Current inlining depth.
 * @return true if the function should be inlined, false otherwise.
 */
bool should_inline(FunctionInfo *func, InlineConfig *config, int call_depth) {
    if (!func || !func->cfg) return false;
    
    // Don't inline recursive functions to avoid infinite expansion
    if (func->is_recursive) return false;
    
    // Respect maximum inlining depth
    if (call_depth >= config->max_inline_depth) return false;
    
    // Calculate cost if not already done
    if (func->inline_cost == 0) {
        calculate_inline_cost(func);
    }
    
    // Check cost threshold
    if (func->inline_cost > config->max_inline_cost) return false;
    
    // If only inlining hot functions, check call count
    if (config->inline_hot_only && func->call_count < config->hot_threshold) {
        return false;
    }
    
    // Very small functions should always be inlined
    if (func->inline_cost <= 10) return true;
    
    // For larger functions, consider call frequency
    if (func->call_count > 0) {
        // Higher call count makes inlining more beneficial
        int adjusted_cost = func->inline_cost / (func->call_count + 1);
        return adjusted_cost <= config->max_inline_cost / 2;
    }
    
    return true;
}

/**
 * @brief Helper function to rename variables during inlining.
 * @param val Original value to rename.
 * @param var_offset Variable ID offset for renaming.
 * @param param_mapping Mapping from parameter IDs to argument values.
 * @param nparam Number of parameters.
 * @return Renamed value.
 */
static IRValue rename_value(IRValue val, int var_offset, 
                           IRValue *param_mapping, size_t nparam) {
    if (ir_is_const(val)) return val;
    
    // Check if this is a parameter
    for (size_t i = 0; i < nparam; i++) {
        if (param_mapping[i].id == val.id) {
            return param_mapping[i]; // Use argument value
        }
    }
    
    // Regular variable - add offset
    return (IRValue){.id = val.id + var_offset};
}

/**
 * @brief Clones an IR instruction with variable renaming.
 * @param instr Original instruction to clone.
 * @param var_offset Variable ID offset for renaming.
 * @param param_mapping Mapping from parameter IDs to argument values.
 * @param nparam Number of parameters.
 * @return Pointer to the cloned instruction.
 */
static IRInstr *clone_instr_with_renaming(IRInstr *instr, int var_offset,
                                         IRValue *param_mapping, size_t nparam) {
    IRInstr *cloned = malloc(sizeof(IRInstr));
    *cloned = *instr; // Copy all fields
    
    cloned->dst = rename_value(cloned->dst, var_offset, param_mapping, nparam);
    cloned->a = rename_value(cloned->a, var_offset, param_mapping, nparam);
    cloned->b = rename_value(cloned->b, var_offset, param_mapping, nparam);
    
    // Handle call instructions specially
    if (instr->op == IR_CALL && instr->extra.call.args) {
        cloned->extra.call.args = malloc(sizeof(IRValue) * instr->extra.call.nargs);
        for (size_t i = 0; i < instr->extra.call.nargs; i++) {
            cloned->extra.call.args[i] = rename_value(instr->extra.call.args[i], 
                                                     var_offset, param_mapping, nparam);
        }
    }
    
    return cloned;
}

/**
 * @brief Clones a CFG for inlining, renaming variables to avoid conflicts.
 * @param source Pointer to the source CFG to clone.
 * @param var_offset Variable ID offset for renaming.
 * @param param_mapping Mapping from parameter IDs to argument values.
 * @param nparam Number of parameters.
 * @return Pointer to the cloned CFG.
 */
CFG *clone_cfg_for_inline(CFG *source, int var_offset, 
                          IRValue *param_mapping, size_t nparam) {
    CFG *cloned = malloc(sizeof(CFG));
    cloned->nblocks = source->nblocks;
    cloned->blocks = malloc(sizeof(BasicBlock*) * source->nblocks);
    
    // First pass: Create all basic blocks
    for (size_t i = 0; i < source->nblocks; i++) {
        BasicBlock *src_bb = source->blocks[i];
        BasicBlock *cloned_bb = malloc(sizeof(BasicBlock));
        
        *cloned_bb = *src_bb; // Copy metadata
        cloned_bb->id += var_offset; // Rename block ID
        
        // Clone instructions
        cloned_bb->instrs = malloc(sizeof(IRInstr*) * src_bb->ninstrs);
        for (size_t j = 0; j < src_bb->ninstrs; j++) {
            cloned_bb->instrs[j] = clone_instr_with_renaming(
                src_bb->instrs[j], var_offset, param_mapping, nparam);
        }
        
        cloned->blocks[i] = cloned_bb;
    }
    
    // Second pass: Fix successor/predecessor relationships
    for (size_t i = 0; i < source->nblocks; i++) {
        BasicBlock *src_bb = source->blocks[i];
        BasicBlock *cloned_bb = cloned->blocks[i];
        
        // Clone successor array
        if (src_bb->nsucc > 0) {
            cloned_bb->succ = malloc(sizeof(BasicBlock*) * src_bb->nsucc);
            for (size_t j = 0; j < src_bb->nsucc; j++) {
                // Find corresponding cloned successor
                for (size_t k = 0; k < source->nblocks; k++) {
                    if (source->blocks[k] == src_bb->succ[j]) {
                        cloned_bb->succ[j] = cloned->blocks[k];
                        break;
                    }
                }
            }
        }
        
        // Clone predecessor array
        if (src_bb->npred > 0) {
            cloned_bb->pred = malloc(sizeof(BasicBlock*) * src_bb->npred);
            for (size_t j = 0; j < src_bb->npred; j++) {
                // Find corresponding cloned predecessor
                for (size_t k = 0; k < source->nblocks; k++) {
                    if (source->blocks[k] == src_bb->pred[j]) {
                        cloned_bb->pred[j] = cloned->blocks[k];
                        break;
                    }
                }
            }
        }
    }
    
    // Set entry block
    for (size_t i = 0; i < source->nblocks; i++) {
        if (source->blocks[i] == source->entry) {
            cloned->entry = cloned->blocks[i];
            break;
        }
    }
    
    return cloned;
}

/**
 * @brief Inlines a specific function at a call site.
 * @param caller_cfg Pointer to the caller's CFG.
 * @param call_block Pointer to the block containing the call.
 * @param call_instr Pointer to the call instruction.
 * @param callee Pointer to the function to inline.
 * @return true if inlining was successful, false otherwise.
 */
bool inline_function_at_site(CFG *caller_cfg, BasicBlock *call_block, 
                             IRInstr *call_instr, FunctionInfo *callee) {
    if (!callee->cfg || call_instr->op != IR_CALL) return false;
    
    // Create parameter mapping from arguments to parameters
    IRValue *param_mapping = malloc(sizeof(IRValue) * callee->nparam);
    for (size_t i = 0; i < callee->nparam; i++) {
        if (i < call_instr->extra.call.nargs) {
            param_mapping[i] = call_instr->extra.call.args[i];
        } else {
            param_mapping[i] = ir_const(0); // Default parameter value
        }
    }
    
    // Generate unique variable offset for inlined function
    int var_offset = 1000; // Simple offset strategy
    
    // Clone the callee's CFG
    CFG *inlined_cfg = clone_cfg_for_inline(callee->cfg, var_offset, 
                                           param_mapping, callee->nparam);
    
    // Find call instruction position in block
    size_t call_pos = 0;
    for (size_t i = 0; i < call_block->ninstrs; i++) {
        if (call_block->instrs[i] == call_instr) {
            call_pos = i;
            break;
        }
    }
    
    // Split the call block at the call instruction
    BasicBlock *after_call_block = malloc(sizeof(BasicBlock));
    after_call_block->id = var_offset + 999;
    after_call_block->ninstrs = call_block->ninstrs - call_pos - 1;
    after_call_block->instrs = malloc(sizeof(IRInstr*) * after_call_block->ninstrs);
    
    // Move instructions after call to new block
    for (size_t i = 0; i < after_call_block->ninstrs; i++) {
        after_call_block->instrs[i] = call_block->instrs[call_pos + 1 + i];
    }
    
    // Truncate call block before call instruction
    call_block->ninstrs = call_pos;
    call_block->instrs = realloc(call_block->instrs, 
                                sizeof(IRInstr*) * call_pos);
    
    // Connect call block to inlined function entry
    call_block->nsucc = 1;
    call_block->succ = malloc(sizeof(BasicBlock*));
    call_block->succ[0] = inlined_cfg->entry;
    
    // Add inlined CFG blocks to caller CFG
    size_t old_nblocks = caller_cfg->nblocks;
    caller_cfg->nblocks += inlined_cfg->nblocks + 1; // +1 for after_call_block
    caller_cfg->blocks = realloc(caller_cfg->blocks, 
                                sizeof(BasicBlock*) * caller_cfg->nblocks);
    
    // Add inlined blocks
    for (size_t i = 0; i < inlined_cfg->nblocks; i++) {
        caller_cfg->blocks[old_nblocks + i] = inlined_cfg->blocks[i];
    }
    
    // Add after-call block
    caller_cfg->blocks[caller_cfg->nblocks - 1] = after_call_block;
    
    // Find and redirect return instructions to after-call block
    for (size_t i = 0; i < inlined_cfg->nblocks; i++) {
        BasicBlock *bb = inlined_cfg->blocks[i];
        for (size_t j = 0; j < bb->ninstrs; j++) {
            IRInstr *instr = bb->instrs[j];
            if (instr->op == IR_RETURN) {
                // Replace return with move to call result variable
                if (call_instr->dst.id != 0) { // Has return value
                    instr->op = IR_MOV;
                    instr->dst = call_instr->dst;
                    // instr->a remains the return value
                    instr->b = (IRValue){.id = 0}; // Unused
                } else {
                    // Void return - replace with NOP
                    instr->op = IR_NOP;
                }
                
                // Add jump to after-call block
                bb->nsucc = 1;
                bb->succ = malloc(sizeof(BasicBlock*));
                bb->succ[0] = after_call_block;
            }
        }
    }
    
    free(param_mapping);
    free(inlined_cfg->blocks); // Don't free individual blocks, they're now in caller
    free(inlined_cfg);
    
    return true;
}

/**
 * @brief Performs function inlining on a CFG.
 * @param cfg Pointer to the control flow graph to optimize.
 * @param table Pointer to the function table.
 * @param config Pointer to the inlining configuration.
 * @return true if any inlining was performed, false otherwise.
 */
bool inline_functions(CFG *cfg, FunctionTable *table, InlineConfig *config) {
    if (!cfg || !table) return false;
    
    if (!config) config = (InlineConfig*)&default_config;
    
    bool changed = false;
    bool made_changes;
    
    // Iterate until no more inlining opportunities
    do {
        made_changes = false;
        
        // Scan all basic blocks for function calls
        for (size_t i = 0; i < cfg->nblocks; i++) {
            BasicBlock *bb = cfg->blocks[i];
            
            for (size_t j = 0; j < bb->ninstrs; j++) {
                IRInstr *instr = bb->instrs[j];
                
                if (instr->op == IR_CALL) {
                    FunctionInfo *callee = function_table_get(table, 
                                                             instr->extra.call.func_id);
                    
                    if (callee && should_inline(callee, config, 0)) {
                        if (inline_function_at_site(cfg, bb, instr, callee)) {
                            made_changes = true;
                            changed = true;
                            // Restart scanning since CFG structure changed
                            goto restart_scan;
                        }
                    }
                }
            }
        }
        
        restart_scan:;
    } while (made_changes);
    
    return changed;
}

/**
 * @brief Frees a function table and all associated memory.
 * @param table Pointer to the function table to free.
 */
void function_table_free(FunctionTable *table) {
    if (!table) return;
    
    for (size_t i = 0; i < table->nfunctions; i++) {
        free(table->functions[i].name);
        free(table->functions[i].params);
    }
    
    free(table->functions);
    free(table);
}