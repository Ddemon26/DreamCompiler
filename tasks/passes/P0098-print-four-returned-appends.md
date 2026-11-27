# P0098 - Release four returned appends after direct Console output

- Date: 2026-11-27 UTC
- Task: T011
- Parent checkpoint: `760a174348c27a0f2f02cdb36c2da87cd9ecef22` (P0097)
- Commit lookup: exact `Pass-ID: P0098` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Both direct Console writers printed a factory's original fourth join and left
the allocation live until shutdown. The statement emitter now applies the
[four-append proof](../../docs/development/string-ownership.md#direct-console-output-of-four-returned-appends-p0098)
to the writer's direct argument. The factory and its arguments evaluate once,
the writer reads the original pointer, and the caller releases it after the
writer returns.

The [Dream fixture](../../tests/basics/memory/owned_call_argument.dr) checks
the output and argument effects of both writers. The
[Go regression](../../codex/go/owned_call_argument_test.go) checks ordinary
and imported factory calls. Its
[lifetime harness](../../codex/c/owned_call_argument_harness.c) checks pointer
identity, reference count, and release order.

## Validation

- The active schedule uses the existing local checkout. The clean, attached
  branch matched the approved fetch. Both
  [P0097 CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/46013496222)
  passed at the verified parent SHA.
- Zig 0.17.1 and re2c 3.0 built the parent and patched Windows compilers in
  15/15 steps each.
- On P0097, the focused Go regression failed on missing release blocks in both
  modes (0.726s, `-count=1`). Four isolated baseline audit probes each measured
  64 releases and eight live joins after eight calls (8.227s for all probes).
- `go test -count=1 -v -run '^TestCompiler_OwnedCall(Argument|Initializer|Assignment|Discard)$' .`
  passed from `codex/go` (18.539s). In each mode, 10,000 paired calls tested
  one `Write` and one `WriteLine` per pair. Each call evaluated its argument
  once and released the original fourth join at reference count one after the
  writer observed it. The registry was empty after each pair. Checked and
  normally linked Windows output matched. Checked Linux binaries cross-linked.
- `zig build test` discovered 197 Windows entries. **179 passed**, **18 skipped**,
  and zero failed or errored (63.88s). Report
  `test_results_20261127_130110_windows.json`. The Console fixture added two
  expected output lines. The other 196 entries matched P0097's statuses,
  outputs, and skip reasons.
- The first Go quick run failed in both modes of `nested_join_return`
  (171.605s). The fixture's `Console.WriteLine(beyond());` now releases its
  fourth join, so the test observed two live results at shutdown while it
  expected three. The updated assertion requires 27 releases, two live
  results, the generated Console release block, and the same exact output.
  The focused `nested_join_return` retry passed in both modes (3.701s,
  `-count=1`).
- The complete Windows Go quick retry passed all **43** groups (174.044s,
  `-count=1`) using
  `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  from `codex/go`.
- `python codex/python/owned_result_audit.py` passed all **110** probes
  (136.866s). Each eight-call writer and mode probe measured 72 releases
  and zero live fourth joins, compared with 64 releases and eight live joins
  at P0097. The other 106 measurements matched P0097.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.054s).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is tracked in CI.
- Go formatting, 15 changed local links and anchors, all 98 pass index entries,
  diff review, and `git diff --check` passed. Logs are in `build/P0098-*.log`.

## Limits and handoff

Five-append calls, factory calls inside larger expressions, and fourth joins
consumed by wrappers or append expressions use ordinary emission. General
expression and argument ownership, async, and exception unwinding remain open.

Verify P0098 publication and both exact-commit CI jobs. Then measure retention
when a complete `view(fourth());` statement passes the original fourth join
to a proven synchronous wrapper for either Console writer. Check release after
the wrapper returns and once-only arguments in ordinary and imported calls.
