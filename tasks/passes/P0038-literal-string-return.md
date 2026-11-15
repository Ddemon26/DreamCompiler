# P0038 - Clean locals before direct literal string returns

- Date: 2026-11-15 UTC
- Task: T011 bounded direct literal string-result lifetime and cleanup
- Parent checkpoint: `9803ea8626d180e24b17a6a65d638737a5c95cd6`
- Commit lookup: exact `Pass-ID: P0038` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Direct string-literal returns bypassed local cleanup. The new checked regression
failed the empty-registry-before-shutdown assertion in both ordinary/module-entry
modes. Literal C storage has static lifetime and cannot be invalidated by releasing
locals, so this bounded result form needs no owning-return ABI or allocation.

`src/codegen/stmt.c` now qualifies synchronous string results whose AST is directly
`ND_STRING`, outside active exception contexts. Parentheses are transparent; empty
and escaped literals qualify. With qualified locals, the existing collision-safe
result temporary captures the static pointer before reverse-order string/scalar-
class cleanup. Without qualified locals the direct return remains. No result copy,
allocation, retain or consumption is introduced. Caller declarations still copy.
Emitter metadata is unchanged, preserving normal/other branches and conditional
unbraced returns. Parameters/receivers and containers retain their prior lifetime.

The annotated `literal_string_return.dr` and `TestCompiler_LiteralStringReturn`
require 51 nonnull releases (18 classes, 33 strings), 19 exact output lines including
an empty line, and an empty registry before and after shutdown. Independent release
total: nested true/false 11; for true/false 12; switch true/default 4; while 3; do 2;
static 1; instance 2; recursion 6; normal exception completion 3; zero loop 2;
collision 1; escaped result 1; caller backing/snapshot/receiver 3.

Mixed/nested/null/const/empty locals, later declarations, unbraced true/false paths,
while/do/for/switch, headers/updates, zero/multiple iterations, recursion, methods,
name collisions, caller survival and normal exception completion execute. Exact C
assertions require reverse cleanup order, static results without copying, and
unchanged borrowed/compound/active-exception emission. The latter controls are not
executed or qualified. Both quick protocols and CI select the new group. P0037's
unused literal control now checks the new emission; its runtime counts/retained-
result assertions are unchanged. No allocator or harness assertions were weakened.

## Validation

- Approved checkout/path/attached branch/origin verified clean; one registered
  worktree, no competing active project writer shown by available chats. Previous
  memory/history identify delivered P0037; no unpublished prior pass. Automation
  view/TOML already use local execution and approved cwd; no schedule change.
- Restricted fetch failed DNS. Approved normal network fetch/`git ls-remote`
  matched the parent. P0037 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43695861891)
  passed Linux/Windows build, runner, quick and full-suite steps before work.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted baseline could not locate Zig's installation. Approved normal
  toolchain/cache access succeeded without configuration/cache/permission changes.
- Baseline `zig build test`: **164 discovered, 146 passed, 18 skipped, zero
  failures/errors**, 49.46s; `test_results_20261115_021139_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_LiteralStringReturn$' .`: **both modes failed** pre-shutdown
  emptiness, 2.479s (group 2.00s).
- `zig build`: **passed**, exit 0. Initial focused new and P0037 groups: **both
  modes passed**, 5.930s combined (new group 2.75s, prior group 2.65s).
  Strengthened C assertions initially failed because debug directives/comments
  intervene between `if` and its return block. The match now starts at the block;
  true/false runtime assertions remain. Exact-C focused rerun: **both passed**,
  3.087s (group 2.61s). Native UBSan/live registry/refcounts/shutdown, Linux harness
  cross-linking and direct compiler-linked output are checked in each mode.
- `zig build test`: **165 discovered, 147 passed, 18 skipped, zero failures/
  errors**, 52.12s; `test_results_20261115_021549_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.035s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 31 groups passed**, 59.603s; new group
  **2.12s**, both modes including final exact C assertions. P0037 and prior
  primitive/void/ownership groups passed. Compiler invocations use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-building/cross-linking is not native P0038 Linux
  runtime evidence; inspect CI after publication.
- **161 local Markdown paths resolved**, 31-group quick/CI selector equality,
  PowerShell UTF-8 BOM, gofmt, complete scope/diff review and `git diff --check`
  checked before commit. Precommit fetch and remote SHA matched the parent;
  no divergence or unrelated changes.

Ignored logs: `build/P0038-{baseline,before,build,focused,emission,full,quick,linux}.log`.
The before-fix failure remains reproduction evidence.

## Limits and handoff

Same eighteen skips and existing shape/operator limits. Only direct literal results
are newly qualified. Even a conditional choosing two literals retains prior emission.
Borrowed/compound result lifetime and general caller consumption remain open. P0037
owned-local results still retain their original allocation/reference until shutdown.
String-index return typing remains limited; custom-type for headers and class null
initializers remain unsupported. Pointer/class results, aliases/containers,
destructors/fields, async and active exception/finally unwinding remain separate.

Publish and verify the remote SHA, then inspect native CI; repair failures first.
Next reproduce retained locals on conditional string returns with all-literal
value branches, then qualify static-result lifetime and once-only condition
evaluation before extending cleanup. End after this one pass.
