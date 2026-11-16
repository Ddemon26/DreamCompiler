# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0047 - direct read-only static string-field return cleanup**.
- Record: [P0047](passes/P0047-static-member-return.md).
- Parent/fetched approved remote: `3e1db012b833c7060217120533d82321d05a9df9` (P0046).
- Resolve this pass commit by its exact `Pass-ID: P0047` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0047 implements the [P0046 static-field contract](../docs/development/static-member-return-cleanup.md)
for direct `Type.field` results on unshadowed registered class types and actual
scalar static string declarations/owners. A separate predicate shares the unchanged
whole-body read-only allowlist using an explicit guarded name. Capture once before
reverse local cleanup, preserving inherited/hidden owner symbols. No result copy/
retain, global release, fake receiver binding or field ownership is introduced.
Conditional static leaves and other qualifier/field shapes keep prior emission.

Both modes reproduced retained callee allocations before the fix. Each now checks
40 nonnull releases (one receiver, 39 strings), 23 output lines, exact C, caller
backing/snapshot survival and empty pre/post-shutdown registries. All 252 static
group subtests pass, including effects/alias/shape/receiver barriers, migrated
direct-static positives and twelve artifact-free semantic rejections. Local type
shadowing and indexed class-array receivers hit existing semantic boundaries;
no grammar/semantics changed. Earlier member/parameter/allocator assertions remain.

Clean approved checkout/attached branch/origin matched published P0046; one
registered worktree and available chats showed no competing active writer.
Schedule already uses local execution and approved cwd. P0046
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43821203292)
passed build, runner, quick and full-suite steps for the exact parent SHA.

Windows Zig 0.17.1 full suite: **172 discovered, 154 passed, 18 skipped, zero
failures/errors**, 46.49s. All 38 Go quick groups, five Python tests and Linux
cross-build 15/15 passed.
Exact results are recorded in the [measured snapshot](MostRecentTestResults.md).
Cross-build/cross-linking alone does not prove native P0047 Linux runtime behavior.

Next invocation:

1. Verify P0047 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Qualify conditional static string-field value leaves under the unchanged
   read-only proof, with mixed borrowed/owned/literal lifetime and failed-leaf
   controls. Preserve the direct/static and P0037-P0045 proofs and assertions.
3. Keep caller consumption/retained owned-result accumulation, pointer/class result
   transfer, static pointer/array emission, class-parameter syntax, other receivers,
   aliases/containers/destructors/fields and async/exception unwinding separate.
   The eighteen skips, string-index return typing, custom-type for headers and
   direct class null initializer limits persist. Do not repeat P0026-P0029 maintenance.
