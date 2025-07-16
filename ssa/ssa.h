#ifndef SSA_H
#define SSA_H
#include "../cfg/cfg.h"
#include <stdbool.h>

void ssa_place_phi(CFG *cfg, int nvars);
void ssa_rename(CFG *cfg, int nvars);
bool ssa_verify(CFG *cfg);

#endif
