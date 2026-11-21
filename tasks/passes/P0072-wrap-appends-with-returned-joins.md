# P0072 - Release proven returned joins in appends passed to Console wrappers

- Date: 2026-11-21 UTC
- Task: T011
- Parent checkpoint: `377aa7f407484b058ae7427704f599b4b4794a50` (P0071)
- Commit lookup: exact `Pass-ID: P0072` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In a statement such as `view(text += joined());`, the original join
previously remained registered until shutdown. The complete-statement emitter
now accepts proven returned joins in the existing owned append path for a
single-parameter Console wrapper. The caller evaluates the operand once, then
the helper appends it to the current destination and releases the original
join. The wrapper receives the updated value, which the destination still owns.

The [Dream fixture](../../tests/basics/memory/owned_returned_join_append_wrapper.dr)
and [Go regression](../../codex/go/indexed_strings_test.go) cover wrappers
using both writers, a destination replaced during argument evaluation, a
once-only increment, subsequent destination access, and a borrowed
self-append. The audit also covers a wrapper with two statements.
See the [ownership rule](../../docs/development/string-ownership.md#append-arguments-with-proven-returned-joins-p0072).

## Validation

- The active schedule targets the approved local checkout. The fetched tip
  and `git ls-remote` matched the clean, attached P0071 parent with zero
  ahead or behind. The exact-parent Windows and Ubuntu jobs passed in
  [P0071 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44848733343).
- Zig version was **0.17.1**. The parent Windows build passed 15/15 steps.
  Git and compiler checks used the standard approval flow for GitHub and the
  installed toolchains/caches.
- The new Go regression failed against the parent at the missing owned append
  emission in both modes (2.883s, `-count=1`). Eight qualified wrapper calls per
  mode measured 32 nonnull releases and eight live joins in the baseline audit.
  All 52 baseline probes passed (164.241s). The two-statement control measured
  four releases and one live join in each mode.
- Focused Go `go test -count=1 -v -run
  '^TestCompiler_(AppendedStrings|OwnedCallArgument|OwnedCallInitializer|OwnedCallAssignment)$' .`
  passed all four groups (104.574s). Each mode verified the owned helper in
  generated C, 10,000 repeated wrapper calls, 40,009 nonnull releases at
  reference count one, and zero live allocations before shutdown. The
  checked harness and normally linked program both matched exact output.
- `python codex/python/owned_result_audit.py` passed all 52 final probes
  (138.854s). Eight qualified wrapper calls per mode now make 40 releases and
  leave zero joins. The other 50 measurements matched the parent, including
  one live join in each two-statement control.
- `zig build test` discovered 186 entries on Windows. 168 passed, 18 skipped,
  and zero failed or errored (143.88s). Report
  `test_results_20261121_000614_windows.json` confirms exact fixture output
  and the same skip paths and reasons as P0071.
- Windows Go quick from `codex/go` (`powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`) passed all 43 groups (349.757s,
  `-count=1`).
- The final Windows build and `zig build -Dtarget=x86_64-linux-gnu --prefix
  build/linux --summary all` each passed 15/15 steps. Native Linux execution is
  checked separately by CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed five tests (0.153s).
- Reviewed the diff and checked Go formatting, 14 added local links and
  anchors, all 72 pass index entries, and the staged diff with `git diff --check`.
  Logs are in `build/P0072-*.log`.

## Limits and handoff

Direct synchronous returns of appends with returned join operands remain open.
General expression and argument ownership, async lifetime, and exception
unwinding remain open. Verify this commit's publication and native CI, then
measure retention in `return (text += joined());` from an owned local and
verify that the caller can read the returned value in both modes.
