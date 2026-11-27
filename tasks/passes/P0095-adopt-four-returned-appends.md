# P0095 - Adopt four returned appends in scalar initializers

- Date: 2026-11-27 UTC
- Task: T011
- Parent checkpoint: `ae33455e0e8d07c823a5d4b9b0814ed2ac472c46` (P0094)
- Commit lookup: exact `Pass-ID: P0095` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `string snapshot = fourth();`, the initializer copied the fourth join and
left the original allocation registered until shutdown. The direct scalar
initializer now applies the existing factory proof through four returned
appends. Each append consumes its operand, and the caller adopts the original
fourth join and releases it at scope exit. The call and its arguments evaluate
once. Lowered imported definitions go through the same proof.

The [Dream fixture](../../tests/basics/memory/owned_call_initializer.dr) covers
const declarations, `for` header cleanup after `break`, and an argument that
increments a counter. The [Go regression](../../codex/go/owned_call_initializer_test.go)
checks direct adoption in generated C and exact Windows output with checked and
ordinary linking. The [C harness](../../codex/c/owned_call_initializer_harness.c)
checks original pointer identity and counts live allocations for the ordinary
and imported calls. See the
[initializer rule](../../docs/development/string-ownership.md#scalar-initializers-that-adopt-four-returned-appends-p0095).

## Validation

- The active schedule uses `C:/Users/damon/cApps/DreamCompiler`. The clean,
  attached branch matched the approved fetch. Both [P0094 CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/45888665473)
  passed at that SHA.
- Zig 0.17.1 and re2c 3.0 built the Windows baseline and patched compiler in
  15/15 steps each. `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux
  --summary all` passed 15/15 steps. Native Linux execution awaits CI.
- The new Go lifetime checks failed against P0094 because both modes emitted
  the caller copy (`-count=1`, 0.584s). The isolated baseline allocation audit
  counted 72 releases and eight live original joins after eight initializers in
  each mode (5.890s).
- `go test -count=1 -v -run '^(TestCompiler_OwnedCallInitializer|TestCompiler_AppendedStrings)$' .`
  passed (67.475s). In each mode, 10,000 calls checked once-only evaluation,
  50,000 copies for factory storage, 40,000 joins, 90,000 releases, and an empty
  registry after each scope exit. The harness releases the original fourth
  join at reference count one. The five-append fallback still copies its
  result. The previous append-return fixture recorded the same 32 releases
  and five surviving allocations, down from six at P0094.
- `zig build test` discovered 197 Windows entries. **179 passed**, **18 skipped**,
  and zero failed or errored (64.94s). Report `test_results_20261126_190729_windows.json`.
  The initializer fixture printed the fourth join and its argument count.
  The other 196 entries matched P0094's status, output, and skip reason.
- The Windows Go quick protocol passed all **43** groups with `-count=1`
  (170.749s) using `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`.
- `python codex/python/owned_result_audit.py` passed all **104** probes
  (129.14s). Eight initializers in each mode made 72 releases and left zero
  original allocations live, down from eight at P0094. All 102 prior
  measurements matched.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.047s).
- Go formatting, 17 local links and anchors, all 95 pass index entries,
  diff review, and `git diff --check` passed. Logs are in `build/P0095-*.log`.

## Limits and handoff

Calls that fail the proof, including factories with five returned appends,
and conditional initializers use the existing copy path. Fourth joins used
as assignment, discard, Console, or append operands remain candidates for
separate qualification. General expression and argument ownership, async,
and exception unwinding remain open.

Verify P0095 publication and native CI, then measure `target = fourth();`
with an owned scalar target. Check original allocation identity and evaluate
the arguments before releasing the destination's current allocation.
