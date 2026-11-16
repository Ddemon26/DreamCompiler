# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0045 - conditional read-only instance string-field return cleanup**.
- Record: [P0045](passes/P0045-conditional-member-return.md).
- Parent/fetched approved remote: `106d95cd0db5a727e8a7f0a577670f0e81f91b93` (P0044).
- Resolve this pass commit by its exact `Pass-ID: P0045` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0045 qualifies conditional result trees mixing proven scalar nonstatic `this.field`
reads, literals, known-owned scalar locals and unchanged scalar parameters. Any
member leaf requires P0044's unchanged whole-body read-only proof, including
conditions, unselected branches and later code. One unsupported leaf preserves
the entire previous return. Capture once before reverse cleanup and preserve only
selected owned storage; no member result copy/retain or receiver release is added.
Inherited casts, collision-safe temporaries and no-local direct returns remain intact.

The initial both-mode matrix reproduced 50 live allocations instead of four before
the fix. Final coverage adds two inherited two-field reads: each mode checks 59
nonnull releases (one receiver, 58 strings), 27 output lines, four original-reference
owned survivors until shutdown and an empty registry afterward. The shared 196
emission controls run on conditional trees, plus fourteen expression rejections and
two nested-else positives. Caller backing and independent snapshots survive.

Clean approved checkout/attached branch/origin matched published P0044; one
registered worktree and available chats showed no competing active writer.
Schedule already uses the approved local cwd. P0044
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43793738851)
passed build, runner, quick and full-suite steps before this work.

Windows full suite: 171 discovered, 153 passed, 18 skipped, zero failures/errors;
37 Go quick groups, five Python runner tests and Linux cross-build 15/15 passed.
Cross-build evidence does not prove native P0045 Linux runtime behavior. See
[results](MostRecentTestResults.md) and the pass record for exact checks/timings.

Next invocation:

1. Verify P0045 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Audit static scalar string-field result lifetime and mutation/alias barriers,
   then define a bounded implementation contract and both-mode acceptance matrix.
3. Preserve retained owned-result accumulation/caller consumption, class-parameter
   grammar limits, local/struct/nested/base receivers, aliases/containers/destructors/
   fields, async/exception unwinding, shape/operator limits and eighteen skips.
   String-index return typing, custom-type for headers and class null initializers
   remain limited. Completed P0026-P0029 maintenance must not repeat.
