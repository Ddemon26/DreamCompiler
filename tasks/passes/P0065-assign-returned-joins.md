# P0065 - Adopt proven returned joins in scalar string assignments

- Date: 2026-11-19 UTC
- Task: T011
- Parent checkpoint: `33cda09a4fae126dfc3ef9fc96aa8487791dc96a` (P0064)
- Commit lookup: exact `Pass-ID: P0065` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `target = joined();`, the caller previously copied the joined value and
left the original allocation registered until shutdown. A complete assignment
to an already-owned scalar string now reuses the P0064 proof for returned
joins. The existing assignment emitter evaluates the call once into a local
temporary, releases the current destination and installs the original join.

The [Go regression](../../codex/go/owned_call_assignment_test.go) and
[checked harness](../../codex/c/owned_call_assignment_harness.c) cover ordinary
and imported module calls. A pair of assignments uses the old slot as a
source, then replaces the slot from inside the next call's argument. The
harness checks the exact original allocations, release order, reference count
one and the empty registry. See the [ownership rules](../../docs/development/string-ownership.md#complete-assignments-that-adopt-returned-joins-p0065).

## Validation

- The existing local checkout was clean and attached to the approved branch.
  Fetch and `git ls-remote` matched P0064, and both native jobs passed at
  that SHA. See the [P0064 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44530574682).
- The unchanged compiler failed the new pointer-order assertion in both
  modes (9.685s). The baseline audit passed all 36 probes (31.667s), with
  32 releases and eight surviving joins per mode in the isolated assignment.
- Focused Go assignment, initializer and append regressions passed (19.207s,
  `-count=1`). Ordinary and imported module calls each ran 10,000 scopes
  with two assignments. Each scope recorded six copies, two original joins,
  eight releases in the expected order, a peak of four live allocations, and
  an empty registry. Checked harnesses cross-linked for Linux. The C fallback
  checks covered for-header, initializer, chained-assignment and borrowed
  parameter contexts, plus the existing unproven factory cases.
- `python codex/python/owned_result_audit.py` passed all 36 probes (31.375s).
  The eight isolated assignments made 32 releases and left zero survivors in
  each mode. Checked and normally linked programs produced the same expected text.
- Windows native and Linux cross-builds passed 15/15 steps each with Zig
  **0.17.1**. Python runner unit tests passed 5/5 (0.036s).
- `zig build test` discovered 182 entries. 164 passed, 18 skipped and zero
  failed or errored (41.04s). The report is
  `test_results_20261119_120914_windows.json`. It confirms the assignment
  fixture passed and the 18 unannotated skip paths match P0064.
- The Windows Go quick protocol from `codex/go`,
  `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`,
  passed all 43 groups (108.450s Go result, `-count=1`).
- Reviewed the diff and verified `git diff --check`, 14 local links and
  anchors, and the unique pass index entry. Logs are in ignored
  `build/P0065-*.log`.

## Limits and handoff

The returned-join proof covers one direct append with a proven operand.
General assignment expressions, discards, arguments, direct Console output,
async and exception unwinding still require separate qualification. Verify
publication and both native CI jobs, then measure original join retention
for a complete discarded `joined();` in ordinary and module-entry modes.
