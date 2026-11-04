# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0004 — daily-agent instructions and documentation flow**.
- Record: [P0004](passes/P0004-daily-agent-workflow.md).
- Parent checkpoint: `6fe29c4` (**P0003**, verified pushed before P0004 started).
- Find P0004's commit with `git log --all --grep='^Pass-ID: P0004$' --format='%h %s' -1`.
- Next unused pass ID: **P0005**. Confirm the index and remote tip before allocating it.
- Next ready work: **T001** in [the backlog](TODO.md), unless CI or baseline failures take priority.

The compiler builds with Zig 0.17.1. Declaration-based single inheritance and
function-only multi-file imports are implemented. Optimized IR still does not feed
the C backend; SSA construction is disabled. Access control, virtual dispatch,
imported type/global exports, async, and graphics remain outside the supported subset.

Current measured tests are recorded in [MostRecentTestResults.md](MostRecentTestResults.md).
P0004 changes documentation and CI triggering; it reuses P0003's compiler evidence,
without claiming that those checks were rerun. Its own validation is in the pass record.

Every pass must be committed, pushed, and checked against the remote SHA before the
next pass starts. A local commit alone is not delivered. If fetch/push is blocked,
preserve the checkpoint and report the issue instead of advancing this handoff.
