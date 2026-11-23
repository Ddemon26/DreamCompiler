# P0081 - Release the final join from two returned appends before printing an append

- Date: 2026-11-23 UTC
- Task: T011
- Parent checkpoint: `80c3694a13f9f66555169cba28eaa083b7c49df4` (P0080)
- Commit lookup: exact `Pass-ID: P0081` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `Console.WriteLine(text += nested());` and `Console.Write(text += nested());`,
the factory's original final join stayed live until shutdown. The direct
Console statement emitter now uses the existing two-append proof for the
operand. The factory and its arguments run once. The owned-append helper
reads the current destination, installs the concatenation, and releases the
factory operand. The writer reads the installed string, which remains owned
by the destination after output.

The [Dream fixture](../../tests/basics/memory/owned_nested_join_append_console.dr)
and [Go regression](../../codex/go/indexed_strings_test.go) check both writers,
destination replacement during operand evaluation, the argument side effects,
and borrowed self-append. See the
[Console append rule](../../docs/development/string-ownership.md#console-output-of-appends-with-two-returned-appends-p0081).

## Validation

- The existing local checkout was clean and attached to the delivery branch.
  The approved fetch and `git ls-remote` matched P0080. Both native jobs passed
  at that SHA in [P0080 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45142890890).
- Zig reported 0.17.1. Native Windows builds passed 15/15 steps before and
  after the change.
- On P0080, the new Go subtest failed at the missing release helper in both
  modes (0.739s, `-count=1`). The isolated baseline audit measured 48 nonnull
  releases and eight live joins after eight calls for each writer in each
  mode (four probes, 6.679s).
- The new Go subtest passed (2.569s, `-count=1`). Each ordinary and module-entry
  checked binary made 16 nonnull releases and left zero live allocations
  before shutdown. Checked and normally linked output matched exactly.
- Focused Go `TestCompiler_(AppendedStrings|OwnedCallInitializer|OwnedCallArgument|OwnedCallAssignment)`
  passed (38.045s, `-count=1`).
- `zig build test` discovered 190 entries on Windows. **172 passed**, **18
  skipped**, and zero failed or errored (47.83s suite time). Report
  `test_results_20261123_072451_windows.json` confirms exact fixture output and
  matching statuses and skip reasons for all 189 previous entries.
- Windows Go quick (`powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`) passed all 43 groups
  (129.001s wall time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all 70 probes (62.467s).
  Eight calls to each writer made 56 nonnull releases and left zero factory
  joins live before shutdown in each mode. All 66 earlier measurements matched.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed all 15 steps. The new allocation probes also cross-linked for Linux.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.031s).
- Go formatting, 12 added local links and anchors, all 81 index records, diff
  review, and `git diff --check` passed. Logs are in `build/P0081-*.log`.

## Limits and handoff

The two-append proof here applies to an append sent directly to either Console
writer. Appends passed to wrappers, general expression and argument ownership,
async, and exception unwinding still need qualification. Native Linux execution
for this pass is tracked in CI.

Verify P0081 publication and both native CI jobs. Then measure retention in
`view(text += nested());` for wrappers proven to call either Console writer.
Qualify the two-append operand and check that the destination stays readable
after the wrapper returns.
