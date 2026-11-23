# P0079 - Release the final join from two returned appends after a complete append statement

- Date: 2026-11-23 UTC
- Task: T011
- Parent checkpoint: `f8b1bab219289e4cb115ed54cbf00b30c8b846f0` (P0078)
- Commit lookup: exact `Pass-ID: P0079` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete `text += nested();` statement to an owned scalar string left the
factory's final join live until shutdown. The statement uses the existing
two-append factory proof. It evaluates the factory and its arguments once,
appends the original result to the current destination, and releases that
result after installing the new concatenation.

The [Dream fixture](../../tests/basics/memory/owned_nested_join_append.dr) and
[Go regression](../../codex/go/indexed_strings_test.go) cover an unbraced
statement where an argument replaces the destination, a once-only increment,
and borrowed self-append. The [allocation audit](../../codex/python/owned_result_audit.py)
isolates the retained joins. See the
[append statement rule](../../docs/development/string-ownership.md#complete-append-statements-with-two-returned-appends-p0079).

## Validation

- The schedule targets the existing local checkout. The approved fetch and
  `git ls-remote` matched the clean, attached P0078 commit. Both native jobs
  passed at that SHA in [P0078 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45058950909).
- On P0078, the new `nested_join_operand` Go subtest failed in both modes at
  the missing emitted operand release (0.846s, `-count=1`). Eight isolated
  appends made 48 releases and left eight joins live per mode (5.456s).
- Focused Go tests `TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)`
  passed (40.277s, `-count=1`). Each mode checked 10,000 repeated appends,
  70,009 nonnull releases at reference count one and zero live allocations
  before shutdown. The harness and normally linked program matched exact
  output, including the replacement destination and once-only increment.
  The checked harness also cross-linked for Linux.
- `zig build test` with Zig 0.17.1 discovered 188 entries on Windows.
  **170 passed**, **18 skipped**, and zero failed or errored (57.43s).
  Report `test_results_20261122_180443_windows.json` confirms the new fixture's
  exact output and all 187 previous statuses and skip reasons.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed all five tests (0.030s).
- Windows Go quick (`powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`) passed all 43 groups
  (136.797s, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all 64 probes (71.254s).
  Eight isolated appends made 56 releases and left zero live joins in each
  mode. All 62 previous measurements matched P0078.
- The native Windows build and `zig build -Dtarget=x86_64-linux-gnu --prefix
  build/linux --summary all` passed 15/15 steps each.
- Go formatting, local Markdown links and anchors, all 79 index records,
  diff review, and `git diff --check` passed. Logs are in `build/P0079-*.log`.

## Limits and handoff

The new proof applies to complete append statements. Larger append expressions,
general expression and argument ownership, async, and exception unwinding remain
open. Native Linux runtime validation is tracked in CI.

Verify P0079 publication and both native CI jobs. Then measure retention in
`string snapshot = (text += nested());` and qualify the two-append operand,
checking independent destination and snapshot storage across replacement.
