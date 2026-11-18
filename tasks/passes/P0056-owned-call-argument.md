# P0056 - Release proven factory arguments after Console wrappers

- Date: 2026-11-18 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `4b2a9a869c3ec45d5dd9fb419c810bdfc50e3fdb`
- Commit lookup: exact `Pass-ID: P0056` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

P0055 measured eight live originals after eight isolated `view(owned())`
calls in each mode. In P0056, a complete call statement captures the proven
factory result and passes the original pointer to the actual synchronous void
free function when its sole scalar string parameter is used only by one
`Console.Write` or `Console.WriteLine` statement. The caller releases the same
pointer after normal return. The existing factory proof also covers imported
definitions and continues to constrain eligibility. The temporary is local to
the emitted C block, with name selection covering bindings, functions and
lowered enum constants.

See the [argument contract](../../docs/development/owned-call-argument.md)
for eligibility and the historical before measurements.

## Validation

- Verified the approved local checkout, attached branch, clean start, one
  registered worktree, and matching origin URLs. The local schedule already
  targets this directory, and no competing active writer or unpublished pass
  was found. Restricted fetch failed DNS. Approved fetch succeeded and HEAD
  matched the fetched parent with zero commits on either side.
- P0055 exact-parent [CI 44213186405](https://github.com/Ddemon26/DreamCompiler/actions/runs/44213186405)
  passed on Windows (134496395863) and Linux (134496396347).
- Zig version **0.17.1**. The restricted build failed to locate the SDK.
  Subsequent compiler validation used approved installed SDK and cache access.
- The new emitted-C assertions failed at the unchanged parent in both modes
  for the expected missing release blocks. The checked harness then verified
  10,000 isolated calls in each mode, with the original allocation readable
  in Console, exactly one release at reference count one and peak live count
  one. It checks for no live entries after each call and at shutdown. Imported
  exact definitions, same-spelled fallbacks, name collisions, argument effects,
  unbraced and scoped calls, emission-only null/pointer cases and exact normal
  and checked output are covered.
- `zig build --summary all`: **15/15** steps succeeded.
- From `codex/go`, `go test -count=1 -v -run 'TestCompiler_(OwnedCallArgument|ParserRecovery)$' .`:
  **both groups passed** (4.608s). The new wrapper group passed its five
  subtests, including native UBSan execution and Linux harness cross-link.
  Parser recovery preserves the array-parameter rejection and source position
  in ordinary, module-entry and dependency modes.
- `python codex/python/owned_result_audit.py`: **18 probes passed** (20.912s).
  In each mode the isolated argument probe measured **8 releases/0 live**,
  the aggregate probe **5/2**, and the escape and sibling probes **0/1**
  each. Generated-C checks, exact native checked and normally linked output,
  and Linux harness cross-links passed.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed** (0.055s).
- `zig build test`: **177 discovered, 159 passed, 18 skipped, zero failed
  or errored** (58.96s). Report: `test_results_20261118_002529_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`:
  **43 groups passed** (119.340s, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **15/15** steps succeeded. Linux binaries were cross-built on Windows.
- Reviewed the diff, local Markdown links, consecutive pass IDs, Python
  syntax, matching CI and quick selectors, and the PowerShell script BOM.
  `git diff --check` passed. Logs are in ignored `build/P0056-*.log`.

## Limits and handoff

This rule covers only normal return from the exact one-statement wrapper.
Direct Console and append consumption account for the two remaining live
results in the aggregate audit. General or nested argument flow, alias and
container ownership, async lifetime and exception unwinding remain separate.
Null is checked at emission because the Console runtime passes the pointer
to a C string formatter. Eighteen unannotated Dream entries remain skipped.

After verifying delivery and exact-commit CI, isolate the direct Console
factory-result cases in both modes and measure lifetime before selecting a
release rule. Repair a failed baseline first.
