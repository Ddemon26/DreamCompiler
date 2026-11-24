# P0083 - Release the final join from two returned appends in a directly returned append

- Date: 2026-11-24 UTC
- Task: T011
- Parent checkpoint: `bd6676eed26edba9f2d6c1d6f46c8f790af8d8e9` (P0082)
- Commit lookup: exact `Pass-ID: P0083` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `return (text += nested());`, the two-append factory's original final
join stayed registered until shutdown. The return emitter now applies the
two-append factory proof to the operand for an owned scalar local in a
synchronous string function outside an exception region. After the factory
and its arguments run once, the existing helper uses the current destination,
installs the new concatenation, and releases the operand. The caller receives
the installed string.

The [Dream fixture](../../tests/basics/memory/owned_nested_join_append_return.dr)
and [Go regression](../../codex/go/indexed_strings_test.go) check an unbraced
conditional return, destination replacement during argument evaluation, one
observable side effect, a borrowed operand, and an operand past the proof's
depth. The [allocation audit](../../codex/python/owned_result_audit.py)
isolates eight calls in each mode. See the
[return ownership rule](../../docs/development/string-ownership.md#direct-append-returns-with-two-returned-appends-p0083).

## Validation

- The active automation points to the existing local checkout. The clean,
  attached HEAD, approved fetch, and `git ls-remote` matched P0082. The
  exact-commit Windows and Ubuntu jobs passed in
  [P0082 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45256716669).
- Zig reported 0.17.1. Parent and final Windows builds passed 15/15 steps.
- The new Go subtest failed against P0082 at the missing owned-append call in
  both modes (1.020s, `-count=1`). The isolated P0082 audit measured 48 nonnull
  releases and 16 live joins after eight calls in each mode (two probes, 5.905s).
- The fixed Go subtest passed (3.512s, `-count=1`). In each mode the fixture
  made 25 checked releases and had five expected allocations still registered
  before shutdown. Checked and normally linked binaries matched exact output.
- Focused Go `TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)`
  passed all four groups (54.760s, `-count=1`).
- `zig build test` discovered 192 entries on Windows. **174 passed**, **18
  skipped**, and zero failed or errored (59.58s suite time). Report
  `test_results_20261123_190115_windows.json` confirms exact fixture output and
  matching statuses and skip reasons for all 191 previous entries.
- Windows Go quick from `codex/go` (`powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`) passed all 43 groups (146.671s Go
  suite time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all 76 probes (73.957s).
  Eight calls made 56 nonnull releases per mode and left only the eight
  returned allocations live. All 74 earlier measurements matched P0082.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed all 15 steps. Both new allocation probes also cross-linked for Linux.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.061s).
- Go formatting, 14 added local links and anchors, all 83 index records, diff
  review, and `git diff --check` passed. Logs are in `build/P0083-*.log`.

## Limits and handoff

Callers still copy results beyond the two-append adoption limit. The new
audit leaves eight final results registered until shutdown. Cleanup of other
locals at these returns, general expression and argument ownership, async,
and exception unwinding remain open. Native Linux execution is tracked in CI.

Verify P0083 publication and both exact-commit CI jobs. Then measure retention
in a direct scalar initializer calling a factory with three proven returned
appends. Check original-pointer identity if the result qualifies for adoption.
