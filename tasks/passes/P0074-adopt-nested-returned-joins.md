# P0074 - Adopt the final join from two returned appends

- Date: 2026-11-21 UTC
- Task: T011
- Parent checkpoint: `6f5bd7427fe78e84513321820dd35e8f336aaad3` (P0073)
- Commit lookup: exact `Pass-ID: P0074` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A direct scalar initializer copied the final result of a function returning
`return (local += joined());` even when both functions met the ownership checks
and released their append operands. The copied value was released at scope exit
while the original final join stayed live. The declaration emitter now applies
the existing factory proof with a limit of two returned appends. Scope cleanup
releases the final allocation once.

The [Dream fixture](../../tests/basics/memory/owned_call_initializer.dr),
[Go regression](../../codex/go/owned_call_initializer_test.go), and
[checked harness](../../codex/c/owned_call_initializer_harness.c) verify
scope cleanup of the final join in ordinary and imported calls. The
[allocation audit](../../codex/python/owned_result_audit.py) measures
retention in eight consecutive calls. See the
[ownership rule](../../docs/development/string-ownership.md#scalar-initializers-that-adopt-a-join-of-a-returned-join-p0074).

## Validation

- The active schedule uses the existing checkout. Fetch and `git ls-remote`
  matched the clean, attached P0073 parent. The exact-commit Windows and Ubuntu
  jobs passed in [P0073 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44907693639).
- Zig version was **0.17.1**. The Windows build passed **15/15** steps.
- On P0073 the checked initializer failed in both modes when cleanup passed
  the address of a copy while the original was still live (9.643s, `-count=1`).
  The two isolated parent probes made 40 releases and left eight live joins
  per mode (2.046s).
- Focused Go `go test -count=1 -v -run
  '^TestCompiler_(OwnedCallInitializer|AppendedStrings)$' .` passed (29.915s).
  Each mode checked 10,000 scoped declarations, with three copies, two joins,
  five releases and an empty registry per call. The final release matched the
  original pointer at reference count one. The existing append-return fixture
  kept exact output and 49 releases while live allocations fell from ten to two.
  Generated C checks cover the fallback for three appends and verify assignment,
  standalone, Console and conditional consumers of a two-append result in both modes.
- `python codex/python/owned_result_audit.py` passed **56** probes (56.143s).
  The eight calls made 40 releases in each mode and left zero live allocations.
  All 54 pre-existing probe measurements matched P0073.
- `zig build test` discovered 187 entries on Windows. **169 passed**, **18 skipped**,
  and zero failed or errored (50.34s). Report `test_results_20261121_120323_windows.json`
  confirms both fixtures' exact output and the same skip paths and reasons as P0073.
- Windows Go quick from `codex/go` (`powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`) passed all **43** groups (123.073s,
  `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed **15/15** steps. Native Linux execution is checked separately by CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed **5/5** (0.043s).
- Diff review, `git diff --check`, Go formatting and all 14 added local links
  and anchors checked. The pass index resolves all 74 records. Logs are in
  `build/P0074-*.log`.

## Limits and handoff

The proof covers direct scalar initializers with at most two returned appends.
Other consumers use their existing ownership rules. General expression and
argument ownership, async and exception unwinding remain open. Verify P0074
publication and native CI, then measure retention in a complete assignment to
an owned scalar from the same two-append return. Check original-pointer identity
and replacement ordering if assignment can adopt it.
