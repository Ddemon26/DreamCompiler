# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0009 - checked literal arithmetic reaches emitted C**.
- Record: [P0009](passes/P0009-literal-emission.md).
- Parent/fetched approved remote: `2d9c6ea191cb7af8b2164aa735cb170c88cd3c3b` (P0008).
- Commit lookup: exact `Pass-ID: P0009` trailer in git history.
- Verify P0009 publication and inspect CI before allocating P0010.

Use only `C:/Users/damon/cApps/DreamCompiler`, attached to the integration branch.
P0009 began clean with HEAD equal to the fetched approved branch, one registered
worktree, and no competing writer in the app inventory. History and prior memory
showed P0008 delivered and no unfinished pass. The schedule already uses local
execution and this directory. Restricted fetch failed DNS; approved fetch passed.
P0008 [CI run 42373515160](https://github.com/Ddemon26/DreamCompiler/actions/runs/42373515160)
completed successfully on both Linux and Windows, including stability/Dream suites.

T004's first bounded implementation now folds canonical decimal literal `+`/`-`/`*`
trees through isolated SCCP IR into the AST emitter at `-O1`/`-O2`/`-O3`. Default
emission retains arithmetic. All leaves/intermediates must be in `0..INT_MAX-1`,
with at most 256 operations and depth 64. Unsupported maximal binary/unary trees
remain intact. Known containers and function/method/imported bodies are visited.
The transformed root is verified before committing arena-owned text with original
source metadata. Internal failures return before outputs are opened. The driver
no longer executes the unqualified discarded whole-program pipeline.

The common object emitter now finds runtime headers from temporary C and returns
failures to the CLI. Both modes still expect the project working directory/runtime
sources. See [the implementation contract](../docs/development/optimization-emission.md).

Windows validation on 2026-11-08 UTC: Zig 0.17.1; unchanged baseline 118 passes/18
skips, final full Dream suite **137 discovered, 119 passed, 18 skipped, zero
failures/errors**; all seven Go quick groups, five Python runner tests, and Linux
cross-build passed. Focused C/runtime checks pass at default/`-O1`/`-O2`/`-O3`,
including side effects, method/module bodies, fallback, and resource boundaries.
A real-implementation UBSan harness verifies allocation/invariant errors and atomic
replacement. Initial object emission failed on missing runtime headers; it was fixed
and retested. No SDK/cache changes, weakened tests, or new exclusions were used.
See [MostRecentTestResults.md](MostRecentTestResults.md) for exact evidence.

P0009 Linux binaries were not executed locally; inspect its native CI after push.
The 18 unannotated entries remain unverified. Supporting modules remain fixtures;
graphics/concurrency exclusions remain. General CFG/SSA emission, mutable-variable
propagation, and loop transforms remain unqualified. Method argument shapes, field
initializers, all-path returns, and other deferred language work are unchanged.

After verified publication, inspect P0009 CI and fix any failed baseline first.
Then take T005: primitive scalar method argument compatibility, preserving receiver
resolution and adding ordinary/inherited/static/instance and module entry tests,
useful diagnostics, and artifact-free rejection. Leave broader pointer/array shapes
for later. End this invocation after P0009; preserve older ignored recovery evidence.
