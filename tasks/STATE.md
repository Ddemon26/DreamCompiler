# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0031 - class-local cleanup on break/continue**.
- Record: [P0031](passes/P0031-loop-exit-classes.md).
- Parent/fetched approved remote: `d11cc7b280fe1da60122790d1634756a7aac03af` (P0030).
- Resolve this pass commit by its exact `Pass-ID: P0031` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0031 extends `emit_control_jump` to release class locals using the existing normal
block-cleanup classification and loop/switch binding boundaries. Reverse order,
conditional jumps, surviving outer instances, string headers, nested targets and
existing scalar-string behavior are preserved. The new ordinary/module-entry
matrix checks 27 class and six string releases, thirteen output lines and an empty
live-allocation registry before shutdown. Both modes failed that pre-shutdown
assertion before the fix. Independent fresh class allocations are qualified;
alias retention, class pointers/arrays, destructors and field destruction are not.

The approved checkout started clean, attached, with one registered worktree and
no competing active project writer. P0030 publication matched the parent; its
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43665107774)
passed during this run. Windows validation: **156 discovered, 138 passed, 18 skipped,
zero failures/errors**, **24 Go quick groups**, **5 runner tests**, Linux cross-build.
See [results](MostRecentTestResults.md). Linux cross-build/harness cross-linking does
not establish native P0031 runtime behavior; inspect CI after publication.

Next invocation:

1. Verify P0031 publication by trailer and remote SHA, then inspect native CI.
   Repair failed checks before new work.
2. Continue T011 with **scalar string assignment-expression initializers**, for
   example `string copy = (source = "next")`. Reproduce alias/replacement cleanup
   with the checked live registry, then qualify independent storage and once-only
   assignment effects in ordinary/module-entry modes. The assignment expression
   is an `ND_BINOP` with `TK_EQ`, outside the current initializer-copy predicate.
3. Keep return ownership, class aliases/containers/destructors, field ownership and
   exception/finally unwinding separate. Preserve all 18 existing skips and shape/
   operator limits. P0026-P0029 maintenance work is complete; do not repeat it.
