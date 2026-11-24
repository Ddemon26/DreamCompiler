# P0086 - Discard the final join from three returned appends

- Date: 2026-11-24 UTC
- Task: T011
- Parent checkpoint: `df949be00f946d46f6369d741a1e832111f2c64c` (P0085)
- Commit lookup: exact `Pass-ID: P0086` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A standalone call to a factory with three proven returned appends discarded
the final join's pointer. The original allocation stayed registered until
shutdown. The expression-statement emitter now uses the existing factory
proof with a limit of three returned appends. After the call and its arguments
run once, the caller releases the original final join.

The [Dream fixture](../../tests/basics/memory/owned_call_discard.dr) checks
observable output. The [Go regression](../../codex/go/owned_call_discard_test.go)
checks emission for ordinary and imported definitions. The
[checked harness](../../codex/c/owned_call_discard_harness.c) verifies the
allocation identity and registry state. See the
[discard rule](../../docs/development/string-ownership.md#standalone-calls-that-discard-three-returned-appends-p0086).

## Validation

- The active schedule uses the existing local checkout. Fetch and
  `git ls-remote` matched the clean, attached P0085 parent. Both native jobs
  passed at that SHA in [P0085 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45411129575).
- The focused baseline check failed in both modes because the third join
  was still live at probe return (2.562s Go suite time). The isolated audit
  measured 48 releases and eight live joins per mode (two probes, 2.024s).
- Zig 0.17.1 built the parent and patched Windows compilers in 15/15 steps.
  The first focused run passed the discard and assignment groups. The
  initializer group's cross-consumer check still expected a bare `extended();`
  and now requires the release, with its Console and conditional checks intact.
  `go test -count=1 -v -run '^TestCompiler_OwnedCall(Assignment|Discard|Initializer)$' .`
  passed from `codex/go` on rerun (10.971s).
- The checked discard harness ran 10,000 calls per mode for each of zero to
  three returned appends. Each three-append call made four copies, three
  joins and seven releases. Both intermediate joins and the original final
  pointer were released at reference count one, leaving an empty registry.
  The argument increment occurred once. Checked and normally linked Windows
  binaries matched expected output. Checked Linux binaries cross-linked.
- `zig build test` discovered 192 entries on Windows. **174 passed**,
  **18 skipped**, and zero failed or errored (50.90s). Report
  `test_results_20261124_130246_windows.json` confirms exact discard fixture
  output and the same entry statuses and skip reasons as P0085.
- The Windows Go quick protocol from `codex/go` (`powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`) passed all 43 groups
  (131.206s Go suite time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all 80 probes (83.517s).
  Eight standalone calls in each mode made 56 releases and left zero final
  joins live. The other 78 measurements matched P0085.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is tracked in CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.057s).
- Go formatting, 14 changed local links and anchors, 86 pass index records,
  diff review, and `git diff --check` passed. Logs are in `build/P0086-*.log`.

## Limits and handoff

Loop headers, conditional expressions and nested arguments follow ordinary
emission. Four-append calls exceed the discard proof. Direct Console output,
Console wrappers and append operands still use the two-append proof. General
expression and argument ownership, async and exception unwinding remain open.

Verify P0086 publication and exact-commit Windows and Ubuntu CI. Then measure
retention in complete direct `Console.Write` and `Console.WriteLine` statements
that print the final join from three proven returned appends.
