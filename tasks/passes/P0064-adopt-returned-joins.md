# P0064 - Adopt proven returned joins in scalar string initializers

- Date: 2026-11-19 UTC
- Task: T011
- Parent checkpoint: `25bda9d61e635a6f385f929ae186a19ff7e5889b` (P0063)
- Commit lookup: exact `Pass-ID: P0064` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `string snapshot = joined();`, the caller copied the joined value and
released the copy at scope exit. The returned join remained registered until
shutdown. A direct scalar initializer now adopts the original allocation
when the resolved synchronous free function declares one qualifying mutable
string local and directly returns an append to that local. The operand must
pass the original factory proof. The check resolves lowered calls, guards
against shadowing in both functions, and limits the proof to one append.

The [Dream fixture](../../tests/basics/memory/owned_call_initializer.dr)
checks the resulting text. The [Go regression](../../codex/go/owned_call_initializer_test.go)
and [checked harness](../../codex/c/owned_call_initializer_harness.c)
exercise ordinary and imported module functions, identify the concatenation's
allocation base at cleanup, and check emitted C for both adoption and fallback.
The [ownership rules](../../docs/development/string-ownership.md#scalar-initializers-that-adopt-returned-joins-p0064)
describe the qualified shape.

## Validation

- The local schedule used the approved checkout, clean and attached with no
  unpublished pass. Approved fetch and `git ls-remote` matched P0063.
  Both [P0063 native CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/44477695219)
  passed at that SHA.
- The pointer check on the unchanged compiler failed in both modes as expected.
  A live, distinct copy was passed to cleanup while the original join was
  still registered. The earlier restricted probe could not locate the
  installed Zig SDK. The approved run reproduced the pointer mismatch.
- The first post-change focused run reached the new probe's final checks but
  failed the fixture's old release total. One join added to the normal
  fixture output is allocated by concatenation, so the harness now counts
  that allocation alongside the copies and the surviving global.
- Focused Go initializer and append-return regressions passed (7.550s,
  `-count=1`). Each mode ran 10,000 scoped declarations, with two copies,
  one join and three releases per call. Each caller released the original
  joined pointer at reference count one and left the registry empty.
  Checked and normally linked programs matched the expected text.
  The harness also compiled for Linux. Eight new fallback cases across
  the two modes checked a borrowed operand, a different append destination,
  an extra statement and a nested append factory. Existing P0063 coverage
  still measured 29 releases and 12 surviving allocations.
- `python codex/python/owned_result_audit.py` passed all **34** probes
  (33.860s). Eight isolated calls per mode still made 24 releases.
  Survivors fell from eight original joins to zero.
- Native build with Zig **0.17.1** passed **15/15** steps. Python runner
  unit tests passed **5/5** (0.068s).
- `zig build test` discovered **182** Dream entries. **164 passed**,
  **18 skipped**, with zero failures or errors (60.27s). The JSON report
  `test_results_20261119_060528_windows.json` confirms these totals and
  a pass for the updated initializer fixture. The 18 unannotated skip paths
  match P0063.
- The Windows Go quick protocol from `codex/go`,
  `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`,
  passed all **43** groups (141.473s Go result, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux` passed
  **15/15** steps. Linux runtime behavior is checked by native CI.
- Reviewed the diff and checked `git diff --check`, 14 local links and
  anchors, and the unique pass index entry. Logs are in ignored
  `build/P0064-*.log`.

## Limits and handoff

Complete assignments, discards, arguments, conditional initializers and
direct Console consumers of returned joins keep their existing emission.
Unproven functions still copy readable return values in scalar declarations.
General return flow, nested result expressions, async and exception unwinding
remain open. Verify P0064 publication and both native CI jobs, then measure
join identity and release order for `target = joined();` as a complete
assignment to an owned scalar in both modes.
