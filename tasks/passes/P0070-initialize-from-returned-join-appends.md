# P0070 - Release proven returned joins in append initializers

- Date: 2026-11-20 UTC
- Task: T011
- Parent checkpoint: `5941066b2230e4c32bdd379a4c995c8a7c18444a` (P0069)
- Commit lookup: exact `Pass-ID: P0070` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A direct scalar initializer such as `string snapshot = (text += joined());`
kept the factory's original returned join live until shutdown. The declaration
emitter now accepts the existing returned join proof for the append operand.
The initializer evaluates the call once. The append helper concatenates into
the current destination and releases the original operand. The initializer then
takes an independent managed snapshot of the updated destination.

The [Dream fixture](../../tests/basics/memory/owned_returned_join_append_initializer.dr)
and [Go regression](../../codex/go/indexed_strings_test.go) cover destination
replacement during operand evaluation, a once-only side effect, independent
snapshot lifetime, a const snapshot, borrowed self-append, and cleanup of a
snapshot declared in a zero-iteration loop header. See the
[ownership rule](../../docs/development/string-ownership.md#append-initializers-with-proven-returned-joins-p0070).

## Validation

- The local schedule targets the authorized checkout. The approved fetch and
  `git ls-remote` matched the clean, attached P0069 commit with zero ahead or
  behind. Both exact-parent jobs passed in the
  [P0069 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44741352440).
- Zig version was **0.17.1**. Parent and final Windows builds passed 15/15 steps.
- The new Go regression failed against the parent at the missing release
  helper in both modes (0.802s, `-count=1`). The parent lifetime audit measured
  48 nonnull releases and eight live joins per mode after eight initializers.
- The ownership audit (`python codex/python/owned_result_audit.py`) passed all
  46 probes at baseline (48.717s) and after the change (46.217s). The final
  eight-initializer probe measured 56 releases and zero live joins in each
  mode. The other 44 paired measurements matched.
- Focused Go `go test -count=1 -v -run
  '^TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)$' .`
  passed four groups (28.555s). In each mode the new subtest checks 10,000
  repeated initializers, 70,015 nonnull releases at reference count one,
  zero live allocations before shutdown, generated C, and exact output in the
  checked harness and normally linked program. Windows also cross-linked the
  harness for Linux.
- `zig build test` discovered 184 entries on Windows. 166 passed, 18 skipped,
  and zero failed or errored (49.98s). The report
  `test_results_20261120_120858_windows.json` confirms exact fixture output
  and the same 18 skip paths and reasons as P0069.
- Windows Go quick (`powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`) passed all 43 groups
  (115.464s, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is checked separately by CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.044s).
- Reviewed the diff and checked Go formatting, 14 added local links and
  anchors, 70 pass index entries, and `git diff --check`. Logs are in
  `build/P0070-*.log`.

## Limits and handoff

Returned joins in append expressions consumed by Console, wrappers or a
return remain open. General expression and argument ownership, async lifetime,
and exception unwinding remain open. Verify this commit's publication and native
CI, then measure join retention in `Console.WriteLine(text += joined());` and
check that the destination remains readable after printing in both modes.
