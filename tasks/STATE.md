# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0048 - conditional read-only static string-field return cleanup**.
- Record: [P0048](passes/P0048-conditional-static-return.md).
- Parent/fetched approved remote: `f06e6c84dfadfeeb93db39cd999145d126b8f16a` (P0047).
- Resolve this pass commit by its exact `Pass-ID: P0048` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0048 adds proven static string-field reads to conditional value leaves under the
unchanged [static-field contract](../docs/development/static-member-return-cleanup.md).
Every static/instance leaf requires the whole-body read-only proof; parameter,
owned-local and literal guards remain unchanged. Capture once before reverse
cleanup and preserve only selected owned storage. No result copy/retain, global
release or field ownership is introduced. One unsupported leaf/effect preserves
the entire old return, even in an unselected branch. Async/exception guards remain.

Pre-fix both modes fail the complete regression's capture/cleanup C assertion;
no pre-fix lifetime run is claimed. Post-fix each mode checks 67 nonnull releases
(one receiver, 66 strings), 35 output lines, four original-reference selected owned
survivors until shutdown and an empty registry afterward. All 246 new-group
subtests pass. Coverage includes mixed static/instance/parameter/local/literal
leaves, inherited/hidden owner symbols, caller backing/snapshot survival, scope/
header/loop/switch paths, collisions, and unsafe body/leaf/shape/guard fallbacks.
Former eligible static fallback controls now assert positive exact C; prior
allocator/refcount assertions and read-only/parameter/literal proofs are preserved.

Clean approved checkout/attached branch/origin matched published P0047; one
registered worktree and available chats showed no competing active writer.
Schedule already uses local execution and approved cwd. P0047
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43874515506)
passed build, Python, quick and full-suite steps for the exact parent SHA.

Windows Zig 0.17.1 full suite: **173 discovered, 155 passed, 18 skipped, zero
failures/errors**, 47.26s. All 39 Go quick groups, five Python tests and Linux
cross-build 15/15 passed. Exact outcomes are recorded in the [measured snapshot](MostRecentTestResults.md).
Cross-build/cross-linking alone does not prove native P0048 Linux runtime behavior.

Next invocation:

1. Verify P0048 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Audit caller consumption of owned string results and retained owned-result
   accumulation. Distinguish borrowed and owned calls, reproduce the boundary
   with checked lifetime evidence and define one bounded implementation contract.
3. Keep pointer/class result transfer, static pointer/array emission, class-parameter
   syntax, other receivers, aliases/containers/destructors/fields and async/exception
   unwinding separate. The eighteen skips, string-index return typing, custom-type
   for headers and direct class null initializer limits persist. Do not repeat
   P0026-P0029 maintenance.
