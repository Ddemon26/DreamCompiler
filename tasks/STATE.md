# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0017 - give literal string declarations managed storage**.
- Record: [P0017](passes/P0017-literal-string-ownership.md).
- Parent/fetched approved remote: `08cfbed6bb06a236eea6c307d46abfaad72d016d` (P0016).
- Resolve the pass commit by its exact `Pass-ID: P0017` trailer.
- Publication requires push and matching `git ls-remote`; report that identity
  after delivery. Inspect native CI before selecting the next task.

The 2026-11-13 pass started clean in the approved primary checkout, attached branch
and origin, one worktree, no unpublished pass or competing active project writer.
Approved fetch matched HEAD after restricted DNS failure. Automation uses local
execution and the approved cwd. P0016 native
[CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43431180404) passed
both Linux and Windows jobs; no baseline repair was needed.

P0017 completes one T011 slice. Scalar declarations initialized directly from
string literals now call `dream_string_copy`, allowing existing block cleanup to
release managed storage. Empty/escaped/parenthesized literals, const bindings,
nested blocks, loop bodies, free/static/instance functions and top-level storage
are covered. Concatenation allocation remains unchanged. The ordinary/module
matrix checks seven copy sites, seven release sites/eight actual releases and ten
exact output lines, using the real allocator's registry and single-reference
checks. Existing indexed ownership assertions and dependency behavior remain.
Both quick protocols and CI select the new matrix. Other ownership remains open.

Unchanged baseline: 127 passed/18 skipped/no failures. Before implementation both
focused modes failed the live-allocation assertion on literal cleanup; after it
both passed. Final full suite: **146 discovered, 128 passed, 18 skipped, zero
failures/errors**, 36.68s. All **fourteen Go quick groups passed**, 18.349s;
Python **five passed**, 0.056s. Linux compiler cross-build and both harness mode
cross-links passed; Linux binaries were not run locally. Native P0017 runtime
awaits CI. See [MostRecentTestResults.md](MostRecentTestResults.md).

Same 18 skips and previous exclusions remain. Identifier/call/for-header
initializers, alias/reassignment/return/field ownership, array/pointer locals and
early-exit cleanup remain unqualified. Copies add allocation cost. General shape,
bounds, null and operator correctness remain separate. Ownership-harness
const-discard warnings remain. Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17,
Go 1.27.4. Approved SDK/cache access used without content/permission changes;
compiler checks sequential.

Next: verify P0017 delivery and inspect native CI, repairing failures first.
Otherwise take a bounded **T011 scalar identifier initializer** slice: reproduce
alias cleanup across nested scopes with the checked allocator, preserve cleanup
and prove backing/copy lifetimes in both modes. Do not start a second pass here.
