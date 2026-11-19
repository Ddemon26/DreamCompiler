# P0063 - Release proven factory operands in direct append returns

- Date: 2026-11-19 UTC
- Task: T011
- Parent checkpoint: `6c9a5d6923bed4e89a694e590dd19e3efdeeddc0` (P0062)
- Commit lookup: exact `Pass-ID: P0063` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `return (text += owned());`, the caller could read the join, but the
factory operand stayed registered until shutdown. The return emitter now
uses the existing owned-append helper for a complete synchronous string
return outside an active exception context when the destination is an owned
local scalar and the right-hand call passes the existing factory proof.
The helper evaluates the call once, appends using the destination value
after argument evaluation, and releases the factory operand after the join.
The return passes the joined pointer to the caller.

The [Dream fixture](../../tests/basics/memory/owned_call_append_return.dr)
and the Go quick regression check caller reads after return, caller snapshots,
an unbraced conditional return, a destination replacement and a once-only
side effect during argument evaluation, a null result and a borrowed
operand. The [ownership contract](../../docs/development/string-ownership.md#direct-append-returns-with-proven-factory-operands-p0063)
and [function guide](../../docs/language-features/functions.md) describe the rule.

## Validation

- The active automation uses the existing local checkout. It was clean and
  attached to the approved branch, with no unpublished pass. The approved fetch
  and `git ls-remote` matched P0062. Both P0062 native
  [CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/44434733409)
  passed on that SHA.
- On the unchanged compiler, the isolated eight-call audit measured 16
  nonnull releases and 16 live allocations per mode. Eight were factory
  operands and eight were joins. The broader fixture measured 20 releases and
  21 live allocations per mode. The linked and checked executables printed
  the expected text in all four probes.
- With the change, `python codex/python/owned_result_audit.py` passed 34
  probes (41.896s). The isolated case measured 24 releases and eight live
  joins per mode. Generated C selects the consuming helper and the caller's
  declaration still copies the join.
- From `codex/go`, `go test -count=1 -v -run '^TestCompiler_AppendedStrings$/^owned_factory_return$' .`
  passed both modes (4.457s). Each measured 29 nonnull releases and 12 live
  result allocations before shutdown. The checked harness verifies release
  validity and counts. Both the checked and normally linked programs matched
  exact output. The harness also compiled for Linux.
- Windows native build and Linux cross-build passed **15/15** steps each
  with Zig **0.17.1**. Python runner tests passed **5/5** (0.059s).
- `zig build test` discovered **182** Dream entries. **164 passed**, **18
  skipped**, with zero failures or errors (58.44s). Report
  `test_results_20261119_000727_windows.json`.
- The Windows Go quick protocol from `codex/go`,
  `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`,
  passed all **43** groups (164.436s, `-count=1`).
- Reviewed the diff and checked `git diff --check`, 11 added local links and
  anchors, the unique pass index entry, and the Dream report. Local logs
  are in ignored `build/P0063-*.log`.

## Limits and handoff

Caller consumers can still keep returned joins registered until shutdown.
Cleanup of other locals at these returns, nested result expressions, async
and exception unwinding remain open. Linux cross-builds did not execute Linux
binaries. Verify this pass's remote SHA and native CI, then compare the
returned join and caller copy by allocation identity for
`string snapshot = joined();` when `joined` consists of an owned scalar
declaration and a direct append return. Use both compilation modes before
extending the factory proof.
