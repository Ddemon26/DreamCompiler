# P0036 - Release scoped locals on primitive scalar return

- Date: 2026-11-15 UTC
- Task: T011 bounded synchronous primitive scalar-return cleanup
- Parent checkpoint: `1dce40c55e4b9166fa73c28b9435c9b1d49997cd`
- Commit lookup: exact `Pass-ID: P0036` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Primitive value returns bypassed scoped string/class cleanup, retaining allocations
until shutdown. Both ordinary/module-entry modes reproduced a nonempty live registry
before shutdown. `src/codegen/stmt.c` now evaluates synchronous `int`, `float`, `bool`
and `char` results once into a temporary of the declared C return type before
reverse-order qualified owned-string/scalar-class cleanup. The saved result preserves
field/index reads and assignment/increment effects before storage can be freed.

The return predicate is shared with P0034/P0035 bare void cleanup. Parameters and
implicit `this`, borrowed arrays/pointers and later declarations are excluded;
emitter bindings survive for other paths, and a compound statement keeps unbraced
returns conditional. Only returns with qualified locals need a temporary. Name
selection checks active bindings and a preflight rendering of the lowered expression,
including module function names, so the C initializer cannot shadow its inputs.
Preflight renders C only; the resulting program evaluates the expression once.
String/pointer/class result transfer, async and active exception exits stay separate.

`scalar_return_cleanup.dr` and `TestCompiler_ScalarReturnCleanup` require 47 nonnull
releases (21 class instances, 26 strings), 23 exact output lines, exact typed-result/
reverse-cleanup sequences and an empty registry before shutdown in each mode.
Independent release total: nested true/false 9; while 3; do 2; for true/false 8;
switch true/false 4; zero loop 2; recursion 3; static 1; instance 2; character
calls 2; boolean calls 4; collision-local 1; collision-call 1; completed exception 3;
caller backing/receiver 2. Mixed/nested/null/const/empty locals, later declarations,
unbraced paths, loops/switches, headers, recursion, four result types, free/static/
instance functions, caller survival and temporary-name collisions are covered.
Unused active exception exits and an owning string result have unchanged-emission
checks only. Existing allocator/refcount/shutdown assertions are intact. Both quick
protocols and CI select the new group; feature/function/storage/tooling docs match.

## Validation

- Approved checkout/path/attached branch/origin verified clean; one registered
  worktree, no competing active project writer shown. Memory, history and worktree
  records show delivered P0035, with no unpublished previous pass. Schedule view
  and TOML already use local execution and the approved project cwd; no change needed.
- Restricted fetch failed DNS. Approved normal network fetch and `git ls-remote`
  matched the parent. P0035 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43689021933)
  passed Linux and Windows build, runner, quick and full-suite steps.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted baseline could not locate Zig's installation; approved normal
  installed-toolchain/cache access succeeded. No toolchain/cache/permission changes.
- Baseline `zig build test`: **162 discovered, 144 passed, 18 skipped, zero
  failures/errors**, 51.34s; `test_results_20261115_011055_windows.json`.
- Initial fixture probes were rejected by existing identifier conflict and string-
  index return typing rules. Final fixtures use distinct names and a supported
  char array; no semantic/grammar restrictions were weakened or claimed implemented.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_ScalarReturnCleanup$' .`: **both modes failed** the empty-registry-
  before-shutdown assertion, 2.796s (group 2.31s).
- `zig build`: **passed**, exit 0. Initial focused matrix **passed both modes**,
  3.349s (group 2.86s), checking native UBSan/live registry/refcounts/shutdown,
  pre-shutdown emptiness, Linux harness cross-linking and direct linked output.
- `zig build test`: **163 discovered, 145 passed, 18 skipped, zero failures/
  errors**, 48.85s; `test_results/test_results_20261115_011545_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.046s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 29 groups passed**, 55.924s; new group
  **2.58s**, both modes including final exact C assertions and string-result
  emission controls. External compiler invocations use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-link success is not native P0036
  Linux execution evidence; inspect CI after publication.
- **150 local Markdown paths resolved**, 29-group quick/CI selector equality,
  PowerShell UTF-8 BOM, gofmt, full scope/diff review and `git diff --check`
  passed before commit. Precommit fetch and remote SHA still matched the parent;
  no divergence. Publication requires a matching remote SHA.

Ignored logs: `build/P0036-{baseline,before,build,focused,full,quick,linux}.log`.
The before-fix assertion failure remains reproduction evidence.

## Limits and handoff

Same eighteen skips and prior shape/operator boundaries. String-index return typing
remains limited despite backend console support. Primitive shape checking, owning
string/pointer/class results, aliases/containers/destructors/fields, async and active
exception/finally unwinding remain unqualified. No allocator changes, escape/lifetime
analysis, dangling-result repair or result/temporary ownership transfer.

Publish this pass to the approved branch and verify its remote SHA; inspect native
CI and fix failures first. Next reproduce string-valued returns retaining owned
locals; design one bounded result-lifetime contract before adding cleanup,
preserving borrowed and local result sources. Keep exception unwinding separate.
End this invocation after this one pass; do not repeat completed maintenance.
