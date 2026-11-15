# P0037 - Clean other locals around owned string returns

- Date: 2026-11-15 UTC
- Task: T011 bounded direct owned-local string-result lifetime and cleanup
- Parent checkpoint: `43772c74fe258d24d1389e6d8e50857dc0202297`
- Commit lookup: exact `Pass-ID: P0037` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Direct string-local returns bypassed all local cleanup. The checked regression
reproduced 44 live allocations before shutdown in both ordinary/module-entry modes,
where only 17 nonnull returned allocations should remain. The bounded contract in
`docs/development/string-ownership.md` preserves the returned known-owned scalar
local's allocation and original reference, while reclaiming the other qualified
string/scalar-class locals in reverse declaration order.

`src/codegen/stmt.c` qualifies synchronous string results whose AST is a direct
identifier of a known-owned scalar body/header binding. Parentheses are transparent.
The existing result-temporary path saves the pointer once, skips exactly that
binding during cleanup and returns it. No binding metadata is changed, so other
branches and normal completion retain cleanup. Unbraced returns remain conditional,
and generated names still avoid active bindings/lowered names. No result copy,
retain, allocator change or owning-return ABI is introduced. Caller declarations
still copy results without consuming the original, which remains live until shutdown.

The annotated `local_string_return.dr` and `TestCompiler_LocalStringReturn` require
45 nonnull releases (20 classes, 25 strings), 21 output lines, exact C sequences and
exactly 17 live returned allocations with one reference each before shutdown, then
an empty registry afterward. Independent release total: nested true/false 9;
for true/false 10; switch cases 2; null 2; empty 1; append 2; static 1; instance 1;
recursion 3; name collision 1; completed exception 3; zero loop 1; while 4; do 1;
caller backing/replacement/snapshot/receiver 4. Returned allocation total: nested 2;
for 2; switch 2; empty/append/static/instance 4; recursion 2; collision/completed/
zero/while/do 5. Null produces no allocation.

Nested/const/null/empty locals, later declarations, unbraced true/false paths,
while/do/for/switch, headers, zero/multiple iterations, append/replacement, recursion,
methods, name collisions, caller snapshot/backing/receiver survival and normal
exception completion are exercised. Other result forms and active exception exits
have emission-only controls and are never executed in this fixture. The harness's
existing release/refcount/empty-before-shutdown assertions are preserved; an opt-in
exact live-count assertion qualifies the retained result lifetime. Both quick
protocols and CI select the group. P0036's unused owned-string control now requires
P0037 preservation emission; its primitive/exception behavior and counts are unchanged.

## Validation

- Approved checkout/path/attached branch/origin verified clean. One registered
  worktree; available chats showed no competing active project writer. Previous
  memory/history identify delivered P0036, with no unpublished pass. Automation
  view/TOML already use local execution and the approved cwd; no schedule change.
- Restricted fetch failed DNS; approved normal network fetch and `git ls-remote`
  matched the parent. P0036 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43694034403)
  passed Linux/Windows build, runner, quick and full-suite steps before work.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted baseline could not locate Zig's installation. Approved normal installed-
  toolchain/cache access succeeded without toolchain/cache/permission changes.
- Baseline `zig build test`: **163 discovered, 145 passed, 18 skipped, zero
  failures/errors**, 51.23s; `test_results_20261115_013451_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_LocalStringReturn$' .`: **both modes failed**, 3.033s (group 2.44s),
  with **44 live allocations versus 17 expected** before shutdown.
- An initial development build failed because the return-case scope was placed in
  debug dispatch; it was corrected in source before validation. Final `zig build`:
  **passed**, exit 0. Focused test: **both modes passed**, 3.025s (group 2.54s),
  with native UBSan/live registry/refcounts/shutdown, exact output, Linux harness
  cross-linking and direct compiler-linked output.
- `zig build test`: **164 discovered, 146 passed, 18 skipped, zero failures/
  errors**, 50.76s; `test_results/test_results_20261115_013843_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.033s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 30 groups passed**, 55.520s; new group
  **2.18s**, both modes including final exact C assertions. Prior primitive-result,
  void-return and ownership groups passed. Compiler invocations use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-link success is not native P0037
  Linux runtime evidence; inspect CI after publication.
- **156 local Markdown paths resolved**, 30-group quick/CI selector equality, PowerShell UTF-8 BOM,
  gofmt, complete scope/diff review and `git diff --check` checked before commit.
  Precommit fetch and remote SHA matched the parent; no divergence.

Ignored logs: `build/P0037-{baseline,before,build,focused,full,quick,linux}.log`.
The before-fix assertion failure remains reproduction evidence.

## Limits and handoff

Same eighteen skips and existing shape/operator limits. This is direct owned-local
result preservation, not automatic result consumption: returned allocations can
accumulate until shutdown, including per-level recursive results. Other string
return expressions retain prior emission; no borrowed-source lifetime inference,
dangling-alias repair or universal snapshot allocation is added. Pointer/class
results, class aliases/containers/destructors/fields, async and active exception/
finally unwinding remain separate. Existing unsupported initializer/alias mutation
paths are not qualified by the owned-binding flag.

Publish this pass, verify its remote SHA and inspect native CI; repair failures
first. Next reproduce locals retained by direct literal string returns, then
qualify cleanup using their static-result lifetime. Keep borrowed/compound string
results and general caller consumption separate. End after this one pass.
