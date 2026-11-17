# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0049 - audit caller consumption of owned string results**.
- Record: [P0049](passes/P0049-owned-call-result-audit.md).
- Parent/fetched approved remote: `7f356d9ab1e2145f0f10fe31142f3db34a09072b` (P0048).
- Resolve this pass commit by its exact `Pass-ID: P0049` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0049 audits and measures caller result accumulation, then defines a bounded
[adoption contract](../docs/development/owned-call-result-consumption.md).
Compiler behavior is unchanged. Every direct scalar call initializer still copies;
the selected owned result remains registered until shutdown. Borrowed results may
be literals or caller-owned storage; a function can return borrowed and owned
storage on different invocations. Return type and registry membership cannot
establish transferred ownership.

The reproducible [audit helper](../codex/python/owned_result_audit.py) checks real
generated C, exact checked/native output, live release operands/refcounts, final
registries and Linux cross-linking in ordinary/module-entry modes. In each mode,
owned initializers leave five originals (six nonnull releases); borrowed controls
leave zero (seven releases); mixed initializers leave one (four releases); other
consumers leave five (three releases). All eight probes pass. These are measured
baseline limitations, not adoption acceptance or native Linux execution evidence.

The first implementation slice adopts only a complete scalar string declaration's
direct free-function call when the actual synchronous definition has exactly one
scalar string declaration and its terminal direct local return. Only literal,
null/absent or scalar string-parameter initializers qualify. Keep caller shadowing,
actual lowered definitions, parameter shapes and binding identity explicit. No
method, mixed return, effect, alias, general flow analysis, ABI change or other
consumer is included. The contract specifies positive/unchanged-emission controls,
original allocation identity checks and migrations of genuinely eligible fixtures.

Clean approved checkout/attached branch/origin matched published P0048; one
registered worktree and available chats showed no competing active writer.
Schedule already uses local execution and approved cwd. P0048 exact-parent
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43935983779)
passed build, Python, quick and full-suite steps.

Current measured checks are in [MostRecentTestResults.md](MostRecentTestResults.md).
Windows Zig 0.17.1 full suite: **173 discovered, 155 passed, 18 skipped, zero
failures/errors**, 49.46s. Prior ownership proofs/fixtures/assertions are unchanged.
All 39 Go quick groups, five Python tests and Linux cross-build 15/15 pass.

Next invocation:

1. Verify P0049 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Implement the contract's direct proven-owned free-function call initializer
   adoption, beginning with intact pre-fix C/lifetime regressions in both modes.
   Require original allocation identity and bounded repeated-call storage;
   preserve borrowed/mixed/method/unsupported fallback behavior.
3. Keep other consumers, general return flow, methods, pointer/class transfer,
   static pointer/array emission, class-parameter syntax, aliases/containers/
   destructors/fields and async/exception unwinding separate. The eighteen skips,
   string-index return typing, custom-type for headers and direct class null
   initializer limits persist. Do not repeat P0026-P0029 maintenance.
