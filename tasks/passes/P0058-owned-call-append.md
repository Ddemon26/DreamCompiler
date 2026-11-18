# P0058 - Release proven factory operands after owned string append

- Date: 2026-11-18 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `03a5c817cc5f579a34faecf79004bf0379832749`
- Commit lookup: exact `Pass-ID: P0058` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

The parent passed the original pointer from `text += owned();` to
`__dream_string_append` and left that pointer live until shutdown. Eight
isolated appends produced 16 nonnull releases and left eight original
factory allocations live in each compilation mode.

A complete append statement now captures the entire operand once when it
passes the existing `adopts_string_call` proof and the destination is a known-owned
scalar string identifier. The helper reads the current destination after the
factory finishes. It copies both strings with `dream_concat`, releases the former
destination and stores the joined result. The statement then releases the
factory original. The temporary uses the existing collision checks. Null remains
an empty operand and `dr_release(NULL)` is a no-op. See the
[ownership rule](../../docs/development/string-ownership.md#append-of-proven-factory-results-p0058).

## Validation

- The active schedule already targets the existing local project. It started on
  the attached integration branch with a clean checkout, one worktree, and
  matching fetch and push URLs. Restricted fetch failed DNS. The approved
  fetch and `git ls-remote` verified the parent tip with zero unpublished commits
  or divergence.
- Both P0057 [exact-parent CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/44298800009)
  passed natively on Windows (134782744726) and Linux (134782745116).
- Zig **0.17.1**. Native `zig build --summary all` passed **15/15** steps.
- The isolated parent audit measured **16 releases and eight survivors** in
  each mode (two probes, 12.134s). The new Go regression failed against the
  parent at the missing generated release block in both modes.
- Focused Go `go test -count=1 -v -run '^TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)$' .`
  passed **four groups** (17.051s). The new subtest checks 10,000 repeated
  appends in each mode, 30,012 nonnull releases per mode, and no survivors
  before shutdown. It checks exact emitted C and output, a factory argument
  that replaces the destination and increments a counter once, unbraced
  control flow, null and borrowed operands, temporary name collision, and
  Windows-to-Linux harness cross-linking. The initializer group still checks
  nested append emission. During test development, the first focused run
  exposed debug lines between the condition and release block. The next run
  exposed pointer comparison in the fixture's string equality guards. The
  final fixture compares string content through exact output, including the
  managed empty result between brackets.
- `python codex/python/owned_result_audit.py` passed **24 probes** (28.084s).
  The isolated append reached **24 releases and zero survivors** in each mode.
  The aggregate reached **seven releases and zero survivors** in each mode.
  Static-field escape and sibling-argument probes each remained at zero releases
  and one survivor. The native checked harness and normal linked output matched,
  and the Linux harnesses cross-linked.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  **5 tests** (0.044s).
- `zig build test`: **178 discovered, 160 passed, 18 skipped, zero failed or
  errored** (59.72s). Report: `test_results_20261118_060712_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  passed **43 groups** (131.950s, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed **15/15** steps. This was a Windows-to-Linux cross-build.
- Diff review and `git diff --check` passed. Verified 247 local Markdown
  links, 58 indexed passes, Go formatting, Python syntax, matching quick
  and CI selectors, and the PowerShell BOM. Logs are in ignored `build/P0058-*.log`.

## Limits and handoff

Nested append expressions, unproven factory results, general argument
consumption, async lifetime, and exception unwinding remain open.
Eighteen unannotated Dream entries remain skipped.

Verify publication and exact-commit CI. Then isolate
`string snapshot = (text += owned());` in both modes and measure the factory
original while checking that the destination and snapshot have independent storage.
