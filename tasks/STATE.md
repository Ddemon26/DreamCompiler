# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0036 - scoped cleanup on primitive scalar return**.
- Record: [P0036](passes/P0036-scalar-return-cleanup.md).
- Parent/fetched approved remote: `1dce40c55e4b9166fa73c28b9435c9b1d49997cd` (P0035).
- Resolve this pass commit by its exact `Pass-ID: P0036` trailer.
- Delivery requires push and matching `git ls-remote`; inspect native CI next.

P0036 evaluates synchronous `int`, `float`, `bool` and `char` return expressions
once into a typed temporary before reverse-order qualified string/scalar-class
cleanup. This shares P0034/P0035's predicate: parameters/`this`, borrowed containers
and later declarations receive no release. Bindings survive for other paths and
unbraced returns stay conditional. Temporary names avoid active bindings and the
lowered expression, including module function names; returns without qualified
locals retain direct emission. String-result ownership, async and active exception
unwinding remain separate.

Both modes reproduced a nonempty registry before shutdown. The new matrix requires
47 nonnull releases (21 classes, 26 strings), 23 output lines, exact evaluation/
cleanup sequences and an empty pre-shutdown registry. Mixed/nested scopes, null/
const/empty strings, loops/switches, headers, zero/multiple iterations, recursion,
methods, primitive types, field/index reads, once-only assignment/increment effects,
caller survival, name collisions and normal exception completion are checked.
Unused active exception exits and an owning string result have emission checks only.

Checkout/path/attached branch/origin started clean; one registered worktree and no
competing active project writer shown. Schedule already uses the approved local cwd.
P0035 publication matched the parent and its [native Linux and Windows CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43689021933)
passed before work. Windows: **163 discovered, 145 passed, 18 skipped, zero failures/
errors**, **29 Go quick groups**, **5 runner tests**, Linux cross-build **15/15 steps**.
See [results](MostRecentTestResults.md). Cross-building is not native P0036 runtime evidence.

Next invocation:

1. Verify P0036 publication by trailer/remote SHA and inspect native CI. Repair
   failed checks before new work.
2. Reproduce string-valued returns retaining owned locals; design one bounded
   result-lifetime contract before adding cleanup, preserving borrowed and local
   result sources. Keep exception unwinding separate.
3. Preserve existing alias/container/destructor/field, async, exception and shape/
   operator limits and eighteen skips. String-index return typing, custom-type for
   headers and class null initializers remain limited/unsupported. Completed
   P0026-P0029 maintenance must not repeat.
