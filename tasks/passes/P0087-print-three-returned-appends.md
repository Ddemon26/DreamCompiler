# P0087 - Release the final join from three returned appends after direct Console output

- Date: 2026-11-25 UTC
- Task: T011
- Parent checkpoint: `428fe702006a8e90f9e7d6c2afe0d9a009728650` (P0086)
- Commit lookup: exact `Pass-ID: P0087` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Complete `Console.Write` and `Console.WriteLine` statements printed the third
returned join and left the original allocation registered until shutdown. The
statement emitter now uses the existing three-append factory proof for a call
that is the entire operand of a direct writer. It evaluates the call and its
arguments once and releases the original pointer after the writer returns.

The [Dream fixture](../../tests/basics/memory/owned_call_argument.dr) checks
exact output and argument side effects. The
[Go regression](../../codex/go/owned_call_argument_test.go) checks emitted C
and runs the [checked harness](../../codex/c/owned_call_argument_harness.c)
for ordinary and imported definitions. The
[Console rule](../../docs/development/string-ownership.md#direct-console-output-of-three-returned-appends-p0087)
describes the ownership boundary.

## Validation

- The active schedule uses the existing local checkout. Fetch and
  `git ls-remote` matched the clean, attached P0086 parent. Both native jobs
  passed at that SHA in [P0086 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45477398152).
- Zig 0.17.1 built the parent and patched Windows compilers in 15/15 steps.
  The parent omitted both release blocks in each focused lifetime case. The
  isolated allocator baseline measured 48 releases and eight live final
  joins per writer per mode after eight calls (four probes, 7.112s).
- From `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_OwnedCall(Argument|Initializer|Discard)$' .` passed (11.941s).
  The checked harness ran each writer 10,000 times per mode. Each factory
  call made four copies, three joins and seven releases. The writer observed
  the original final pointer as the sole live allocation at reference count
  one, then the caller released it. The argument increment occurred once.
  Checked and normally linked Windows binaries matched exact output. Checked
  Linux binaries cross-linked.
- `zig build test` discovered 192 entries on Windows. **174 passed**,
  **18 skipped**, and zero failed or errored (52.48s). Report
  `test_results_20261124_190441_windows.json` confirms exact Console fixture
  output and the same entry statuses and skip reasons as P0086.
- From `codex/go`, the Windows quick protocol (`powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`) passed all
  **43** groups (140.193s Go suite time, `-count=1`).
- `python codex/python/owned_result_audit.py` passed all **84** probes
  (101.143s). Each new eight-call probe made 56 releases and left zero
  final joins live. All 80 previous measurements matched P0086.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is tracked in CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.042s).
- Go formatting, 13 changed local links and anchors, 87 pass index entries,
  diff review, and `git diff --check` passed. Logs are in `build/P0087-*.log`.

## Limits and handoff

Four-append calls and compound Console operands follow ordinary emission.
Console wrappers and append operands still use the two-append proof. General
expression and argument ownership, async, and exception unwinding remain open.

Verify P0087 publication and exact-commit Windows and Ubuntu CI. Then measure
retention in complete calls to proven Console wrappers when their argument
returns three appends. Check that each wrapper reads the original pointer and
that the caller releases it after the wrapper returns.
