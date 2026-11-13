# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0019 - give member string declarations independent storage**.
- Record: [P0019](passes/P0019-member-string-ownership.md).
- Parent/fetched approved remote: `8a8329f2869399b602abbbec25f3e3d0039f0f48` (P0018).
- Resolve the pass commit by its exact `Pass-ID: P0019` trailer.
- Publication requires push and matching `git ls-remote`; report that identity
  after delivery. Inspect native CI before selecting the next task.

The 2026-11-13 pass started clean in the approved primary checkout, attached branch
and origin, one worktree, no unpublished pass or competing active project writer.
Approved fetch matched HEAD after restricted DNS failure. Automation uses local
execution and approved cwd. P0018
[CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43515951800) passed
native Linux and Windows jobs; no baseline repair was needed.

P0019 completes one T011 slice. Scalar declarations initialized directly from
member expressions (`ND_FIELD`/`ND_BASE`) call `dream_string_copy`, leaving source
ownership unchanged and giving each nonnull copy independent storage for normal
cleanup. The receiver runs once. Literal/managed/null fields, inherited `this`/
`base`, static fields, const/grouped declarations, nested blocks, loops, top-level
snapshots and a temporary receiver are covered. Concatenation, field assignment,
allocator and normal cleanup remain unchanged. The ordinary/module-entry matrix
checks ten member-copy sites, thirteen release sites, sixteen nonnull releases
and twenty-seven output lines per mode. Sources survive nested copy cleanup;
a top-level snapshot survives field replacement. Shared helper checks prove copy
lifetime after backing release. All previous ownership checks remain selected.

Unchanged baseline: 129 passed/18 skipped/no failures. Both new modes reproduced
invalid field cleanup with the live registry before implementation; all eight
ownership modes passed afterward. Final full suite: **148 discovered, 130 passed,
18 skipped, zero failures/errors**, 36.98s. All **sixteen Go quick groups passed**,
29.766s; Python **five passed**, 0.043s. Linux compiler cross-build and both new
harness mode cross-links passed; Linux binaries were not run locally. Native
P0019 runtime awaits CI. See [MostRecentTestResults.md](MostRecentTestResults.md).

Same 18 skips/exclusions remain. Call/for-header initializers, reassignment,
return/field ownership, array/pointer-local and early-exit cleanup remain
unqualified. Copies add allocation cost and require readable input. The temporary
receiver is reclaimed at shutdown by the existing registry. General shape/bounds/
operator correctness remains separate; existing harness const-discard warnings
remain. Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Approved
SDK/cache access used without content/permission changes; compiler checks sequential.

Next: verify P0019 delivery and inspect native CI, repairing failures first.
Otherwise take a bounded **T011 for-header declaration** slice: reproduce string
binding scope/cleanup emission for zero and nonzero iterations in both modes,
then qualify storage ownership and loop-exit cleanup. Do not start a second pass here.
