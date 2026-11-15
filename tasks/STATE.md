# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0042 - conditional borrowed string-result scoped cleanup**.
- Record: [P0042](passes/P0042-conditional-borrowed-return.md).
- Parent/fetched approved remote: `b57c2b2406f6c4621f8b30cb4b902eadc70d207d` (P0041).
- Resolve this pass commit by its exact `Pass-ID: P0042` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0042 qualifies conditional string-result trees whose value leaves are literals,
known-owned scalar locals or borrowed scalar parameters proven unchanged by
P0041's whole-body scan. Every possible value must qualify, including unreachable
constant branches. Capture the selected pointer once, compare only owned-local
candidates and release other qualified strings/classes in reverse order. Borrowed
parameters retain their original caller backing without copy/retain/consumption.
Selected owned results keep their original reference until shutdown. Rebound or
unsupported leaves preserve the entire previous return; async/active exception
exits remain separate. See the [contract](../docs/development/string-ownership.md#conditional-borrowed-string-parameter-returns-p0042).

The regression reproduced 62 live allocations instead of five in both modes.
Each now requires 75 nonnull releases (19 classes, 56 strings), 39 output lines,
five selected owned results before shutdown and an empty registry afterward.
All nested paths, two borrowed parameters, repeated/null/empty/const leaves,
header updates, methods, collisions, owned replacement in a condition, completed
exceptions and caller backing/snapshot survival execute. Exact C checks require
capture before reverse cleanup and preserve unsupported/exception returns.
Forty-eight additional cases preserve both original returns for twelve placements
of rebinding nested on either side in both modes. P0039/P0040/P0041 unused controls
now check qualified cleanup; their runtime and earlier harness assertions remain.

Clean approved checkout/attached branch/origin matched published P0041; one
registered worktree and available chats showed no competing active writer.
Schedule already uses the approved local cwd. P0041
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43751573004)
passed build, runner, quick and full-suite steps before work. See
[results](MostRecentTestResults.md) for P0042's measured validation.

Next invocation:

1. Verify P0042 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Audit direct borrowed member-result cleanup: establish receiver/backing lifetime
   and field-mutation/call barriers before selecting a safe implementation slice.
   Preserve fallback wherever that proof is unavailable.
3. Preserve retained owned-result accumulation, aliases/containers/destructors/
   fields, async/exception unwinding, shape/operator limits and eighteen skips.
   String-index return typing, custom-type for headers and class null initializers
   remain limited. Completed P0026-P0029 maintenance must not repeat.
