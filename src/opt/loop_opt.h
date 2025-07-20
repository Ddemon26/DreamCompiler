/**
 * @file loop_opt.h
 * @brief Header file for advanced loop optimization passes.
 *
 * This file declares advanced loop optimization passes including loop unrolling,
 * strength reduction, loop fusion, and loop distribution.
 */

#ifndef LOOP_OPT_H
#define LOOP_OPT_H

#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include <stdbool.h>

/**
 * @brief Loop information structure for advanced optimizations.
 */
typedef struct Loop {
    BasicBlock *header;         /**< Loop header block. */
    BasicBlock *latch;          /**< Loop latch block (back edge source). */
    BasicBlock *preheader;      /**< Loop preheader block. */
    BasicBlock **blocks;        /**< Array of blocks in the loop. */
    size_t nblocks;            /**< Number of blocks in the loop. */
    IRValue induction_var;     /**< Primary induction variable. */
    IRValue initial_value;     /**< Initial value of induction variable. */
    IRValue step_value;        /**< Step value (increment) of induction variable. */
    IRValue limit_value;       /**< Loop bound/limit value. */
    IROp comparison_op;        /**< Comparison operation for loop condition. */
    int trip_count;            /**< Known trip count (-1 if unknown). */
    bool is_countable;         /**< True if loop has analyzable bounds. */
    struct Loop *parent;       /**< Parent loop (for nested loops). */
    struct Loop **children;    /**< Child loops. */
    size_t nchildren;          /**< Number of child loops. */
} Loop;

/**
 * @brief Loop nest information for optimization decisions.
 */
typedef struct {
    Loop **loops;              /**< Array of all loops in the function. */
    size_t nloops;             /**< Number of loops. */
    int nest_depth;            /**< Maximum nesting depth. */
} LoopNest;

/**
 * @brief Configuration for loop optimizations.
 */
typedef struct {
    int max_unroll_count;      /**< Maximum unrolling factor. */
    int max_unroll_size;       /**< Maximum code size increase for unrolling. */
    bool enable_strength_reduction; /**< Enable strength reduction. */
    bool enable_loop_fusion;   /**< Enable loop fusion. */
    bool enable_vectorization; /**< Enable loop vectorization hints. */
} LoopOptConfig;

/**
 * @brief Induction variable information for strength reduction.
 */
typedef struct {
    IRValue var;               /**< The induction variable. */
    IRValue base;              /**< Base value. */
    IRValue multiplier;        /**< Multiplier (coefficient). */
    IRValue addend;            /**< Additive constant. */
    bool is_linear;            /**< True if var = base + multiplier * i + addend. */
} InductionVar;

/**
 * @brief Discovers loops in a CFG using natural loop detection.
 * @param cfg Pointer to the control flow graph.
 * @return Pointer to the loop nest information.
 */
LoopNest *discover_loops(CFG *cfg);

/**
 * @brief Analyzes induction variables in a loop.
 * @param loop Pointer to the loop structure.
 * @return Array of induction variables found.
 */
InductionVar *analyze_induction_variables(Loop *loop, size_t *count);

/**
 * @brief Determines if a loop is suitable for unrolling.
 * @param loop Pointer to the loop structure.
 * @param config Pointer to the optimization configuration.
 * @return Recommended unrolling factor (1 means no unrolling).
 */
int determine_unroll_factor(Loop *loop, LoopOptConfig *config);

/**
 * @brief Performs loop unrolling on a specific loop.
 * @param cfg Pointer to the control flow graph.
 * @param loop Pointer to the loop to unroll.
 * @param unroll_factor Number of times to unroll the loop.
 * @return true if unrolling was successful, false otherwise.
 */
bool unroll_loop(CFG *cfg, Loop *loop, int unroll_factor);

/**
 * @brief Performs strength reduction on induction variables in a loop.
 * @param loop Pointer to the loop structure.
 * @param induction_vars Array of induction variables.
 * @param count Number of induction variables.
 * @return true if any strength reduction was performed, false otherwise.
 */
bool strength_reduction(Loop *loop, InductionVar *induction_vars, size_t count);

/**
 * @brief Attempts to fuse adjacent loops with compatible iteration patterns.
 * @param cfg Pointer to the control flow graph.
 * @param nest Pointer to the loop nest.
 * @return true if any loops were fused, false otherwise.
 */
bool fuse_compatible_loops(CFG *cfg, LoopNest *nest);

/**
 * @brief Distributes a loop to enable better optimization opportunities.
 * @param cfg Pointer to the control flow graph.
 * @param loop Pointer to the loop to distribute.
 * @return true if distribution was performed, false otherwise.
 */
bool distribute_loop(CFG *cfg, Loop *loop);

/**
 * @brief Performs loop interchange to improve cache locality.
 * @param nest Pointer to the loop nest.
 * @param outer_loop Pointer to the outer loop.
 * @param inner_loop Pointer to the inner loop.
 * @return true if interchange was performed, false otherwise.
 */
bool interchange_loops(LoopNest *nest, Loop *outer_loop, Loop *inner_loop);

/**
 * @brief Main entry point for advanced loop optimizations.
 * @param cfg Pointer to the control flow graph.
 * @param config Pointer to the optimization configuration.
 * @return true if any optimizations were performed, false otherwise.
 */
bool optimize_loops(CFG *cfg, LoopOptConfig *config);

/**
 * @brief Estimates the cost of a loop for optimization decisions.
 * @param loop Pointer to the loop structure.
 * @return Estimated execution cost.
 */
int estimate_loop_cost(Loop *loop);

/**
 * @brief Checks if two loops are fusion candidates.
 * @param loop1 Pointer to the first loop.
 * @param loop2 Pointer to the second loop.
 * @return true if loops can be fused, false otherwise.
 */
bool are_loops_fusible(Loop *loop1, Loop *loop2);

/**
 * @brief Frees a loop nest and all associated memory.
 * @param nest Pointer to the loop nest to free.
 */
void loop_nest_free(LoopNest *nest);

/**
 * @brief Frees an array of induction variables.
 * @param vars Pointer to the induction variables array.
 * @param count Number of variables.
 */
void induction_vars_free(InductionVar *vars, size_t count);

#endif