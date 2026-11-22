# P0075 - Adopt the final join from two returned appends in a scalar assignment

- Date: 2026-11-22 UTC
- Task: T011
- Parent checkpoint: `5e4d598570a64e4a36ee23cb010a62bf688d7740` (P0074)
- Commit lookup: exact `Pass-ID: P0075` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete scalar string assignment copied the final allocation of a factory
that returned two proven appends. The original stayed live until shutdown.
Complete assignments to an already-owned scalar string now use the two-append
factory proof from direct declarations. The existing statement-local temporary
captures the right-hand side once. After evaluation, the assignment releases
the current destination and installs the original final pointer.

The [Dream fixture](../../tests/basics/memory/owned_call_assignment.dr),
[Go regression](../../codex/go/owned_call_assignment_test.go), and
[checked harness](../../codex/c/owned_call_assignment_harness.c) verify original
allocation identities and replacement order in ordinary and imported calls.
The [allocation audit](../../codex/python/owned_result_audit.py) isolates the
retention change. See the
[assignment rule](../../docs/development/string-ownership.md#complete-assignments-that-adopt-two-returned-appends-p0075).

## Validation

- The existing local schedule targets the approved checkout. Fetch and
  `git ls-remote` matched the clean, attached P0074 parent. Native Windows and
  Ubuntu passed at that SHA in [P0074 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44939209746).
- Zig version was **0.17.1**. The native Windows build and
  `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  each passed **15/15** steps.
- On P0074 the new lifetime regression failed the exact release-order check in
  both modes (5.298s, `-count=1`). The isolated eight-call audit made 48 releases
  and left eight final joins live per mode (2.164s).
- Focused Go `go test -count=1 -v -run
  '^TestCompiler_(OwnedCallAssignment|OwnedCallInitializer)$' .` passed (13.047s).
  Each mode checked 10,000 scopes with two assignments per scope, including
  RHS argument replacement. Each scope made eight copies, four joins and 12
  releases at reference count one, with five peak live allocations and zero
  survivors. Generated C preserved copying for assignment expressions and
  assignments of results beyond two returned appends.
- `python codex/python/owned_result_audit.py` passed **58** probes (63.806s).
  The eight assignments made 48 releases in each mode and left zero allocations
  live. All 56 pre-existing measurements matched P0074.
- `zig build test` discovered 187 entries on Windows. **169 passed**, **18 skipped**,
  and zero failed or errored (56.26s). Report `test_results_20261121_180340_windows.json`
  confirms the assignment fixture's exact output and the same skip paths and
  reasons as P0074.
- Windows Go quick from `codex/go` (`powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`) passed all **43** groups (133.490s,
  `-count=1`).
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed **5/5** (0.032s).
- Go formatting, diff review, `git diff --check`, 16 added local links and
  anchors, and all 75 pass index entries checked. Logs are in `build/P0075-*.log`.

## Limits and handoff

The proof applies to complete assignments with at most two returned appends.
Nested assignment expressions keep their copying behavior. Standalone and
argument calls retain the one-append proof. General argument ownership,
async and exception unwinding remain open. Verify P0075 publication and native
CI, then measure final-result retention in a standalone call with two proven
returned appends and check release identity and once-only argument evaluation.
