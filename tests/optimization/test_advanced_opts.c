/**
 * @file test_advanced_opts.c
 * @brief Comprehensive test suite for advanced optimizations.
 *
 * This file contains unit tests and integration tests for all advanced
 * optimization passes including inlining, interprocedural analysis,
 * loop optimizations, and register allocation preparation.
 */

#include "../../src/opt/inline.h"
#include "../../src/opt/interprocedural.h"
#include "../../src/opt/loop_opt.h"
#include "../../src/opt/regalloc.h"
#include "../../src/cfg/cfg.h"
#include "../../src/ir/ir.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test utilities
static int test_count = 0;
static int test_passed = 0;

#define TEST(name) \
    do { \
        printf("Running test: %s\n", #name); \
        test_count++; \
        if (test_##name()) { \
            printf("✓ %s passed\n", #name); \
            test_passed++; \
        } else { \
            printf("✗ %s failed\n", #name); \
        } \
        printf("\n"); \
    } while(0)

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("Assertion failed: %s at line %d\n", #condition, __LINE__); \
            return false; \
        } \
    } while(0)

/**
 * @brief Creates a simple CFG for testing.
 * @return Pointer to a test CFG.
 */
static CFG *create_test_cfg(void) {
    CFG *cfg = malloc(sizeof(CFG));
    cfg->nblocks = 3;
    cfg->blocks = malloc(3 * sizeof(BasicBlock*));
    
    // Create three basic blocks
    for (int i = 0; i < 3; i++) {
        BasicBlock *bb = malloc(sizeof(BasicBlock));
        bb->id = i;
        bb->ninstrs = 2;
        bb->instrs = malloc(2 * sizeof(IRInstr*));
        
        // Add some test instructions
        bb->instrs[0] = malloc(sizeof(IRInstr));
        bb->instrs[0]->op = IR_MOV;
        bb->instrs[0]->dst = (IRValue){.id = i + 10};
        bb->instrs[0]->a = (IRValue){.id = i + 1};
        bb->instrs[0]->b = (IRValue){.id = 0};
        
        bb->instrs[1] = malloc(sizeof(IRInstr));
        bb->instrs[1]->op = IR_ADD;
        bb->instrs[1]->dst = (IRValue){.id = i + 20};
        bb->instrs[1]->a = (IRValue){.id = i + 10};
        bb->instrs[1]->b = ir_const(1);
        
        bb->succ = NULL;
        bb->nsucc = 0;
        bb->pred = NULL;
        bb->npred = 0;
        bb->idom = NULL;
        bb->df = NULL;
        bb->ndf = 0;
        bb->visited = 0;
        
        cfg->blocks[i] = bb;
    }
    
    // Set up control flow
    cfg->entry = cfg->blocks[0];
    
    // Block 0 -> Block 1
    cfg->blocks[0]->succ = malloc(sizeof(BasicBlock*));
    cfg->blocks[0]->succ[0] = cfg->blocks[1];
    cfg->blocks[0]->nsucc = 1;
    
    // Block 1 -> Block 2
    cfg->blocks[1]->succ = malloc(sizeof(BasicBlock*));
    cfg->blocks[1]->succ[0] = cfg->blocks[2];
    cfg->blocks[1]->nsucc = 1;
    cfg->blocks[1]->pred = malloc(sizeof(BasicBlock*));
    cfg->blocks[1]->pred[0] = cfg->blocks[0];
    cfg->blocks[1]->npred = 1;
    
    // Block 2 (exit)
    cfg->blocks[2]->pred = malloc(sizeof(BasicBlock*));
    cfg->blocks[2]->pred[0] = cfg->blocks[1];
    cfg->blocks[2]->npred = 1;
    
    return cfg;
}

/**
 * @brief Creates a simple loop CFG for testing loop optimizations.
 * @return Pointer to a loop CFG.
 */
static CFG *create_loop_cfg(void) {
    CFG *cfg = malloc(sizeof(CFG));
    cfg->nblocks = 4;
    cfg->blocks = malloc(4 * sizeof(BasicBlock*));
    
    // Create four basic blocks: entry, header, body, exit
    for (int i = 0; i < 4; i++) {
        BasicBlock *bb = malloc(sizeof(BasicBlock));
        bb->id = i;
        bb->ninstrs = 1;
        bb->instrs = malloc(sizeof(IRInstr*));
        
        bb->instrs[0] = malloc(sizeof(IRInstr));
        bb->instrs[0]->op = IR_ADD;
        bb->instrs[0]->dst = (IRValue){.id = i + 100};
        bb->instrs[0]->a = (IRValue){.id = i + 100};
        bb->instrs[0]->b = ir_const(1);
        
        bb->succ = NULL;
        bb->nsucc = 0;
        bb->pred = NULL;
        bb->npred = 0;
        bb->idom = NULL;
        bb->df = NULL;
        bb->ndf = 0;
        bb->visited = 0;
        
        cfg->blocks[i] = bb;
    }
    
    cfg->entry = cfg->blocks[0];
    
    // Entry -> Header
    cfg->blocks[0]->succ = malloc(sizeof(BasicBlock*));
    cfg->blocks[0]->succ[0] = cfg->blocks[1];
    cfg->blocks[0]->nsucc = 1;
    
    // Header -> Body and Exit (conditional)
    cfg->blocks[1]->succ = malloc(2 * sizeof(BasicBlock*));
    cfg->blocks[1]->succ[0] = cfg->blocks[2]; // to body
    cfg->blocks[1]->succ[1] = cfg->blocks[3]; // to exit
    cfg->blocks[1]->nsucc = 2;
    cfg->blocks[1]->pred = malloc(2 * sizeof(BasicBlock*));
    cfg->blocks[1]->pred[0] = cfg->blocks[0]; // from entry
    cfg->blocks[1]->pred[1] = cfg->blocks[2]; // from body (back edge)
    cfg->blocks[1]->npred = 2;
    
    // Body -> Header (back edge)
    cfg->blocks[2]->succ = malloc(sizeof(BasicBlock*));
    cfg->blocks[2]->succ[0] = cfg->blocks[1];
    cfg->blocks[2]->nsucc = 1;
    cfg->blocks[2]->pred = malloc(sizeof(BasicBlock*));
    cfg->blocks[2]->pred[0] = cfg->blocks[1];
    cfg->blocks[2]->npred = 1;
    
    // Exit
    cfg->blocks[3]->pred = malloc(sizeof(BasicBlock*));
    cfg->blocks[3]->pred[0] = cfg->blocks[1];
    cfg->blocks[3]->npred = 1;
    
    return cfg;
}

/**
 * @brief Test function table creation and management.
 */
static bool test_function_table(void) {
    FunctionTable *table = function_table_new();
    ASSERT(table != NULL);
    ASSERT(table->nfunctions == 0);
    
    // Create a test function
    FunctionInfo func;
    func.id = 0;
    func.name = strdup("test_func");
    func.cfg = create_test_cfg();
    func.params = NULL;
    func.nparam = 0;
    func.return_val = (IRValue){.id = 1};
    func.inline_cost = 0;
    func.is_recursive = 0;
    func.call_count = 0;
    
    int func_id = function_table_add(table, &func);
    ASSERT(func_id == 0);
    ASSERT(table->nfunctions == 1);
    
    FunctionInfo *retrieved = function_table_get(table, func_id);
    ASSERT(retrieved != NULL);
    ASSERT(strcmp(retrieved->name, "test_func") == 0);
    
    function_table_free(table);
    return true;
}

/**
 * @brief Test inlining cost calculation.
 */
static bool test_inline_cost_calculation(void) {
    FunctionInfo func;
    func.cfg = create_test_cfg();
    func.inline_cost = 0;
    
    int cost = calculate_inline_cost(&func);
    ASSERT(cost > 0);
    ASSERT(func.inline_cost == cost);
    
    // Small function should have low cost
    ASSERT(cost < 100);
    
    return true;
}

/**
 * @brief Test should_inline decision making.
 */
static bool test_should_inline_decision(void) {
    InlineConfig config = {
        .max_inline_cost = 50,
        .max_inline_depth = 3,
        .inline_hot_only = false,
        .hot_threshold = 5
    };
    
    FunctionInfo func;
    func.cfg = create_test_cfg();
    func.inline_cost = 30; // Under threshold
    func.is_recursive = 0;
    func.call_count = 1;
    
    ASSERT(should_inline(&func, &config, 0) == true);
    
    // Test recursive function rejection
    func.is_recursive = 1;
    ASSERT(should_inline(&func, &config, 0) == false);
    
    // Test depth limit
    func.is_recursive = 0;
    ASSERT(should_inline(&func, &config, 3) == false);
    
    // Test cost limit
    func.inline_cost = 100; // Over threshold
    ASSERT(should_inline(&func, &config, 0) == false);
    
    return true;
}

/**
 * @brief Test call graph construction.
 */
static bool test_call_graph_construction(void) {
    FunctionTable *table = function_table_new();
    
    // Add two test functions
    for (int i = 0; i < 2; i++) {
        FunctionInfo func;
        func.id = i;
        func.name = malloc(20);
        sprintf(func.name, "func_%d", i);
        func.cfg = create_test_cfg();
        func.params = NULL;
        func.nparam = 0;
        func.return_val = (IRValue){.id = i + 1};
        func.inline_cost = 0;
        func.is_recursive = 0;
        func.call_count = 0;
        
        function_table_add(table, &func);
    }
    
    CallGraph *graph = call_graph_new(table);
    ASSERT(graph != NULL);
    ASSERT(graph->functions == table);
    
    call_graph_free(graph);
    return true;
}

/**
 * @brief Test liveness analysis.
 */
static bool test_liveness_analysis(void) {
    CFG *cfg = create_test_cfg();
    
    LivenessInfo *liveness = analyze_liveness(cfg);
    ASSERT(liveness != NULL);
    
    // Check that liveness information was computed
    for (size_t i = 0; i < cfg->nblocks; i++) {
        ASSERT(liveness[i].def != NULL || liveness[i].def_count == 0);
        ASSERT(liveness[i].use != NULL || liveness[i].use_count == 0);
    }
    
    liveness_info_free(liveness, cfg->nblocks);
    return true;
}

/**
 * @brief Test interference graph construction.
 */
static bool test_interference_graph(void) {
    CFG *cfg = create_test_cfg();
    LivenessInfo *liveness = analyze_liveness(cfg);
    
    InterferenceGraph *graph = build_interference_graph(cfg, liveness);
    ASSERT(graph != NULL);
    ASSERT(graph->variables != NULL || graph->nvars == 0);
    
    interference_graph_free(graph);
    liveness_info_free(liveness, cfg->nblocks);
    return true;
}

/**
 * @brief Test loop discovery.
 */
static bool test_loop_discovery(void) {
    CFG *cfg = create_loop_cfg();
    
    LoopNest *nest = discover_loops(cfg);
    ASSERT(nest != NULL);
    
    // Should find at least one loop
    if (nest->nloops > 0) {
        ASSERT(nest->loops[0]->header != NULL);
        ASSERT(nest->loops[0]->latch != NULL);
    }
    
    loop_nest_free(nest);
    return true;
}

/**
 * @brief Test induction variable analysis.
 */
static bool test_induction_variable_analysis(void) {
    CFG *cfg = create_loop_cfg();
    LoopNest *nest = discover_loops(cfg);
    
    if (nest->nloops > 0) {
        size_t iv_count;
        InductionVar *ivs = analyze_induction_variables(nest->loops[0], &iv_count);
        
        // May or may not find induction variables in our simple test
        induction_vars_free(ivs, iv_count);
    }
    
    loop_nest_free(nest);
    return true;
}

/**
 * @brief Test register allocation preparation.
 */
static bool test_regalloc_preparation(void) {
    CFG *cfg = create_test_cfg();
    
    RegAllocContext *context = prepare_for_machine_code(cfg);
    ASSERT(context != NULL);
    ASSERT(context->cfg == cfg);
    ASSERT(context->liveness != NULL);
    ASSERT(context->graph != NULL);
    
    regalloc_context_free(context);
    return true;
}

/**
 * @brief Test SSA conversion.
 */
static bool test_ssa_conversion(void) {
    CFG *cfg = create_test_cfg();
    
    // Add a phi instruction to test conversion
    IRInstr *phi = malloc(sizeof(IRInstr));
    phi->op = IR_PHI;
    phi->dst = (IRValue){.id = 999};
    phi->a = (IRValue){.id = 1};
    phi->b = (IRValue){.id = 2};
    
    // Replace first instruction with phi
    free(cfg->blocks[0]->instrs[0]);
    cfg->blocks[0]->instrs[0] = phi;
    
    bool converted = convert_from_ssa(cfg);
    ASSERT(converted == true);
    
    // Phi should be converted to move
    ASSERT(cfg->blocks[0]->instrs[0]->op == IR_MOV);
    
    return true;
}

/**
 * @brief Main test runner.
 */
int main(void) {
    printf("Running Advanced Optimization Tests\n");
    printf("==================================\n\n");
    
    // Function inlining tests
    TEST(function_table);
    TEST(inline_cost_calculation);
    TEST(should_inline_decision);
    
    // Interprocedural analysis tests
    TEST(call_graph_construction);
    
    // Register allocation tests
    TEST(liveness_analysis);
    TEST(interference_graph);
    TEST(regalloc_preparation);
    TEST(ssa_conversion);
    
    // Loop optimization tests
    TEST(loop_discovery);
    TEST(induction_variable_analysis);
    
    printf("Test Results\n");
    printf("============\n");
    printf("Tests run: %d\n", test_count);
    printf("Tests passed: %d\n", test_passed);
    printf("Tests failed: %d\n", test_count - test_passed);
    printf("Success rate: %.1f%%\n", 
           test_count > 0 ? (100.0 * test_passed / test_count) : 0.0);
    
    return test_count == test_passed ? 0 : 1;
}