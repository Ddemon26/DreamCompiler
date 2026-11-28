# P0101 - Release four returned appends in snapshot initializers

- Date: 2026-11-28 UTC
- Task: T011
- Parent checkpoint: `4d04942a686b852f1ed1f8d9356bae6c79a6c632` (P0100)
- Commit lookup: exact `Pass-ID: P0101` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `string snapshot = (text += fourth());`, the original fourth join stayed
live until shutdown. The scalar declaration now uses the existing four-append
proof for the operand. The append evaluates the factory and its arguments once,
reads the current destination, installs the concatenation, and releases the
original fourth join. The initializer copies the installed value into independently
owned storage.

The [Dream fixture](../../tests/basics/memory/owned_fourth_join_append_initializer.dr)
checks snapshot output after changing the destination and destination output
after snapshot cleanup. The [Go regression](../../codex/go/indexed_strings_test.go)
checks ordinary and imported calls, const and loop-header declarations, one
argument that replaces the destination, one that increments a counter, borrowed
self-append, and ordinary emission for a fifth join. See the
[initializer rule](../../docs/development/string-ownership.md#append-initializers-with-four-returned-appends-p0101).

## Validation

- The active schedule uses the existing local checkout. The clean, attached
  branch matched the fetched remote. Both
  [P0100 CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/46124891497)
  passed at the verified parent SHA.
- Zig 0.17.1 and re2c 3.0 built the unchanged and patched Windows compilers in
  15/15 steps each. The Linux cross-build passed 15/15 steps with
  `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`.
- On P0100 the focused Go regression failed on missing ownership emission
  in both modes (1.396s). Two isolated baseline audit probes measured 96
  releases and eight live joins after eight initializers per mode (8.241s).
- From `codex/go`,
  `go test -count=1 -v -run '^TestCompiler_AppendedStrings$/^fourth_join_(initializer|operand)$' .`
  passed (6.210s). For the new case, each mode made 130,027 checked releases
  of live allocations at reference count one across 10,000 repetitions and
  the additional cases. The registry was empty before shutdown. Exact output
  matched in checked and normally linked Windows binaries. The checked harnesses
  also cross-linked for Linux.
- `zig build test` discovered 199 Windows entries. **181 passed**, **18 skipped**,
  and zero failed or errored (67.57s). Report `test_results_20261128_070518_windows.json`.
  The new snapshot fixture matched exact output. All 198 previous entries matched
  P0100's statuses, outputs, and skip reasons.
- The Windows Go quick protocol passed all **43** groups (247.270s, `-count=1`)
  using `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  from `codex/go`.
- `python codex/python/owned_result_audit.py` passed all **118** probes
  (164.331s). Eight initializers per mode made 104 releases and left zero fourth
  joins live. All 116 prior measurements matched P0100.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.049s).
- Go formatting, 18 changed local links and anchors, all 101 pass index
  entries, diff review, and `git diff --check` passed. Logs are in
  `build/P0101-*.log`.

## Limits and handoff

Five-append calls, borrowed and conditional operands, and appends within
larger initializer expressions use ordinary append emission. General
expression and argument ownership, async, and exception unwinding remain open.
Eighteen unannotated Dream entries remain skipped. Native Linux execution is
tracked in CI.

Verify publication and both exact-commit CI jobs. Then measure retention in
`Console.Write(text += fourth());` and `Console.WriteLine(text += fourth());`.
Check exact output, once-only argument evaluation, and that the destination
remains readable after printing in ordinary and imported calls.
