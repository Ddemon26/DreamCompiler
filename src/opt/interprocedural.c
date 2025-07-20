#include "interprocedural.h"
#include "../cfg/cfg.h"
#include "../ir/ir.h"
#include "inline.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates a new call graph from a function table.
 * @param functions Pointer to the function table.
 * @return Pointer to the newly created call graph.
 */
CallGraph *call_graph_new(FunctionTable *functions) {
    CallGraph *graph = malloc(sizeof(CallGraph));
    graph->functions = functions;
    graph->edges = calloc(functions->nfunctions, sizeof(CallEdge*));
    graph->edge_counts = calloc(functions->nfunctions, sizeof(size_t));
    graph->topo_order = malloc(functions->nfunctions * sizeof(int));
    graph->visited = calloc(functions->nfunctions, sizeof(bool));
    
    // Build call graph by scanning all function CFGs for call instructions
    for (size_t i = 0; i < functions->nfunctions; i++) {
        FunctionInfo *func = &functions->functions[i];
        if (!func->cfg) continue;
        
        for (size_t j = 0; j < func->cfg->nblocks; j++) {
            BasicBlock *bb = func->cfg->blocks[j];
            for (size_t k = 0; k < bb->ninstrs; k++) {
                IRInstr *instr = bb->instrs[k];
                if (instr->op == IR_CALL) {
                    call_graph_add_edge(graph, func->id, instr->extra.call.func_id, bb, instr);
                }
            }
        }
    }
    
    return graph;
}

/**
 * @brief Adds a call edge to the call graph.
 * @param graph Pointer to the call graph.
 * @param caller_id ID of the calling function.
 * @param callee_id ID of the called function.
 * @param call_site Basic block containing the call.
 * @param call_instr The call instruction.
 */
void call_graph_add_edge(CallGraph *graph, int caller_id, int callee_id,
                        BasicBlock *call_site, IRInstr *call_instr) {
    if (caller_id < 0 || caller_id >= (int)graph->functions->nfunctions ||
        callee_id < 0 || callee_id >= (int)graph->functions->nfunctions) {
        return;
    }
    
    CallEdge *edge = malloc(sizeof(CallEdge));
    edge->caller_id = caller_id;
    edge->callee_id = callee_id;
    edge->call_site = call_site;
    edge->call_instr = call_instr;
    edge->next = graph->edges[caller_id];
    graph->edges[caller_id] = edge;
    graph->edge_counts[caller_id]++;
    
    // Update call count in function info
    FunctionInfo *callee = function_table_get(graph->functions, callee_id);
    if (callee) {
        callee->call_count++;
    }
}

/**
 * @brief Helper function for topological sort using DFS.
 * @param graph Pointer to the call graph.
 * @param node Current function ID.
 * @param stack DFS stack for topological ordering.
 * @param stack_pos Current stack position.
 * @return true if no cycles detected, false otherwise.
 */
static bool topo_sort_dfs(CallGraph *graph, int node, int *stack, int *stack_pos) {
    graph->visited[node] = true;
    
    CallEdge *edge = graph->edges[node];
    while (edge) {
        if (!graph->visited[edge->callee_id]) {
            if (!topo_sort_dfs(graph, edge->callee_id, stack, stack_pos)) {
                return false; // Cycle detected
            }
        }
        edge = edge->next;
    }
    
    stack[(*stack_pos)++] = node;
    return true;
}

/**
 * @brief Computes topological ordering of functions in the call graph.
 * @param graph Pointer to the call graph.
 * @return true if ordering was computed successfully (no cycles), false otherwise.
 */
bool call_graph_topological_sort(CallGraph *graph) {
    // Reset visited array
    memset(graph->visited, 0, graph->functions->nfunctions * sizeof(bool));
    
    int stack_pos = 0;
    for (size_t i = 0; i < graph->functions->nfunctions; i++) {
        if (!graph->visited[i]) {
            if (!topo_sort_dfs(graph, (int)i, graph->topo_order, &stack_pos)) {
                return false; // Cycle detected
            }
        }
    }
    
    // Reverse the stack to get correct topological order
    for (int i = 0; i < (int)graph->functions->nfunctions / 2; i++) {
        int temp = graph->topo_order[i];
        graph->topo_order[i] = graph->topo_order[graph->functions->nfunctions - 1 - i];
        graph->topo_order[graph->functions->nfunctions - 1 - i] = temp;
    }
    
    return true;
}

/**
 * @brief Detects recursive functions in the call graph.
 * @param graph Pointer to the call graph.
 */
void call_graph_detect_recursion(CallGraph *graph) {
    // Use DFS to detect cycles (strongly connected components)
    memset(graph->visited, 0, graph->functions->nfunctions * sizeof(bool));
    
    for (size_t i = 0; i < graph->functions->nfunctions; i++) {
        if (!graph->visited[i]) {
            // Simple cycle detection: if we can reach ourselves, we're recursive
            bool *in_path = calloc(graph->functions->nfunctions, sizeof(bool));
            
            // DFS to check reachability
            int *stack = malloc(graph->functions->nfunctions * sizeof(int));
            int stack_top = 0;
            stack[stack_top++] = (int)i;
            in_path[i] = true;
            
            while (stack_top > 0) {
                int current = stack[--stack_top];
                graph->visited[current] = true;
                
                CallEdge *edge = graph->edges[current];
                while (edge) {
                    if (edge->callee_id == (int)i) {
                        // Found a cycle back to start node
                        FunctionInfo *func = function_table_get(graph->functions, (int)i);
                        if (func) func->is_recursive = 1;
                    } else if (!in_path[edge->callee_id]) {
                        in_path[edge->callee_id] = true;
                        stack[stack_top++] = edge->callee_id;
                    }
                    edge = edge->next;
                }
            }
            
            free(stack);
            free(in_path);
        }
    }
}

/**
 * @brief Builds function summaries for interprocedural analysis.
 * @param graph Pointer to the call graph.
 * @return Array of function summaries.
 */
FunctionSummary *build_function_summaries(CallGraph *graph) {
    FunctionSummary *summaries = calloc(graph->functions->nfunctions, sizeof(FunctionSummary));
    
    for (size_t i = 0; i < graph->functions->nfunctions; i++) {
        FunctionInfo *func = &graph->functions->functions[i];
        FunctionSummary *summary = &summaries[i];
        
        summary->function_id = func->id;
        summary->nparams = func->nparam;
        summary->arg_info = calloc(func->nparam, sizeof(ArgumentInfo));
        summary->is_pure = true; // Assume pure until proven otherwise
        summary->modifies_globals = false;
        summary->calls_external = false;
        summary->has_constant_return = false;
        
        if (!func->cfg) continue;
        
        // Analyze function body for side effects
        for (size_t j = 0; j < func->cfg->nblocks; j++) {
            BasicBlock *bb = func->cfg->blocks[j];
            for (size_t k = 0; k < bb->ninstrs; k++) {
                IRInstr *instr = bb->instrs[k];
                
                switch (instr->op) {
                    case IR_CALL:
                        // Function calls potentially have side effects
                        summary->is_pure = false;
                        // Check if calling external function
                        if (instr->extra.call.func_id >= (int)graph->functions->nfunctions) {
                            summary->calls_external = true;
                        }
                        break;
                    
                    // Memory operations that might modify global state
                    case IR_MOV:
                        // Check if storing to global variable (simple heuristic)
                        if (instr->dst.id < 100) { // Assume low IDs are globals
                            summary->modifies_globals = true;
                            summary->is_pure = false;
                        }
                        break;
                    
                    default:
                        break;
                }
            }
        }
    }
    
    return summaries;
}

/**
 * @brief Analyzes constant arguments at call sites.
 * @param graph Pointer to the call graph.
 * @param summaries Array of function summaries.
 */
static void analyze_constant_arguments(CallGraph *graph, FunctionSummary *summaries) {
    for (size_t i = 0; i < graph->functions->nfunctions; i++) {
        CallEdge *edge = graph->edges[i];
        while (edge) {
            IRInstr *call = edge->call_instr;
            FunctionSummary *callee_summary = &summaries[edge->callee_id];
            
            // Check each argument for constant values
            for (size_t j = 0; j < call->extra.call.nargs && j < callee_summary->nparams; j++) {
                IRValue arg = call->extra.call.args[j];
                ArgumentInfo *arg_info = &callee_summary->arg_info[j];
                
                if (ir_is_const(arg)) {
                    if (!arg_info->is_constant) {
                        // First constant value seen for this parameter
                        arg_info->is_constant = true;
                        arg_info->constant_value = arg;
                        arg_info->param_index = (int)j;
                    } else if (arg_info->constant_value.id != arg.id) {
                        // Different constant values seen - not always constant
                        arg_info->is_constant = false;
                    }
                } else {
                    // Non-constant argument
                    arg_info->is_constant = false;
                }
            }
            
            edge = edge->next;
        }
    }
}

/**
 * @brief Performs interprocedural constant propagation.
 * @param graph Pointer to the call graph.
 * @param summaries Array of function summaries.
 * @return true if any constants were propagated, false otherwise.
 */
bool interprocedural_constant_propagation(CallGraph *graph, FunctionSummary *summaries) {
    bool changed = false;
    
    // First, analyze which arguments are always constant
    analyze_constant_arguments(graph, summaries);
    
    // Process functions in topological order for better precision
    if (!call_graph_topological_sort(graph)) {
        return false; // Can't handle recursive programs yet
    }
    
    for (size_t i = 0; i < graph->functions->nfunctions; i++) {
        int func_id = graph->topo_order[i];
        FunctionInfo *func = function_table_get(graph->functions, func_id);
        FunctionSummary *summary = &summaries[func_id];
        
        if (!func || !func->cfg) continue;
        
        // Replace parameter uses with constant values where possible
        for (size_t j = 0; j < summary->nparams; j++) {
            ArgumentInfo *arg_info = &summary->arg_info[j];
            if (!arg_info->is_constant) continue;
            
            IRValue param_val = func->params[j];
            IRValue const_val = arg_info->constant_value;
            
            // Replace all uses of this parameter with the constant
            for (size_t k = 0; k < func->cfg->nblocks; k++) {
                BasicBlock *bb = func->cfg->blocks[k];
                for (size_t l = 0; l < bb->ninstrs; l++) {
                    IRInstr *instr = bb->instrs[l];
                    
                    if (instr->a.id == param_val.id) {
                        instr->a = const_val;
                        changed = true;
                    }
                    if (instr->b.id == param_val.id) {
                        instr->b = const_val;
                        changed = true;
                    }
                    
                    // Handle call arguments
                    if (instr->op == IR_CALL && instr->extra.call.args) {
                        for (size_t m = 0; m < instr->extra.call.nargs; m++) {
                            if (instr->extra.call.args[m].id == param_val.id) {
                                instr->extra.call.args[m] = const_val;
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return changed;
}

/**
 * @brief Marks reachable functions from a given entry point.
 * @param graph Pointer to the call graph.
 * @param func_id Function to mark as reachable.
 */
static void mark_reachable_functions(CallGraph *graph, int func_id) {
    if (func_id < 0 || func_id >= (int)graph->functions->nfunctions || graph->visited[func_id]) {
        return;
    }
    
    graph->visited[func_id] = true;
    
    CallEdge *edge = graph->edges[func_id];
    while (edge) {
        mark_reachable_functions(graph, edge->callee_id);
        edge = edge->next;
    }
}

/**
 * @brief Eliminates dead (unreachable) functions from the program.
 * @param graph Pointer to the call graph.
 * @param entry_function_id ID of the program entry function.
 * @return true if any functions were eliminated, false otherwise.
 */
bool eliminate_dead_functions(CallGraph *graph, int entry_function_id) {
    // Reset visited array
    memset(graph->visited, 0, graph->functions->nfunctions * sizeof(bool));
    
    // Mark all functions reachable from entry point
    mark_reachable_functions(graph, entry_function_id);
    
    bool changed = false;
    
    // Remove unreachable functions
    for (size_t i = 0; i < graph->functions->nfunctions; i++) {
        if (!graph->visited[i]) {
            FunctionInfo *func = &graph->functions->functions[i];
            if (func->cfg) {
                // Free the CFG to mark function as dead
                // Note: In a real implementation, you'd want to properly free the CFG
                func->cfg = NULL;
                changed = true;
            }
        }
    }
    
    return changed;
}

/**
 * @brief Identifies pure functions (functions with no side effects).
 * @param graph Pointer to the call graph.
 * @param summaries Array of function summaries.
 */
void identify_pure_functions(CallGraph *graph, FunctionSummary *summaries) {
    bool changed;
    
    // Iteratively refine purity analysis
    do {
        changed = false;
        
        for (size_t i = 0; i < graph->functions->nfunctions; i++) {
            FunctionSummary *summary = &summaries[i];
            
            if (!summary->is_pure) continue; // Already marked as impure
            
            // Check if function calls any impure functions
            CallEdge *edge = graph->edges[i];
            while (edge) {
                FunctionSummary *callee_summary = &summaries[edge->callee_id];
                if (!callee_summary->is_pure || callee_summary->calls_external) {
                    summary->is_pure = false;
                    changed = true;
                    break;
                }
                edge = edge->next;
            }
        }
    } while (changed);
}

/**
 * @brief Performs escape analysis to determine which objects can be stack-allocated.
 * @param graph Pointer to the call graph.
 * @param summaries Array of function summaries.
 * @return true if any optimizations were applied, false otherwise.
 */
bool perform_escape_analysis(CallGraph *graph, FunctionSummary *summaries) {
    // This is a simplified escape analysis
    // In a full implementation, you'd track object allocations and their uses
    bool changed = false;
    
    for (size_t i = 0; i < graph->functions->nfunctions; i++) {
        FunctionInfo *func = &graph->functions->functions[i];
        if (!func->cfg) continue;
        
        // Look for allocation sites that don't escape
        for (size_t j = 0; j < func->cfg->nblocks; j++) {
            BasicBlock *bb = func->cfg->blocks[j];
            for (size_t k = 0; k < bb->ninstrs; k++) {
                IRInstr *instr = bb->instrs[k];
                
                // Look for patterns that could benefit from stack allocation
                // This is a placeholder for more sophisticated analysis
                if (instr->op == IR_CALL) {
                    // Check if allocated object never leaves this function
                    // If so, could optimize to stack allocation
                    // Implementation would track data flow here
                }
            }
        }
    }
    
    return changed;
}

/**
 * @brief Frees a call graph and all associated memory.
 * @param graph Pointer to the call graph to free.
 */
void call_graph_free(CallGraph *graph) {
    if (!graph) return;
    
    // Free call edges
    for (size_t i = 0; i < graph->functions->nfunctions; i++) {
        CallEdge *edge = graph->edges[i];
        while (edge) {
            CallEdge *next = edge->next;
            free(edge);
            edge = next;
        }
    }
    
    free(graph->edges);
    free(graph->edge_counts);
    free(graph->topo_order);
    free(graph->visited);
    free(graph);
}

/**
 * @brief Frees an array of function summaries.
 * @param summaries Pointer to the summaries array.
 * @param count Number of summaries.
 */
void function_summaries_free(FunctionSummary *summaries, size_t count) {
    if (!summaries) return;
    
    for (size_t i = 0; i < count; i++) {
        free(summaries[i].arg_info);
    }
    
    free(summaries);
}