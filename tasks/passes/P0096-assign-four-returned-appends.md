# P0096 - Adopt four returned appends in complete scalar assignments

- Date: 2026-11-27 UTC
- Task: T011
- Parent checkpoint: `cbab66b23f5a362c2c285fd6ad0a85b64dba8f3d` (P0095)
- Commit lookup: exact `Pass-ID: P0096` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete `target = fourth();` statement copied the final result of a proven
four-append factory and left the original join registered until shutdown. The
statement now uses the existing four-append proof. The call and its arguments
evaluate once. A temporary holds the original join while the assignment releases
the current destination, including a replacement installed by an argument.
The destination owns the original fourth join after the assignment.

The [Dream fixture](../../tests/basics/memory/owned_call_assignment.dr) checks
output and the argument count. The
[Go regression](../../codex/go/owned_call_assignment_test.go) checks emitted C,
ordinary and imported calls, and copy behavior in other contexts. The
[lifetime harness](../../codex/c/owned_call_assignment_harness.c) tracks original
pointer identities and verifies release order and reference counts. See the
[assignment rule](../../docs/development/string-ownership.md#complete-assignments-that-adopt-four-returned-appends-p0096).

## Validation

- The active automation uses the existing local checkout. The clean, attached
  branch matched the approved fetch. Both [P0095 CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/45927057532)
  passed at the parent SHA.
- Zig 0.17.1 and re2c 3.0 built the Windows baseline and patched compiler in
  15/15 steps each.
- The new Go lifetime assertions failed at P0095 because both modes emitted
  the copying assignment (0.683s, `-count=1`). The isolated baseline audit
  measured 73 releases and eight live original joins after eight assignments in
  each mode (2.543s). The first audit attempt expected 80 releases. The probe
  uses one destination outside the loop, so it releases that slot on eight
  replacements and at one scope exit, in addition to 64 factory releases.
- `go test -count=1 -v -run '^TestCompiler_OwnedCall(Assignment|Initializer)$' .`
  passed in 12.485s. In each mode, 10,000 invocations each checked two calls,
  12 copies, eight joins, 20 releases, a peak of seven live allocations, and an
  empty registry after scope exit. The harness verified that argument evaluation
  releases the first original join, the outer assignment releases the argument's
  replacement after the call, and scope cleanup releases the second original join.
  Checked and normally linked Windows output matched. Checked Linux binaries cross-linked.
- `zig build test` discovered 197 Windows entries. **179 passed**, **18 skipped**,
  and zero failed or errored (68.22s). Report `test_results_20261127_010426_windows.json`.
  The assignment fixture printed the fourth join and its argument count.
  The other 196 entries matched P0095's status, output, and skip reason.
- The Windows Go quick protocol passed all **43** groups with `-count=1`
  (178.724s), using `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`.
- `python codex/python/owned_result_audit.py` passed all **106** probes
  (124.685s). Eight assignments per mode left zero original joins live, down
  from eight at P0095, with 73 releases in both runs. All 104 previous
  measurements matched.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.049s).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution awaits CI.
- Go formatting, 19 local links and anchors, all 96 pass index entries, diff
  review, and `git diff --check` passed. Logs are in `build/P0096-*.log`.

## Limits and handoff

The rule applies to a complete assignment to an owned scalar string identifier.
Factories with five returned appends and assignments nested in expressions or
used as `for` updates continue to copy the result. General expression and argument
ownership, async, and exception unwinding remain open.

Verify P0096 publication and both exact-commit CI jobs. Then measure retention
from a standalone `fourth();` call and check once-only argument evaluation and
original-pointer release in ordinary and imported calls.
