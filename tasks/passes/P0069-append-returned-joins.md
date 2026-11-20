# P0069 - Release proven returned joins after complete append statements

- Date: 2026-11-20 UTC
- Task: T011
- Parent checkpoint: `5bed7405f8a8fbe4ff55e0972d70947c596fa29f` (P0068)
- Commit lookup: exact `Pass-ID: P0069` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete `text += joined();` statement to an owned scalar string left the
factory's original join live until shutdown. The statement emitter now accepts
the existing returned join proof for its operand. The caller evaluates the call
once, appends the original result to the current destination, and releases that
result after the helper installs the newly concatenated value.

The [Dream fixture](../../tests/basics/memory/owned_returned_join_append.dr)
and [Go regression](../../codex/go/indexed_strings_test.go) exercise argument
evaluation that replaces the destination before it is read, an unbraced
statement, and borrowed self-append. See the
[ownership rule](../../docs/development/string-ownership.md#complete-appends-with-proven-returned-joins-p0069).

## Validation

- The active local schedule already targets the requested checkout. The
  approved fetch and `git ls-remote` matched the clean, attached P0068 commit
  with zero ahead or behind. Both exact-parent native CI jobs passed. See the
  [P0068 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44693285083).
- Zig version was **0.17.1**. The native build passed 15/15 steps.
- The new Go subtest failed against the parent binary at the missing generated
  release in both modes (0.819s, `-count=1`).
- The ownership audit (`codex/python/owned_result_audit.py`) passed all 44
  probes at baseline (50.910s) and after the change (48.313s). Eight isolated appends went from
  32 releases and eight live joins to 40 releases and zero live joins in each
  mode. The other 42 paired measurements matched.
- Focused Go `go test -count=1 -v -run
  '^TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)$' .`
  passed four groups (28.877s). In each mode the new subtest checks 10,000
  repeated appends, 50,007 nonnull releases at reference count one and zero
  live allocations before shutdown. It verifies emitted order, once-only
  effects and exact output in the checked harness and normally linked
  program. Windows also cross-linked the harness for Linux.
- `zig build test` discovered 183 entries on Windows. 165 passed, 18 skipped,
  and zero failed or errored (53.23s). The report
  `test_results_20261120_060253_windows.json` confirms exact fixture output
  and the same 18 skip paths and reasons as P0068.
- Windows Go quick (`powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`) passed all 43 groups
  (127.214s, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is checked separately by CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed all five tests (0.038s).
- Reviewed the diff and checked Go formatting, 14 added local Markdown links
  and anchors, the pass index and `git diff --check`. Logs are in
  `build/P0069-*.log`.

## Limits and handoff

Returned joins in larger append expressions and general expression ownership
remain open, along with async lifetime and exception unwinding.
Verify P0069 publication and its native CI jobs, then measure retention in
`string snapshot = (text += joined());` and check independent destination
and snapshot storage in both modes.
