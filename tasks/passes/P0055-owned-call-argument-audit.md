# P0055 - Audit proven factory results passed as arguments

- Date: 2026-11-18 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `04daec85aeee2d7c2a352e7d498bd5c24f7ac914`
- Commit lookup: exact `Pass-ID: P0055` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Isolate the retained allocation from `view(owned())`, a synchronous consumer
from the P0054 aggregate audit. Trace call emission and borrowed parameter
bindings before choosing when caller cleanup is safe. A synchronous void
callee can assign the original pointer to a static field without changing the
parameter slot, so the function signature and the existing unchanged-parameter
analysis cannot establish lifetime. A sibling-argument control records emitted
call shape and exact effects without prescribing C argument order.

The [argument contract](../../docs/development/owned-call-argument.md) selects
complete statements calling an actual synchronous void free function with a
single scalar string parameter, used solely as the operand of `Console.Write`
or `Console.WriteLine`. The proposed caller captures a proven factory argument,
passes the exact pointer and releases it after normal return. This pass adds
the three checked audit cases and records the future acceptance criteria.
Compiler emission is unchanged.

## Validation

- Verified the approved local checkout, attached integration branch, clean
  worktree, one registered worktree, matching fetch and push URLs, and local
  automation directory. The only active project chat observed was this run.
  No unpublished pass appeared in history. Restricted fetch failed DNS, and
  approved fetch succeeded. HEAD matched the fetched parent with zero commits
  on either side.
- Exact-parent [CI 44193733124](https://github.com/Ddemon26/DreamCompiler/actions/runs/44193733124)
  passed Windows job 134429561655 and Linux job 134429562058, including both
  Dream and Go stability suites.
- Zig version **0.17.1**. Restricted `zig build` could not locate the SDK
  installation from its executable path. With approved SDK access,
  `zig build --summary all` succeeded **15/15**.
- `python codex/python/owned_result_audit.py`: **18 probes passed** in
  **23.658s** on Windows. Per mode, eight isolated calls had **0 nonnull
  releases and 8 live originals**. The static-store and sibling-argument
  controls each had **0 releases and 1 live original**. Existing probes
  preserved their counts, including **4 releases and 3 live originals** in
  the aggregate case. Generated-C checks, native UBSan and ordinary linking
  with exact stdout, and Linux harness cross-linking passed. The harness
  checks reference counts and live entries at shutdown. It does not establish
  exact release identity or peak live allocations for the proposed rule.
- Reused P0054's 2026-11-17 full Dream suite (**176 discovered, 158 passed,
  18 skipped, zero failed or errored**), **42 Windows Go quick groups**,
  **5 Python runner tests** and **15/15 Linux cross-build steps**. These full
  checks were not rerun because the audit changes no compiler or runner behavior.
- Reviewed the diff and checked the changed Markdown paths, the pass index,
  Python syntax and `git diff --check`. Logs are in ignored
  `build/P0055-{build,audit}.log`.

## Limits and handoff

The argument result still stays registered until shutdown. The measured probes
cover ordinary and module-entry compilation. The implementation must also
qualify imported definitions and original-pointer release after the console
wrapper returns. The sibling probe imposes no inter-argument ordering
guarantee. Native Linux behavior for the new probes is unmeasured locally.
The same eighteen unannotated Dream entries remain skipped in P0054's full suite.

Verify P0055 publication and inspect available CI before implementing the
selected rule. Preserve fallback for escaped parameters, sibling arguments,
unproven factories and excluded expression contexts.
