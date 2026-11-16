# P0046 - Define safe direct static string-field return cleanup

- Date: 2026-11-16 UTC
- Task: T011 static field/backing lifetime audit (documentation only)
- Parent checkpoint: `18242ec38b8188f4d109654746ee721ad4ced376`
- Commit lookup: exact `Pass-ID: P0046` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Complete P0045's requested static scalar string-field result audit. The new
[contract](../../docs/development/static-member-return-cleanup.md) traces static
declaration, inherited owner lookup, raw assignment, copied local snapshots,
return qualification, whole-body proof and runtime lifetime. A global pointer
slot does not own its pointee: direct or aliased writes/calls can install local
storage, and previously released backing cannot be repaired at return.

Select direct class type-qualified scalar static fields under existing whole-body
read-only barriers. Require an unshadowed type reference and actual static scalar
declaration/owner, capture once before reverse cleanup, and add no field ownership
or result copy/retain. Define both-mode lifetime/C/output acceptance and unsafe
emission-only controls. Conditional static leaves and struct/instance/base/nested
receivers remain separate. Static pointer/array declarations also have a distinct
emitter shape limitation; exclude them without changing that emitter in the
planned return slice.

State, backlog, pass index, source map and ownership contract link the design.
Compiler, fixtures, tests, runner, build/CI inputs and measured snapshot are unchanged.
Counterexamples are source-derived; no new runtime behavior is claimed.

## Validation

- Verified approved checkout, attached branch, clean worktree, configured origin,
  recent all-ref history, preceding automation memory and one registered worktree.
  Available chats showed no competing active project writer or unfinished pass.
- Schedule view/TOML already use local execution and the approved cwd; unchanged.
- Restricted fetch failed DNS; approved `git fetch origin
  codex/compiler-stabilization` succeeded. HEAD and fetched branch matched the
  parent above, with zero commits on either side.
- P0045 [native CI 43805269601](https://github.com/Ddemon26/DreamCompiler/actions/runs/43805269601)
  completed successfully for the exact parent SHA (run metadata checked separately).
  Linux job 133094487062 and Windows job 133094487295 passed build, Python runner,
  Go quick compiler regressions and full Dream suite. Restricted CLI metadata
  lookup failed network access; approved read succeeded. Connector job results
  supplied the per-step outcomes.
- Compiler tests were **not rerun** for this documentation-only audit. Reuse
  **P0045, 2026-11-16 UTC**, [MostRecentTestResults.md](../MostRecentTestResults.md):
  Windows Zig 0.17.1, 171 discovered, 153 passed, 18 skipped, zero failures/errors;
  all 37 Go quick groups, five Python tests and Linux cross-build 15/15 passed.
  Native parent CI above is separate evidence from that local cross-build.
- All **150 local Markdown paths across seven documents**, UTF-8 checks,
  **21 source-symbol references in six source files**, P0001-P0046 index continuity
  and current handoff checks passed. These documents contain no local fragment
  links requiring anchor checks. Reviewed all seven new/changed documents against
  the implementation paths above; `git diff --check` passed. `git diff --exit-code
  HEAD -- src tests codex .github build.zig tasks/MostRecentTestResults.md` passed,
  confirming unchanged tracked compiler/test/runner/CI/build inputs and snapshot.
  Precommit fetch and `git ls-remote` still matched the parent; no divergence or
  unrelated changes were present.

## Limits and handoff

The audit is complete; static-result early cleanup is not implemented. All prior
ownership/shape limits and eighteen skips persist. Valid incoming backing and
single-threaded synchronous execution remain preconditions. Conditional static
leaves, caller consumption, fields/aliases/containers/destructors and async or
exception unwinding remain separate; no static initializer/shape repair is claimed.

Publish and verify the remote SHA, then inspect native CI and repair failures first.
Next implement the direct class type-qualified static field slice, starting with
both-mode retained-allocation reproduction and mutation/call/shape fallback controls.
Preserve P0045 controls and lifetime assertions. End after this pass.
