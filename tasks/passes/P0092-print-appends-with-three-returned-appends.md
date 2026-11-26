# P0092 - Release three-append factory operands before Console prints an append

- Date: 2026-11-26 UTC
- Task: T011
- Parent checkpoint: `dd4c41132558c9fb238f75b75e7f75e60d51b305` (P0091)
- Commit lookup: exact `Pass-ID: P0092` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

When either Console writer printed `text += deeper()`, the third join returned
by a proven factory stayed registered until shutdown. The complete Console
statement emitter now applies the three-append proof and uses the existing
owned append helper. The factory and its arguments evaluate once. The helper
reads the current destination, installs the concatenation, and releases the
original factory join before printing. The destination keeps the printed value.

The [Dream fixture](../../tests/basics/memory/owned_triple_join_append_console.dr)
covers both writers, replacement of the destination during argument evaluation,
once-only increments, later use of the destination, and borrowed self-append.
The [Go regression](../../codex/go/indexed_strings_test.go) checks generated C,
checked allocation lifetimes, and exact output in ordinary and module-entry
compilation. The [allocation audit](../../codex/python/owned_result_audit.py)
isolates each writer. The [context matrix](../../codex/go/owned_call_argument_test.go)
now requires the owned helper for a third join and checks ordinary append
emission for a fourth join. See the
[Console append rule](../../docs/development/string-ownership.md#console-output-of-appends-with-three-returned-appends-p0092).

## Validation

- The active schedule uses the existing local checkout. The clean, attached
  branch matched both the fetch and live remote lookup for the P0091 SHA.
  Windows and Ubuntu passed at that SHA in
  [P0091 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45740317726).
- Zig 0.17.1 built the Windows baseline and patched compiler in 15/15 steps
  each. The sandboxed Zig invocation could not locate its SDK. The approved
  invocation succeeded with the installed SDK and caches.
- The focused `TestCompiler_AppendedStrings/triple_join_console_append` Go
  regression failed at P0091 as expected because both modes emitted an ordinary
  append (0.864s, `-count=1`). The baseline audit measured 64 releases and eight
  live joins after eight appends per writer and mode (7.766s).
- The focused Go regression passed after the fix in ordinary and module-entry
  modes (3.110s, `-count=1`). Each checked harness verified 22 releases of live,
  single-reference allocations and an empty registry before shutdown cleanup.
  Exact output matched in checked and normally linked Windows programs. Both
  checked harnesses cross-linked for Linux.
- `zig build test` discovered 195 Windows entries. **177 passed**, **18 skipped**,
  and zero failed or errored (71.66s). Report `test_results_20261126_010640_windows.json`.
  All 194 prior entries matched P0091 on status, passing output, and skip reason.
- The first Go quick run passed 42 groups and failed the Console context matrix
  in two modes (172.982s). The matrix had required the old ordinary-helper
  emission for the third join. After updating that assertion and adding the
  fourth-join control, `TestCompiler_OwnedCallArgument/contexts` passed in both
  modes (0.594s, `-count=1`).
- The final `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` run from `codex/go` passed all **43** groups
  (164.229s Go suite time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all **96** probes
  (112.586s). The four new writer/mode probes each made 72 releases and left
  zero final joins live after eight appends. All 92 prior measurements matched
  P0090, the last full audit.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is checked in CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.048s).
- Go formatting, 17 changed local links and anchors, all 92 pass index
  entries, diff review, and `git diff --check` passed. Logs are in
  `build/P0092-*.log`.

## Limits and handoff

Borrowed operands and calls outside the three-append proof retain ordinary
append emission. Console wrapper append arguments and direct append returns
still use the two-append proof. General expression and argument ownership,
async, and exception unwinding remain open.

Verify P0092 publication and native CI. Then measure retained factory joins in
`view(text += deeper());` for proven synchronous wrappers for each writer.
Check release before the wrapper call, once-only argument evaluation, and
output from the updated destination when an argument replaces it.
