# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0038 - cleanup before direct literal string returns**.
- Record: [P0038](passes/P0038-literal-string-return.md).
- Parent/fetched approved remote: `9803ea8626d180e24b17a6a65d638737a5c95cd6` (P0037).
- Resolve this pass commit by its exact `Pass-ID: P0038` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0038 releases all qualified string/scalar-class locals in reverse declaration
order before returning a direct string literal. Parenthesized, empty and escaped
literals qualify; static storage survives every release. It reuses the existing
collision-safe result temporary when cleanup is needed. No result allocation,
retain or consumption is added. Caller declarations still copy. Unbraced returns
remain conditional and normal/other branches keep their cleanup. Borrowed/compound
results, async and active exception exits remain separate. See the bounded
[contract](../docs/development/string-ownership.md#direct-literal-string-returns-p0038).

The new matrix failed pre-shutdown emptiness in both modes before the fix. Each
now requires 51 nonnull releases (18 classes, 33 strings), 19 output lines and an
empty registry before and after shutdown. Nested/const/null/empty locals, later
declarations, unbraced true/false paths, loops/switches, headers/updates, zero/multiple
iterations, recursion, methods, name collisions, caller survival and normal
exception completion execute. Borrowed/compound/active-exception controls check
unchanged emission only. P0037's unused literal control now checks P0038 emission;
its runtime counts and exact retained-result assertions remain intact.

Checkout/path/attached branch/origin started clean; one registered worktree and no
competing active project writer shown. Schedule already uses the approved local cwd.
P0037 publication matched the parent and its [native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43695861891)
passed before work. Windows: **165 discovered, 147 passed, 18 skipped, zero failures/
errors**, **31 Go quick groups**, **5 runner tests**, Linux cross-build **15/15 steps**.
See [results](MostRecentTestResults.md). Cross-building is not native P0038 Linux
runtime evidence.

Next invocation:

1. Verify P0038 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Reproduce retained locals on conditional string returns whose value branches
   are all literals; qualify static-result lifetime and once-only condition
   evaluation before extending cleanup. General borrowed/compound results and
   caller consumption need separate lifetime design.
3. Preserve alias/container/destructor/field, async, exception and shape/operator
   limits and eighteen skips. String-index return typing, custom-type for headers
   and class null initializers remain limited/unsupported. Completed P0026-P0029
   maintenance must not repeat.
