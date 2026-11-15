# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0039 - cleanup before literal-only conditional string returns**.
- Record: [P0039](passes/P0039-conditional-literal-return.md).
- Parent/fetched approved remote: `ea001f013a7fff6b78b30b065d7012c8ca4d9d2c` (P0038).
- Resolve this pass commit by its exact `Pass-ID: P0039` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0039 recursively qualifies `?:` string results only when all value leaves are
literals. The existing collision-safe temporary captures the selected static
pointer before reverse-order cleanup of qualified string/scalar-class locals.
Reached conditions execute once while local/header/field storage is still live;
unselected branches do not execute. No result allocation, copy, retain or consumption
is added. Caller declarations still copy. Any nonliteral leaf keeps the entire
return on its prior path; async and active exception exits remain separate. See
the [contract](../docs/development/string-ownership.md#conditional-literal-string-returns-p0039).

The new regression failed pre-shutdown registry emptiness in both modes before
the fix. Each now checks 55 nonnull releases (18 classes, 37 strings), 34 output
lines and an empty registry before and after shutdown. Four nested leaves,
selected-only condition effects, null/const/empty locals, later declarations,
unbraced paths, loop/header updates, methods, name collisions, caller survival and
normal exception completion execute. Exact C checks preserve nonliteral leaves
in either recursive branch and active exception emission. P0038's unused
conditional control now checks P0039 cleanup; earlier runtime assertions remain.

Checkout/path/attached branch/origin started clean; one registered worktree and no
competing active project writer shown. Schedule already uses the approved local cwd.
P0038 publication matched the parent and its [native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43698784933)
passed before work. Windows: **166 discovered, 148 passed, 18 skipped, zero failures/
errors**, **32 Go quick groups**, **5 runner tests**, Linux cross-build **15/15 steps**.
See [results](MostRecentTestResults.md), including the initial trailing-blank fixture
mismatch. Cross-building is not native P0039 Linux runtime evidence.

Next invocation:

1. Verify P0039 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Reproduce conditional returns selecting known-owned locals and literals;
   define preservation of the selected allocation and cleanup of unselected
   owners before extending support. General borrowed/compound results and caller
   consumption need separate lifetime design.
3. Preserve alias/container/destructor/field, async, exception and shape/operator
   limits and eighteen skips. String-index return typing, custom-type for headers
   and class null initializers remain limited/unsupported. Completed P0026-P0029
   maintenance must not repeat.
