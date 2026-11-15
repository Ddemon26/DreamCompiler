# P0042 - Clean locals on conditional borrowed string returns

- Date: 2026-11-15 UTC
- Task: T011 bounded conditional borrowed string-result lifetime and scoped cleanup
- Parent checkpoint: `b57c2b2406f6c4621f8b30cb4b902eadc70d207d`
- Commit lookup: exact `Pass-ID: P0042` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Conditional returns containing borrowed scalar string parameters retained scoped
allocations. The new checked regression reproduced **62 live allocations instead
of five** before shutdown in both ordinary/module-entry modes. A parameter may
be rebound to local storage, so a parameter-shaped leaf alone is not sufficient.

`src/codegen/stmt.c` now recursively qualifies all value leaves as literals,
known-owned scalar locals or borrowed scalar parameters satisfying P0041's
whole-body unchanged proof. Conditions, later statements and loop updates are
included in the proof; every borrowed value parameter must pass, even under
constant conditions. One unsupported/rebound leaf retains the entire prior
return. Writes to a parameter unused as a value leaf do not disqualify the result.

Capture the selected pointer once before reverse-order qualified string/class
cleanup. Only owned-local value candidates receive P0040's pointer comparison;
borrowed parameters are neither released nor retained. Caller backing retains
its original lifetime, caller declarations still copy, and selected owned results
keep their original reference until shutdown. No ownership ABI change or result
allocation/retain/consumption is introduced. Async/active exception exits and
other unsupported value leaves preserve previous emission.

`conditional_borrowed_return.dr` and `TestCompiler_ConditionalBorrowedReturn`
check **75 nonnull releases** (19 classes, 56 strings), **39 exact output lines**,
**five retained owned results** with one reference before shutdown and an empty
registry afterward. Independent release count: six nested calls 36; three loop
paths 12; static calls 2; instance calls 3; null/empty calls 7; collision 1;
completed exception 3; other-parameter write 1; repeated leaves 2; condition-side
owned append/replacement 3; caller backing replacement/final backing/second/
snapshot/receiver cleanup 5. The five results are two nested locals, a header,
an instance-local copy and an empty-string local; borrowed selections retain none.

All four nested value paths, two borrowed parameters, repeated/null/empty/const
leaves, later declarations, unbraced paths, header updates, methods, temporary
collisions, owned replacement in a condition, normal exception completion and
caller backing/snapshot survival execute. Exact C requires capture before reverse
cleanup and preserves call/assignment/concatenation, condition-rebound and active
exception returns. An unreachable constant branch with a second rebound parameter
also disqualifies. **48 additional cases** place rebinding twelve ways with the
borrowed leaf nested on either side in both modes. The shared helper preserves
P0041's 24 direct-return controls and exact two-return/no-temporary assertions.
P0039/P0040/P0041 unused conditional controls now require qualified emission with
unchanged runtime assertions. Both quick protocols and CI select the new group.
Feature, function, ownership, source-map and Go docs describe the boundary.

## Validation

- Clean approved checkout/path/attached branch/origin matched published P0041.
  One registered worktree and available chats showed no competing active writer;
  memory/history showed no unfinished delivery. Schedule view/TOML already use
  local execution and the approved cwd; unchanged.
- Restricted fetch failed DNS; approved normal fetch succeeded and the remote-
  tracking SHA matched the parent. P0041 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43751573004)
  passed Linux/Windows build, runner, quick and full-suite steps before work.
- Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**. Restricted
  baseline could not locate Zig's installation; approved normal access succeeded
  without changing toolchains, caches or permissions.
- Baseline `zig build test`: **168 discovered, 150 passed, 18 skipped, zero failures/
  errors**, 49.80s; `test_results_20261115_130630_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_ConditionalBorrowedReturn$' .`: **both modes failed**, 2.545s
  (group 2.11s), with 62 live allocations instead of five.
- `zig build`: **passed**, exit 0. Initial focused check: **both modes passed**,
  3.132s (group 2.64s), with 75 releases and five original-reference results.
- Strengthened focused checks initially failed only the concatenation fallback's
  exact C assertion (11.770s): the expected string omitted the emitter's existing
  parentheses around `dream_concat`. Corrected the exact expectation without
  changing emission or runtime assertions. Final new/P0041/P0040/P0039 groups:
  **all passed**, 12.613s; new group **3.72s**, including the 48 rebinding cases.
- `zig build test`: **169 discovered, 151 passed, 18 skipped, zero failures/errors**,
  75.83s; `test_results_20261115_131203_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.040s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 35 groups passed**, 80.416s; new group **4.73s**.
  Both modes pass final exact C, native UBSan, checked live release operands/
  reference counts, Linux harness cross-linking and directly linked exact output.
  Compiler invocations use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-linking is not native Linux execution.
- 181 local Markdown paths, 35-group quick/CI selector equality, PowerShell UTF-8 BOM,
  39 annotated output lines, gofmt, full scope/diff review and `git diff --check`
  passed. Precommit fetch and `git ls-remote` still matched the parent; no
  divergence or unrelated changes.

Ignored logs: `build/P0042-{baseline,before,build,focused,focused-final,full,quick,linux}.log`.
The reproduced failure remains evidence; no exclusion or assertion was weakened.

## Limits and handoff

Same eighteen skips and prior shape/operator limits. Unchanged scalar parameters
gain conditional cleanup; rebound parameters and arbitrary aliases remain
unqualified. Member/index/call/assignment/concatenation value-result lifetime,
retained owned-result accumulation and caller consumption, pointer/class results,
aliases/containers/destructors/fields and async/active exception unwinding stay
separate. String-index return typing, custom-type for headers and class null
initializers remain limited.

Publish and verify the remote SHA, then inspect native CI; repair failures first.
Next audit direct borrowed member-result cleanup: establish receiver/backing
lifetime and field-mutation/call barriers before selecting a safe implementation
slice. Preserve fallback wherever that proof is unavailable. End after this pass.
