# P0066 - Release proven returned joins after standalone calls

- Date: 2026-11-20 UTC
- Task: T011
- Parent checkpoint: `d0bebc061335ed1fea6bd54ecdfc675b80156492` (P0065)
- Commit lookup: exact `Pass-ID: P0066` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A standalone `joined();` left the returned join registered until shutdown.
The expression-statement emitter now uses the same two-statement factory
proof as scalar declarations and complete assignments. The generated C
evaluates the call once and passes the original join to `dr_release`.

The [Go regression](../../codex/go/owned_call_discard_test.go) and
[checked harness](../../codex/c/owned_call_discard_harness.c) cover
ordinary and imported module calls. The annotated Dream fixture adds one
output line for the counter increment in the discarded call's argument.
The initializer matrix now requires a release at its standalone join call
site and keeps its existing assertions for the surrounding expressions.
See the [ownership rules](../../docs/development/string-ownership.md#standalone-calls-that-discard-returned-joins-p0066).

## Validation

- Fetch and `git ls-remote` matched clean, attached P0065. Both native CI jobs
  passed at the exact parent SHA. See the
  [P0065 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44595800169).
- The focused baseline check failed the original-join lifetime assertion in
  both modes (15.522s). The baseline audit passed all 38 probes (67.174s),
  with 16 releases and eight surviving joins per mode after eight discarded
  calls. The initial sandboxed check hit a Zig installation lookup failure.
  The approved local rerun accessed the SDK and reproduced the retention.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed all five tests (0.065s).
- The first post-fix Go run passed the discard checks. The initializer
  matrix still expected a bare standalone call and failed that assertion in
  both modes (26.966s). The regression now requires the direct release.
- The final focused Go run passed discard, initializer, assignment, and
  argument groups (26.331s, `-count=1`). Ordinary and imported module calls
  each passed 10,000 checked discards. Each call evaluated the argument once,
  made two operand copies and one join, released the original join at
  reference count one after its operands, and emptied the registry. Checked
  and normally linked output matched. The harness also cross-linked for
  Linux, and generated-C checks covered headers, conditional and nested
  expressions, and an unproven append operand.
- `python codex/python/owned_result_audit.py` passed all 38 probes (64.188s).
  Eight discarded calls made 24 releases with zero surviving joins in each
  mode. The other 36 measurements matched the baseline.
- `zig build test` discovered 182 entries on Windows. 164 passed, 18 skipped
  and zero failed or errored (102.63s). Report
  `test_results_20261119_171916_windows.json` confirms the discard fixture
  passed with exact output and all 18 unannotated skip paths match P0065.
- Windows Go quick, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`, passed all 43 groups
  (207.748s Go result, `-count=1`).
- With Zig **0.17.1**, `zig build --summary all` and
  `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps each. Native Linux execution is verified separately by CI.
- Reviewed the diff and verified `git diff --check`, 14 local links and
  anchors, and the unique pass index entry. Logs are in ignored
  `build/P0066-*.log`.

## Limits and handoff

The proof covers a standalone call to a synchronous free function with one
qualifying local and a direct append return using a proven factory operand.
Nested and compound expressions, unproven operands, other argument consumers,
async, and exception unwinding still require separate qualification. Verify
publication and both native CI jobs, then measure the join lifetime in a complete
`Console.WriteLine(joined());` statement in ordinary and module-entry modes.
