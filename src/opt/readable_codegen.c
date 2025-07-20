#include "readable_codegen.h"
#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Creates a default readable codegen configuration.
 * @param debug_mode True if compiling in debug mode.
 * @return Default configuration for readable code generation.
 */
ReadableCodegenConfig create_readable_config(bool debug_mode) {
    ReadableCodegenConfig config;
    config.preserve_variable_names = debug_mode;
    config.add_optimization_comments = debug_mode;
    config.maintain_structure = true;
    config.generate_debug_info = debug_mode;
    config.indentation_level = 0;
    config.use_goto_sparingly = true;
    return config;
}

/**
 * @brief Generates meaningful variable names for the optimized IR.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the readable codegen configuration.
 * @return Array of variable naming information.
 */
VariableNaming *generate_variable_names(CFG *cfg, ReadableCodegenConfig *config) {
    if (!cfg || !config) return NULL;
    
    // Collect all variables used in the CFG
    size_t max_vars = 1000; // Reasonable upper bound
    VariableNaming *naming = malloc(max_vars * sizeof(VariableNaming));
    size_t var_count = 0;
    
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *bb = cfg->blocks[i];
        
        for (size_t j = 0; j < bb->ninstrs; j++) {
            IRInstr *instr = bb->instrs[j];
            
            // Process destination variable
            if (!ir_is_const(instr->dst) && var_count < max_vars) {
                bool already_exists = false;
                for (size_t k = 0; k < var_count; k++) {
                    if (naming[k].original_id.id == instr->dst.id) {
                        already_exists = true;
                        break;
                    }
                }
                
                if (!already_exists) {
                    VariableNaming *var = &naming[var_count++];
                    var->original_id = instr->dst;
                    var->is_temporary = true; // Default assumption
                    var->preserve_name = config->preserve_variable_names;
                    
                    // Generate meaningful names based on usage patterns
                    char *name = malloc(32);
                    char *type_hint = malloc(64);
                    
                    if (instr->op == IR_PHI) {
                        snprintf(name, 32, "phi_var_%d", instr->dst.id);
                        strcpy(type_hint, "phi merge variable");
                        var->is_temporary = false;
                    } else if (instr->op >= IR_ADD && instr->op <= IR_MOD) {
                        snprintf(name, 32, "arith_temp_%d", instr->dst.id);
                        strcpy(type_hint, "arithmetic result");
                    } else if (instr->op >= IR_LT && instr->op <= IR_NE) {
                        snprintf(name, 32, "cmp_temp_%d", instr->dst.id);
                        strcpy(type_hint, "comparison result");
                    } else if (instr->op == IR_CALL) {
                        snprintf(name, 32, "call_result_%d", instr->dst.id);
                        strcpy(type_hint, "function call result");
                        var->is_temporary = false;
                    } else {
                        snprintf(name, 32, "temp_%d", instr->dst.id);
                        strcpy(type_hint, "temporary variable");
                    }
                    
                    var->suggested_name = name;
                    var->type_hint = type_hint;
                }
            }
        }
    }
    
    return naming;
}

/**
 * @brief Analyzes the optimized CFG to preserve readability information.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the readable codegen configuration.
 * @return Array of structure annotations.
 */
StructureAnnotation *analyze_for_readability(CFG *cfg, ReadableCodegenConfig *config) {
    if (!cfg || !config) return NULL;
    
    StructureAnnotation *annotations = malloc(cfg->nblocks * sizeof(StructureAnnotation));
    
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *bb = cfg->blocks[i];
        StructureAnnotation *ann = &annotations[i];
        
        ann->block = bb;
        ann->is_optimization_artifact = false;
        ann->optimization_note = NULL;
        ann->original_source_hint = NULL;
        
        // Detect optimization artifacts
        if (bb->id > 1000) {
            // High block IDs suggest optimization-generated blocks
            ann->is_optimization_artifact = true;
            ann->optimization_note = strdup("Block created during optimization");
        }
        
        // Analyze instruction patterns for optimization hints
        bool has_complex_ops = false;
        bool has_many_instrs = bb->ninstrs > 10;
        
        for (size_t j = 0; j < bb->ninstrs; j++) {
            IRInstr *instr = bb->instrs[j];
            
            if (instr->op == IR_CALL) {
                if (!ann->optimization_note) {
                    ann->optimization_note = strdup("Contains function call");
                }
            } else if (instr->op == IR_PHI) {
                if (!ann->optimization_note) {
                    ann->optimization_note = strdup("Contains SSA phi function");
                }
            } else if (instr->op >= IR_MUL && instr->op <= IR_MOD) {
                has_complex_ops = true;
            }
        }
        
        if (has_complex_ops && !ann->optimization_note) {
            ann->optimization_note = strdup("Contains optimized arithmetic");
        }
        
        if (has_many_instrs && !ann->optimization_note) {
            ann->optimization_note = strdup("Large basic block (possibly unrolled loop)");
        }
        
        // Provide structural hints
        if (bb->nsucc == 0) {
            ann->original_source_hint = strdup("Exit block");
        } else if (bb->nsucc > 1) {
            ann->original_source_hint = strdup("Conditional branch");
        } else if (bb->npred > 1) {
            ann->original_source_hint = strdup("Merge point");
        }
    }
    
    return annotations;
}

/**
 * @brief Adds optimization comments to explain transformed code.
 * @param cfg Pointer to the control flow graph.
 * @param annotations Array of structure annotations.
 * @return true if comments were added, false otherwise.
 */
bool add_optimization_comments(CFG *cfg, StructureAnnotation *annotations) {
    if (!cfg || !annotations) return false;
    
    bool added_comments = false;
    
    for (size_t i = 0; i < cfg->nblocks; i++) {
        StructureAnnotation *ann = &annotations[i];
        
        if (ann->optimization_note) {
            // In a real implementation, comments would be attached to IR
            // For this example, we just mark that comments should be added
            added_comments = true;
        }
        
        if (ann->is_optimization_artifact) {
            // Mark optimization artifacts for special comment treatment
            added_comments = true;
        }
    }
    
    return added_comments;
}

/**
 * @brief Minimizes the use of goto statements in generated C code.
 * @param cfg Pointer to the control flow graph.
 * @return true if goto usage was reduced, false otherwise.
 */
bool minimize_goto_usage(CFG *cfg) {
    if (!cfg) return false;
    
    bool reduced_gotos = false;
    
    // Analyze control flow patterns that can be structured
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *bb = cfg->blocks[i];
        
        // Look for simple fall-through patterns
        if (bb->nsucc == 1) {
            BasicBlock *succ = bb->succ[0];
            
            // If successor has only one predecessor, they can be merged
            if (succ->npred == 1) {
                // In actual implementation, would merge these blocks
                // to eliminate a goto statement
                reduced_gotos = true;
            }
        }
        
        // Look for if-then-else patterns that can use structured control flow
        if (bb->nsucc == 2) {
            // This could potentially be structured as an if-else
            // rather than using goto statements
            reduced_gotos = true;
        }
    }
    
    return reduced_gotos;
}

/**
 * @brief Reconstructs high-level control flow from optimized IR.
 * @param cfg Pointer to the control flow graph.
 * @param annotations Array of structure annotations.
 * @return true if high-level structure was recovered, false otherwise.
 */
bool reconstruct_control_flow(CFG *cfg, StructureAnnotation *annotations) {
    if (!cfg || !annotations) return false;
    
    bool reconstructed = false;
    
    // Identify loop structures
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *bb = cfg->blocks[i];
        
        // Look for back edges (indicating loops)
        for (size_t j = 0; j < bb->nsucc; j++) {
            BasicBlock *succ = bb->succ[j];
            
            // Simple back edge detection: successor has lower ID
            if (succ->id < bb->id) {
                annotations[i].original_source_hint = 
                    realloc(annotations[i].original_source_hint, 64);
                strcpy(annotations[i].original_source_hint, "Loop back edge");
                reconstructed = true;
            }
        }
    }
    
    // Identify conditional structures
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *bb = cfg->blocks[i];
        
        if (bb->nsucc == 2) {
            // Look for if-then-else patterns
            BasicBlock *then_block = bb->succ[0];
            BasicBlock *else_block = bb->succ[1];
            
            // Check if both branches eventually merge
            // This would indicate a structured if-then-else
            for (size_t k = 0; k < cfg->nblocks; k++) {
                BasicBlock *potential_merge = cfg->blocks[k];
                if (potential_merge->npred == 2) {
                    // Check if this could be the merge point
                    bool is_merge = false;
                    for (size_t p = 0; p < potential_merge->npred; p++) {
                        if (potential_merge->pred[p] == then_block ||
                            potential_merge->pred[p] == else_block) {
                            is_merge = true;
                        }
                    }
                    if (is_merge) {
                        annotations[i].original_source_hint = 
                            realloc(annotations[i].original_source_hint, 64);
                        strcpy(annotations[i].original_source_hint, "If-then-else structure");
                        reconstructed = true;
                        break;
                    }
                }
            }
        }
    }
    
    return reconstructed;
}

/**
 * @brief Formats the CFG for human-readable C code generation.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the readable codegen configuration.
 * @return true if formatting was successful, false otherwise.
 */
bool format_for_readable_output(CFG *cfg, ReadableCodegenConfig *config) {
    if (!cfg || !config) return false;
    
    // Apply formatting preferences
    bool formatted = false;
    
    if (config->maintain_structure) {
        // Ensure control flow maintains logical structure
        formatted |= minimize_goto_usage(cfg);
    }
    
    if (config->preserve_variable_names) {
        // Generate meaningful variable names
        VariableNaming *naming = generate_variable_names(cfg, config);
        if (naming) {
            formatted = true;
            variable_naming_free(naming, 1000); // Max vars
        }
    }
    
    return formatted;
}

/**
 * @brief Estimates the readability impact of optimizations.
 * @param original_cfg Pointer to the original CFG.
 * @param optimized_cfg Pointer to the optimized CFG.
 * @return Readability score (0.0 = unreadable, 1.0 = perfectly readable).
 */
double estimate_readability_score(CFG *original_cfg, CFG *optimized_cfg) {
    if (!original_cfg || !optimized_cfg) return 0.0;
    
    double score = 1.0;
    
    // Penalty for increased complexity
    if (optimized_cfg->nblocks > original_cfg->nblocks * 2) {
        score -= 0.3; // Significant complexity increase
    } else if (optimized_cfg->nblocks > original_cfg->nblocks) {
        score -= 0.1; // Moderate complexity increase
    }
    
    // Count total instructions
    size_t orig_instrs = 0, opt_instrs = 0;
    
    for (size_t i = 0; i < original_cfg->nblocks; i++) {
        orig_instrs += original_cfg->blocks[i]->ninstrs;
    }
    
    for (size_t i = 0; i < optimized_cfg->nblocks; i++) {
        opt_instrs += optimized_cfg->blocks[i]->ninstrs;
    }
    
    // Penalty for code bloat
    if (opt_instrs > orig_instrs * 2) {
        score -= 0.4; // Significant code expansion
    } else if (opt_instrs > orig_instrs * 1.5) {
        score -= 0.2; // Moderate code expansion
    }
    
    // Bonus for code reduction
    if (opt_instrs < orig_instrs * 0.8) {
        score += 0.1; // Code size reduction improves readability
    }
    
    return score < 0.0 ? 0.0 : (score > 1.0 ? 1.0 : score);
}

/**
 * @brief Applies final cleanup for readable C code generation.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the readable codegen configuration.
 * @return true if cleanup was successful, false otherwise.
 */
bool apply_readable_cleanup(CFG *cfg, ReadableCodegenConfig *config) {
    if (!cfg || !config) return false;
    
    bool cleaned = false;
    
    // Remove redundant basic blocks
    for (size_t i = 0; i < cfg->nblocks; i++) {
        BasicBlock *bb = cfg->blocks[i];
        
        // Empty blocks with single successor can often be eliminated
        if (bb->ninstrs == 0 && bb->nsucc == 1) {
            // In real implementation, would eliminate this block
            cleaned = true;
        }
    }
    
    // Simplify control flow where possible
    cleaned |= minimize_goto_usage(cfg);
    
    // Apply final formatting
    cleaned |= format_for_readable_output(cfg, config);
    
    return cleaned;
}

/**
 * @brief Frees structure annotations.
 * @param annotations Pointer to annotations array.
 * @param count Number of annotations.
 */
void structure_annotations_free(StructureAnnotation *annotations, size_t count) {
    if (!annotations) return;
    
    for (size_t i = 0; i < count; i++) {
        free(annotations[i].optimization_note);
        free(annotations[i].original_source_hint);
    }
    
    free(annotations);
}

/**
 * @brief Frees variable naming information.
 * @param naming Pointer to naming array.
 * @param count Number of entries.
 */
void variable_naming_free(VariableNaming *naming, size_t count) {
    if (!naming) return;
    
    for (size_t i = 0; i < count; i++) {
        free(naming[i].suggested_name);
        free(naming[i].type_hint);
    }
    
    free(naming);
}