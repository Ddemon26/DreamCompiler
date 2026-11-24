# P0084 - Adopt the final join from three returned appends in a direct scalar initializer

- Date: 2026-11-24 UTC
- Task: T011
- Parent checkpoint: `196f12c48a8e76621ad733909b9802ff97433b85` (P0083)
- Commit lookup: exact `Pass-ID: P0084` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A direct scalar initializer copied the final join from a factory with three
proven returned appends, leaving the original registered until shutdown. The
declaration now asks the existing factory proof to consider three appends.
The outermost returned append already consumes its two-append operand under
P0083. The declaration takes the returned allocation and scope cleanup releases
that original reference. The same proof checks lowered imports.

A const declaration checks an observable argument side effect. The regression
checks generated C and exact output in both modes, as well as fallback for four
appends and for three appends in conditional declarations, assignments, discards,
and Console output. The harness checks the original join's release, allocation
counts, and registry after each call. See the
[Dream fixture](../../tests/basics/memory/owned_call_initializer.dr),
[Go regression](../../codex/go/owned_call_initializer_test.go),
[pointer harness](../../codex/c/owned_call_initializer_harness.c), and
[initializer rule](../../docs/development/string-ownership.md#scalar-initializers-that-adopt-three-returned-appends-p0084).

## Validation

- The active automation targets the existing local checkout. The clean, attached
  HEAD, approved fetch, and `git ls-remote` matched P0083. The exact-commit Windows
  and Ubuntu jobs passed in [P0083 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45309986340).
- Zig reported 0.17.1. Parent and patched Windows builds passed all 15 steps.
- The P0083 allocation audit measured 56 nonnull releases and eight final joins
  still registered after eight calls in each mode (two probes, 7.345s). The new
  Go lifetime harness failed against P0083 because cleanup released a copy while
  the original join was live, in both ordinary and imported modes (3.187s).
- `go test -count=1 -v -run '^TestCompiler_OwnedCallInitializer$' .` passed
  from `codex/go` (9.477s). Each mode ran 10,000 triple-append initializers with
  four copies, three joins, and seven releases per call, and an empty registry
  after each call. Checked and normally linked binaries produced exact output,
  including the argument effect once. Both checked binaries cross-linked for Linux.
- The first Windows Go quick run failed in both modes of `nested_join_return`
  because it expected a caller copy. The fixture now requires direct adoption
  and four live allocations instead of five. Its exact output and expected 25
  releases still pass. The focused Go subtest `TestCompiler_AppendedStrings/nested_join_return`
  passed in both modes (4.403s, `-count=1`).
- The Windows Go quick rerun from `codex/go` (`powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`) passed all 43 groups
  (171.245s Go suite time, `-count=1`).
- `zig build test` discovered 192 entries on Windows. **174 passed**, **18
  skipped**, and zero failed or errored (71.17s). Report
  `test_results_20261124_010058_windows.json` confirms exact fixture output,
  all 192 entry statuses, and all 18 skip reasons from P0083.
- `python codex/python/owned_result_audit.py` passed all 76 probes (113.453s).
  Eight calls in each mode made 56 nonnull releases and left zero final joins
  registered. The other 74 measurements matched P0083.
- `python -m unittest discover -s codex/python -p test_test_manager.py` passed
  all five tests (0.066s).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps.
- Go formatting, 15 changed local links and anchors, all 84 pass index records,
  diff review, and `git diff --check` passed. Logs are in `build/P0084-*.log`.

## Limits and handoff

The three-append proof applies to direct scalar call initializers. Four-append
factories and compound initializers still use copies. Qualified assignments,
discards, Console calls, and append operands keep their two-append limit.
General expression and argument ownership, async, and exception unwinding
remain open. Native Linux execution is tracked in CI.

Verify P0084 publication and both exact-commit CI jobs. Then measure retention
in a complete assignment to an owned scalar string when its factory returns
three proven appends. If adoption qualifies, check original-pointer identity
and a destination replacement during argument evaluation.
