# P0073 - Release proven returned joins in directly returned appends

- Date: 2026-11-21 UTC
- Task: T011
- Parent checkpoint: `83dc1d527f1eb30ee894570cd43f4069cae3f9d4` (P0072)
- Commit lookup: exact `Pass-ID: P0073` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `return (text += joined());`, the intermediate join previously stayed
registered until shutdown. The synchronous return emitter now consumes a proven
returned join through the existing owned append helper. After argument
evaluation, the helper appends to the current owned local, releases the
intermediate join, and returns the new concatenation to the caller.

The [Dream fixture](../../tests/basics/memory/owned_returned_join_append_return.dr)
and [Go regression](../../codex/go/indexed_strings_test.go) check the caller's
snapshot, a destination replaced during a conditional return's argument
evaluation, a side effect that runs once, and a borrowed operand. The
[allocation audit](../../codex/python/owned_result_audit.py) isolates eight
consecutive returned appends. See the
[ownership rule](../../docs/development/string-ownership.md#direct-append-returns-with-proven-returned-joins-p0073).

## Validation

- The active schedule uses the approved local checkout. Fetch and `git ls-remote`
  matched the clean, attached P0072 parent. The exact-commit Windows and Ubuntu
  jobs passed in [P0072 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44879397034).
- Zig version was **0.17.1**. Parent and final native builds passed 15/15 steps.
- The new Go regression failed against the parent on missing owned append
  emission in both modes (0.809s, `-count=1`). The isolated parent probe
  measured 24 nonnull releases and 16 live allocations per mode (5.316s).
- Focused Go `go test -count=1 -v -run
  '^TestCompiler_AppendedStrings/returned_join_return$' .` passed (2.930s).
  The fixture matched exact output in the checked and normally linked binaries.
  Each mode made 49 checked releases at reference count one and had 10 final
  results registered before shutdown.
- `python codex/python/owned_result_audit.py` passed all 54 probes (58.677s).
  The eight calls in each new probe made 32 releases and left eight final
  results live. The other 52 measurements matched P0072.
- `zig build test` discovered 187 entries on Windows. 169 passed, 18 skipped,
  and zero failed or errored (53.81s). Report
  `test_results_20261121_060912_windows.json` confirms exact fixture output
  and the same skip paths and reasons as P0072.
- Windows Go quick from `codex/go` (`powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`) passed all 43 groups (135.255s,
  `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is checked separately by CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed five tests (0.047s).
- Diff review, `git diff --check`, Go formatting and all 13 added local links
  and anchors checked. The pass index resolves all 73 records. Logs are in
  `build/P0073-*.log`.

## Limits and handoff

Caller adoption remains bounded by the existing proof. The final results in
the new audit remain registered until shutdown. Cleanup of other locals at
these returns, nested result expressions, async and exception unwinding remain
open. Verify P0073 publication and both exact-commit CI jobs. Then measure
final-result retention in a scalar initializer calling a two-statement function
that returns an append of a proven returned join. Check original-pointer
identity if a bounded caller proof qualifies the result for adoption.
