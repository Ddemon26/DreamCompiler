# P0090 - Release the final join from three returned appends in an append initializer

- Date: 2026-11-25 UTC
- Task: T011
- Parent checkpoint: `efc760d5a03637bcafc75f7139b972c7adb06acf` (P0089)
- Commit lookup: exact `Pass-ID: P0090` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `string snapshot = (text += deeper());`, the factory's third returned join
stayed live until shutdown. The scalar declaration emitter now applies the
three-append factory proof and uses the existing owned append helper. The
factory and its arguments evaluate once. The helper reads the current
destination, installs the concatenation, and releases the original factory
join. The declaration takes an independent managed copy of the installed value.

The [Dream fixture](../../tests/basics/memory/owned_triple_join_append_initializer.dr)
checks that the snapshot survives a subsequent destination append and that the
destination survives snapshot cleanup. It also covers replacement during
argument evaluation, a once-only increment, a const snapshot, borrowed
self-append, and a scoped `for`-header declaration. The
[Go regression](../../codex/go/indexed_strings_test.go) checks emitted C and
exact output, and the [allocation audit](../../codex/python/owned_result_audit.py)
isolates the lifetime change. See the
[initializer rule](../../docs/development/string-ownership.md#append-initializers-with-three-returned-appends-p0090).

## Validation

- The active schedule uses the existing local checkout. The fetched branch
  matched the clean, attached P0089 parent. Both native jobs passed at that SHA
  in [P0089 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45630310875).
- Zig 0.17.1 built the baseline and patched Windows compilers in 15/15 steps.
  The new Go regression failed on P0089 because the initializer emitted an
  ordinary append in both modes (0.756s, `-count=1`). The isolated P0089
  audit measured 80 releases and eight live factory joins after eight
  append initializers per mode (4.849s).
- The focused `TestCompiler_AppendedStrings/triple_join_initializer` Go
  regression passed in both modes (3.107s, `-count=1`). The checked harness
  verified 110,023 releases of live, single-reference allocations per mode
  and an empty registry before shutdown cleanup. Checked and normally linked
  Windows output matched. Both harnesses cross-linked for Linux.
- `zig build test` discovered 194 entries on Windows. **176 passed**,
  **18 skipped**, and zero failed or errored (54.79s). Report
  `test_results_20261125_130206_windows.json` confirms exact fixture output
  and the same statuses and skip reasons for all 193 previous entries.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` passed all **43** groups (146.884s Go suite
  time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all **92** probes
  (99.092s). Eight isolated append initializers per mode made 88 releases and
  left zero final joins live. All 90 prior measurements matched P0089.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is tracked in CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.041s).
- Go formatting, 16 changed local links and anchors, all 90 pass index
  entries, diff review, and `git diff --check` passed. Logs are in
  `build/P0090-*.log`.

## Limits and handoff

Append expressions used in Console output, wrapper arguments, and direct
returns still use the two-append proof. General expression and argument
ownership, async, and exception unwinding remain open.

Verify P0090 publication and both native CI jobs. Then measure retained
factory joins when either Console writer prints `text += deeper()`.
Check release of the original join and that each writer reads the updated
destination after argument evaluation.
