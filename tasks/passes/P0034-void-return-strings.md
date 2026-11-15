# P0034 - Release owned scalar strings before bare void return

- Date: 2026-11-15 UTC (local reports November 14, America/Vancouver)
- Task: T011 bounded synchronous void-return cleanup slice
- Parent checkpoint: `376b74402d038c9d55aa2b790c1facdd785675a6`
- Commit lookup: exact `Pass-ID: P0034` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Bare `return;` bypassed block/header cleanup and retained owned scalar strings
until shutdown. Both modes of the new regression reproduced a nonempty live
allocation registry before shutdown. `src/codegen/stmt.c` now emits reverse-order
releases of active bindings marked `owns_string` before bare returns in
synchronous void free/static/instance functions. A compound statement keeps
cleanup conditional for unbraced returns. Emitter bindings remain intact for other
branches and existing fall-through cleanup; a taken return bypasses that cleanup.

`context.h` records synchronous-function eligibility and active exception depth.
Parameters, `this`, borrowed arrays/pointers and later declarations receive no
release. For headers are active locals and are released when a return leaves the
function. Value returns, async functions/methods, and returns inside active try,
catch or finally keep their previous emission; exception unwinding is separate.
Returns after a normally completed exception context use the new path.

The annotated `void_return_strings.dr` and `TestCompiler_VoidReturnStrings` cover
both ordinary/module-entry modes. Each requires 40 nonnull releases (39 strings,
one normally cleaned class instance), 13 exact output lines and an empty registry
before shutdown. Independent count: nested paths 7; while 3; do 2; for paths 10
(including two header replacements); switch paths 5; zero loop 2; recursion 3;
static/instance locals 3; completed exception context 3; caller backing/class 2.
Exact C sequences qualify nested/const/null/empty locals, unbraced true/false paths,
later declarations, while/do/for/switch returns, zero/multiple iterations, header
updates, recursion and methods. Borrowed caller backing remains readable.
Unused exception-return paths are checked only for unchanged emission and are not
executed or qualified. All existing allocator/refcount/shutdown assertions remain.
Both quick protocols and CI select the new group. Feature, function, storage and
source-map documentation describe this bounded behavior.

## Validation

- Approved checkout/path/attached branch/origin verified clean at start; one
  registered worktree, no competing active project writer shown. Memory/history
  showed delivered P0033, no unpublished pass. Automation view/TOML already uses
  local execution and the approved cwd; no schedule configuration change needed.
- Restricted fetch failed DNS; normal approved network fetch and `git ls-remote`
  succeeded and matched the parent. P0033
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43677590309)
  passed Linux and Windows build/runner/quick/full steps before implementation.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted baseline could not locate the Zig installation; normal approved
  toolchain access succeeded. No toolchain/cache/permission configuration changes.
- Baseline `zig build test`: **160 discovered, 142 passed, 18 skipped, zero
  failures/errors**, 54.54s; `test_results_20261114_230800_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_VoidReturnStrings$' .`: **both modes failed** empty-registry-
  before-shutdown, 3.097s (group 2.58s). No passing lifetime claim for that run.
- `zig build`: **passed**, exit 0. Initial focused group: **both modes passed**,
  3.554s (group 3.01s), with exact output/emission, native checked UBSan harness,
  Linux harness cross-linking, and direct compiler-linked output.
- Final `zig build test` after explicit synchronous-function gating:
  **161 discovered, 143 passed, 18 skipped, zero failures/errors**, 49.14s;
  `test_results/test_results_20261114_231035_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.043s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 27 groups passed**, 53.977s;
  void-return group **2.38s**, both modes. External compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-link success is not native P0034
  Linux runtime evidence; native CI follows publication.
- **134 local Markdown paths resolved**, 27-group quick/CI selector equality, PowerShell UTF-8 BOM,
  gofmt, complete scope/diff review and `git diff --check` checked before commit.
  Precommit fetch and remote SHA still matched the parent; no divergence.

Ignored logs: `build/P0034-{baseline,before,build,focused,full,quick,linux}.log`.
The before-fix failure is retained as reproduction evidence.

## Limits and handoff

Same eighteen skips and previous shape/operator limits. Class-local return
cleanup, aliases/containers/destructors/fields, value-return transfer, async and
exception/finally unwinding remain unqualified. Fresh managed temporaries/results
can remain registered until shutdown; this pass adds no escape/lifetime analysis,
result transfer, allocator changes or dangling-source repair. Active exception-
return paths keep their existing limitations and are not executed by this fixture.

Publish to the approved branch and verify the remote SHA, then inspect native CI
and repair failures first. Next reproduce fresh independent class locals retained
by bare `return;` in synchronous void functions/methods. Qualify bounded class-local
cleanup with checked pre-shutdown allocations, preserving borrowed `this`, owned
string locals and nested/header scopes. Keep class aliases/containers/destructors,
field ownership, value-return transfer and exception unwinding separate. Stop
after this one pass; completed maintenance tasks must not be repeated.
