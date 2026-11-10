# P0010 - Check primitive scalar method arguments before C emission

- Date: 2026-11-09; recovered 2026-11-10 UTC
- Task: T005
- Parent checkpoint: `37d310c626b9af347343baf214a02a46b2e07081`
- Commit lookup: exact `Pass-ID: P0010` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Close the gap where `reader.take(1.5)` silently converted a float to an `int`
parameter through generated C. The real semantic method-call path now resolves
the receiver/member once and checks each supplied argument once against matching
primitive scalar parameters. `int`, `float`, `bool`, `char`, and `string` must match
exactly, with existing expression promotions retained and no boundary conversion.
Diagnostics include the one-based argument index, method, expected/actual type,
and call position before outputs are opened. Existing receiver/access and arity
checks remain, and excess arguments still receive expression analysis.

Tests cover ordinary and multi-file entry sources, all 20 distinct primitive
type mismatches, inherited instance/static methods, static access through objects,
`this`/`base`, explicit `init`, declared receiver/overridden signature selection,
struct methods, second arguments, nested calls, mixed numeric conditionals,
void/null values, and arity errors. Each rejection requires exit 1, line 3 with a
useful column, the expected diagnostic without duplicate type diagnostics, and no
new build artifacts. Two positive programs require exact runtime output, including
all primitive types, expressions, upcasts, inherited calls, and imported values.
Two annotated Dream entries supply an executable and a rejection regression.

The new Go group is selected by both quick protocols and Linux/Windows CI. Feature,
function/type documentation, source map, and durable handoff describe the boundary.
No generated lexer or runtime/backend code changes are involved.

## Validation

Starting state: primary checkout, attached approved branch/origin, clean worktree,
one registered worktree, and no competing writer in app inventory. History and
preceding memory showed P0009 delivered with no unfinished pass. The schedule
already used local execution and this directory, so it was unchanged. Restricted
fetch failed DNS; approved `git fetch origin codex/compiler-stabilization` passed,
and HEAD equaled the fetched parent. P0009
[CI run 42481471439](https://github.com/Ddemon26/DreamCompiler/actions/runs/42481471439)
passed on Linux and Windows, including stability and Dream suites.

Windows toolchain: Zig **0.17.1**, re2c 3.0, Python 3.13.15, Go 1.27.0.
Restricted baseline could not locate Zig's installation; compiler checks used
approved installed-toolchain/cache access. No SDK or cache changes were made.

- Unchanged `zig build test`: **137 discovered, 119 passed, 18 skipped, zero
  failures/errors**, 47.31s.
- Before implementation, focused `go test -count=1 -v -run
  '^TestCompiler_MethodArguments/ordinary/int_from_float$' .` failed correctly:
  the compiler returned 0 and created C instead of rejecting the argument.
- `zig build`: passed after the semantic change.
- Initial focused matrix: all rejection cases passed, but both new positive
  fixtures failed host compilation because `int*` method parameters are emitted
  as C `int`. The new fixture had incorrectly assumed this deferred shape worked.
  The unchanged backend explains the failure. Corrected the scalar execution
  fixture by removing that unsupported call; retained a rejection case with a deferred
  pointer parameter and a mismatched second scalar argument, asserting exactly
  one type diagnostic. This qualifies neither pointer emission nor compatibility.
- Focused `go test -count=1 -v -run '^TestCompiler_MethodArguments$' .`:
  **80 rejections and two exact-output positive programs passed**, 2.925s. A later
  nested-invalid-argument case is included in the final quick check.
- First final `zig build test`: **139 discovered, 120 passed, 18 skipped, zero
  assertion failures, one compile error**, 44.64s. Unchanged `test_debug_simple.dr`
  hit Zig mingw cache `file_open Unexpected` on SDK `misc/__winitenv.c`.
  That file exists; focused `python codex/test_cli.py run --pattern
  '**/test_debug_simple.dr'` passed **1/1**, 0.81s, without changing tests/caches.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.045s.
- Full-suite retry: **139 discovered, 119 passed, 18 skipped, zero assertion
  failures, two compile errors**, 50.37s. Unchanged `string_array.dr` and
  `global_release.dr` hit the same mingw cache error on SDK `libsrc/power-uuid.c`
  and `gdtoa/g_xfmt.c`. Both referenced files exist. Report:
  `test_results/test_results_20261109_053831_windows.json`. **Neither full run passed.**
- Windows Go quick, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`: **failed**, 11.233s. Seven of eight
  groups passed, including **82 method argument rejections and two exact-output
  programs**, 38 existing diagnostic cases, arena growth, warnings, 25 module cases,
  literal emission, and literal failure harness. The default loop case failed in
  Zig's `libzigc` cache on SDK `std/zig/string_literal.zig`; the `-O1`/`-O2`/`-O3`
  loop cases passed. All external-compiler checks use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux executables were not run locally. No P0010 commit/CI exists yet.
- All introduced local links resolve; 27 local Markdown link occurrences resolve
  across 10 changed/new documents. The broader audit found 12 pre-existing broken
  link occurrences in `docs/fundamentals/type-system.md`, verified unchanged from
  HEAD and left outside this compiler pass. New method anchor, source paths,
  quick/CI selection, PowerShell BOM, full diff review, and `git diff --check` pass.
  Commit, push, and remote-SHA verification are blocked by the required failed
  validation; no publication was attempted.

Ignored `build/P0010-*.log` files preserve baseline, pre-change rejection,
initial/fixed focus, first full-suite error, focused debug retry, and final checks.

## Recovery validation - 2026-11-10 UTC

Resumed only the documented P0010 files in the existing primary checkout, with
HEAD attached to `codex/compiler-stabilization`. Approved origin and the single
registered worktree are unchanged; no competing writer appeared in app inventory.
Restricted fetch failed DNS; approved fetch passed and both HEAD and the approved
remote still equal the parent above. No P0010 commit existed. The schedule already
uses local execution and this project directory; no schedule change was necessary.
P0009 CI run 42481471439 was rechecked and remains successful.

The same toolchain versions above were verified. Restricted `zig build test`
could not find the installed SDK. With approved SDK/cache access:

- `zig build test`: **139 discovered, 121 passed, 18 skipped, zero failures/errors**,
  47.48s. Both new entries passed. Report:
  `test_results/test_results_20261110_052829_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all eight groups passed**, 10.964s. This rebuilds
  the compiler and runs with `-count=1`, including 82 method argument rejections
  and two exact-output programs (focused group 2.19s), 38 diagnostic cases,
  arena growth, nonfatal warnings, four loop settings, 25 module cases, and literal
  emission/failure checks.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.052s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; inspect native CI after publication.

The changing SDK/cache errors did not recur. Their cause remains unproven; no SDK,
cache, permissions, tests, exclusions, assertions, or implementation were changed
to obtain these recovery results. Ignored evidence is stored in
`build/P0010-recovery-{full-suite,full-suite-approved,go-quick,python,linux-cross-build}.log`.
The November 9 failures above remain historical evidence, superseded by these
passing required checks. Full diff review and `git diff --check` passed. All 29
local link occurrences resolve; 12 legacy broken links in the type-system guide
are unchanged from HEAD. Method anchor, regression paths, quick/CI selection,
PowerShell BOM, and Go formatting were verified. Publication still requires the
push and matching remote SHA.

## Limits and handoff

This checks scalar type tokens, not complete expression shapes. Pointer parameters
are excluded; array/pointer actuals with matching base tokens remain unqualified.
Custom-type compatibility, implicit `new Type(...)` argument checks, broader
operator validity, and class field initializers remain separate work. The pointer
backend defect above remains deferred. Existing 18 unannotated entry skips,
supporting module fixtures, and graphics/concurrency exclusions are unchanged.

**Validation recovered:** commit with the P0010 trailer, push only the approved
branch, and verify its SHA before calling this pass delivered. Resolve its commit
through the trailer; the record cannot contain its own SHA. Inspect native CI after
publication, repairing a failed baseline first. A useful next bounded task is
primitive implicit-constructor arguments through resolved `init`, with arity and
artifact-free diagnostics. Do not start that task in this recovery run.
