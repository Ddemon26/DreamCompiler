/**
 * @file licm.h
 * @brief Header file for the Loop-Invariant Code Motion (LICM) optimization.
 *
 * This file declares the LICM function, which performs loop-invariant code motion
 * optimization on a control flow graph (CFG).
 */

#ifndef LICM_H
#define LICM_H
#include "../cfg/cfg.h"

/**
 * @brief Performs loop-invariant code motion (LICM) optimization on a control flow graph (CFG).
 *
 * @param cfg Pointer to the control flow graph to optimize.
 * @return true if the CFG was modified.
 */
bool licm(CFG *cfg);

#endif