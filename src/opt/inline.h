/**
 * @file inline.h
 * @brief Header file for function inlining optimization.
 *
 * This file declares the function inlining optimization pass that can inline
 * small functions at their call sites to reduce function call overhead and
 * enable further optimizations.
 */

#ifndef INLINE_H
#define INLINE_H

#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include <stdbool.h>

/**
 * @brief Information about a function for inlining analysis.
 */
typedef struct {
    int id;                 /**< Function identifier. */
    char *name;             /**< Function name. */
    CFG *cfg;               /**< Control flow graph of the function. */
    IRValue *params;        /**< Function parameters. */
    size_t nparam;          /**< Number of parameters. */
    IRValue return_val;     /**< Return value (for testing compatibility). */
    int inline_cost;        /**< Cached inline cost. */
    int call_count;         /**< Number of times this function is called. */
    bool is_recursive;      /**< True if function is recursive. */
} FunctionInfo;

/**
 * @brief Function table for interprocedural analysis and inlining.
 */
typedef struct {
    FunctionInfo *functions;  /**< Array of function metadata. */
    size_t nfunctions;       /**< Number of functions. */
    size_t capacity;         /**< Allocated capacity. */
} FunctionTable;

/**
 * @brief Configuration for inlining decisions.
 */
typedef struct {
    int max_inline_cost;     /**< Maximum cost threshold for inlining. */
    int max_inline_depth;    /**< Maximum recursion depth for inlining. */
    bool inline_hot_only;    /**< Only inline frequently called functions. */
    int hot_threshold;       /**< Minimum call count to be considered hot. */
} InlineConfig;

/**
 * @brief Creates a new function table.
 * @return Pointer to the newly created function table.
 */
FunctionTable *function_table_new(void);

/**
 * @brief Adds a function to the function table.
 * @param table Pointer to the function table.
 * @param func Pointer to the function info to add.
 * @return Function ID assigned to the added function.
 */
int function_table_add(FunctionTable *table, FunctionInfo *func);

/**
 * @brief Retrieves function info by ID.
 * @param table Pointer to the function table.
 * @param func_id Function identifier.
 * @return Pointer to function info, or NULL if not found.
 */
FunctionInfo *function_table_get(FunctionTable *table, int func_id);

/**
 * @brief Calculates the inlining cost for a function.
 * @param func Pointer to the function info.
 * @return Estimated cost of inlining this function.
 */
int calculate_inline_cost(FunctionInfo *func);

/**
 * @brief Determines if a function should be inlined at a call site.
 * @param func Pointer to the function info.
 * @param config Pointer to the inlining configuration.
 * @param call_depth Current inlining depth.
 * @return true if the function should be inlined, false otherwise.
 */
bool should_inline(FunctionInfo *func, InlineConfig *config, int call_depth);

/**
 * @brief Performs function inlining on a CFG.
 * @param cfg Pointer to the control flow graph to optimize.
 * @param table Pointer to the function table.
 * @param config Pointer to the inlining configuration.
 * @return true if any inlining was performed, false otherwise.
 */
bool inline_functions(CFG *cfg, FunctionTable *table, InlineConfig *config);

/**
 * @brief Inlines a specific function at a call site.
 * @param caller_cfg Pointer to the caller's CFG.
 * @param call_block Pointer to the block containing the call.
 * @param call_instr Pointer to the call instruction.
 * @param callee Pointer to the function to inline.
 * @return true if inlining was successful, false otherwise.
 */
bool inline_function_at_site(CFG *caller_cfg, BasicBlock *call_block, 
                             IRInstr *call_instr, FunctionInfo *callee);

/**
 * @brief Clones a CFG for inlining, renaming variables to avoid conflicts.
 * @param source Pointer to the source CFG to clone.
 * @param var_offset Variable ID offset for renaming.
 * @param param_mapping Mapping from parameter IDs to argument values.
 * @param nparam Number of parameters.
 * @return Pointer to the cloned CFG.
 */
CFG *clone_cfg_for_inline(CFG *source, int var_offset, 
                          IRValue *param_mapping, size_t nparam);

/**
 * @brief Frees a function table and all associated memory.
 * @param table Pointer to the function table to free.
 */
void function_table_free(FunctionTable *table);

#endif