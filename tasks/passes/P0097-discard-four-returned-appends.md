# P0097 - Discard the final join from four returned appends

- Date: 2026-11-27 UTC
- Task: T011
- Parent checkpoint: `6fc810987bf07518214469799af5b33e41379941` (P0096)
- Commit lookup: exact `Pass-ID: P0097` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A standalone call to a factory with four proven returned appends discarded the
final join's pointer. The original allocation stayed registered until shutdown.
The expression-statement emitter now accepts four returned appends under the
existing factory proof. It evaluates the call and its arguments once, then
releases the original fourth join immediately after the call returns.

The [Dream fixture](../../tests/basics/memory/owned_call_discard.dr) checks
observable argument effects. The
[Go regression](../../codex/go/owned_call_discard_test.go) checks ordinary and
imported calls and verifies the generated release. The
[checked harness](../../codex/c/owned_call_discard_harness.c) verifies allocation
identity and release order. See the
[discard rule](../../docs/development/string-ownership.md#standalone-calls-that-discard-four-returned-appends-p0097).

## Validation

- The active schedule uses the existing local checkout. The clean, attached
  branch matched the approved fetch. Both
  [P0096 CI jobs](https://github.com/Ddemon26/DreamCompiler/actions/runs/45963647397)
  passed at the verified parent SHA.
- Zig 0.17.1 and re2c 3.0 built the parent and patched Windows compilers in
  15/15 steps each. The default sandbox could not resolve Zig's installed
  toolchain path. The build passed with approved toolchain access.
- The focused baseline test failed in both modes at the empty-registry assertion
  (6.498s, `-count=1`). The isolated baseline audit measured 64 releases and
  eight live original joins after eight discards per mode (2.894s).
- `go test -count=1 -v -run '^TestCompiler_OwnedCall(Assignment|Discard|Initializer)$' .`
  passed from `codex/go` (16.372s). The discard harness checked 10,000 fourth-join
  calls in each mode. Every call evaluated its argument once, made five copies,
  four joins, and nine releases, and left an empty registry. Each release used a
  registered pointer at reference count one, and the last release used the
  original fourth join. Checked and normally linked Windows output matched.
  Checked Linux binaries cross-linked.
- `zig build test` discovered 197 Windows entries. **179 passed**, **18 skipped**,
  and zero failed or errored (66.00s). Report
  `test_results_20261127_070424_windows.json`. The discard fixture printed one
  new expected argument count. The other 196 entries matched P0096's statuses,
  outputs, and skip reasons.
- The Windows Go quick protocol passed all **43** groups (175.573s, `-count=1`),
  using `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  from `codex/go`.
- `python codex/python/owned_result_audit.py` passed all **108** probes
  (132.007s). Eight standalone calls per mode made 72 releases and left zero
  original joins live, down from eight at P0096. All 106 prior measurements
  matched.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.033s).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution is tracked in CI.
- Go formatting, 16 changed local links and anchors, all 97 pass index entries,
  diff review, and `git diff --check` passed. Logs are in `build/P0097-*.log`.

## Limits and handoff

Calls that return five appends and calls inside loop headers, conditionals, or
arguments continue through ordinary emission. General expression and argument
ownership, async, and exception unwinding remain open.

Verify P0097 publication and both exact-commit CI jobs. Then measure retention
when complete `Console.Write(fourth());` and `Console.WriteLine(fourth());`
statements print the factory's fourth join. Verify that the original allocation
can be released after output and that arguments evaluate once in both modes.
