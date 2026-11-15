# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0041 - scoped cleanup on unchanged borrowed string-parameter returns**.
- Record: [P0041](passes/P0041-borrowed-parameter-return.md).
- Parent/fetched approved remote: `df5ddb965d2df77a5a0fe9c84b1191e77171973d` (P0040).
- Resolve this pass commit by its exact `Pass-ID: P0041` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0041 qualifies a direct identifier result resolving to a borrowed scalar string
parameter only when a whole-body scan proves it unchanged. Capture the pointer
once before reverse-order cleanup of qualified strings/scalar-class locals. The
result keeps its caller-provided lifetime without a copy, retain or consumption;
caller declarations still copy. Later/nested assignments and loop updates block
the proof, as do same-name declarations/catch bindings and unknown body nodes.
Rebound parameters can alias local storage, so preserve their previous emission.
Conditional/other borrowed results and async/active exception exits stay separate.
See the [contract](../docs/development/string-ownership.md#unchanged-borrowed-string-parameter-returns-p0041).

The regression reproduced a nonempty pre-shutdown registry in both modes. Each
now requires 58 nonnull releases (20 classes, 38 strings), 17 output lines and an
empty registry before/after shutdown. Nested/header/switch scopes, unbraced paths,
null/const/empty storage, later declarations, recursion, methods, collisions,
normal exception completion and caller snapshots/backing survival execute. Exact
C requires capture before reverse cleanup and preserves unsupported returns.
Twenty-four additional cases check twelve placements of rebinding in both modes.
P0037/P0038 unused borrowed controls now check P0041 emission with unchanged
runtime assertions. Earlier allocator/refcount/shutdown checks remain intact.

Clean approved checkout/attached branch/origin matched published P0040; one
registered worktree and available chats showed no competing active project writer.
Schedule already uses the approved local cwd. P0040
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43744953713)
passed build, runner, quick and full-suite steps before work. See
[results](MostRecentTestResults.md) for P0041's measured validation.

Next invocation:

1. Verify P0041 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Qualify conditional string-result trees whose value leaves are literals,
   known-owned locals or unchanged borrowed scalar parameters. Reuse the whole-
   body lifetime proof, preserve only selected owned storage, and require selected-
   only/once-only effects plus caller backing survival in both modes. Rebound or
   other unsupported leaves must retain the entire previous return.
3. Preserve retained owned-result accumulation, aliases/containers/destructors/
   fields, async/exception unwinding, shape/operator limits and eighteen skips.
   String-index return typing, custom-type for headers and class null initializers
   remain limited. Completed P0026-P0029 maintenance must not repeat.
