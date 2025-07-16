#include "pipeline.h"
#include "sccp.h"
#include "dce.h"
#include <stdbool.h>

void run_pipeline(CFG *cfg, bool opt1) {
    if (!cfg) return;
    if (opt1) {
        sccp(cfg);
        dce(cfg);
    }
}
