/**
 * @file interprocedural.h
 * @brief Header file for interprocedural optimization passes.
 *
 * This file declares interprocedural optimization passes that analyze and
 * optimize across function boundaries, including constant propagation,
 * dead function elimination, and escape analysis.
 */

#ifndef INTERPROCEDURAL_H
#define INTERPROCEDURAL_H

#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include "inline.h"
#include <stdbool.h>

/**
 * @brief Call graph edge representing a function call relationship.
 */
typedef struct CallEdge {
    int caller_id;          /**< ID of the calling function. */
    int callee_id;          /**< ID of the called function. */
    BasicBlock *call_site;  /**< Basic block containing the call. */
    IRInstr *call_instr;    /**< The actual call instruction. */
    struct CallEdge *next;  /**< Next edge in the list. */
} CallEdge;

/**
 * @brief Call graph for interprocedural analysis.
 */
typedef struct {
    FunctionTable *functions;   /**< Function table. */
    CallEdge **edges;          /**< Adjacency list of call edges. */
    size_t *edge_counts;       /**< Number of outgoing edges per function. */
    int *topo_order;           /**< Topological ordering of functions. */
    bool *visited;             /**< Visited array for graph traversal. */
} CallGraph;

/**
 * @brief Information about a function argument at a call site.
 */
typedef struct {
    int param_index;        /**< Parameter index. */
    IRValue constant_value; /**< Constant value if known. */
    bool is_constant;       /**< True if this argument is always constant. */
} ArgumentInfo;

/**
 * @brief Summary information about a function for interprocedural analysis.
 */
typedef struct {
    int function_id;                /**< Function identifier. */
    bool is_pure;                   /**< True if function has no side effects. */
    bool modifies_globals;          /**< True if function modifies global state. */
    bool calls_external;           /**< True if function calls external functions. */
    ArgumentInfo *arg_info;         /**< Information about each parameter. */
    size_t nparams;                 /**< Number of parameters. */
    IRValue constant_return;        /**< Constant return value if known. */
    bool has_constant_return;       /**< True if function always returns constant. */
} FunctionSummary;

/**
 * @brief Creates a new call graph from a function table.
 * @param functions Pointer to the function table.
 * @return Pointer to the newly created call graph.
 */
CallGraph *call_graph_new(FunctionTable *functions);

/**
 * @brief Adds a call edge to the call graph.
 * @param graph Pointer to the call graph.
 * @param caller_id ID of the calling function.
 * @param callee_id ID of the called function.
 * @param call_site Basic block containing the call.
 * @param call_instr The call instruction.
 */
void call_graph_add_edge(CallGraph *graph, int caller_id, int callee_id,
                        BasicBlock *call_site, IRInstr *call_instr);

/**
 * @brief Computes topological ordering of functions in the call graph.
 * @param graph Pointer to the call graph.
 * @return true if ordering was computed successfully (no cycles), false otherwise.
 */
bool call_graph_topological_sort(CallGraph *graph);

/**
 * @brief Detects recursive functions in the call graph.
 * @param graph Pointer to the call graph.
 */
void call_graph_detect_recursion(CallGraph *graph);

/**
 * @brief Builds function summaries for interprocedural analysis.
 * @param graph Pointer to the call graph.
 * @return Array of function summaries.
 */
FunctionSummary *build_function_summaries(CallGraph *graph);

/**
 * @brief Performs interprocedural constant propagation.
 * @param graph Pointer to the call graph.
 * @param summaries Array of function summaries.
 * @return true if any constants were propagated, false otherwise.
 */
bool interprocedural_constant_propagation(CallGraph *graph, FunctionSummary *summaries);

/**
 * @brief Eliminates dead (unreachable) functions from the program.
 * @param graph Pointer to the call graph.
 * @param entry_function_id ID of the program entry function.
 * @return true if any functions were eliminated, false otherwise.
 */
bool eliminate_dead_functions(CallGraph *graph, int entry_function_id);

/**
 * @brief Performs escape analysis to determine which objects can be stack-allocated.
 * @param graph Pointer to the call graph.
 * @param summaries Array of function summaries.
 * @return true if any optimizations were applied, false otherwise.
 */
bool perform_escape_analysis(CallGraph *graph, FunctionSummary *summaries);

/**
 * @brief Identifies pure functions (functions with no side effects).
 * @param graph Pointer to the call graph.
 * @param summaries Array of function summaries.
 */
void identify_pure_functions(CallGraph *graph, FunctionSummary *summaries);

/**
 * @brief Frees a call graph and all associated memory.
 * @param graph Pointer to the call graph to free.
 */
void call_graph_free(CallGraph *graph);

/**
 * @brief Frees an array of function summaries.
 * @param summaries Pointer to the summaries array.
 * @param count Number of summaries.
 */
void function_summaries_free(FunctionSummary *summaries, size_t count);

#endif