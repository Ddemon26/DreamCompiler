# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0014 - indexed scalar string ownership**.
- Record: [P0014](passes/P0014-indexed-string-ownership.md).
- Parent/fetched approved remote: `36adec3f7569686806195ab55a3d70d9a950c961` (P0013).
- Resolve the pass commit by its exact `Pass-ID: P0014` trailer.
- Publication requires push and matching `git ls-remote`; report that identity
  after delivery. Inspect native CI before selecting the next task.

The 2026-11-12 pass started clean in the approved primary checkout on the attached
branch/origin, one registered worktree, no unpublished agent pass or competing
active writer in app inventory. Approved fetch passed after restricted DNS failure
and matched HEAD. P0013 [CI run 43265683253](https://github.com/Ddemon26/DreamCompiler/actions/runs/43265683253)
passed on Windows and Linux. Automation already uses local execution and the
approved cwd; unchanged.

Scalar string declarations initialized directly from indexing now call
`dream_string_copy`, producing independent managed storage for literals or
allocated backing. Null is preserved. Normal block cleanup releases the copy;
top-level copies are reclaimed at shutdown. The allocator and other string
initializer/assignment paths are unchanged. See the
[bounded ownership checkpoint](../docs/development/string-ownership.md).

One annotated Dream entry and an ordinary/module-entry Go matrix check emitted
copy/release calls, exact direct-linked output and a checked allocator registry.
They cover nested blocks, literal/dynamic/null backing, top-level copying and a
once-only index side effect. The harness checks four nonnull local releases,
shutdown reclamation, independent copies, unchanged backing reference counts and
helper use after backing is freed. Both quick protocols and CI select the group.

Uninstrumented pre-change execution happened to pass; the checked live registry
reproduced invalid literal cleanup deterministically in both modes. Fixture setup
initially used unsupported shadowing, duplicate copied/source headers and a module
name conflict; corrected before qualification. The first full run caught incorrect
expected-line ordering in the new fixture; corrected to the Go oracle without
implementation or runtime assertion changes. All failures are recorded in P0014.

Windows toolchain: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.
Unchanged baseline: 125 passed, 18 skipped, zero failures/errors, 35.84s.
Final focused matrix passed, 2.317s. Final full suite: **144 discovered, 126 passed,
18 skipped, zero failures/errors**, 33.45s. All **twelve Go quick groups passed**,
15.706s. Python: **five passed**, 0.040s. Linux cross-build passed; native P0014
runtime awaits CI. SDK/cache checks used normal approved access without content
or permission changes. See [MostRecentTestResults.md](MostRecentTestResults.md).

The same 18 unannotated skips and exclusions remain. This adds a copy/allocation
cost for indexed scalar string declarations. For-header declarations, other initializer forms, assignment,
returns, early exits, fields, array/pointer-local cleanup and general string
ownership remain unqualified (T011). Pointer shapes, bounds, scalar string indexing,
async and general CFG/SSA behavior remain unsupported/unqualified.

Next: verify P0014 delivery and inspect native CI; repair failures first. Otherwise
take **T010**, indexed primitive console formatting. Do not start a second pass here.
