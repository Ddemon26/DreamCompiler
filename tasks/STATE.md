# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0032 - scalar string assignment-initializer ownership**.
- Record: [P0032](passes/P0032-assignment-initializer-ownership.md).
- Parent/fetched approved remote: `f028fc63f48cbc93723b9a11c3438e7619900189` (P0031).
- Resolve this pass commit by its exact `Pass-ID: P0032` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0032 copies the complete result of a plain `=` scalar string declaration
initializer, such as `string copy = (source = "next")`. Assignment evaluates
once; source and declaration have independent storage. The declaration receives
existing owned replacement and jump/header cleanup. Parameter, field and indexed
destinations retain their borrowed behavior. Fresh managed assignment operands
can remain registered until shutdown; destination ownership does not transfer.

Both ordinary/module-entry modes failed checked live-registry cleanup before the
fix. Each now checks seventeen emitted copy sites, thirty nonnull releases and
twenty-three output lines, source/copy replacement, self/nested assignment,
null/empty results, borrowed destinations, once-only call/index effects, const
bindings, managed operands, methods, loop/header/jump cleanup and top-level snapshots.

The approved checkout started clean, attached, with one registered worktree and
no competing active project writer shown. P0031 publication matched the parent;
its [native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43666265892)
passed before implementation. Windows validation: **157 discovered, 139 passed,
18 skipped, zero failures/errors**, **25 Go quick groups**, **5 runner tests**,
Linux cross-build. See [results](MostRecentTestResults.md). Cross-build/harness
cross-linking does not establish native P0032 Linux runtime behavior.

Next invocation:

1. Verify P0032 publication by trailer and remote SHA, then inspect native CI.
   Repair failed checks before new work.
2. Continue T011 with owned scalar string `+=`. Reproduce generic C operator
   emission for `TK_PLUSEQ`, then qualify a bounded string/string replacement
   contract using checked allocations in both modes. Require once-only RHS
   effects, self-append and expression results; preserve numeric compound operators
   and specify null/temporary ownership policy.
3. Keep return transfer, class aliases/containers/destructors, field ownership and
   exception/finally unwinding separate. Preserve all 18 existing skips and shape/
   operator limits. P0026-P0029 maintenance work is complete; do not repeat it.
