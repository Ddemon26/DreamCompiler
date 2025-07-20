/**
 * @file advanced_pipeline.h
 * @brief Header file for the advanced optimization pipeline.
 *
 * This file declares the main entry point for advanced optimizations including
 * function inlining, interprocedural analysis, loop optimizations, and
 * register allocation preparation.
 */

#ifndef ADVANCED_PIPELINE_H
#define ADVANCED_PIPELINE_H

#include "../cfg/cfg.h"
#include "inline.h"
#include "interprocedural.h"
#include "loop_opt.h"
#include "regalloc.h"
#include <stdbool.h>

/**
 * @brief Configuration for advanced optimizations.
 */
typedef struct {
    // Function inlining configuration
    InlineConfig inline_config;
    
    // Loop optimization configuration
    LoopOptConfig loop_config;
    
    // Register allocation configuration
    int target_registers;          /**< Number of target registers. */
    bool prepare_for_codegen;      /**< Prepare for machine code generation. */
    
    // General optimization settings
    int optimization_level;        /**< Optimization level (1-3). */
    bool maintain_debug_info;      /**< Preserve debug information. */
    bool optimize_for_size;        /**< Optimize for code size vs. speed. */
    bool enable_experimental;      /**< Enable experimental optimizations. */
} AdvancedOptConfig;

/**
 * @brief Results of advanced optimization passes.
 */
typedef struct {
    bool inlining_performed;       /**< True if any inlining was done. */
    bool interprocedural_opt;      /**< True if interprocedural opts applied. */
    bool loop_opt_performed;       /**< True if loop opts were applied. */
    bool regalloc_prepared;        /**< True if register allocation was prepared. */
    
    int functions_inlined;         /**< Number of function calls inlined. */
    int dead_functions_removed;    /**< Number of dead functions eliminated. */
    int loops_unrolled;           /**< Number of loops unrolled. */
    int loops_fused;              /**< Number of loops fused. */
    int variables_spilled;        /**< Number of variables spilled to memory. */
    
    double compile_time;          /**< Time spent in advanced optimizations. */
    size_t code_size_reduction;   /**< Estimated code size reduction. */
    double performance_gain;      /**< Estimated performance improvement. */
} OptimizationResults;

/**
 * @brief Multi-function compilation context for interprocedural analysis.
 */
typedef struct {
    FunctionTable *functions;     /**< All functions in the program. */
    CallGraph *call_graph;       /**< Call graph for interprocedural analysis. */
    int entry_function_id;       /**< ID of the program entry function. */
    bool *function_processed;    /**< Track which functions have been optimized. */
} MultiFunction;

/**
 * @brief Runs the complete advanced optimization pipeline on a single function.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the optimization configuration.
 * @return Pointer to optimization results.
 */
OptimizationResults *run_advanced_pipeline(CFG *cfg, AdvancedOptConfig *config);

/**
 * @brief Runs interprocedural optimizations on multiple functions.
 * @param multi_func Pointer to the multi-function context.
 * @param config Pointer to the optimization configuration.
 * @return Pointer to optimization results.
 */
OptimizationResults *run_interprocedural_pipeline(MultiFunction *multi_func, 
                                                  AdvancedOptConfig *config);

/**
 * @brief Creates a default advanced optimization configuration.
 * @param opt_level Optimization level (1-3).
 * @return Default configuration for the given optimization level.
 */
AdvancedOptConfig create_default_config(int opt_level);

/**
 * @brief Validates that the CFG is in a consistent state after optimizations.
 * @param cfg Pointer to the control flow graph.
 * @return true if CFG is valid, false if corruption detected.
 */
bool validate_cfg_after_optimization(CFG *cfg);

/**
 * @brief Estimates the performance impact of applied optimizations.
 * @param original_cfg Pointer to the original CFG.
 * @param optimized_cfg Pointer to the optimized CFG.
 * @return Estimated performance improvement factor.
 */
double estimate_performance_improvement(CFG *original_cfg, CFG *optimized_cfg);

/**
 * @brief Applies post-optimization cleanup and canonicalization.
 * @param cfg Pointer to the control flow graph.
 * @return true if any cleanup was performed, false otherwise.
 */
bool post_optimization_cleanup(CFG *cfg);

/**
 * @brief Generates optimization report for debugging and analysis.
 * @param results Pointer to optimization results.
 * @param output_file File to write the report to (NULL for stdout).
 */
void generate_optimization_report(OptimizationResults *results, const char *output_file);

/**
 * @brief Creates a multi-function context from an array of CFGs.
 * @param cfgs Array of control flow graphs.
 * @param function_names Array of function names.
 * @param count Number of functions.
 * @param entry_name Name of the entry function.
 * @return Pointer to the multi-function context.
 */
MultiFunction *create_multi_function_context(CFG **cfgs, char **function_names, 
                                            size_t count, const char *entry_name);

/**
 * @brief Applies whole-program optimization across all functions.
 * @param multi_func Pointer to the multi-function context.
 * @param config Pointer to the optimization configuration.
 * @return true if any whole-program optimizations were applied, false otherwise.
 */
bool apply_whole_program_optimization(MultiFunction *multi_func, AdvancedOptConfig *config);

/**
 * @brief Frees optimization results structure.
 * @param results Pointer to optimization results.
 */
void optimization_results_free(OptimizationResults *results);

/**
 * @brief Frees multi-function context.
 * @param multi_func Pointer to multi-function context.
 */
void multi_function_free(MultiFunction *multi_func);

#endif