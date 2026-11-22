# P0077 - Release the final join from two returned appends after direct Console output

- Date: 2026-11-22 UTC
- Task: T011
- Parent checkpoint: `e259ff5cf3ba4c43387b7f19a8b72ee6722e097d` (P0076)
- Commit lookup: exact `Pass-ID: P0077` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Direct Console output of `nested()` left the final join live until shutdown
when the factory returned two proven appends. Complete `Console.Write` and
`Console.WriteLine` statements now apply the existing two-append factory
proof. The statement evaluates the call and its arguments once, passes the
original final pointer to the synchronous writer, and releases it after output.

The [Dream fixture](../../tests/basics/memory/owned_call_argument.dr),
[Go regression](../../codex/go/owned_call_argument_test.go), and
[checked harness](../../codex/c/owned_call_argument_harness.c) exercise both
writers with ordinary and imported definitions. The
[allocation audit](../../codex/python/owned_result_audit.py) also measures
retention before shutdown. See the
[Console output rule](../../docs/development/string-ownership.md#direct-console-output-of-two-returned-appends-p0077).

## Validation

- The active schedule uses the existing local checkout. Fetch and
  `git ls-remote` matched the clean, attached P0076 parent. Both native jobs
  passed at that SHA in
  [P0076 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44997427544).
- The isolated P0076 baseline audit measured 32 releases and eight live final
  joins after eight `Console.WriteLine` calls per mode (11.378s). The expanded
  Go argument test failed on P0076 because both writers lacked the release
  block in ordinary and imported calls (0.811s, `-count=1`).
- Zig 0.17.1 native Windows and Linux cross-builds passed 15/15 steps each.
- The focused Go argument and initializer tests passed (11.509s, `-count=1`).
  The checked harness ran 10,000 calls through each writer per mode. Each
  two-append call made three copies, two joins and five releases. The writer
  saw the original final join as the sole live allocation at reference count
  one. The harness checked its release after output and one argument increment
  per call. Checked and normally linked output matched in both modes, and
  both harnesses cross-linked for Linux.
- `zig build test` discovered 187 entries. **169 passed**, **18 skipped**,
  and zero failed or errored (66.04s). Report
  `test_results_20261122_060729_windows.json` confirms exact fixture output
  and identical entry statuses, skip paths and reasons to P0076.
- The Windows Go quick protocol passed all **43** groups (155.605s, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all **62** probes (77.149s).
  Four calls per writer made 40 releases and left zero live final joins per
  mode. All 58 other P0076 measurements matched. The two new wrapper probes
  made 32 releases and left eight final joins live per mode.
- Python runner unit tests passed 5/5 (0.067s).
- Go formatting, diff review, `git diff --check`, local links and anchors, and
  all 77 pass index entries checked. Logs are in `build/P0077-*.log`.

## Limits and handoff

The proof covers a complete direct Console statement whose entire operand
is a proven call with up to two returned appends. Compound operands and longer
chains follow ordinary emission. Console wrappers still use the one-append
proof. General argument ownership, async, and exception unwinding remain open.

Verify publication and both native CI jobs, then qualify release after a proven
Console wrapper returns from processing a two-append result. The
`nested_join_wrapper` audit covers the current retention.
