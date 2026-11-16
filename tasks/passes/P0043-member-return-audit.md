# P0043 - Define safe borrowed member-return cleanup

- Date: 2026-11-16 UTC
- Task: T011 prerequisite member/backing lifetime audit (documentation only)
- Parent checkpoint: `3038fdcaad863e287232971fb48ec34ae6f96acd`
- Commit lookup: exact `Pass-ID: P0043` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Complete P0042's requested direct borrowed member-result audit before enabling
scoped cleanup. The [contract](../../docs/development/member-return-cleanup.md)
traces member lookup/shape, parameter bindings, field reads/writes, scalar copies,
class cleanup, runtime release and current return qualification.

An unchanged receiver does not establish field backing lifetime. Direct/aliased
writes, calls and constructors can install callee-local storage. Class alias locals
receive no retain but can release the borrowed receiver at an earlier nested-block
exit. A shallow extension of P0041's parameter-slot scan would be unsafe. Examples
are source-derived analysis, not executed regressions; no new behavior is claimed.

The selected slice requires a direct scalar nonstatic string field on a borrowed
depth-1 class receiver/explicit this, actual field/owner resolution and a separate
whole-body read-only proof. Reject every write/call/new, custom-type/pointer/array
local, exception/async, shadowing and unknown node. Reuse once-only capture before
reverse local cleanup without result copy/retain or receiver release. Failed proof
preserves entire prior emission; conditional member leaves and static/struct/local/
nested/base receivers stay separate. The acceptance matrix requires both-mode
pre-fix lifetime reproduction, exact C/output/refcounts, caller survival and
mutation/call/class-alias fallback controls.

State, backlog, pass index, source map and ownership contract link the design.
Compiler, tests, runner, build inputs and measured results are unchanged.

## Validation

- Verified approved checkout path, attached branch, clean worktree, origin, one
  registered worktree, recent all-ref history and preceding automation memory.
  No unfinished/unpublished pass or competing active project chat was found.
- Schedule view/TOML already use local execution and the approved cwd; unchanged.
- Restricted fetch failed DNS; approved normal `git fetch origin
  codex/compiler-stabilization` succeeded. HEAD and fetched remote matched the
  parent above, confirming P0042 delivery.
- P0042 [CI run 43757071666](https://github.com/Ddemon26/DreamCompiler/actions/runs/43757071666):
  Linux job 132943379666 and Windows job 132943379883 completed successfully,
  including build, runner, quick compiler regressions and full Dream suite.
- Compiler tests were **not rerun** for this documentation-only audit. Reused
  evidence: P0042, **2026-11-15 UTC**, [MostRecentTestResults.md](../MostRecentTestResults.md):
  Windows 169 discovered, 151 passed, 18 skipped, zero failures/errors; all 35 quick
  groups, five Python tests and Linux cross-build 15/15 passed. Native CI above is
  separate from historical local cross-build evidence.
- All **133 local Markdown paths across seven documents**, UTF-8/BOM checks,
  **18 source-symbol references in six source files**, pass-index continuity and
  P0043 delivery/handoff checks passed. Reviewed all seven new/changed documents
  against the source paths above; `git diff --check` passed. `git diff --exit-code
  HEAD -- src tests codex .github build.zig tasks/MostRecentTestResults.md` passed,
  confirming unchanged tracked compiler/test/runner/CI/build inputs and snapshot.
  Precommit fetch and `git ls-remote` still matched the parent; no divergence or
  unrelated changes were present.

## Limits and handoff

This delivers the requested audit and a conservative implementation contract.
Member returns still have previous emission and retained scoped allocations;
counterexamples are source-derived, and acceptance tests are future work.
No skipped fixture or broader ownership/shape/alias behavior becomes qualified.
The read-only subset intentionally rejects harmless effects rather than claiming
interprocedural purity or alias analysis.

Publish and verify the remote SHA, then inspect native CI; repair failures first.
Next implement the selected direct borrowed class-member slice, starting with
both-mode retained-allocation reproduction and mutation/call/class-alias fallback
controls. Keep P0041/P0042 behavior and assertions intact. End after this pass.
