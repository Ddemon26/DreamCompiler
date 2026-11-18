# P0057 - Release proven factory results after direct Console output

- Date: 2026-11-18 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `a6d7ec365415f4f0c56ea5efce5999e45d4b1350`
- Commit lookup: exact `Pass-ID: P0057` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

The parent emitted `dr_console_write(factory())` and
`dr_console_writeln(factory())` with no release of the transferred reference.
The isolated probes left eight live originals after eight calls to either
writer, with no nonnull releases, in ordinary and module-entry compilation.

The C emitter now captures a factory argument once when it is the entire
operand of a complete `Console.Write` or `Console.WriteLine` statement and
passes the existing `adopts_string_call` proof. The appropriate runtime writer
reads that allocation synchronously. The emitter releases the original pointer
after the writer returns normally. The statement-local block shares temporary
name selection and release emission with the P0056 wrapper rule. See the
[ownership contract](../../docs/development/string-ownership.md#direct-console-output-of-proven-factory-results-p0057).

## Validation

- The approved checkout started clean, attached to `codex/compiler-stabilization`,
  with one registered worktree and matching fetch and push origin URLs. The
  schedule already targets this directory. Restricted fetch failed DNS. The
  approved fetch and `git ls-remote` verified the parent branch tip, with zero
  unpublished commits or divergence.
- Both P0056 [exact-parent CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/44259186333)
  passed natively on Windows (134648097187) and Linux (134648097602).
- Zig **0.17.1** on Windows. `zig build --summary all`: **15/15** steps succeeded.
- The parent ownership audit passed **22 probes** (37.008s) with zero releases
  and eight live originals for each isolated writer in each mode. The expanded
  Go test reproduced the missing release blocks in both modes at the parent.
  Two proposed expression-context controls also failed on the parent because
  Console output in those positions uses unsupported expression lowering.
  The final test checks a supported nested concatenation operand instead.
- From `codex/go`, `go test -count=1 -v -run 'TestCompiler_(OwnedCallArgument|OwnedCallDiscard|IndexedConsole)$' .`:
  **three groups passed** (7.009s). The identity harness checks **10,000 calls
  per direct writer plus 10,000 wrapper calls per mode** at reference count one,
  peak live count one and zero survivors. Generated C, an imported factory,
  temporary collisions, a once-only argument effect, unbraced statements,
  exact checked and normally linked output, fallback shapes, and Windows-to-Linux
  harness cross-linking passed.
- `python codex/python/owned_result_audit.py`: **22 probes passed** (26.942s).
  Each isolated writer reached **8 releases/0 live** in each mode. The aggregate
  reached **6/1**. Static-field escape and sibling-argument probes stayed at
  **0/1**. The audit checks generated C, exact native UBSan and normally linked
  output, and Linux harness cross-links.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed** (0.052s).
- `zig build test`: **177 discovered, 159 passed, 18 skipped, zero failed
  or errored** (57.21s). Report: `test_results_20261118_044750_windows.json`.
- The first Windows Go quick run passed 42 groups and failed the initializer
  group's `other_consumers` assertion in both modes (126.797s). The fixture
  expected direct Console output without a release. Its revised assertion
  checks the exact capture, print and release sequence. A focused rerun of
  `TestCompiler_OwnedCallInitializer` passed (6.568s, `-count=1`).
- From `codex/go`, the required `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  rerun passed **43 groups** (124.747s, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **15/15** steps succeeded. Linux binaries were cross-built on Windows.
- Reviewed the diff and checked **264 local Markdown links**, 57 consecutive
  pass IDs, Go formatting, Python syntax, matching quick/CI selectors and the
  PowerShell script BOM. `git diff --check` passed. Logs are in ignored
  `build/P0057-*.log`.

## Limits and handoff

Null is checked at emission. The writer's `%s` formatting sets the existing
runtime boundary. Borrowed and unproven factories, nested operands, general
argument escapes, async lifetime, and exception unwinding remain separate.
Eighteen unannotated Dream entries remain skipped.

After verifying publication and exact-commit CI, isolate `text += owned()` on
an owned scalar string identifier in both modes and audit the helper's read
and replacement order. Its factory result accounts for the aggregate survivor.
