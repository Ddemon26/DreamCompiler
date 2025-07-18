/**
 * @file sccp.h
 * @brief Header file for the Sparse Conditional Constant Propagation (SCCP) optimization.
 *
 * This file declares the SCCP function, which performs Sparse Conditional
 * Constant Propagation on a control flow graph (CFG).
 */

#ifndef SCCP_H
#define SCCP_H
#include "../cfg/cfg.h"

/**
 * @brief Performs Sparse Conditional Constant Propagation (SCCP) on a control flow graph (CFG).
 *
 * @param cfg Pointer to the control flow graph to optimize.
 * @return true if the CFG was modified.
 */
bool sccp(CFG *cfg);

#endif