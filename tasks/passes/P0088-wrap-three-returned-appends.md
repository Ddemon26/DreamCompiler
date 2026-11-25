# P0088 - Release the final join from three returned appends after a Console wrapper returns

- Date: 2026-11-25 UTC
- Task: T011
- Parent checkpoint: `5d77784eb309c38c358e5cc6adf225be4a299d6c` (P0087)
- Commit lookup: exact `Pass-ID: P0088` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete call to a proven Console wrapper left the factory's third
returned join live until shutdown. The statement emitter now applies the
existing three-append factory proof to the entire argument. The caller
evaluates the factory and its arguments once and passes the original final
pointer to the resolved synchronous wrapper. The wrapper's whole body prints
its sole scalar string parameter. The caller releases the join after the
wrapper returns.

The [Dream fixture](../../tests/basics/memory/owned_call_argument.dr) checks
exact output and argument side effects. The
[Go regression](../../codex/go/owned_call_argument_test.go) checks the emitted
call and release order and runs the
[checked lifetime harness](../../codex/c/owned_call_argument_harness.c) with
ordinary and imported definitions. The
[allocation audit](../../codex/python/owned_result_audit.py) isolates each writer.
See the [wrapper rule](../../docs/development/string-ownership.md#console-wrappers-for-three-returned-appends-p0088).

## Validation

- The active local schedule uses the existing checkout. Fetch and `git ls-remote`
  matched the clean, attached P0087 parent. Both native jobs passed at that SHA in
  [P0087 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45530946894).
- Zig 0.17.1 built the baseline and patched Windows compilers in 15/15 steps.
  The restricted baseline build could not locate the installed SDK. The
  approved SDK build succeeded.
  The baseline Go regression failed on the missing wrapper release blocks (0.709s).
  The isolated baseline audit measured 48 releases and eight live final joins
  after eight calls per writer per mode (four probes, 8.097s).
- From `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_OwnedCall(Argument|Initializer|Discard)$' .` passed (14.213s).
  Each wrapper ran 10,000 times per mode. Each factory call made four copies,
  three joins and seven releases. The writer observed the original join as the
  sole live allocation at reference count one. The caller released that pointer
  after the wrapper returned. Each argument increment ran once. Checked and
  normally linked Windows output matched, and both Linux harnesses cross-linked.
- `zig build test` discovered 192 entries on Windows. **174 passed**,
  **18 skipped**, and zero failed or errored (69.33s). Report
  `test_results_20261125_010137_windows.json` confirms the fixture's exact
  output and the same entry statuses and skip reasons as P0087.
- From `codex/go`, the Windows quick protocol (`powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`) passed all
  **43** groups (164.130s Go suite time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all **88** probes
  (105.656s). The four new probes made 56 releases and left zero live final
  joins per eight calls. All 84 prior measurements matched P0087.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is tracked in CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.033s).
- Go formatting, 18 changed local links and anchors, 88 pass index entries,
  diff review, and `git diff --check` passed. Logs are in `build/P0088-*.log`.

## Limits and handoff

Four-append calls, forwarding wrappers, and conditional arguments use ordinary
emission. Appended factory operands still use the two-append proof. General
argument and expression ownership, async, and exception unwinding remain open.

Verify publication and both native CI jobs. Then measure retention in a complete
`text += deeper();` statement. Check that the append uses the current destination
after operand evaluation, including when a factory argument replaces it, and
releases the original final join from the factory.
