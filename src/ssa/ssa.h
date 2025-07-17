#ifndef SSA_H
#define SSA_H
#include "../cfg/cfg.h"
#include <stdbool.h>

/**
 * @brief Places phi functions in the control flow graph.
 *
 * @param cfg Pointer to the control flow graph.
 * @param nvars Number of variables in the program.
 */
void ssa_place_phi(CFG *cfg, int nvars);

/**
 * @brief Renames variables in the control flow graph using stacks for tracking variable names.
 *
 * @param cfg Pointer to the control flow graph.
 * @param nvars Number of variables in the program.
 */
void ssa_rename(CFG *cfg, int nvars);

/**
 * @brief Verifies the correctness of the control flow graph.
 *
 * @param cfg Pointer to the control flow graph.
 * @return True if the control flow graph is valid, otherwise false.
 */
bool ssa_verify(CFG *cfg);

#endif
