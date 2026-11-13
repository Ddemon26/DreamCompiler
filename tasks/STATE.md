# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0018 - give identifier string declarations independent storage**.
- Record: [P0018](passes/P0018-identifier-string-ownership.md).
- Parent/fetched approved remote: `f15eb03d52203e02bdd6c1d7417412b36c435cf4` (P0017).
- Resolve the pass commit by its exact `Pass-ID: P0018` trailer.
- Publication requires push and matching `git ls-remote`; report that identity
  after delivery. Inspect native CI before selecting the next task.

The 2026-11-13 pass started clean in the approved primary checkout, attached branch
and origin, one worktree, no unpublished pass or competing active project writer.
Approved fetch matched HEAD after restricted DNS failure. Automation uses local
execution and the approved cwd. P0017 native
[CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43489193804) passed
Linux and Windows jobs; no baseline repair was needed.

P0018 completes one T011 slice. Scalar declarations initialized directly from
identifiers now call `dream_string_copy`, preserving source ownership and giving
each nonnull declaration independent storage for existing block cleanup. Local,
top-level and borrowed parameter sources, nested chains, const/grouped bindings,
free/static/instance functions, loops and null are covered. Concatenation emission
and the allocator remain unchanged. The ordinary/module-entry matrix checks nine
identifier-copy sites, ten release sites, twelve actual releases and nineteen
exact output lines per mode. The shared helper also proves copy lifetime after
backing release. Prior indexed/literal coverage remains; quick protocols and CI
select the new matrix. Broader ownership remains open.

Unchanged baseline: 128 passed/18 skipped/no failures. Both new modes reproduced
invalid cleanup with the checked live registry before implementation; all six
ownership modes passed afterward. Final full suite: **147 discovered, 129 passed,
18 skipped, zero failures/errors**, 47.33s. All **fifteen Go quick groups passed**,
23.940s; Python **five passed**, 0.047s. Linux compiler cross-build and both new
harness mode cross-links passed; Linux binaries were not run locally. Native
P0018 runtime awaits CI. See [MostRecentTestResults.md](MostRecentTestResults.md).

Same 18 skips/exclusions remain. Call/member/for-header initializers,
reassignment/return/field ownership, array/pointer-local and early-exit cleanup
remain unqualified. Copies add allocation cost and require readable input.
General shape/bounds/operator correctness remains separate. Existing harness
const-discard warnings remain. Windows tools: Zig 0.17.1, re2c 3.0, Python
3.13.17, Go 1.27.4. Approved SDK/cache access used without content/permission
changes; compiler checks sequential.

Next: verify P0018 delivery and inspect native CI, repairing failures first.
Otherwise take a bounded **T011 member-expression initializer** slice: reproduce
a local borrowing literal/managed field storage with the checked allocator,
retain cleanup and prove source/copy lifetimes in both modes. Do not start a
second pass here.
