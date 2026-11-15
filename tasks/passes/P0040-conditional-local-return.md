# P0040 - Preserve selected owned-local conditional string results

- Date: 2026-11-15 UTC
- Task: T011 bounded conditional result lifetime and scoped cleanup
- Parent checkpoint: `d51ee248c642f44c96ae8b340af7bd89afd9b3f2`
- Commit lookup: exact `Pass-ID: P0040` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Conditional string returns involving owned locals retained unrelated scoped
allocations. The initial checked regression failed before shutdown in both modes:
57 live allocations remained instead of the 12 selected nonnull results.

`src/codegen/stmt.c` now recursively qualifies conditional results whose value
leaves are literals or known-owned scalar string locals under the existing direct
local predicate. Both branches must qualify, even under constant conditions. Any
unsupported leaf preserves the entire return's prior emission. All-literal trees
keep P0039 emission; direct returns keep P0037/P0038 behavior. Async and active
exception exits stay separate.

The collision-safe temporary captures the result once before reverse-order
cleanup. Each candidate local that occurs as a value leaf is released only if its
current pointer differs from the result; other qualified string/class locals are
released normally. Independent owned slots have distinct live allocations even
when contents match. Conditions may replace a candidate before selection, so
comparisons use current slots without rerunning conditions. Repeated leaves do not
duplicate releases; a selected null omits only no-op null releases. Binding metadata
and unbraced/other-path cleanup remain intact. No result allocation, copy, retain or
consumption is added. Selected owned results retain their original reference until
shutdown; caller declarations still copy them.

`conditional_local_return.dr` and `TestCompiler_ConditionalLocalReturn` check
**64 nonnull releases** (18 classes, 46 strings), **36 output lines**, exactly
**14 selected allocations** with one reference before shutdown and an empty registry
afterward, in ordinary and module-entry modes. Independent release count: nested
early/normal paths 34; loop early/normal paths 11; static methods 3; instance method
1; null/empty paths 3; collision 1; completed exception path 3; repeated leaf 1;
condition replacement 2; equal-content slots 1; caller backing replacement/final
backing/snapshot/receiver 4. Null releases do not count.

All four nested leaves, selected-only/once-only effects, body/header storage,
header append updates, unbraced true/false paths, later declarations, null/const/
empty candidates, repeated leaves, equal-content independent slots, append in a
condition, static/instance methods, name collisions, caller snapshot/backing
survival and normal exception completion execute. Exact C checks require result
evaluation before reverse cleanup and unchanged unsupported borrowed/call/active-
exception emission. P0037/P0039 unused owned-conditional controls now check this
emission; their runtime counts and earlier harness assertions remain unchanged.
Both quick protocols and CI select the new group. Feature/function/storage docs,
source map, Go README and handoff describe the same boundary.

## Validation

- Clean approved checkout/path/attached branch/origin matched published P0039.
  One registered worktree; available chats showed no competing active writer.
  Memory/history identified no unpublished or unfinished pass. Automation view/TOML
  already use local execution and approved cwd; unchanged.
- Restricted fetch failed DNS; approved normal network fetch and `git ls-remote`
  matched the parent. P0039 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43717895195)
  passed Linux/Windows build, runner, quick and full-suite steps before work.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted baseline could not locate Zig's installation; approved normal access
  succeeded without toolchain/cache/permission changes.
- Baseline `zig build test`: **166 discovered, 148 passed, 18 skipped, zero failures/
  errors**, 51.48s; `test_results_20261115_105815_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_ConditionalLocalReturn$' .`: **both modes failed**, 2.777s
  (group 2.30s), at live count 57 versus 12.
- `zig build`: **passed**, exit 0. First after-fix regression passed the live-count
  check but found 61 releases versus 60 expected (2.292s). The independent hand
  count omitted caller backing replacement. Corrected that count and added the
  condition-replacement/equal-content cases (final 64 releases, 14 live results).
- Focused new/P0039/P0037 groups: **all passed both modes**, 8.498s; new group
  2.79s. Final exact C assertions subsequently passed in quick validation.
- `zig build test`: **167 discovered, 149 passed, 18 skipped, zero failures/errors**,
  49.51s; `test_results_20261115_110144_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.047s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 33 groups passed**, 65.331s; new group
  **2.23s**, both modes with final exact C assertions. Native UBSan, checked live
  release operands/reference counts, pre/post-shutdown registry, Linux harness
  cross-linking and directly linked output pass. Compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-linking does not prove native P0040
  Linux runtime behavior; inspect CI after publication.
- 171 local Markdown paths resolved, 33-group quick/CI selector equality, PowerShell UTF-8 BOM,
  gofmt, complete scope/diff review and `git diff --check` passed. Precommit fetch
  and remote SHA still matched the parent; no divergence or unrelated changes.

Ignored logs: `build/P0040-{baseline,before,build,focused,focused-final,full,quick,linux}.log`.
The initial failure and corrected release-count mismatch remain evidence.

## Limits and handoff

Same eighteen skips and prior shape/operator limits. Selected owned results still
accumulate until shutdown. General caller consumption and borrowed/member/index/
call/assignment/concatenation result lifetimes remain separate, as do pointer/class
results, aliases/containers/destructors/fields, async and active exception/finally
unwinding. String-index return typing, custom-type for headers and class null
initializers remain limited.

Publish and verify the remote SHA, then inspect native CI; repair failures first.
Next reproduce retained independent locals on direct borrowed scalar string-
parameter returns and audit caller backing lifetime before qualifying cleanup
without a new result copy/retain. End after this one pass.
