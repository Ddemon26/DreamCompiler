# P0100 - Release four returned appends after a complete append statement

- Date: 2026-11-28 UTC
- Task: T011
- Parent checkpoint: `fdc0d2baada6b1b07b1558cb831cebcb81d64ec0` (P0099)
- Commit lookup: exact `Pass-ID: P0100` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete `text += fourth();` statement left the factory's original fourth
join live until shutdown. The statement now applies the existing four-append
factory proof. The call and its arguments evaluate once. The append reads the
current destination, installs the concatenation, and releases the exact operand
it received from the factory.

The [Dream fixture](../../tests/basics/memory/owned_fourth_join_append.dr)
checks output after a factory argument replaces the destination and increments a
counter. The [Go regression](../../codex/go/indexed_strings_test.go) checks
emitted C and live releases for ordinary and imported calls, including a
borrowed self-append and emission for a fifth join. See the
[append statement rule](../../docs/development/string-ownership.md#complete-append-statements-with-four-returned-appends-p0100).

## Validation

- The active schedule uses the existing local checkout. The clean, attached
  branch matched the fetched remote. Both
  [P0099 CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/46097089086)
  passed at the verified parent SHA.
- Zig 0.17.1 and re2c 3.0 built the unchanged and patched Windows compilers in
  15/15 steps each. The Linux cross-build passed 15/15 steps with
  `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`.
- On the P0099 compiler, the focused Go regression failed on the missing
  release block in both modes (0.995s). The two isolated baseline audit probes
  measured 80 releases and eight live joins after eight appends (5.623s).
- `go test -count=1 -v -run '^TestCompiler_AppendedStrings$/^fourth_join_operand$' .`
  from `codex/go` passed (3.267s). In each mode the checked harness verified
  110,013 releases of live allocations at reference count one, including
  10,000 repeated appends, destination replacement, and borrowed self-append.
  The registry was empty before shutdown. Checked and normally linked Windows
  output matched. Both checked harnesses cross-linked for Linux.
  An initial patched run failed because an expectation joined the emitted `if`
  and release block across debug line directives. The final check matches the
  release block and exercises the unbraced branch through its runtime output.
- `zig build test` discovered 198 Windows entries. **180 passed**, **18 skipped**,
  and zero failed or errored (67.24s). Report `test_results_20261128_010833_windows.json`.
  The new fixture matched exact output. All 197 previous entries matched P0099's
  statuses, outputs, and skip reasons.
- The Windows Go quick protocol passed all **43** groups (180.507s, `-count=1`)
  using `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  from `codex/go`.
- `python codex/python/owned_result_audit.py` passed all **116** probes
  (147.110s). Eight appends per mode made 88 releases and left zero final joins
  live. All 114 prior measurements matched P0099.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.059s).
- Go formatting, 12 changed local links and anchors, all 100 pass index
  entries, diff review, and `git diff --check` passed. Logs are in
  `build/P0100-*.log`.

## Limits and handoff

Five-append calls, borrowed and conditional operands, and fourth joins in
nested append expressions use ordinary append emission. General expression
and argument ownership, async, and exception unwinding remain open. Native
Linux execution is tracked in CI.

Verify publication and both exact-commit CI jobs. Then measure retention in
`string snapshot = (text += fourth());`. Check that the append uses the
post-argument destination and that the destination and snapshot own independent
storage in ordinary and imported calls.
