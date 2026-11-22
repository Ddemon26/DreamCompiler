# P0078 - Release the final join from two returned appends after a Console wrapper returns

- Date: 2026-11-22 UTC
- Task: T011
- Parent checkpoint: `b34bc8cb7a1088c05f5b0249bbccf9f8566c6b64` (P0077)
- Commit lookup: exact `Pass-ID: P0078` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A proven Console wrapper left the final join from `nested()` live until
shutdown. Complete wrapper calls now apply the existing proof for factories
that return up to two appends. The caller evaluates the factory and its
arguments once and releases the original final join after the wrapper returns.
The resolved wrapper must print its sole scalar string parameter using
`Console.Write` or `Console.WriteLine` as its entire body.

Both writers are covered in the
[Dream fixture](../../tests/basics/memory/owned_call_argument.dr),
[Go regression](../../codex/go/owned_call_argument_test.go),
[checked harness](../../codex/c/owned_call_argument_harness.c), and
[allocation audit](../../codex/python/owned_result_audit.py).
See the [wrapper rule](../../docs/development/string-ownership.md#console-wrappers-for-two-returned-appends-p0078).

## Validation

- Fetch and `git ls-remote` matched the clean, attached P0077 parent in the
  existing scheduled checkout. Both native jobs passed at that SHA in
  [P0077 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45026250607).
- The isolated P0077 audit made 32 releases and left eight final joins live
  after eight wrapper calls in each mode (11.055s). The expanded Go regression
  failed on P0077 where the required wrapper release blocks were missing
  (0.630s, `-count=1`).
- The focused Go ownership argument and initializer tests passed (9.501s,
  `-count=1`). Per mode, 10,000 calls through each wrapper exercised the
  original final pointer at reference count one. Each call made three copies,
  two joins and five releases, with one argument increment. Generated C showed
  the release after the wrapper call. Checked and normally linked output
  matched in both modes, and both harnesses cross-linked for Linux.
- `zig build test` discovered 187 entries. **169 passed**, **18 skipped**,
  and zero failed or errored (53.94s). Report
  `test_results_20261122_120313_windows.json` confirms exact fixture output
  and identical statuses, skip paths and reasons to P0077.
- Zig 0.17.1 native Windows and Linux cross-builds passed 15/15 steps each.
  Python runner unit tests passed 5/5 (0.046s).
- The Windows Go quick protocol passed all 43 groups (128.533s, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all 62 probes (64.829s).
  Four calls per writer made 40 releases and left zero live final joins in
  each mode. All 60 other measurements matched P0077.
- Go formatting, diff review, `git diff --check`, local links and anchors,
  and all 78 pass index entries checked. Logs are in `build/P0078-*.log`.

## Limits and handoff

The proof covers a complete call to a synchronous Console wrapper with a
sole factory operand. Deeper chains, conditional arguments, and forwarding
retain ordinary emission. Append expressions, general argument ownership,
async, and exception unwinding remain open. Linux execution of P0078 is
tracked by native CI.

Verify publication and both native CI jobs, then measure retention in a
complete `text += nested();` statement and qualify its two-append operand.
Check destination replacement during operand evaluation.
