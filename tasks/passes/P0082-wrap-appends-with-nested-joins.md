# P0082 - Release the final join from two returned appends before a Console wrapper prints an append

- Date: 2026-11-23 UTC
- Task: T011
- Parent checkpoint: `1a63ac96a676877d36819abc5d039d1bde61db04` (P0081)
- Commit lookup: exact `Pass-ID: P0082` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `view(text += nested());` with a proven Console wrapper, the factory's
original final join stayed live until shutdown. The statement emitter now
uses the two-append factory proof for the operand. The factory and its
arguments run once. The owned-append helper reads the current destination,
installs the concatenation, and releases the factory operand before the
wrapper runs. The destination retains the printed value after either writer
returns.

The [Dream fixture](../../tests/basics/memory/owned_nested_join_append_wrapper.dr)
and [Go regression](../../codex/go/indexed_strings_test.go) check both wrappers,
replacement of the destination during operand evaluation, argument side
effects, borrowed self-append, and an extra-statement wrapper that follows
ordinary append emission. See the
[wrapper append rule](../../docs/development/string-ownership.md#console-wrapper-append-arguments-with-two-returned-appends-p0082).

## Validation

- The existing local checkout was clean and attached to the delivery branch.
  The approved fetch matched P0081. Both native jobs passed at that SHA in
  [P0081 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45194744940).
- Zig reported 0.17.1. The sandbox could not access the installed SDK for the
  baseline build. Approved SDK access let the native Windows builds pass
  15/15 steps before and after the change.
- On P0081 the new Go subtest failed at the missing owned-append call in both
  modes (0.795s, `-count=1`). The isolated baseline audit measured 48 nonnull
  releases and eight live joins after eight calls for each writer in each mode
  (four probes, 6.915s).
- The new Go subtest passed (2.746s, `-count=1`). Each checked binary made 22
  nonnull releases and retained one join before shutdown from the
  extra-statement wrapper. Checked and normally linked output matched exactly.
- Focused Go `TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)`
  passed (44.732s, `-count=1`).
- `zig build test` discovered 191 entries on Windows. **173 passed**, **18
  skipped**, and zero failed or errored (50.70s suite time). Report
  `test_results_20261123_130638_windows.json` confirms exact fixture output and
  matching statuses and skip reasons for all 190 previous entries.
- Windows Go quick (`powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`) passed all 43 groups
  (144.218s Go suite time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all 74 probes (74.874s).
  Eight calls through each wrapper made 56 nonnull releases and left zero
  factory joins live before shutdown in each mode. All 70 earlier
  measurements matched P0081.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed all 15 steps. The new allocation probes also cross-linked for Linux.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.048s).
- Go formatting, 14 added local links and anchors, all 82 index records, diff
  review, and `git diff --check` passed. Logs are in `build/P0082-*.log`.

## Limits and handoff

This covers complete call statements to proven one-parameter Console wrappers
with an owned scalar string destination and a qualifying factory operand.
Other wrappers, general expression and argument ownership, async, and
exception unwinding still need qualification. Native Linux execution for this
pass is tracked in CI.

Verify P0082 publication and both native CI jobs. Then measure retention in
`return (text += nested());` for an owned scalar local in a synchronous string
function. Qualify release of the two-append operand and check that the newly
installed join survives return.
