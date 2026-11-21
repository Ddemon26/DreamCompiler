# P0071 - Release proven returned joins in appends printed by Console

- Date: 2026-11-21 UTC
- Task: T011
- Parent checkpoint: `167a742e0e3ad84450b26990cf35a6478fd93200` (P0070)
- Commit lookup: exact `Pass-ID: P0071` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete statement such as `Console.WriteLine(text += joined());` previously
kept the original returned join registered until shutdown. The direct Console
statement emitter now lets the existing owned-append helper consume an operand
that satisfies the returned join proof. It evaluates the call once, appends to
the current destination, and releases the original operand before printing.
The destination owns the new value and can still be read and updated.

The [Dream fixture](../../tests/basics/memory/owned_returned_join_append_console.dr)
and [Go regression](../../codex/go/indexed_strings_test.go) exercise both
writers, a destination replaced while evaluating the operand's arguments,
a once-only increment, a post-print update, and borrowed self-append.
See the [ownership rule](../../docs/development/string-ownership.md#console-output-of-appends-with-proven-returned-joins-p0071).

## Validation

- The active schedule already targets the authorized checkout. The approved
  fetch and `git ls-remote` matched the clean, attached P0070 commit with zero
  ahead or behind. Exact-parent Windows and Ubuntu jobs passed in the
  [P0070 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44799863088).
- Zig version was **0.17.1**. The parent and final Windows builds passed 15/15
  steps. The parent build required normal sandbox approval to access the
  installed SDK.
- The new Go regression failed against the parent at the missing release
  helper in both modes (1.040s, `-count=1`). All 48 baseline audit probes passed
  (62.492s), measuring 32 nonnull releases and eight live joins per mode after
  eight printed appends.
- Focused Go `go test -count=1 -v -run
  '^TestCompiler_(AppendedStrings|OwnedCallArgument|OwnedCallInitializer|OwnedCallAssignment)$' .`
  passed four groups (34.786s). The first post-change run exposed a fixture
  assertion that expected consecutive statements to be adjacent in emitted C.
  The final assertions check both statements individually across debug line
  directives, with exact runtime output enforcing their order. The regression
  checks generated C, 10,000 repeated writes, 40,009 nonnull releases at
  reference count one, and zero live allocations before shutdown per mode.
  Both the checked harness and normally linked program matched expected output.
- The ownership audit (`python codex/python/owned_result_audit.py`) passed
  all 48 probes (54.564s). Eight printed appends per mode now record 40 releases
  and zero live joins. The other 46 measurements matched the parent.
- `zig build test` discovered 185 entries on Windows. 167 passed, 18 skipped,
  and zero failed or errored (51.96s). Report
  `test_results_20261120_180917_windows.json` confirms exact fixture output
  and the same skip paths and reasons as P0070.
- Windows Go quick (`powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`) passed all 43 groups (129.200s,
  `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is checked separately by CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.063s).
- Reviewed the diff and checked Go formatting, added local links and anchors,
  all 71 pass index entries, and `git diff --check`. Logs are in
  `build/P0071-*.log`.

## Limits and handoff

Appends with returned join operands passed to Console wrappers or returned
directly from synchronous functions remain open. General expression and
argument ownership, async lifetime, and exception unwinding remain open.
Verify this commit's publication and native CI, then measure retention in
`view(text += joined());` through a proven Console wrapper and check that the
destination remains readable after the wrapper returns in both modes.
