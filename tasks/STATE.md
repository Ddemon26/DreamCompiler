# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0043 - borrowed member-return lifetime audit and implementation contract**.
- Record: [P0043](passes/P0043-member-return-audit.md).
- Parent/fetched approved remote: `3038fdcaad863e287232971fb48ec34ae6f96acd` (P0042).
- Resolve this pass commit by its exact `Pass-ID: P0043` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0043 completes the requested source audit without changing compiler behavior.
An unchanged receiver slot does not prove unchanged field backing: direct/aliased
field writes and calls/constructors can install local storage. Class alias locals
also receive releases without a retain proof, including in already-exited scopes.
P0041's parameter proof cannot be applied transitively to member results.

The [implementation contract](../docs/development/member-return-cleanup.md) selects
direct scalar string instance fields on borrowed depth-1 class parameters/explicit
`this`, with a separate whole-body read-only proof. Reject all writes, calls/new,
custom-type/pointer/array locals, exceptions/async, shadowing and unknown nodes.
Resolve actual scalar/nonstatic field shape and inherited owner before reusing
capture-before-reverse-cleanup. Preserve the entire previous return on failed proof.
Conditional member leaves, static/struct/local/nested receivers and `base` results
remain separate. The contract specifies positive lifetime/output/C checks and
mutation/alias/call fallback controls for the implementation pass.

Clean approved checkout/attached branch/origin matched published P0042; one
registered worktree and available chats showed no competing active writer.
Schedule already uses the approved local cwd. P0042
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43757071666)
passed build, runner, quick and full-suite steps before this audit.

Compiler tests were **not rerun** for this documentation-only pass. Reuse P0042's
**2026-11-15 UTC** [results](MostRecentTestResults.md): 169 discovered, 151 passed,
18 skipped, zero failures/errors; 35 quick groups, five runner tests and Linux
cross-build 15/15 passed. Source/test/build inputs and the measured snapshot remain
unchanged. See the pass record for documentation verification.

Next invocation:

1. Verify P0043 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Implement the selected read-only direct borrowed class-member result slice,
   beginning with both-mode retained-allocation reproduction and the contract's
   whole-body mutation/call/class-alias fallback controls.
3. Preserve retained owned-result accumulation, aliases/containers/destructors/
   fields, async/exception unwinding, shape/operator limits and eighteen skips.
   String-index return typing, custom-type for headers and class null initializers
   remain limited. Completed P0026-P0029 maintenance must not repeat.
