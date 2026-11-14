# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0022 - scalar string replacement copies and old-value release**.
- Record: [P0022](passes/P0022-assigned-string-ownership.md).
- Parent/fetched approved remote: `8d1897a9e91fdf35ca418e74d1e119e22b81e799` (P0021).
- Resolve the pass commit by its exact `Pass-ID: P0022` trailer.
- Publication requires push and matching `git ls-remote`; inspect native CI before
  selecting the next task.

The 2026-11-14 UTC pass started clean in the approved checkout, attached branch
and origin, one worktree, no unpublished pass or competing active project writer.
Approved fetch matched HEAD after restricted DNS failure. The automation already
uses local execution and approved cwd. P0021
[CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43569739018) passed
native Linux/Windows checks and full suite; no baseline repair was needed.

P0022 completes one T011 slice. Plain `=` on known-owned scalar identifier bindings
copies the readable replacement before releasing the current allocation, stores
the copy and yields it. This preserves self/chained/nested assignment, calls
borrowing the old value and null. Ownership metadata covers qualified copied
initializers, managed concatenation/read-line initialization and no initializer;
the latter now starts as null. Normal final block/header cleanup remains. Parameters,
fields, indexed destinations, array/pointer bindings, compound operators and other
initializer forms stay outside this boundary. Fresh replacement temporaries can
remain registered until shutdown; no return-transfer ABI is introduced.

The new ordinary/module-entry regression requires thirty-nine nonnull checked
releases and thirty-three output lines, with generated copy/release ordering,
source/copy lifetime and once-only effects. Both modes reproduced invalid literal
cleanup before the fix. The previous call fixture now requires fifteen releases,
including its replaced top-level backing. All prior allocator assertions remain.

Baseline: 132 passed/18 skipped/no failures. Final full suite:
**151 discovered, 133 passed, 18 skipped, zero failures/errors**, 33.90s. All
**nineteen Go quick groups passed**, 28.582s; Python **five passed**, 0.038s.
All fourteen focused ownership modes passed, 14.707s. Linux compiler cross-build
and harness cross-links passed; Linux binaries were not run locally. Native P0022
runtime awaits CI. See [MostRecentTestResults.md](MostRecentTestResults.md).

Same 18 skips/exclusions remain. Other initializer forms, compound assignment,
parameter/field/index destination ownership, return transfer, early return/exception
cleanup, owned body locals skipped by break/continue and array/pointer-local
cleanup remain unqualified. A new fixture initially used a local string array and
confirmed invalid scalar-style release of its stack storage; the final scalar
fixture uses borrowed pointer backing. This separate known defect is the next task.
Copies require readable input, add allocations and retain null-on-allocation-failure
behavior. General shape/bounds/operator limits and existing C warnings remain.
Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Approved SDK/cache
access was used without content/permission changes; compiler checks ran sequentially.

Next: verify P0022 delivery and native CI, repairing failures first. Otherwise
qualify T011 normal block cleanup for local string arrays and borrowed string
pointers: reproduce the invalid `dr_release` operands with the live registry,
distinguish binding/container storage from owned scalars, preserve scalar cleanup
and document element ownership. Do not start a second pass here.
