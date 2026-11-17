# P0051 - Audit owned factory assignment and define adoption contract

- Date: 2026-11-17 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `4e0020436ab5997989eb10108ab3f61f43f4e4a7`
- Commit lookup: exact `Pass-ID: P0051` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Complete P0050's requested source audit before implementing another consumer.
The [contract](../../docs/development/owned-call-assignment.md) follows statement,
expression, binding, helper and factory-proof paths. Existing owned-slot assignment
copies the transferred result, releases the old destination and leaves the original
live until shutdown. Historical aggregate evidence is explicitly distinguished from
an isolated assignment or identity measurement.

Select only complete assignment statements with identifier owned scalar destinations
and the unchanged P0050 factory proof. Specify a shared proof, one RHS evaluation,
release of the current slot after RHS completion, self-snapshot and nested argument
replacement semantics, original-pointer/peak-storage acceptance, and unchanged
borrowed/mixed/method/shape/expression fallback. Update state, backlog, source map,
prior contract handoff and pass index. Compiler, runtime, regressions, runners and
editor plugins are unchanged; assignment adoption remains unimplemented.

## Validation

- Verified approved checkout/attached branch/origin and clean starting worktree.
  Previous automation memory, all-ref history, one registered worktree and available
  chats show no unfinished pass or competing active project writer. Schedule view
  and TOML already select local execution and approved cwd; unchanged.
- Restricted fetch failed DNS; normal approved `git fetch origin
  codex/compiler-stabilization` succeeded. Parent matches fetched branch, zero
  commits either side. No remotes or permissions were changed.
- Inspected P0050 [native CI 44004454286](https://github.com/Ddemon26/DreamCompiler/actions/runs/44004454286):
  Windows job 133776241845 and Linux job 133776242313 both completed successfully;
  build, Python, quick and Dream suite steps passed. Linux cross-compile step passed.
- Documentation-only: compiler checks **not rerun**. Reused P0050 results dated
  **2026-11-17 UTC**, recorded in [MostRecentTestResults.md](../MostRecentTestResults.md):
  Windows Zig 0.17.1, **174 discovered/156 passed/18 skipped/zero failures or errors**;
  **40 Go quick groups**, **five Python tests**, **74 focused subtests**, **eight
  audit probes** and Linux cross-build **15/15** passed. Native CI above is separate
  from that cross-build evidence. No new runtime/identity/isolated retention result
  is claimed. MostRecentTestResults remains unchanged.
- Inspected real source paths and historical audit assertions. Checked local Markdown
  paths (141), ten source anchors, seven UTF-8 files and P0001-P0051 index continuity; reviewed the
  complete documentation diff and `git diff --check`. An initial link check ran
  before this new pass record existed and failed that pending path; final check
  includes the completed record. No compiler test failure or assertion change.

## Limits and handoff

P0050's eighteen skips and grammar/operator/shape limits persist. General expression
assignment ownership, nested owned arguments, method/mixed results, other consumers,
pointer/class transfer, aliases/containers/fields/destructors and async/exception
unwinding remain outside the selected slice. Top-level storage keeps program lifetime.

Publish and verify the exact remote SHA; inspect CI when available. Next implement
only the complete-statement assignment contract, first adding isolated pre-fix
retention and original-pointer regressions in both modes. Preserve all prior
lifetime assertions and supersede only eligible assignment audit expectations.
End after this audit pass; no second consumer or pass in this invocation.
