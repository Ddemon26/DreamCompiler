# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0037 - cleanup around direct owned-local string returns**.
- Record: [P0037](passes/P0037-local-string-return.md).
- Parent/fetched approved remote: `43772c74fe258d24d1389e6d8e50857dc0202297` (P0036).
- Resolve this pass commit by its exact `Pass-ID: P0037` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0037 saves a direct known-owned scalar string-local result once, releases the
other qualified string/scalar-class locals in reverse order, and returns the
saved pointer. Its original allocation/reference survives until shutdown; caller
declarations still copy without consuming it. No extra result copy or allocator
change is added. Bindings survive for other paths and unbraced returns remain
conditional. Other string expressions, borrowed-source/result-consumption policy,
async and active exception unwinding remain separate. See the bounded
[contract](../docs/development/string-ownership.md#direct-owned-local-string-returns-p0037).

Both modes reproduced 44 live allocations before shutdown. The matrix now requires
45 nonnull releases (20 classes, 25 strings), 21 output lines and exactly 17 live
returned allocations with one reference each, then an empty shutdown registry.
Mixed/nested/null/const/empty locals, loops/switches, headers, zero/multiple iterations,
recursion, append/replacement, methods, caller survival, name collisions and normal
exception completion are checked. Other result forms and active exception exits
have emission-only controls. Earlier empty-before-shutdown assertions remain intact.

Checkout/path/attached branch/origin started clean; one registered worktree and no
competing active project writer shown. Schedule already uses the approved local cwd.
P0036 publication matched the parent and its [native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43694034403)
passed before work. Windows: **164 discovered, 146 passed, 18 skipped, zero failures/
errors**, **30 Go quick groups**, **5 runner tests**, Linux cross-build **15/15 steps**.
See [results](MostRecentTestResults.md). Cross-building is not native P0037 runtime evidence.

Next invocation:

1. Verify P0037 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Reproduce locals retained by direct literal string returns, then qualify cleanup
   using their static-result lifetime. Keep borrowed/compound string expressions,
   general caller consumption and exception unwinding separate.
3. Preserve existing alias/container/destructor/field, async, exception and shape/
   operator limits and eighteen skips. String-index return typing, custom-type for
   headers and class null initializers remain limited/unsupported. Completed
   P0026-P0029 maintenance must not repeat.
