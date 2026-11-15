# P0031 - Class-local cleanup on break/continue

- Date: 2026-11-15 UTC (local reports November 14, America/Vancouver)
- Task: T011 bounded class-local jump cleanup slice
- Parent checkpoint: `d11cc7b280fe1da60122790d1634756a7aac03af`
- Commit lookup: exact `Pass-ID: P0031` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Class locals received normal block releases but were omitted by `emit_control_jump`.
Both ordinary/module-entry fixtures reproduced retained allocations with an empty
registry assertion before `dr_release_all`, rather than relying on shutdown to
reclaim them. `src/codegen/stmt.c` now includes the existing class classification
alongside owned strings at the existing loop/switch binding boundaries. Cleanup
runs in reverse order; emitter bindings remain available to other branches.

Added `tests/basics/memory/loop_exit_classes.dr` and `TestCompiler_LoopExitClasses`
to both Go quick protocols and CI. Exact C sequences check mixed class/string
cleanup before conditional jumps, excluding later declarations and surviving
outer/header storage. While/do/for, normal completion, nested loops/switches,
switch continue, loop-inside-switch, static/instance methods and top-level loops
are covered. Each mode requires 27 class and six string releases, thirteen output
lines, live registry/refcount assertions and an empty registry before shutdown.
The shared harness adds an opt-in pre-shutdown assertion; earlier assertions
remain unchanged. The allocator itself is unchanged.

Updated ownership, feature, user memory, source-map, Go and handoff/backlog docs.
The schedule already uses local execution and the approved cwd; no configuration,
website, hosting, compiler typing or plugin changes were made.

## Validation

- Clean approved checkout/attached branch/origin; one registered worktree.
  History/all worktree records showed no unpublished pass. The preceding manual
  compiler chat was idle; no other active project writer was shown. The first
  app lookup stalled and was canceled; a subsequent lookup succeeded.
- Restricted fetch failed DNS; approved `git fetch origin
  codex/compiler-stabilization` succeeded. `git ls-remote` matched parent.
  P0030 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43665107774)
  was initially running, then **both Linux and Windows jobs passed** during this run.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted Zig could not locate its installation; approved installed SDK/cache
  access was used without toolchain/cache or permission changes.
- Baseline `zig build test`: **155 discovered, 137 passed, 18 skipped, zero
  failures/errors**, 62.50s; `test_results_20261114_185801_windows.json`.
- Initial harness compilation failed because the wrapper named a nonexistent
  `dr_shutdown`; corrected to the real `dr_release_all`. On the unchanged compiler,
  `go test -count=1 -v -run '^TestCompiler_LoopExitClasses$' .`: **both modes
  failed the empty pre-shutdown registry assertion**, 20.862s (group 20.13s).
  Jump emission assertions were added after this runtime reproduction.
- `zig build`: **passed**. First post-fix focused run passed all lifetime/release
  assertions but failed exact output order. Source audit corrected the fixture:
  the default case prints `3` before its enclosing body prints `1` and `header`.
  No compiler behavior or lifetime assertions were weakened to match output.
- Final `go test -count=1 -v -run
  '^TestCompiler_(LoopExitClasses|LoopExitStrings)$' .`: **both groups/all four
  modes passed**, 19.529s; class group 3.58s. Includes generated C, native UBSan
  harness, pre-shutdown registry, exact output, Linux harness cross-linking and
  direct linked executable checks.
- Final `zig build test`: **156 discovered, 138 passed, 18 skipped, zero failures/
  errors**, 66.91s; `test_results_20261114_190311_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.061s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **24 groups passed**, 51.358s; class group 2.48s.
  Compiler-launching tests use `-count=1`; all prior ownership groups remain selected.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed**. Linux binaries were not executed locally; native P0031 runtime
  evidence awaits CI after publication.
- **123 local Markdown links resolved**; 24 quick/CI selectors match. PowerShell
  UTF-8 BOM, complete diff/scope review and `git diff --check` checked before commit.

Ignored logs: `build/P0031-{baseline,before,before-runtime,build,focused,focused-final,
full,quick,linux}.log`. The initial harness/output failures are diagnostic evidence,
not successful checks.

## Limits and handoff

Same eighteen skipped entries; no exclusions or weakened assertions. Qualified
instances are independent fresh allocations. Class aliases, explicit pointers,
arrays, destructors/field destruction, return ownership and exception/finally
unwinding remain unqualified; this matches existing normal class-local cleanup,
without establishing a general ownership model.

Publish to the approved branch, verify the matching remote SHA, then inspect native
CI. Next take T011 scalar string assignment-expression initializers, for example
`string copy = (source = "next")`. Reproduce source/copy alias cleanup with the
checked registry and qualify independent lifetime and once-only assignment effects
in both modes. `ND_BINOP`/`TK_EQ` remains outside the initializer-copy predicate.
Keep return transfer and class alias/container/destructor work separate. Stop after
this pass; completed pruning/site maintenance must not be repeated.
