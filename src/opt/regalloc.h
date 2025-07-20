/**
 * @file regalloc.h
 * @brief Header file for register allocation preparation and analysis.
 *
 * This file declares data structures and functions for preparing the IR
 * for register allocation, including live variable analysis, interference
 * graph construction, and spill code generation.
 */

#ifndef REGALLOC_H
#define REGALLOC_H

#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include <stdbool.h>

/**
 * @brief Live variable information for a program point.
 */
typedef struct {
    IRValue *live_in;           /**< Variables live at block entry. */
    size_t live_in_count;       /**< Number of live-in variables. */
    IRValue *live_out;          /**< Variables live at block exit. */
    size_t live_out_count;      /**< Number of live-out variables. */
    IRValue *def;               /**< Variables defined in this block. */
    size_t def_count;           /**< Number of defined variables. */
    IRValue *use;               /**< Variables used in this block. */
    size_t use_count;           /**< Number of used variables. */
} LivenessInfo;

/**
 * @brief Variable interference information.
 */
typedef struct {
    IRValue var1;               /**< First interfering variable. */
    IRValue var2;               /**< Second interfering variable. */
    double weight;              /**< Interference weight (frequency). */
} Interference;

/**
 * @brief Interference graph for register allocation.
 */
typedef struct {
    IRValue *variables;         /**< Array of all variables. */
    size_t nvars;              /**< Number of variables. */
    Interference *edges;        /**< Array of interference edges. */
    size_t nedges;             /**< Number of interference edges. */
    int *colors;               /**< Register assignments (colors). */
    bool *spilled;             /**< True if variable is spilled to memory. */
} InterferenceGraph;

/**
 * @brief Register allocation context.
 */
typedef struct {
    CFG *cfg;                   /**< Control flow graph. */
    LivenessInfo *liveness;     /**< Liveness information per block. */
    InterferenceGraph *graph;   /**< Interference graph. */
    int num_registers;          /**< Number of available registers. */
    IRValue *spill_temps;       /**< Temporary variables for spills. */
    size_t nspill_temps;       /**< Number of spill temporaries. */
} RegAllocContext;

/**
 * @brief Coalescing candidate for register allocation.
 */
typedef struct {
    IRValue src;                /**< Source variable. */
    IRValue dst;                /**< Destination variable. */
    double benefit;             /**< Benefit of coalescing these variables. */
    bool is_move;              /**< True if this is a move instruction. */
} CoalescingCandidate;

/**
 * @brief Performs live variable analysis on a CFG.
 * @param cfg Pointer to the control flow graph.
 * @return Array of liveness information for each basic block.
 */
LivenessInfo *analyze_liveness(CFG *cfg);

/**
 * @brief Builds an interference graph from liveness information.
 * @param cfg Pointer to the control flow graph.
 * @param liveness Array of liveness information.
 * @return Pointer to the interference graph.
 */
InterferenceGraph *build_interference_graph(CFG *cfg, LivenessInfo *liveness);

/**
 * @brief Performs graph coloring register allocation.
 * @param graph Pointer to the interference graph.
 * @param num_registers Number of available registers.
 * @return true if all variables were allocated registers, false if spilling needed.
 */
bool graph_coloring_allocation(InterferenceGraph *graph, int num_registers);

/**
 * @brief Identifies move instructions that can be coalesced.
 * @param cfg Pointer to the control flow graph.
 * @param graph Pointer to the interference graph.
 * @return Array of coalescing candidates.
 */
CoalescingCandidate *identify_coalescing_candidates(CFG *cfg, InterferenceGraph *graph, 
                                                   size_t *count);

/**
 * @brief Performs conservative coalescing of move instructions.
 * @param graph Pointer to the interference graph.
 * @param candidates Array of coalescing candidates.
 * @param count Number of candidates.
 * @return true if any coalescing was performed, false otherwise.
 */
bool perform_coalescing(InterferenceGraph *graph, CoalescingCandidate *candidates, 
                       size_t count);

/**
 * @brief Inserts spill code for variables that couldn't be allocated registers.
 * @param cfg Pointer to the control flow graph.
 * @param graph Pointer to the interference graph.
 * @return true if spill code was inserted, false otherwise.
 */
bool insert_spill_code(CFG *cfg, InterferenceGraph *graph);

/**
 * @brief Performs linear scan register allocation (alternative to graph coloring).
 * @param cfg Pointer to the control flow graph.
 * @param liveness Array of liveness information.
 * @param num_registers Number of available registers.
 * @return Pointer to register allocation results.
 */
InterferenceGraph *linear_scan_allocation(CFG *cfg, LivenessInfo *liveness, 
                                         int num_registers);

/**
 * @brief Computes live ranges for all variables.
 * @param cfg Pointer to the control flow graph.
 * @param liveness Array of liveness information.
 * @param ranges Output array for live ranges.
 * @return Number of live ranges computed.
 */
size_t compute_live_ranges(CFG *cfg, LivenessInfo *liveness, 
                          struct LiveRange **ranges);

/**
 * @brief Live range information for linear scan allocation.
 */
struct LiveRange {
    IRValue var;                /**< Variable identifier. */
    int start;                  /**< Start of live range. */
    int end;                    /**< End of live range. */
    int reg;                    /**< Assigned register (-1 if spilled). */
    bool is_spilled;           /**< True if variable is spilled. */
};

/**
 * @brief Optimizes register allocation by removing redundant moves.
 * @param cfg Pointer to the control flow graph.
 * @param graph Pointer to the interference graph.
 * @return true if any moves were eliminated, false otherwise.
 */
bool eliminate_redundant_moves(CFG *cfg, InterferenceGraph *graph);

/**
 * @brief Prepares the IR for future direct machine code generation.
 * @param cfg Pointer to the control flow graph.
 * @return Pointer to register allocation context.
 */
RegAllocContext *prepare_for_machine_code(CFG *cfg);

/**
 * @brief Converts SSA form to conventional form for register allocation.
 * @param cfg Pointer to the control flow graph.
 * @return true if conversion was successful, false otherwise.
 */
bool convert_from_ssa(CFG *cfg);

/**
 * @brief Inserts phi elimination code at block boundaries.
 * @param cfg Pointer to the control flow graph.
 * @return true if phi elimination was performed, false otherwise.
 */
bool eliminate_phi_functions(CFG *cfg);

/**
 * @brief Frees liveness information.
 * @param liveness Pointer to liveness information array.
 * @param nblocks Number of basic blocks.
 */
void liveness_info_free(LivenessInfo *liveness, size_t nblocks);

/**
 * @brief Frees an interference graph.
 * @param graph Pointer to the interference graph.
 */
void interference_graph_free(InterferenceGraph *graph);

/**
 * @brief Frees a register allocation context.
 * @param context Pointer to the register allocation context.
 */
void regalloc_context_free(RegAllocContext *context);

#endif