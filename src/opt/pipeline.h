/**
 * @file pipeline.h
 * @brief Header file for the optimization pipeline.
 *
 * This file declares the run_pipeline function, which executes a series of
 * optimization passes on a control flow graph (CFG).
 */

#ifndef PIPELINE_H
#define PIPELINE_H
#include "../cfg/cfg.h"
#include <stdbool.h>

/**
 * @brief Executes a series of optimization passes on a control flow graph (CFG).
 *
 * @param cfg Pointer to the control flow graph to optimize.
 * @param opt_level Optimization level (0-3).
 */
void run_pipeline(CFG *cfg, int opt_level);

#endif