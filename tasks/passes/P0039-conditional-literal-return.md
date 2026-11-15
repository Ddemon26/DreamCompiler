# P0039 - Clean locals before literal-only conditional string returns

- Date: 2026-11-15 UTC
- Task: T011 bounded conditional static string-result lifetime and cleanup
- Parent checkpoint: `ea001f013a7fff6b78b30b065d7012c8ca4d9d2c`
- Commit lookup: exact `Pass-ID: P0039` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Conditional string returns bypassed scoped cleanup even when all possible result
values were static literals. The new checked regression failed pre-shutdown
registry emptiness in both ordinary/module-entry modes before the fix.

`src/codegen/stmt.c` now recursively qualifies `ND_COND` results whose value leaves
are all `ND_STRING`. Both branches must qualify; any nonliteral leaf keeps the
entire return on its prior path. Synchronous, non-exception exits reuse the existing
collision-safe temporary to evaluate the result before reverse-order cleanup of
qualified string/scalar-class locals. Conditions can read those locals, headers and
fields or have effects: each reached condition executes once, with unselected
branches unevaluated. Static result storage survives cleanup without allocation,
copy, retain or consumption. Caller declarations still copy results. Binding
metadata and unbraced/other-path behavior remain intact. Without qualified locals,
the direct return remains. Async and active exception exits are unchanged.

The annotated `conditional_literal_return.dr` and
`TestCompiler_ConditionalLiteralReturn` require **55 nonnull releases** (18 classes,
37 strings), **34 exact output lines**, and an empty registry before and after
shutdown. Independent release total: four nested early paths 20; two nested
fall-through paths 12; early/normal loop paths 4/8; static methods 2; instance
method 2; normal exception completion 3; collision 1; caller backing/snapshot/
receiver 3. Null releases do not count.

All four nested literal leaves execute, including empty and escaped literals.
Labels/effect counts require selected-only, once-only condition evaluation before
local/header/field cleanup. Mixed/nested/null/const/empty locals, later declarations,
unbraced true/false paths, header updates, methods, temporary collisions, caller
survival and normal exception completion execute. Exact C checks require reverse
cleanup after result evaluation and unchanged emission for nonliteral leaves in
either recursive branch, owned-local/call leaves and active exception exits. These
unsupported controls are not executed. P0038's unused all-literal conditional now
checks P0039 emission; its runtime counts remain intact. Both quick protocols and
CI select the new group. No allocator/refcount/shutdown assertion was weakened.
Feature/function/storage docs, source map and handoff describe the same boundary.

## Validation

- Approved checkout/path/attached branch/origin verified clean; one registered
  worktree, no competing active project writer shown by available chats. Memory/
  history identify delivered P0038 with no unpublished prior pass. Automation
  view/TOML already use local execution and approved cwd; unchanged.
- Restricted fetch failed DNS; approved normal network fetch and `git ls-remote`
  matched the parent. P0038 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43698784933)
  passed Linux/Windows build, runner, quick and full-suite steps before work.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted baseline could not locate Zig's installation; approved normal
  toolchain/cache access succeeded without configuration/cache/permission changes.
- Baseline `zig build test`: **165 discovered, 147 passed, 18 skipped, zero
  failures/errors**, 45.65s; `test_results_20261115_060439_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_ConditionalLiteralReturn$' .`: **both modes failed** pre-shutdown
  emptiness, 2.450s (group 2.02s).
- `zig build`: **passed**, exit 0. Initial focused new regression: **both modes
  passed**, 2.730s (group 2.29s). Strengthened exact C new/P0038/P0037 run:
  **all three groups, both modes passed**, 6.967s (new group 2.26s).
- First full suite: **166 discovered, 147 passed, 1 failed, 18 skipped, zero
  errors**, 42.15s; `test_results_20261115_060804_windows.json`. The runner strips
  trailing whitespace, losing the fixture's final blank output line. Moved that
  checked blank line before the final backing line; all 34 output lines and exact
  Go assertions remain. No runner behavior or assertion was weakened.
- Final `zig build test`: **166 discovered, 148 passed, 18 skipped, zero failures/
  errors**, 42.01s; `test_results_20261115_060924_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.048s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 32 groups passed**, 52.655s; new group
  **1.94s**, both modes with final output ordering and exact C assertions.
  Native UBSan, live release operands/reference counts, pre/post-shutdown registry,
  Linux harness cross-linking and directly linked output are checked. Compiler
  tests use `-count=1`. Earlier ownership/return assertions remain intact.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-building/cross-linking is not native P0039 Linux
  runtime evidence; inspect CI after publication.
- **166 local Markdown paths resolved**, 32-group quick/CI selector equality,
  PowerShell BOM, gofmt, complete scope/diff review and `git diff --check` passed.
  Precommit fetch/remote SHA still matched the parent; no divergence or unrelated
  changes.

Ignored logs: `build/P0039-{baseline,before,build,focused,emission,full,full-final,quick,linux}.log`.
The before-fix failure and initial fixture mismatch remain evidence.

## Limits and handoff

Same eighteen skips and prior shape/operator limits. Conditional trees with any
nonliteral value leaf remain unqualified. General borrowed/compound result lifetime,
caller consumption, pointer/class results, aliases/containers/destructors/fields,
async and active exception/finally unwinding remain separate. P0037 returned owned
allocations still retain their original lifetime until shutdown. String-index return
typing, custom-type for headers and class null initializers remain limited.

Publish and verify the remote SHA, then inspect native CI; repair failures first.
Next reproduce conditional returns selecting known-owned locals and literals;
define preservation of the selected allocation and cleanup of unselected owners
before extending support. End after this one pass.
