# P0089 - Release the final join from three returned appends after a complete append statement

- Date: 2026-11-25 UTC
- Task: T011
- Parent checkpoint: `6f020156216c9a79d115f8dd6737798a755a454b` (P0088)
- Commit lookup: exact `Pass-ID: P0089` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete `text += deeper();` statement left the factory's third returned
join live until shutdown. The statement now applies the same three-append
factory proof as scalar assignment. It evaluates the factory and its
arguments once, appends the original join to the current destination, and
releases that join after installing the concatenation. An unbraced append
uses the replacement value when a factory argument changes the destination.

The [Dream fixture](../../tests/basics/memory/owned_triple_join_append.dr)
checks exact output, replacement, a once-only increment, and borrowed
self-append. The [Go regression](../../codex/go/indexed_strings_test.go)
checks that the emitted code releases the original operand after the append.
The [checked allocator harness](../../codex/c/indexed_string_harness.c) and
[allocation audit](../../codex/python/owned_result_audit.py) verify lifetime
in ordinary and module-entry code. See the
[append statement rule](../../docs/development/string-ownership.md#complete-append-statements-with-three-returned-appends-p0089).

## Validation

- The active local schedule uses the existing checkout. Fetch and `git ls-remote`
  matched the clean, attached P0088 parent. Both native jobs passed at that SHA
  in [P0088 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45573508995).
- Zig 0.17.1 built the baseline and patched Windows compilers in 15/15 steps.
  The new Go regression failed at the missing release block on P0088 in both
  modes (0.768s, `-count=1`). The isolated P0088 audit measured 64 releases
  and eight live final joins after eight append statements per mode (5.050s).
- The focused `TestCompiler_AppendedStrings/triple_join_operand` Go regression
  passed in both modes (2.851s, `-count=1`). The checked harness verified 90,011
  releases of live, single-reference allocations across 10,000 repeated
  appends, destination replacement and borrowed self-append. The registry
  was empty before shutdown cleanup. Checked and normally linked Windows
  output matched. Both harnesses cross-linked for Linux.
- `zig build test` discovered 193 entries on Windows. **175 passed**,
  **18 skipped**, and zero failed or errored (55.10s). Report
  `test_results_20261125_070047_windows.json` confirms exact fixture output
  and the same statuses and skip reasons for all 192 previous entries.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` passed all **43** groups (143.078s Go suite
  time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all **90** probes
  (95.635s). Eight isolated appends per mode made 72 releases and left zero
  final joins live. All 88 prior measurements matched P0088.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is tracked in CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.043s).
- Go formatting, 18 changed local links and anchors, all 89 pass index
  entries, diff review, and `git diff --check` passed. Logs are in
  `build/P0089-*.log`.

## Limits and handoff

The proof covers complete statements. Append expressions used in initializers,
output, wrapper arguments and returns use the two-append proof. General
expression and argument ownership, async, and exception unwinding remain open.
Native Linux execution is tracked in CI.

Verify publication and both native CI jobs. Then measure retention in
`string snapshot = (text += deeper());` with three proven returned appends.
Check that replacement during operand evaluation uses the current destination
and that the updated destination and snapshot have independent storage.
