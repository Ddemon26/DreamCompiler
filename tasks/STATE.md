# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0011 - primitive scalar constructor arguments and arity**.
- Record: [P0011](passes/P0011-constructor-arguments.md).
- Parent/fetched approved remote: `590bb63d506a367df2f163b57d1648bd6b928572` (P0010).
- Resolve the pass commit by its exact `Pass-ID: P0011` trailer.
- Publication requires push and matching `git ls-remote`; report that identity after
  delivery. Inspect native CI before selecting the next task.

The 2026-11-11 pass started in the clean primary checkout at
`C:/Users/damon/cApps/DreamCompiler`, attached to the approved branch, with one
registered worktree and no competing active writer in app inventory. Approved
fetch succeeded after restricted DNS failure; HEAD equaled the fetched parent.
No unfinished pass remained. The schedule already uses local execution and this
directory; no schedule change was needed. P0010
[CI run 42816446088](https://github.com/Ddemon26/DreamCompiler/actions/runs/42816446088)
passed on Linux and Windows, including stability and Dream suites.

`new Type(...)` now checks arity and exact primitive scalar arguments against the
class's own instance `init`. Each supplied argument is analyzed once, including
excess and nested arguments. Rejections identify the type, expected/actual scalar
types and argument index, with the `new` source position, before artifacts. Local
static `init` is rejected as a constructor. Classes without local `init` and structs
take zero arguments; formerly ignored arguments are errors. Implicit inherited
construction remains unsupported; explicit `base.init(...)` is preserved. No
backend/runtime/lexer changes. Both quick protocols and CI include the new group,
and language/feature docs explain the supported boundary.

Windows toolchain: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.
Unchanged baseline: 121 passed, 18 skipped, zero failures/errors (47.96s).
Final `zig build test`: **141 discovered, 123 passed, 18 skipped, zero failures/
errors**, 43.93s. All **nine Go quick groups passed** (12.690s), including 80
constructor rejections and two exact-output programs. Python: **five passed**
(0.038s). Linux cross-build passed; native P0011 Linux runtime awaits CI.

Restricted execution could not locate Zig's SDK, so compiler checks used approved
installed-toolchain/cache access. The first two final full-suite attempts each had
one changing Zig SDK/cache compile error in unchanged entries (`semantics/basic.dr`,
then `inheritance_basic.dr`), with 122 passes and 18 skips. Focused `basic.dr` passed
1/1. Go quick and cross-build then passed; the final full run passed unchanged.
The first referenced SDK .def path was absent; the second big.zig path exists.
The cause remains unproven. No SDK/cache content or permissions, tests/assertions,
exclusions, implementation or fixtures changed between full attempts. See
[MostRecentTestResults.md](MostRecentTestResults.md) and ignored `build/P0011-*.log`.

The initial new nested-constructor fixture used unsupported custom-type parameter
syntax and timed out in both modes at 10s. It now uses the primitive expression
`new Child(1.5).value`, preserving the nested diagnostic and once-only assertions.
Positive fixture names were corrected to avoid module/function/import collisions.
The parser stall is recorded as **T007**, the next bounded task after P0011 delivery
and native CI inspection. Do not start T007 during this run.

The same 18 unannotated skips and existing module/graphics/concurrency exclusions
remain. Pointer/array expression shapes, custom-type compatibility, primitive pointer
parameter emission, field declaration initializers, all-path returns and general
CFG/SSA emission remain unqualified. P0009's literal bridge is unchanged.
