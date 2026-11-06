# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0007 - stable optimized loop compilation and checked loop fixture**.
- Record: [P0007](passes/P0007-loop-optimization-stability.md).
- Parent and fetched approved remote: `f32f95ec373ee9aabc31b6c83d0ed0779a37247a` (P0006).
- Commit lookup: exact `Pass-ID: P0007` trailer in git history.
- Verify P0007 publication before allocating P0008 or starting T004.

Use only `C:/Users/damon/cApps/DreamCompiler`, attached to the integration branch.
This pass began clean with HEAD equal to the approved fetched branch. Git history,
the preceding automation memory, one registered worktree, and app thread inventory
showed no unfinished pass or competing project writer. The existing schedule already
uses local execution and this directory; no schedule change was needed. P0006 was
delivered, and CI run 42135769585 passed on both Linux and Windows.

T003 selected the previously skipped `tests/optimization/test_loop_opt.dr`. The
default setting printed the intended 112 and 8, but `-O1`/`-O2`/`-O3` crashed with
heap corruption. P0007 guards negative SCCP IDs, gives LICM's hoisted instructions
separate ownership, and makes LICM/loop candidate analysis report only real changes.
Intermediate fixes exposed nontermination at `-O2`; the final four-setting regression
passes. The annotated entry now runs at `-O2`, and the matrix is included in both Go
quick protocols and Linux/Windows CI. No compiler output or assertion was special-cased.

Validation on 2026-11-06 UTC, Windows, Zig 0.17.1: full Dream suite **136 discovered,
118 passed, 18 skipped, zero failures/errors** (42.32s); Go quick passed all five
groups (38 diagnostics, arena growth, warnings, four optimization settings, 25 module
cases; `-count=1`, 7.065s); five Python runner tests passed; Linux cross-build passed.
Restricted execution could not locate Zig, so compiler checks used approved installed
toolchain/cache access. Native Linux execution of P0007 awaits CI; a cross-build is
not runtime proof. See [MostRecentTestResults.md](MostRecentTestResults.md).

The remaining 18 entries are unchanged, unannotated skips, not verified features.
Supporting modules remain fixtures; graphics/async examples remain outside the suite.
Optimized IR still does not feed C emission, SSA construction remains disabled, and
this pass does not establish optimizer semantic correctness, transformed generated C,
or performance gains. Loop fusion, empty-loop elimination, and derived induction
variable recognition still lack CFG rewrites. Broader IR/CFG correctness is a T004
prerequisite. Method argument shapes, class field initializers, all-path return proofs,
virtual dispatch, access control, imported types/globals, async, and graphics remain
follow-ups.

After verified delivery, inspect P0007 CI first. Otherwise take T004: define a bounded
optimization-to-emission design with explicit supported lowering and fallback/error
behavior before implementing it. Do not treat the new execution matrix as proof that
the current IR is ready to emit. Preserve ignored recovery evidence from earlier passes.
