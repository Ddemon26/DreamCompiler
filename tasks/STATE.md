# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0035 - scalar class locals on bare void return**.
- Record: [P0035](passes/P0035-void-return-classes.md).
- Parent/fetched approved remote: `d85bfb4b48fd5d560b0ccacbdffbd80f3dcdde26` (P0034).
- Resolve this pass commit by its exact `Pass-ID: P0035` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0035 adds active scalar class locals to P0034's synchronous void bare-return
cleanup. Class and owned-string releases interleave in reverse declaration order.
The parameter scope is excluded, preserving borrowed `this`; class arrays/pointers
are excluded from this new path. Bindings remain available to emit other paths,
and a compound statement keeps unbraced cleanup conditional. Value returns, async
and active exception returns keep their previous emission.

Both modes reproduced a nonempty registry before shutdown. The new checked matrix
requires 40 nonnull releases (30 class instances, ten strings), ten output lines,
exact C sequences and an empty pre-shutdown registry. Nested mixed scopes, later
declarations, while/do/for/switch exits, zero/multiple iterations, recursion,
methods, constructor return and borrowed receiver survival are covered. A return
after normal exception completion executes; unused active exception-return paths
have emission checks only. Fresh independent class instances are qualified;
aliases, containers, destructors and field destruction remain separate.

Checkout/path/attached branch/origin started clean; one registered worktree and no
competing active project writer shown. Schedule already uses the approved local
cwd. P0034 publication matched the parent, and its
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43684568487)
passed before implementation. Windows: **162 discovered, 144 passed, 18 skipped,
zero failures/errors**, **28 Go quick groups**, **5 runner tests**, Linux cross-build
**15/15 steps**. See [results](MostRecentTestResults.md). Cross-building is not
native P0035 Linux runtime evidence.

Next invocation:

1. Verify P0035 publication by trailer and remote SHA, then inspect native CI.
   Repair failed checks before new work.
2. Continue T011 by reproducing scoped allocations retained on primitive numeric,
   bool or char value returns. Design one bounded scalar-result temporary path
   that evaluates once before local cleanup; preserve field reads and side effects.
   Keep string/pointer/class return transfer outside that slice.
3. Preserve existing alias/container/destructor/field, async and exception limits,
   eighteen skips and shape/operator boundaries. Custom-type for headers and direct
   class null initializers remain unsupported. P0026-P0029 maintenance is complete.
