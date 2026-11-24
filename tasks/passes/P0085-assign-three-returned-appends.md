# P0085 - Adopt the final join from three returned appends in a complete scalar assignment

- Date: 2026-11-24 UTC
- Task: T011
- Parent checkpoint: `060f10f37636c739066e51859ed210863f07edc9` (P0084)
- Commit lookup: exact `Pass-ID: P0085` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete `=` assignment to an owned scalar string copied the final join of
a factory with three proven returned appends and left the original registered
until shutdown. The assignment now asks the existing factory proof to consider
three appends. A statement-local temporary holds the original pointer while
the assignment releases the current destination. This covers both an argument
that reads the destination and an argument that installs a replacement before
the call. Ordinary and imported calls use the same proof.

The [Dream fixture](../../tests/basics/memory/owned_call_assignment.dr) checks
output. The [Go regression](../../codex/go/owned_call_assignment_test.go)
checks emission and the [lifetime harness](../../codex/c/owned_call_assignment_harness.c)
checks exact allocation identities and release order. See the
[assignment rule](../../docs/development/string-ownership.md#complete-assignments-that-adopt-three-returned-appends-p0085).

## Validation

- The clean, attached checkout, approved fetch, and remote SHA matched P0084.
  The active automation targets the existing local project directory.
  Exact-commit Windows and Ubuntu jobs passed in
  [P0084 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45353940033).
- Zig reported 0.17.1. The parent and patched Windows builds passed 15/15 steps.
- The baseline audit measured 64 releases and eight final joins still registered
  after eight calls per mode (two probes, 5.455s). The focused baseline Go check
  failed in both modes because the expected adoption was missing from generated C.
- `go test -count=1 -v -run '^TestCompiler_OwnedCall(Assignment|Initializer)$' .`
  passed from `codex/go` (10.967s). Both modes ran 10,000 calls to the new
  assignment probe, with ten copies, six joins, 16 releases, a peak of six
  live allocations and an empty registry after each call. The paired assignments
  released the first original join during argument evaluation, the newly installed
  value after the call, and the second original join at scope exit. Checked and
  normally linked Windows binaries matched expected output. Checked Linux binaries
  cross-linked.
- `zig build test` discovered 192 entries on Windows. **174 passed**, **18 skipped**,
  and zero failed or errored (57.48s). Report
  `test_results_20261124_070517_windows.json` confirms exact assignment fixture
  output and the same entry statuses and skip reasons as P0084.
- The Windows Go quick protocol from `codex/go` (`powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`) passed all 43 groups
  (156.826s Go suite time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all 78 probes (92.004s).
  Eight calls per mode made 64 releases and left zero final joins live. The other
  76 probe results matched P0084.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.051s).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps.
- Go formatting, 14 changed local links and anchors, 85 pass index records,
  diff review, and `git diff --check` passed. Logs are in `build/P0085-*.log`.

## Limits and handoff

The proof applies to complete assignments to owned scalar string identifiers.
Assignment expressions and four-append calls keep their prior emission.
Standalone calls, Console calls and append operands use a proof bounded at two
returned appends. General expression and argument ownership, async, and exception
unwinding remain open. Native Linux execution is tracked in CI.

Verify P0085 publication and exact-commit Windows and Ubuntu CI. Then measure
retention after a standalone call to a factory with three proven returned appends.
If the discard qualifies for release, check original-pointer identity and once-only
argument evaluation in ordinary and imported calls.
