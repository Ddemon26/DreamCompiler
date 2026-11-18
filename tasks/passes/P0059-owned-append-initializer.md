# P0059 - Release proven factory operands in string append initializers

- Date: 2026-11-18 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `6e57297192abe27cb20ef4bd2eb6e75a37c4c18c`
- Commit lookup: exact `Pass-ID: P0059` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `string snapshot = (text += owned());`, the parent copied the joined
string into the snapshot and left the original factory allocation live
until shutdown. In each compilation mode, eight isolated initializers
produced 32 nonnull releases and left eight factory allocations live.

A direct scalar string initializer now shares the complete-statement test
for an owned scalar identifier and an eligible factory operand. The generated
C evaluates the factory once before calling the helper, which appends to the
current destination and releases the factory result after installing the join. The
existing declaration copy gives the snapshot independent storage. This keeps
the `const` and loop-header initialization and cleanup paths intact. See the
[ownership rule](../../docs/development/string-ownership.md#append-initializers-with-proven-factory-results-p0059).

## Validation

- The active schedule already uses the local project directory. The attached
  branch, clean checkout, single worktree and fetch/push origin matched the
  project contract. Restricted fetch failed DNS. Approved fetch and
  `git ls-remote` verified P0058 with zero divergence or unpublished commits.
- P0058 [exact-parent CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44310991032)
  passed natively on Linux (134824775825) and Windows (134824776276).
- Zig **0.17.1**. Restricted `zig cc` could not resolve its installed SDK.
  The approved SDK invocation passed the isolated parent audit in both modes
  (4.243s). The new Go regression then failed at the missing release helper
  in both parent modes (0.763s).
- Native `zig build --summary all` passed **15/15** steps.
- Focused Go `go test -count=1 -v -run '^TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)$' .`
  passed **four groups** (20.189s). The new regression checks 10,000
  repetitions per mode, 50,013 nonnull releases and zero live allocations
  before shutdown in each mode. Exact C and output check that the snapshot
  survives a destination change and the destination survives snapshot cleanup.
  The fixture covers a factory argument that replaces the destination and
  increments a counter once, a null factory result, borrowed self-append,
  const snapshot and zero-iteration header cleanup. The checked native and
  normally linked runs match, and the Linux harness cross-links.
- `python codex/python/owned_result_audit.py` passed **26 probes** (31.859s).
  The isolated append initializer reached **40 releases and zero survivors**
  per mode. The complete append stayed at **24/0** and the aggregate stayed
  at **7/0**. Escape and sibling controls each retained one original.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  **5 tests** (0.036s).
- `zig build test`: **179 discovered, 161 passed, 18 skipped, zero failed or
  errored** (55.29s). Report: `test_results_20261118_063302_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  passed **43 groups** (128.677s, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed **15/15** steps in the Windows-to-Linux cross-build.
- Diff review and `git diff --check` passed. Verified 12 added local Markdown
  links, 59 indexed pass records, Go formatting, Python syntax and the Dream
  report path. Local logs are in ignored `build/P0059-*.log`.

## Limits and handoff

The rule covers a direct append initializer with an eligible operand. Appends
under a conditional or another operator, used as arguments, and returned
remain on the general expression path. Borrowed operands keep their existing
behavior. General argument ownership, async lifetime, and exception unwinding
remain open. Eighteen unannotated Dream entries remain skipped.

Verify publication and exact-commit CI. Then isolate
`Console.WriteLine(text += owned());` in both modes and measure the original
operand while checking that the destination survives printing.
