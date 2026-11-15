# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0034 - owned scalar strings on bare void return**.
- Record: [P0034](passes/P0034-void-return-strings.md).
- Parent/fetched approved remote: `376b74402d038c9d55aa2b790c1facdd785675a6` (P0033).
- Resolve this pass commit by its exact `Pass-ID: P0034` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0034 releases qualified owned scalar string locals in reverse declaration order
before bare `return;` in synchronous void free/static/instance functions. Active
nested scopes and for headers are included. A compound statement keeps cleanup
conditional for unbraced returns. Parameters, `this`, borrowed arrays/pointers and
later declarations receive no release; normal fall-through cleanup remains intact.
Value returns, async exits and returns inside active try/catch/finally contexts
keep their previous emission. Exception-context unwinding remains unqualified.

Both new modes reproduced retained allocations before shutdown before the fix.
The checked regression now requires 40 nonnull releases (39 strings and one
normally cleaned class instance), 13 exact output lines and an empty registry
before shutdown. Nested/const/null/empty locals, true/false unbraced paths,
while/do/for/switch exits, header updates, zero/multiple iterations, recursion,
methods and borrowed caller survival are covered. Unused exception-return paths
are checked only for unchanged emission; a return after a completed exception
context executes. Existing allocator/refcount/shutdown assertions are intact.

The approved checkout started clean and attached, with one registered worktree
and no competing active project writer shown. Schedule configuration already uses
the approved local cwd. P0033 publication matched the parent; its
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43677590309)
passed before implementation. Windows validation: **161 discovered, 143 passed,
18 skipped, zero failures/errors**, **27 Go quick groups**, **5 runner tests**,
Linux cross-build **15/15 steps**. See [results](MostRecentTestResults.md).
Cross-building is not native P0034 Linux runtime evidence.

Next invocation:

1. Verify P0034 publication by trailer and remote SHA, then inspect native CI.
   Repair failed checks before new work.
2. Continue T011 by reproducing fresh independent class locals retained by bare
   `return;` in synchronous void functions/methods. Qualify bounded class-local
   cleanup with checked pre-shutdown allocations, preserving borrowed `this`,
   string locals and nested/header scopes.
3. Keep class aliases/containers/destructors/fields, value-return transfer, async
   and exception/finally unwinding separate. Preserve the 18 existing skips and
   shape/operator limits. P0026-P0029 maintenance is complete; do not repeat it.
