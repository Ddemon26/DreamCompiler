# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0030 - conditional scalar string initializer ownership**.
- Record: [P0030](passes/P0030-conditional-string-ownership.md).
- Parent/fetched approved remote: `f1d61a75fcfe23c3cdb013809f0f55ffed705aff` (P0029).
- Resolve this pass commit by its exact `Pass-ID: P0030` trailer.
- Delivery requires push and matching `git ls-remote`; verify native CI next.

**Compiler stabilization has resumed.** P0026/P0027 pruning and P0028/P0029
website/release work are completed maintenance history. Do not repeat them from
older scheduled-run memory. The repository handoff, ordered backlog and verified
pass trailers define the current checkpoint. The reusable daily prompt now makes
this precedence explicit; no schedule configuration or hosting settings changed.

P0030 adds conditional (`?:`) scalar string declarations to the existing copy
boundary in `src/codegen/stmt.c`. One copy wraps the entire conditional, preserving
once-only condition and selected-branch evaluation. The declaration owns its copy
and uses existing scalar replacement, block/header and break/continue cleanup.
Literal/empty/borrowed/managed/typed-null and nested branches are covered. Fresh
selected managed results can remain registered until shutdown; no result ownership
transfer or literal null/string branch type expansion is claimed.

P0029 publication was verified at the parent SHA. Its
[native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43661838320)
passed both Linux and Windows jobs. This pass started clean in the approved
attached checkout, one worktree, with no competing active project writer. Full
Windows checks now pass: **155 discovered, 137 passed, 18 skipped, zero failures/
errors**, **23 Go quick groups**, **5 runner tests**, and Linux cross-build.
The new ordinary/module-entry checked-runtime matrix requires **19 conditional
copy sites, 22 nonnull releases and 24 exact output lines** per mode. Both modes
failed live-registry cleanup before the fix. See [results](MostRecentTestResults.md).
Cross-build/harness cross-linking does not prove native Linux runtime behavior.

Next invocation:

1. Verify P0030 publication by trailer and remote SHA, then inspect its native CI.
   Repair failed checks before new work.
2. Continue T011 with **class-instance cleanup on break/continue**. Reproduce
   body-local class allocations retained until shutdown with checked live-registry
   and pre-shutdown release evidence. Audit `emit_control_jump` against normal
   class cleanup, preserving outer/header bindings, nested loop/switch targets
   and all existing scalar-string behavior in ordinary/module-entry modes.
3. Keep destructors/field ownership, return ownership and exception/finally
   unwinding separate. Preserve all 18 existing skips and shape/operator limits.
