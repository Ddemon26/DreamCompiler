# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0050 - adopt proven owned free-function string initializers**.
- Record: [P0050](passes/P0050-owned-call-initializer.md).
- Parent/fetched approved remote: `f31b174eb5bad57334c915b769d3c7f63cb3ff66` (P0049).
- Resolve this pass commit by its exact `Pass-ID: P0050` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0050 implements the [bounded factory proof](../docs/development/owned-call-result-consumption.md)
in `adopts_string_call`. A complete scalar string declaration directly adopts a
synchronous free-function result only when its actual lowered body is exactly one
nonstatic scalar string local declaration followed by its terminal direct local
return. Literal, absent/null AST or scalar string-parameter sources qualify;
all parameters have scalar primitive shapes and caller bindings cannot shadow
the callee. Keep destination ownership and all existing callee return proofs.
No outer copy/retain/release, runtime ABI, grammar or semantic broadening is added.

The new both-mode regression checks forward/imported definitions, same-spelled
borrowed import versus owned entry factories, snapshots/backing replacement,
const/escaped/empty/grouped calls, typed-null/absent results, scopes/headers/loops/
jumps/early returns, name collisions and top-level lifetime. Ten thousand calls
per mode each release the exact original allocation with refcount one, peak live
count one and no survivor. A separate string-transfer probe returns that original
to the C harness. The main fixture checks 16 copies/15 scoped releases ordinary,
17/16 module; only its program-lifetime global remains at shutdown, then none.
All 74 focused subtests pass, including 56 emission-only definition fallbacks,
four consumer/primitive-positive controls and twelve artifact-free rejections.

The manual audit's eight probes pass: per mode owned initializers now have six
releases/zero survivors (P0049 had five survivors). Borrowed 7/0, mixed 4/1 and
other consumers 3/5 remain unchanged. The helper supersedes only eligible C/live
counts. No P0037-P0048 fixture, release count or allocator/refcount assertion
changed; complex factory definitions remain outside this narrow proof.

Clean approved checkout/attached branch/origin matched published P0049, with one
registered worktree and no competing active project writer in available chats.
Schedule already uses local execution and approved cwd. P0049 exact-parent
[native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43993419794)
passed build, Python, quick and full-suite steps.

Current measured checks are in [MostRecentTestResults.md](MostRecentTestResults.md).
Windows Zig 0.17.1 full suite: **174 discovered, 156 passed, 18 skipped, zero
failures/errors**, 80.83s. All 40 Go quick groups, five Python tests and Linux
cross-build 15/15 pass. Final focused group and eight manual audit probes pass.
Cross-build/cross-link evidence is distinct from native Linux P0050 execution.

Next invocation:

1. Verify P0050 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Audit plain assignment into an existing owned scalar slot using the same
   proven-factory boundary: once-only evaluation, old-slot release and fallback
   behavior. Define evidence before implementing another consumer.
3. Keep method/mixed/general result flow, nested owned argument temporaries,
   other consumers, pointer/class transfer, aliases/containers/destructors/fields
   and async/exception unwinding separate. Top-level storage retains program
   lifetime. Direct literal-null declarations, custom-type/array parameter
   syntax and async-result string declarations remain rejected. Eighteen skips,
   string-index return typing, static pointer/array emission, custom-type for
   headers and direct class null initializer limits persist. Do not repeat
   P0026-P0029 maintenance or expand this pass into another consumer.
