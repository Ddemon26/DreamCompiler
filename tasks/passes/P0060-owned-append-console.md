# P0060 - Release proven factory operands in printed string appends

- Date: 2026-11-18 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `ac42126c87c011c19fb9f5d342754314fc1ef992`
- Commit lookup: exact `Pass-ID: P0060` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `Console.WriteLine(text += owned());`, the parent installed and printed the
joined value and left the original factory allocation registered until shutdown.
Eight isolated iterations produced 16 nonnull releases and left eight factory
allocations live in each compilation mode.

A complete `Console.Write` or `Console.WriteLine` statement now shares the
direct append initializer's destination and factory proof. It evaluates the
factory once, appends to the destination as it exists after argument evaluation,
and releases the original operand after installing the join. The writer
receives the join, which remains owned by the destination. See the
[Console append ownership rule](../../docs/development/string-ownership.md#console-output-of-append-results-p0060).

## Validation

- The configured schedule uses the approved local directory. The clean attached
  checkout, single worktree, and fetch and push origin matched the project
  contract. Restricted fetch failed DNS. Approved fetch and `git ls-remote`
  verified the parent with no divergence or unpublished passes.
- P0059 [exact-parent CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44315369022)
  passed on Linux (134839928252) and Windows (134839927622).
- Zig **0.17.1**. Both the parent and final native builds passed **15/15** steps.
  The isolated parent audit passed in both modes (2.252s). The new Go regression
  detected the missing ownership emission in both parent modes (0.763s).
- Focused Go `go test -count=1 -v -run '^TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)$' .`
  passed **four groups** (21.628s). The new regression checks 10,000 appends
  per mode, 20,008 nonnull releases, and zero survivors before shutdown. It
  verifies generated C, identical checked and normally linked output, null,
  borrowed self-append, and once-only effects when a factory argument replaces
  the destination. The destination can still be updated after printing.
- `python codex/python/owned_result_audit.py` passed **28 probes** (32.914s).
  The isolated Console append reached **24 releases and zero survivors** in
  each mode. The initializer stayed at **40/0**, the complete append at
  **24/0**, and the aggregate at **7/0**. Escape and sibling controls each
  retained one original.
- `zig build test`: **180 discovered, 162 passed, 18 skipped, zero failed or
  errored** (53.92s). Report: `test_results_20261118_072040_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  **5 tests** (0.047s).
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  passed **43 groups** (120.827s, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed **15/15** steps. The checked regression also cross-linked for Linux.
- Diff review, `git diff --check`, local Markdown links and pass index, Go
  formatting, Python syntax, and the Dream report path were checked. Local
  logs are in ignored `build/P0060-*.log`.

## Limits and handoff

The rule covers only a direct append printed by a complete Console writer
statement. Nested appends, user-function arguments, and returns still use
general expression emission. General expression ownership, async lifetimes,
and exception unwinding remain open. Eighteen Dream entries remain unannotated.
Linux binaries were cross-built locally. Native Linux validation for this
commit comes from CI.

Verify publication and exact-commit CI. Then measure the original factory
allocation in `view(text += owned());` in both modes with a one-parameter user
function that prints the argument, and confirm the destination survives the call.
