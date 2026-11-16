# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0044 - read-only direct instance string-field return cleanup**.
- Record: [P0044](passes/P0044-borrowed-member-return.md).
- Parent/fetched approved remote: `001c5223cc6d9e054b66f8da5f0b821f3731c007` (P0043).
- Resolve this pass commit by its exact `Pass-ID: P0044` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0044 captures direct scalar nonstatic `this.field` string results before reverse
local cleanup, including inherited owning-class casts. A separate whole-body
AST/operator allowlist rejects every write/inc/dec, call/new/console/await,
exception, custom-type/pointer/array local, receiver shadowing and unknown node.
Already-exited class alias scopes are scanned too. No result copy/retain or
receiver release is added. P0041/P0042's parameter proof remains unchanged;
conditional member leaves and static/local/struct/nested/base results stay separate.

Both modes reproduced retained allocations before the fix. Each now checks 38
nonnull releases (one receiver, 37 strings), 21 output lines, empty pre/post-shutdown
registries, caller backing/snapshot survival and exact capture/reverse cleanup.
196 emission-only controls preserve mutation/call/alias/shape/exception/async fallback.
Explicit class parameters and async instance methods remain rejected by grammar;
receiver shadowing is rejected by semantics. See the [contract](../docs/development/member-return-cleanup.md)
for the accepted fixture substitutions and untested boundaries.

Clean approved checkout/attached branch/origin matched published P0043; one
registered worktree and available chats showed no competing active writer.
Schedule already uses the approved local cwd. P0043
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43784740261)
passed build, runner, quick and full-suite steps before this work.

Windows full suite: 170 discovered, 152 passed, 18 skipped, zero failures/errors;
36 Go quick groups, five Python runner tests and Linux cross-build 15/15 passed.
Cross-build evidence does not prove native Linux runtime behavior. See
[results](MostRecentTestResults.md) and the pass record for exact checks/timings.

Next invocation:

1. Verify P0044 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Qualify conditional result trees containing read-only `this.field` leaves under
   the same proof, beginning with both-mode retained-allocation reproduction and
   mixed owned/literal/borrowed lifetime controls. Preserve every write/call/alias
   barrier; do not add explicit class-parameter syntax in this slice.
3. Preserve retained owned-result accumulation, aliases/containers/destructors/
   fields, async/exception unwinding, shape/operator limits and eighteen skips.
   String-index return typing, custom-type for headers and class null initializers
   remain limited. Completed P0026-P0029 maintenance must not repeat.
