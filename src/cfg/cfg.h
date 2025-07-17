#ifndef CFG_H
#define CFG_H
#include "../ir/ir.h"
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Represents a basic block in the control flow graph (CFG).
 *
 * A basic block contains a sequence of instructions and maintains
 * information about its successors, predecessors, and dominance relationships.
 *
 * @param id Unique identifier for the basic block.
 * @param dfnum Depth-first number assigned during traversal.
 * @param instrs Array of instructions in the basic block.
 * @param ninstrs Number of instructions in the basic block.
 * @param succ Array of successor basic blocks.
 * @param nsucc Number of successor basic blocks.
 * @param pred Array of predecessor basic blocks.
 * @param npred Number of predecessor basic blocks.
 * @param idom Immediate dominator of the basic block.
 * @param df Array of dominance frontier basic blocks.
 * @param ndf Number of dominance frontier basic blocks.
 */
typedef struct BasicBlock BasicBlock;
struct BasicBlock {
    int id;
    int dfnum;
    IRInstr **instrs;
    size_t ninstrs;
    BasicBlock **succ;
    size_t nsucc;
    BasicBlock **pred;
    size_t npred;
    BasicBlock *idom;
    BasicBlock **df;
    size_t ndf;
};

/**
 * @brief Represents a control flow graph (CFG).
 *
 * A CFG consists of an array of basic blocks, the total number of blocks,
 * and an entry block that serves as the starting point.
 *
 * @param blocks Array of pointers to basic blocks in the CFG.
 * @param nblocks Number of basic blocks in the CFG.
 * @param entry Entry point of the CFG.
 */
typedef struct {
    BasicBlock **blocks;
    size_t nblocks;
    BasicBlock *entry;
} CFG;

/**
 * @brief Creates a new control flow graph (CFG).
 *
 * Allocates and initializes a new CFG structure.
 *
 * @return Pointer to the newly created CFG.
 */
CFG *cfg_new(void);

/**
 * @brief Adds a new basic block to the CFG.
 *
 * Creates and appends a new basic block to the specified CFG.
 *
 * @param cfg Pointer to the CFG to which the block will be added.
 * @return Pointer to the newly created basic block.
 */
BasicBlock *cfg_add_block(CFG *cfg);

/**
 * @brief Adds a directed edge between two basic blocks in the CFG.
 *
 * Establishes a control flow relationship from one basic block to another.
 *
 * @param from Pointer to the source basic block.
 * @param to Pointer to the destination basic block.
 */
void cfg_add_edge(BasicBlock *from, BasicBlock *to);

/**
 * @brief Computes the dominator tree for the CFG.
 *
 * Calculates the immediate dominators for each basic block in the CFG.
 *
 * @param cfg Pointer to the CFG for which to compute the dominator tree.
 */
void cfg_compute_dominators(CFG *cfg);

#endif
