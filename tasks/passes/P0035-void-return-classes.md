# P0035 - Release scalar class locals before bare void return

- Date: 2026-11-15 UTC
- Task: T011 bounded synchronous void-return class cleanup slice
- Parent checkpoint: `d85bfb4b48fd5d560b0ccacbdffbd80f3dcdde26`
- Commit lookup: exact `Pass-ID: P0035` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Fresh independent class locals were retained until shutdown when a bare return
bypassed block cleanup. Both ordinary/module-entry modes reproduced a nonempty
live registry before shutdown. `src/codegen/stmt.c` now includes scalar class
locals in P0034's reverse-order synchronous void-return cleanup, interleaving
class and owned-string releases across active scopes.

The class predicate requires a scope deeper than the parameter scope and zero
array/pointer indirections. Borrowed parameters and implicit `this` survive.
Existing normal cleanup and emitter bindings stay intact for other paths;
unbraced returns keep releases conditional. Value returns, async and returns
inside active exception contexts retain their emission. This qualifies fresh
independent instances, without adding class alias retention or field destruction.

The annotated `void_return_classes.dr` and `TestCompiler_VoidReturnClasses`
require 40 nonnull releases (30 class instances and ten strings), ten output
lines, exact reverse-order C sequences and an empty registry before shutdown.
Independent total: nested true/false 7; while 3; do 2; for true/false 11; switch
true/false 4; zero loop 2; recursion 3; static method 1; instance method 2;
completed exception 3; caller receiver/backing 2. Nested mixed scopes, later
declarations, true/false unbraced returns, loop/switch scopes, string headers,
zero/multiple iterations, recursion, static/instance methods, a constructor's
bare return and caller receiver survival are covered. Unused active exception
returns have emission checks only; return after a normally completed context runs.
All prior allocator/refcount/shutdown assertions remain unchanged. Both quick
protocols and CI select the new group. Feature, function, storage and source-map
docs describe this boundary.

## Validation

- Approved checkout/path/attached branch/origin verified clean at start, one
  registered worktree, no competing active project writer shown. Prior memory,
  history and worktree records showed delivered P0034, no unpublished agent pass.
  Schedule view/TOML already uses local execution and the approved cwd.
- Restricted fetch failed DNS; approved normal network fetch and `git ls-remote`
  succeeded, matching the parent. P0034
  [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43684568487)
  passed Linux/Windows build, runner, quick and full-suite steps.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted baseline could not locate Zig's installation; approved installed
  toolchain/cache access succeeded. No toolchain/cache/permission changes.
- Baseline `zig build test`: **161 discovered, 143 passed, 18 skipped, zero
  failures/errors**, 56.08s; `test_results_20261115_000721_windows.json`.
- Fixture preparation exposed unsupported custom-type for-header declarations
  and direct class null initialization; these failed before runtime. The fixture
  uses supported string headers/fresh class instances. Neither restriction was
  weakened or claimed implemented.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_VoidReturnClasses$' .`: **both modes failed** the empty-registry-
  before-shutdown assertion, 2.855s (group 2.30s).
- `zig build`: **passed**, exit 0. Initial focused regression **passed both
  modes**, 3.466s (group 2.91s), checking native UBSan/live registry/refcounts/
  shutdown, Linux harness cross-linking and direct compiler-linked exact output.
- `zig build test`: **162 discovered, 144 passed, 18 skipped, zero failures/
  errors**, 48.56s; `test_results/test_results_20261115_001002_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.038s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 28 groups passed**, 59.076s; new group
  **2.67s**, both modes, including final exact C sequence assertions.
  External compiler invocations use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-link success is not native P0035
  Linux execution evidence; inspect native CI after publication.
- **139 local Markdown paths resolved**, 28-group quick/CI selector equality,
  PowerShell UTF-8 BOM, gofmt,
  full scope/diff review and `git diff --check` verified before commit.
  Precommit fetch/remote SHA still matched the parent; no divergence.

Ignored logs: `build/P0035-{baseline,before,build,focused,full,quick,linux}.log`.
The before-fix assertion failure remains reproduction evidence.

## Limits and handoff

Same eighteen skips and previous shape/operator limits. Class aliases/containers,
destructors/fields, value-return transfer, async and active exception/finally
unwinding remain unqualified. Class null initialization and custom-type for
headers are unsupported. No allocator changes, automatic field destruction,
escape/lifetime analysis, dangling-source repair or temporary/result transfer.

Publish to the approved branch and verify the remote SHA, then inspect native CI
and repair failures first. Next reproduce scoped allocations retained on primitive
numeric/bool/char value returns. Qualify one bounded scalar-result temporary that
evaluates once before cleanup, preserving field reads and side effects. Keep
string/pointer/class result transfer and exception exits separate. End this
invocation after this one pass; completed maintenance must not repeat.
