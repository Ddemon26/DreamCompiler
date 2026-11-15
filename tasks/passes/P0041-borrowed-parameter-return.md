# P0041 - Clean locals before unchanged borrowed parameter returns

- Date: 2026-11-15 UTC
- Task: T011 bounded borrowed string-result lifetime and scoped cleanup
- Parent checkpoint: `df5ddb965d2df77a5a0fe9c84b1191e77171973d`
- Commit lookup: exact `Pass-ID: P0041` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Direct borrowed scalar string-parameter returns retained unrelated scoped
allocations. Both ordinary/module-entry checked regressions failed the empty-
registry-before-shutdown assertion before the fix. A parameter can be rebound
to an owned local, so merely resolving an identifier to a parameter does not
prove its result independent of cleanup.

`src/codegen/stmt.c` now qualifies a direct scalar string-parameter result only
when a whole-body scan establishes that parameter is unchanged. `CGCtx` retains
the enclosing body for this proof. Later statements, loop conditions/updates,
nested expressions and exception bodies are scanned. Assignments and increment/
decrement block the proof; same-name declarations/catch bindings and unknown/
nested declarations conservatively disqualify. Writes to other parameters do
not disqualify the result. There is no parsed address-of expression exposing the
scalar parameter slot; by-value calls cannot rebind it. This is not alias analysis.

The existing collision-safe temporary captures the caller-backed pointer once
before reverse-order cleanup of qualified owned scalar strings/classes. No result
allocation, copy, retain or consumption is introduced. Caller declarations still
copy; the borrowed pointer retains its original caller-provided lifetime. With
no cleanup, emission remains direct. Rebound parameters, conditional and other
borrowed result forms, async and active exception exits retain previous emission.

`borrowed_parameter_return.dr` and `TestCompiler_BorrowedParameterReturn` check
**58 nonnull releases** (20 classes, 38 strings), **17 output lines**, and an empty
registry before/after shutdown in both modes. Independent release count: five
nested calls 25; loop early/normal paths 12; switch 3; static/instance methods 3;
three recursion levels 6; completed exception 3; collision 1; other-parameter
write 1; caller backing replacement/final backing/snapshot/receiver 4. Null
releases do not count. Literal, empty, typed-null and managed caller storage,
nested/header/switch scopes, unbraced true/false paths, later declarations,
const/null/empty locals, header append updates, recursion, methods, temporary
collisions, normal exception completion and caller snapshot/backing survival
execute. Exact C checks require capture before reverse cleanup and preserve
rebound/conditional/active-exception returns. **24 additional cases** check twelve
placements of rebinding in both modes, including later statements and updates.
P0037/P0038 unused borrowed controls now require P0041 emission with unchanged
runtime assertions. Both quick protocols and CI select the new group. Feature,
function, storage, source-map and Go docs describe the same boundary.

## Validation

- Clean approved checkout/path/attached branch/origin matched published P0040.
  One registered worktree and available chats showed no competing active writer.
  Memory/history identified no unfinished delivery. Schedule view/TOML already
  use local execution and the approved cwd; unchanged.
- Restricted fetch failed DNS; approved normal fetch and `git ls-remote` matched
  the parent. P0040 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43744953713)
  passed Linux/Windows build, runner, quick and full-suite steps before work.
- Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**. Restricted
  baseline could not locate Zig's installation; approved normal access succeeded
  without toolchain/cache/permission changes.
- Baseline `zig build test`: **167 discovered, 149 passed, 18 skipped, zero failures/
  errors**, 46.03s; `test_results_20261115_120617_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_BorrowedParameterReturn$' .`: **both modes failed** at the empty-
  pre-shutdown registry assertion, 2.505s (group 2.02s).
- `zig build`: **passed**, exit 0. Initial focused check: **both modes passed**,
  2.923s (group 2.47s), with 58 checked releases and an empty registry.
- Final focused new/P0037/P0038 groups: **all passed both modes**, 8.451s; new
  group **3.33s**, including final exact C and 24 rebound-parameter cases.
- `zig build test`: **168 discovered, 150 passed, 18 skipped, zero failures/errors**,
  44.16s; `test_results_20261115_121123_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.035s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 34 groups passed**, 70.423s; new group **2.76s**.
  Native UBSan, live release operands/original refcounts, pre/post-shutdown registry,
  Linux harness cross-linking and linked output pass. Compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-linking is not native Linux execution.
- 176 local Markdown paths, 34-group quick/CI selector equality, PowerShell UTF-8
  BOM, gofmt, full scope/diff review and `git diff --check` passed. Precommit fetch
  and remote SHA still matched the parent; no divergence or unrelated changes.

Ignored logs: `build/P0041-{baseline,before,build,focused,focused-final,full,quick,linux}.log`.
The reproduced failure remains evidence; no assertion or exclusion was weakened.

## Limits and handoff

Same eighteen skips and prior shape/operator limits. Only unchanged direct scalar
string parameters gain cleanup; rebound parameters and arbitrary aliases remain
unqualified. Conditional/member/index/call/assignment/concatenation result lifetime,
retained owned-result accumulation and caller consumption, pointer/class results,
aliases/containers/destructors/fields and async/active exception unwinding stay
separate. String-index return typing, custom-type for headers and class null
initializers remain limited.

Publish and verify the remote SHA, then inspect native CI; repair failures first.
Next qualify conditional result trees with literal/known-owned-local/unchanged
borrowed scalar parameter leaves, preserving selected owned storage, once-only
selection, selected-only effects and caller backing survival in both modes.
Rebound/other unsupported leaves must preserve the entire previous return.
End after this one pass.
