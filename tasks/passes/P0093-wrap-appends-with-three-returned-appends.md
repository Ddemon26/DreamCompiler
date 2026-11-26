# P0093 - Release three-append factory operands before Console wrapper calls

- Date: 2026-11-26 UTC
- Task: T011
- Parent checkpoint: `28021d90f6c3847beeff045e865bd826d22a2e83` (P0092)
- Commit lookup: exact `Pass-ID: P0093` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `view(text += deeper());`, an eligible factory's third join stayed
registered until shutdown. The complete call emitter now uses the
three-append factory proof when a proven synchronous Console wrapper receives
an append to an owned mutable scalar string. The factory and its arguments
run once. The append reads the current destination, installs the concatenation,
and releases the factory's original final join before the wrapper runs. The
destination keeps the concatenation.

The [Dream fixture](../../tests/basics/memory/owned_triple_join_append_wrapper.dr)
exercises `Console.Write` and `Console.WriteLine` wrappers, destination
replacement during argument evaluation, once-only increments, subsequent
destination use, and borrowed self-append. The [Go regression](../../codex/go/indexed_strings_test.go)
checks generated C, allocation lifetimes, and exact output in ordinary and
module-entry modes. The [context matrix](../../codex/go/owned_call_argument_test.go)
checks ordinary append emission for fourth joins and wrappers outside the
proof. The [allocation audit](../../codex/python/owned_result_audit.py) isolates
each writer. See the
[wrapper append rule](../../docs/development/string-ownership.md#console-wrapper-append-arguments-with-three-returned-appends-p0093).

## Validation

- The active schedule uses the existing local checkout. Its clean, attached
  branch matched the approved fetch and live remote SHA. Both native jobs passed
  at that SHA in [P0092 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45781591416).
- Zig 0.17.1 built the Windows baseline and patched compiler in 15/15 steps
  each. The default sandbox could not locate the installed SDK. The approved
  build used the installed SDK and caches.
- `TestCompiler_AppendedStrings/triple_join_wrapper_append` failed against
  P0092 at the missing owned helper in both modes (1.088s, `-count=1`). The four
  baseline allocation probes measured 64 releases and eight live joins after
  eight appends per writer and mode (7.592s).
- The focused regression and `TestCompiler_OwnedCallArgument/contexts` passed
  in both modes (3.403s, `-count=1`). Each checked fixture counted 22 releases
  of live, single-reference allocations and verified an empty registry before
  shutdown cleanup. Checked and normally linked Windows output matched. Both
  checked harnesses cross-linked for Linux.
- `zig build test` discovered 196 Windows entries. **178 passed**, **18 skipped**,
  and zero failed or errored (59.69s). Report `test_results_20261126_070216_windows.json`.
  All 195 prior statuses, passing outputs, and skip reasons matched P0092.
- The Windows Go quick protocol passed all **43** groups with `-count=1`
  (170.493s) using `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`.
- `python codex/python/owned_result_audit.py` passed all **100** probes
  (116.527s). Each new writer and mode measured 72 releases and zero live
  joins after eight appends. All 96 prior measurements matched P0092.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is checked in CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.047s).
- Go formatting, 18 changed local links and anchors, all 93 pass index entries,
  diff review, and `git diff --check` passed. Logs are in `build/P0093-*.log`.

## Limits and handoff

Borrowed operands, fourth joins, and wrappers outside the proof use ordinary
append emission. Direct append returns still apply the two-append operand
proof. General expression and argument ownership, async, and exception
unwinding remain open.

Verify P0093 publication and native CI. Then measure retention in
`return (text += deeper());` for an owned scalar local in a synchronous string
function. Check release of the third operand join, once-only argument
evaluation, and the lifetime of the returned fourth join.
