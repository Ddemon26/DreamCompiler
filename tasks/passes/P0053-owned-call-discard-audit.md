# P0053 - Discarded factory-result audit and release contract

- Date: 2026-11-17 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `5a6e4ac3835f031e245f9d2050cbb5e905e10e4e`
- Commit lookup: exact `Pass-ID: P0053` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Complete the requested discarded-call source audit and define one bounded
[implementation contract](../../docs/development/owned-call-discard.md). Trace
expression-statement construction, module lowering, statement/call emission,
actual-definition proof and allocation/release paths. A discarded proven factory
result has no binding for scoped cleanup; the aggregate historical audit includes
this debt but does not measure isolated discard identity or peak storage.

Select only complete direct call statements accepted by the unchanged
`adopts_string_call`. Emit one call inside a release on normal completion, without
copy/retain or new helper/binding. Arguments finish first; borrowed backing and
independent snapshots must survive. No new argument ordering, recursive call
cleanup or exception unwinding claim. Define pre-fix isolated retention, both-mode
original-pointer/refcount checks, 10,000 repetitions, null/side-effect/scope/module
coverage and preserved fallback criteria before implementation.

Compiler, runtime, tests and `MostRecentTestResults.md` are unchanged. Update state,
backlog, source map, ownership contract cross-reference and pass index. Correct
the index's stale P0051 next-step banner as part of the current handoff.

## Validation

- Approved existing checkout and attached branch, clean worktree and approved
  origin verified. Fetch succeeded with normal approved access after restricted
  DNS failure; `git rev-list --left-right --count` reports **0/0** against the
  fetched approved branch. Parent equals published P0052. One registered worktree;
  available chats show no competing active project writer. Prior memory/history
  identify P0052 delivered and no unpublished pass. Schedule view/TOML already
  uses local execution and the approved cwd; no schedule changes needed.
- Exact-parent [CI 44090301495](https://github.com/Ddemon26/DreamCompiler/actions/runs/44090301495)
  has head SHA equal to the parent above and conclusion success. Windows job
  **134063382790** and Linux job **134063383021** passed build, Python runner,
  compiler stability/Go quick and full Dream checks.
- Documentation-only validation: local Markdown paths, audited source anchors,
  UTF-8 decoding, pass-index continuity and full diff review/check pass: **148
  local Markdown paths, 13 source anchors, seven UTF-8 files and 53 consecutive
  pass IDs**. No new compiler or test inputs.
- Compiler checks **not rerun**. Reuse **P0052, 2026-11-17 UTC**: full Windows
  suite **175 discovered/157 passed/18 skipped/zero failures or errors**; **41 Go
  quick groups**, **86 focused subtests**, **five Python tests**, **ten audit
  probes** and Linux cross-build **15/15** passed. Those are historical results,
  not P0053 executions. Native parent CI above supplies separate Linux evidence.
- A read attempted the nonexistent shortened P0051 filename; corrected discovery
  identifies `P0051-owned-call-assignment-audit.md`. No check or test was skipped
  to hide that navigation error.

## Limits and handoff

No discard cleanup is implemented yet. The contract's predicted aggregate change
to four releases/three survivors is future acceptance, not measured evidence.
The eighteen skips and existing grammar/operator/shape limits persist. Other
consumers, general/mixed/method result flow, nested owned arguments, pointer/class
transfer, aliases/containers/destructors/fields and async/exception unwinding stay
separate.

Publish and verify the exact remote SHA, then inspect native CI when available.
Next implement only complete discarded proven factory-call statements, first
reproducing isolated retention and failing C gates, then checking original pointer
release/refcount one, bounded peak storage and zero survivors in both modes.
End after this one audit pass.
