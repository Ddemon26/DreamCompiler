/**
 * @file value_numbering.h
 * @brief Header file for value numbering optimization.
 *
 * This file declares the function used to perform value numbering
 * on a control flow graph (CFG).
 */

#ifndef VALUE_NUMBERING_H
#define VALUE_NUMBERING_H
#include "../cfg/cfg.h"

/**
 * @brief Performs value numbering on the control flow graph.
 *
 * This function processes the control flow graph to eliminate
 * redundant computations by assigning unique numbers to equivalent
 * expressions.
 *
 * @param cfg Pointer to the control flow graph to process.
 */
void value_numbering(CFG *cfg);

#endif