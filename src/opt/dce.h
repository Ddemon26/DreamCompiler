#ifndef DCE_H
#define DCE_H
#include "../cfg/cfg.h"
/**
 * @brief Performs dead code elimination on a control flow graph.
 *
 * @param cfg Pointer to the control flow graph to optimize.
 * @return true if the CFG was modified.
 */
bool dce(CFG *cfg);

#endif
