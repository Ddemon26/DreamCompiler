# P0099 - Release four returned appends after a proven Console wrapper

- Date: 2026-11-28 UTC
- Task: T011
- Parent checkpoint: `43052c3a2cda19e9e9037b1366e57a5891c011a2` (P0098)
- Commit lookup: exact `Pass-ID: P0099` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete `view(fourth());` statement previously left the factory's original
fourth join live until shutdown. The statement emitter now applies the existing
four-append factory proof to the argument of a
[proven Console wrapper](../../docs/development/string-ownership.md#console-wrappers-for-four-returned-appends-p0099).
The factory and its arguments evaluate once, the wrapper passes the original
pointer to the writer, and the caller releases it after the wrapper returns.

The [Dream fixture](../../tests/basics/memory/owned_call_argument.dr) checks
output and argument evaluation for both writers. The
[Go regression](../../codex/go/owned_call_argument_test.go) checks emitted C for
ordinary and imported calls, and the
[lifetime harness](../../codex/c/owned_call_argument_harness.c) checks that the
writer observes the original pointer at reference count one before release.

## Validation

- The active schedule uses the existing local checkout. The clean, attached
  branch matched the approved remote. Both
  [P0098 CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/46061777001)
  passed at the verified parent SHA.
- Zig 0.17.1 and re2c 3.0 built the unchanged and patched Windows compilers in
  15/15 steps each.
- On P0098, `go test -count=1 -v -run '^TestCompiler_OwnedCallArgument$' .`
  from `codex/go` failed on missing release blocks for both wrappers in both
  modes (0.782s). Four isolated baseline audit probes each measured 64 releases
  and eight live joins after eight calls (7.876s total).
- `go test -count=1 -v -run '^TestCompiler_OwnedCall(Argument|Initializer|Assignment|Discard)$' .`
  passed from `codex/go` (17.980s). The fourth-join harness ran 10,000 groups
  of two direct outputs and two wrapper outputs in each mode. It checked
  once-only arguments, original-pointer release at reference count one, an
  empty registry after each group, and exact output matching an ordinarily
  linked binary. Checked Linux binaries cross-linked.
- `zig build test` discovered 197 Windows entries. **179 passed**, **18 skipped**,
  and zero failed or errored (64.76s). Report
  `test_results_20261127_190827_windows.json`. The wrapper fixture adds one
  output line and changes the argument count from two to four. The other 196
  entries matched P0098's statuses, outputs, and skip reasons.
- The Windows Go quick protocol passed all **43** groups (175.945s, `-count=1`)
  using `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  from `codex/go`.
- `python codex/python/owned_result_audit.py` passed all **114** probes
  (137.943s). Eight wrapper calls per writer and mode measured 72 releases
  and zero live joins, compared with 64 releases and eight live joins at
  P0098. All 110 prior measurements matched.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.050s).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is tracked in CI.
- Go formatting, 17 changed local links and anchors, all 99 pass index
  entries, diff review, and `git diff --check` passed. Logs are in
  `build/P0099-*.log`.

## Limits and handoff

Five-append results, conditional arguments, forwarding wrappers, and fourth
joins used as append operands use ordinary emission. General expression and
argument ownership, async, and exception unwinding remain open.

Verify P0099 publication and both exact-commit CI jobs. Then measure retention
when a complete `text += fourth();` statement consumes the original fourth join.
Check once-only arguments, destination lifetime, and release after the append
installs the concatenation in ordinary and imported calls.
