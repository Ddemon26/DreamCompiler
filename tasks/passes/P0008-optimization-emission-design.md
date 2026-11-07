# P0008 - Define the first safe optimization-to-emission slice

- Date: 2026-11-07 UTC
- Task: T004 prerequisite design (documentation only)
- Parent checkpoint: `c162e0826d21a329ce3ca58a615c58f542cab92f`
- Commit lookup: exact `Pass-ID: P0008` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Complete the bounded design requested by T004 and the P0007 handoff before
connecting unqualified IR to emitted code. The source audit in
[optimization-emission.md](../../docs/development/optimization-emission.md)
traces the actual driver, lowering, SCCP, constant representation, module expansion,
and AST emitter. It distinguishes Dream transformations from the host C compiler's
existing `-O2`/`-O3` flags.

The design selects pure literal `+`/`-`/`*` trees, with all intermediates in
`0..INT_MAX-1`, a fresh single-block IR, restricted SCCP, and a verified literal
replacement consumed by the existing AST emitter. It specifies checked decimal
parsing/arithmetic, resource limits, ownership/source metadata, bounded convergence,
unsupported fallback, explicit internal errors, and no new artifacts on errors.
It requires replacing the unsafe discarded whole-program pipeline invocation;
unsupported fallback must not first run through zero/addition lowering defaults
or unchecked optimizer arithmetic. General CFG emission and SSA remain deferred.

The acceptance matrix requires generated-C evidence before host compilation,
equivalence at default/`-O1`/`-O2`/`-O3`, function/method/module coverage, side-effect
preservation, boundary/fallback checks, and both emission paths. These are future
implementation requirements, not new test results. State, backlog, pass index,
source map, and feature boundaries link the design. No compiler or runner changed.

## Validation

Verified the exact primary checkout path, attached approved branch, clean worktree,
approved origin, one registered worktree, git history, preceding automation memory,
and app thread inventory; no competing project writer or unfinished delivery found.
The saved schedule has `execution_environment = "local"` and the primary directory;
no schedule change required. Initial restricted fetch failed to resolve github.com;
`git fetch origin codex/compiler-stabilization` succeeded with approved network access.
HEAD and the fetched branch both matched the parent above, confirming P0007 delivery.

Read-only GitHub inspection of
[P0007 CI run 42219194531](https://github.com/Ddemon26/DreamCompiler/actions/runs/42219194531)
found both `build (ubuntu-latest)` and `build (windows-latest)` completed successfully,
including compiler stability regressions and the Dream regression suite. The prior
pending native CI limitation is resolved for P0007.

Compiler tests were **not rerun**, as required for documentation-only passes.
Reused evidence is P0007, **2026-11-06 UTC**, recorded in
[MostRecentTestResults.md](../MostRecentTestResults.md): Windows full Dream suite
136 discovered, 118 passed, 18 skipped, zero failures/errors; five Go quick groups
passed (38 diagnostics, arena growth, warnings, four loop optimization settings,
25 module cases); five Python runner tests passed; Linux cross-build passed.

Documentation validation: 31 local Markdown links across all seven changed/new
documents resolved. Source paths and named entry points were checked against the
driver, lowering, IR representation, SCCP/pipeline, module, and codegen sources;
the default invocation, `--dev`, emission flags, quick/CI commands, and output paths
match current code. State/backlog/index consistently mark design complete and
implementation open, with P0009 next. All seven documentation diffs were reviewed;
`git diff --check` passed. No generated files, tests, or measured results changed.

## Limits and handoff

This pass delivers a design, not optimized generated C. T004's implementation and
behavior-equivalence acceptance remain open. No additional language behavior,
optimizer correctness, performance, or skipped-test coverage is claimed. The 18
unannotated entries, fixture exclusions, and existing language limitations are
unchanged. Historical pass records and measured test results remain intact.

Publish this pass and verify its remote SHA before allocating P0009. Inspect its
CI, then implement the documented restricted expression bridge and focused matrix;
repair any failed baseline first. Do not broaden that implementation to variables,
loops, or general CFG emission. End this invocation after this design pass.
