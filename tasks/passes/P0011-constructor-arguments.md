# P0011 - Check primitive constructor arguments before C emission

- Date: 2026-11-11 UTC
- Task: T006
- Parent checkpoint: `590bb63d506a367df2f163b57d1648bd6b928572`
- Commit lookup: exact `Pass-ID: P0011` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Previously `new Reader(1.5)` silently converted to a declared `int` constructor
parameter in generated C; arguments without an invoked constructor were ignored.
The semantic `ND_NEW` path now resolves the same local class `init` selected by
the emitter, checks arity and exact primitive scalar parameter types, and analyzes
every supplied argument once, including excess arguments. Parser nodes preserve
the `new` token's position for ordinary and module-entry diagnostics before emission.

Only a class's own instance `init` is a constructor. A derived class without one
takes zero arguments; explicit `base.init(...)` is preserved. Struct construction
still zero-initializes without invoking `init`, and takes zero arguments. A local
static `init` is diagnosed rather than producing an invalid generated C call.
No backend/runtime/lexer behavior is changed.

The Go matrix covers all 20 primitive mismatches in each compilation mode, arity,
default/local/inherited constructor selection, static `init`, an `init` field,
structs, second arguments, nested methods/functions/construction, conditional
promotion, void/null, a deferred pointer parameter followed by a checked scalar,
and once-only analysis of excess arguments. Each rejection requires exit 1,
line 3 with a useful column, the expected diagnostic, and no artifacts. Two valid
programs require exact runtime output for all scalar types, expression arguments,
explicit base construction, upcasting, default construction and imported values.
Two annotated Dream entries cover execution and rejection. The new Go group is
selected by both quick protocols and native CI; language/feature docs are updated.

## Validation

Starting state: approved primary checkout, attached branch/origin, clean worktree,
one registered worktree, no competing active writer in app inventory, no unpublished
pass. Restricted fetch failed DNS; approved fetch succeeded and HEAD equaled the
approved remote parent. P0010 [CI run 42816446088](https://github.com/Ddemon26/DreamCompiler/actions/runs/42816446088)
passed on Linux and Windows, including stability and Dream suites. The schedule
already uses local execution and this directory and was left unchanged.

Windows toolchain: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.
Restricted baseline could not locate the installed Zig SDK; compiler checks use
approved installed-toolchain/cache access. No SDK/cache modifications were made.

- Unchanged `zig build test`: 139 discovered, 121 passed, 18 skipped, zero
  failures/errors, 47.96s; `test_results/test_results_20261111_052841_windows.json`.
- Before implementation, `go test -count=1 -v -run
  '^TestCompiler_ConstructorArguments/ordinary/int_from_float$' .` failed correctly:
  compiler exit 0 and generated C rather than a Dream argument error.
- `zig build`: passed after implementation.
- Initial focused Go matrix failed: unsupported custom parameter syntax in the
  new nested-constructor fixture timed out in both modes at 10s, and the positive
  module fixture used names colliding with a local function/import. The nested
  fixture now uses `new Child(1.5).value` as a primitive argument and keeps its
  diagnostic/once-only assertions; positive names were corrected. A first retry
  still found the import-name collision. No assertion or exclusion was weakened.
- Final `go test -count=1 -v -run '^TestCompiler_ConstructorArguments$' .`:
  80 rejections and two exact-output programs passed, 2.830s (test body 2.41s).
- First final `zig build test`: 141 discovered, 122 passed, 18 skipped, zero
  assertion failures, one compile error, 44.34s. Both new entries passed.
  Unchanged `semantics/basic.dr` failed a Zig cache check opening SDK
  `lib/libc/mingw/lib64/api-ms-win-crt-private-l1-1-0.def` (FileNotFound).
  The referenced path was absent when checked. Focused
  `python codex/test_cli.py run --pattern '**/semantics/basic.dr'` then passed
  1/1 in 0.80s without SDK/cache/test changes. First full report:
  `test_results/test_results_20261111_053344_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  five passed, 0.038s.
- Full-suite retry: 141 discovered, 122 passed, 18 skipped, zero assertion
  failures, one compile error, 44.21s. Unchanged `inheritance_basic.dr` failed
  a Zig libzigc cache check with `file_open Unexpected` on SDK `lib/std/math/big.zig`.
  That path exists. Report: `test_results/test_results_20261111_053501_windows.json`.
- Windows Go quick from `codex/go`, `powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`: all nine groups passed, 12.690s.
  Includes 80 constructor rejections and two exact-output programs (1.85s),
  82 method rejections/two programs, 38 diagnostic cases, arena growth, warnings,
  four loop settings, 25 module cases, and literal emission/failure checks.
  External compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: passed.
  Linux binaries were not executed locally; native P0011 runtime awaits CI.
- Final unchanged `zig build test`, after quick/cross-build: 141 discovered,
  123 passed, 18 skipped, zero failures/errors, 43.93s. Both new entries passed.
  Report: `test_results/test_results_20261111_053712_windows.json`.
- Full diff review, Go formatting, new constructor anchor, source/fixture paths,
  quick/CI selection, PowerShell UTF-8 BOM and `git diff --check` passed. Local
  Markdown links resolve except the same 12 legacy broken type-system links,
  verified unchanged from the parent.

The two earlier full attempts did not pass and remain recorded above. Their
changing SDK/cache failures did not recur in the final run; the root cause is
unproven. No SDK/cache content or permissions, assertions, exclusions, implementation
or fixtures changed between those full attempts and the final passing run.

Ignored logs: `build/P0011-*.log` preserve baseline, pre-change reproduction,
initial/fixed/final focus, build, full-suite attempts and required checks.

## Limits and handoff

Pointer/array expression shapes, custom-type compatibility, primitive pointer
parameter emission, class field initializers and implicit inherited construction
remain unqualified/unsupported as documented. The custom-parameter parser stall
exposed by the initial fixture is recorded as T007, the next bounded task. The same
18 unannotated skips and existing module/graphics/concurrency exclusions remain.

Required validation passed. Publication requires committing this pass, pushing
only the approved branch and verifying the remote SHA. Inspect native CI after
publication, repairing failures first; then take T007's parser recovery task.
Resolve this pass's commit through its trailer.
