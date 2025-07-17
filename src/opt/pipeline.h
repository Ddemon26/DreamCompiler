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
 * @param opt1 Boolean flag to enable or disable the optimization pipeline.
 */
void run_pipeline(CFG *cfg, bool opt1);

#endif