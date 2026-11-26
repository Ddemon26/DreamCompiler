# P0094 - Release three-append factory operands in direct append returns

- Date: 2026-11-26 UTC
- Task: T011
- Parent checkpoint: `77d965c29c5ac70ee55b253de546377e3810b18c` (P0093)
- Commit lookup: exact `Pass-ID: P0094` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In `return (text += deeper());`, the third join returned by a qualifying
factory stayed registered until shutdown. The synchronous return emitter
now uses the existing three-append operand proof for an owned scalar local
outside exception handling. The factory and its arguments run once. The append
reads the current local after operand evaluation, installs the concatenation,
and releases the original third join. The fourth join survives for the caller.

The [Dream fixture](../../tests/basics/memory/owned_triple_join_append_return.dr)
exercises destination replacement, once-only argument evaluation, an unbraced
conditional return with both condition values, and caller use. The
[Go regression](../../codex/go/indexed_strings_test.go) checks generated C,
managed allocation releases, exact output, and fallback for borrowed operands
and fourth joins in ordinary and module-entry modes. The
[allocation audit](../../codex/python/owned_result_audit.py) isolates a fourth
join returned to direct Console output. See the
[return rule](../../docs/development/string-ownership.md#direct-append-returns-with-three-returned-appends-p0094).

## Validation

- The active schedule uses the existing local checkout. Its clean, attached
  branch matched the approved fetch. Both native jobs passed at that SHA in
  [P0093 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45835953349).
- Zig 0.17.1 built the Windows baseline and patched compiler in 15/15 steps
  each. The restricted shell could not locate the installed SDK. The approved
  build used the installed SDK and caches.
- `TestCompiler_AppendedStrings/triple_join_return` failed at the missing
  owned helper on P0093 in both modes (`-count=1`, 1.027s). The isolated baseline
  audit counted 56 releases and 16 live joins after eight calls per mode (6.568s).
- `TestCompiler_AppendedStrings/{nested_join_return,triple_join_return}`
  passed in both modes (`-count=1`, 5.864s). The new fixture counted 32
  releases of live, single-reference allocations and six expected live
  results before shutdown cleanup in each mode. The older fixture now
  counts 26 releases and three expected live results, with identical output.
  Checked and normally linked Windows output matched. The checked harnesses
  cross-linked for Linux.
- `zig build test` discovered 197 Windows entries. **179 passed**, **18 skipped**,
  and zero failed or errored (66.85s). Report `test_results_20261126_130004_windows.json`.
  All 196 prior statuses, outputs, and skip reasons matched P0093.
- The Windows Go quick protocol passed all **43** groups with `-count=1`
  (210.808s) using `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`.
- `python codex/python/owned_result_audit.py` passed all **102** probes
  (150.499s). Eight direct append returns per mode counted 64 releases and
  eight live fourth joins, compared with 56 releases and 16 live joins at
  P0093. All 100 prior measurements matched P0093.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps. Native Linux execution remains for CI.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.051s).
- Go formatting, 17 local links and anchors, all 94 pass index entries,
  diff review, and `git diff --check` passed. Logs are in `build/P0094-*.log`.

## Limits and handoff

Borrowed operands and factories with four returned appends use ordinary
append emission. Copying the returned fourth join into a scalar declaration
leaves the original registered until shutdown. General expression and argument
ownership, async, and exception unwinding remain open.

Verify P0094 publication and native CI. Then measure retention in
`string snapshot = fourth();`. Check original allocation identity, once-only
argument evaluation, and release on scope exit before extending initializer
adoption.
