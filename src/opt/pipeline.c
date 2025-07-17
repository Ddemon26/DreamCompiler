#include "pipeline.h"
#include "sccp.h"
#include "dce.h"
#include "value_numbering.h"
#include "licm.h"
#include <stdbool.h>
/**
 * @brief Executes a series of optimization passes on a control flow graph (CFG).
 *
 * This function runs a pipeline of optimizations, including Sparse Conditional
 * Constant Propagation (SCCP), Dead Code Elimination (DCE), Value Numbering,
 * and Loop-Invariant Code Motion (LICM), if the opt1 flag is enabled.
 *
 * @param cfg Pointer to the control flow graph to optimize.
 * @param opt1 Boolean flag to enable or disable the optimization pipeline.
 */
void run_pipeline(CFG *cfg, bool opt1) {
    if (!cfg) return;
    if (opt1) {
        sccp(cfg);
        dce(cfg);
        value_numbering(cfg);
        licm(cfg);
    }
}
