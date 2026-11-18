# P0061 - Release proven factory operands in appends passed to Console wrappers

- Date: 2026-11-18 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `69059dcfe4cd20e6ce2ac2dea24a0a568a6ab621`
- Commit lookup: exact `Pass-ID: P0061` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `view(text += owned());`, the parent installed and printed the joined
value but kept the original factory allocation registered until shutdown.
Eight isolated calls produced 16 nonnull releases and left eight factory
allocations live in each compilation mode.

Complete calls to a synchronous void free function that only prints its sole
scalar string parameter use the existing owned append helper when the
destination and operand pass the existing ownership proofs. The generated call
evaluates the operand once. The helper then reads the current destination,
installs the join, and releases the factory operand. The destination keeps
the join through and after the wrapper call. See the
[Console wrapper append rule](../../docs/development/string-ownership.md#append-arguments-to-console-wrappers-p0061).

## Validation

- The active schedule uses the approved local directory. The sole attached,
  clean checkout and both origin URLs matched the project contract. Restricted
  fetch failed DNS. Approved fetch and `git ls-remote` verified the parent with
  no unpublished passes or divergence.
- P0060 [exact-parent CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44323725290)
  passed on Linux (134869256198) and Windows (134869255388).
- Zig **0.17.1**. Parent and final native builds passed **15/15** steps. The
  isolated parent audit passed **four probes** (7.288s), and the new Go
  regression detected missing ownership emission in both parent modes (0.751s).
- Focused Go `go test -count=1 -v -run '^TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)$' .`
  passed **four groups** (23.043s). The checked regression covers 10,000 appends
  per mode, 20,008 nonnull releases, zero survivors before shutdown, exact
  emitted C, and matching checked and normally linked output. It exercises
  both writer variants, destination replacement during operand evaluation,
  a once-only side effect, post-call reuse, null, and borrowed self-append.
- `python codex/python/owned_result_audit.py` passed **32 probes** (34.977s).
  The isolated wrapper append reached **24 releases and zero survivors** in
  each mode. A two-statement wrapper retained one original factory allocation
  and made two nonnull releases in each mode. The aggregate remained at **7/0**.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  **5 tests** (0.053s).
- `zig build test`: **181 discovered, 163 passed, 18 skipped, zero failed or
  errored** (51.75s). Report: `test_results_20261118_120053_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  passed **43 groups** (116.625s, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed **15/15** steps. The checked regression also cross-linked for Linux.
- Diff review, `git diff --check`, local Markdown links, pass index, Go
  formatting, Python syntax, and the report path were checked. Local logs
  are in ignored `build/P0061-*.log`.

## Limits and handoff

General calls, nested expressions, returned appends, async lifetimes, and
exception unwinding remain open. Eighteen Dream entries remain unannotated.
Local Linux checks cross-compile binaries. Native Linux runtime validation
for this commit comes from CI.

Verify publication and exact-commit CI. Then isolate `return (text += owned());`
from a synchronous string function with an owned local in both modes. Measure
the factory operand and verify that a caller can read the joined result.
