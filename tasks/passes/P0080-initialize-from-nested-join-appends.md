# P0080 - Release the final join from two returned appends in a snapshot initializer

- Date: 2026-11-23 UTC
- Task: T011
- Parent checkpoint: `97ff788c7ba16b19e5977d906bdde30ace3bcca3` (P0079)
- Commit lookup: exact `Pass-ID: P0080` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `string snapshot = (text += nested());`, an append operand from a proven
two-append factory stayed live until shutdown. The declaration emitter now
uses the two-append proof for the operand. The factory and its arguments run
once. The append helper uses the current destination, installs the new
concatenation, and releases the factory's original final join. The declaration
copies the installed value into independent managed storage.

The [Dream fixture](../../tests/basics/memory/owned_nested_join_append_initializer.dr)
and [Go regression](../../codex/go/indexed_strings_test.go) cover destination
replacement during argument evaluation, a once-only increment, a const
snapshot, borrowed self-append, and a snapshot in a zero-iteration loop header.
Each mode checks snapshot survival after replacement and destination survival
after snapshot cleanup. See the
[initializer rule](../../docs/development/string-ownership.md#append-initializers-with-two-returned-appends-p0080).

## Validation

- The active schedule uses the authorized local checkout. The approved fetch
  and `git ls-remote` matched the clean, attached P0079 commit. Both native jobs
  passed at that SHA in [P0079 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45092901302).
- Zig reported 0.17.1. Native Windows builds passed 15/15 steps on the parent
  and with the change.
- On P0079, the new Go subtest failed in both modes at the missing release
  helper (0.853s, `-count=1`). The isolated probe measured 64 nonnull releases
  and eight live joins after eight initializers in each mode (5.394s).
- Focused Go `TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)`
  passed (40.859s, `-count=1`). The new case checks 10,000 repeated initializers
  and 90,019 nonnull releases at reference count one per mode, with zero live
  allocations before shutdown. The checked harness and normally linked program
  matched exact output. The harness also cross-linked for Linux.
- `zig build test` discovered 189 entries on Windows. **171 passed**, **18
  skipped**, and zero failed or errored (55.477s wall time). Report
  `test_results_20261123_015234_windows.json` confirms exact fixture output and
  matching statuses and skip reasons for the 188 previous entries.
- Windows Go quick (`powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`) passed all 43 groups
  (166.191s, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all 66 probes (80.877s).
  Eight isolated initializers made 72 releases and left zero final joins live
  per mode. All 64 existing measurements matched P0079.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed all 15 steps.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.045s).
- Go formatting, 11 added local links and anchors, all 80 index records, diff
  review, and `git diff --check` passed. Logs are in `build/P0080-*.log`.

## Limits and handoff

The two-append proof in this pass applies to appends that directly initialize
scalar string declarations. General expression and argument ownership, async,
and exception unwinding remain open. Native Linux execution for this pass is
tracked in CI.

Verify P0080 publication and both native CI jobs. Then measure retention in
`Console.WriteLine(text += nested());` and its `Console.Write` counterpart.
Qualify the two-append operand and check that the destination remains readable
after either writer returns.
