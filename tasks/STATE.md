# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0005 - primitive function return checks and checkout recovery**.
- Record: [P0005](passes/P0005-primitive-return-checks.md).
- Parent checkpoint: `bf5661f20ad46f1c98e3ea19e2a9ff761cb5df6b` (P0004).
- Find P0005's commit with `git log --all --grep='^Pass-ID: P0005$' --format='%h %s' -1`.
- Next unused pass ID: **P0006**. Confirm the index and remote tip before allocating it.
- Next ready work: **T002** in [the backlog](TODO.md), unless CI or baseline failures take priority.

Use only `C:/Users/damon/cApps/DreamCompiler`, with HEAD attached to the integration
branch. The legacy detached automation worktree was removed on 2026-11-04 after all
16 changed files and 11 logs/reports were copied and hash-verified. Original files
and a SHA256 manifest remain under ignored `build/P0005-worktree-recovery/`.
The root instructions, workflow, daily prompt, and existing automation now require
the primary checkout. Do not create separate worktrees or detached checkouts.

P0005 checks explicit primitive returns, void/bare returns, method return contexts,
nested switch/try statements, unary/conditional expressions, and dependency source
locations. Return types match exactly; the return boundary does not convert numbers.

Recovery validation on 2026-11-04: Zig 0.17.1; full Dream suite 133 discovered,
114 passed, 19 skipped, no failures/errors; all four Go quick groups passed
(23 diagnostic cases, arena growth, warning behavior, 20 module cases); five Python
runner tests passed; Linux cross-build succeeded. Native Linux execution depends on CI.
Earlier SDK/cache errors did not recur in this recovery run with approved toolchain
access outside the restricted sandbox; their original root cause remains unproven.
See [MostRecentTestResults.md](MostRecentTestResults.md) for commands and limits.

Missing-return proofs, pointer/array shapes, general operator validation, and instance
field declaration initializers remain follow-ups. Optimized IR still does not feed
the C backend; SSA construction is disabled. Access control, virtual dispatch,
imported type/global exports, async, and graphics remain outside the supported subset.

Every pass must be committed, pushed, and checked against the remote SHA before the
next pass starts. A local commit alone is not delivered. Preserve the checkpoint and
report any delivery blocker instead of starting another feature.
