# Most Recent Test Results - DreamCompiler

**Date:** November 12, 2026 UTC

**Pass:** P0012 - parser parameter recovery and statement progress

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

## Current measured state

- Unchanged `zig build test`: **141 discovered, 123 passed, 18 skipped, zero
  failures/errors**, 43.58s. Report: `test_results/test_results_20261112_041227_windows.json`.
- Pre-change focused original-trigger regression: **two ten-second timeouts**,
  ordinary and module-entry paths, as expected. Go total 21.348s (body 20.88s).
- `zig build`: **passed** after implementation.
- Final `go test -count=1 -v -run '^TestCompiler_ParserRecovery$' .` from `codex/go`:
  **75 bounded rejections passed**, 1.907s. Covers ordinary/module-entry/dependency
  parsing, useful positions, following declarations and no generated artifacts.
- Final `zig build test`: **142 discovered, 124 passed, 18 skipped, zero failures/
  errors**, 38.82s. New custom-parameter rejection passed. Report:
  `test_results/test_results_20261112_041748_windows.json`.
- Windows Go quick protocol from `codex/go`: **all ten groups passed**, 12.752s.
  Includes 75 parser rejections (0.99s), 80 constructor rejections/two programs,
  82 method rejections/two programs, 38 diagnostics, arena growth, warnings,
  four loop settings, 25 module cases and literal emission/failure checks.
  Compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.044s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not run locally; inspect P0012 native CI after publication.
- Full diff review, `git diff --check`, all 36 local Markdown link occurrences,
  Go formatting, source/regression paths, quick/CI selections and PowerShell BOM
  checks **passed**.

Initial focused failures were test position expectations: the enclosing brace was
on line 4, then EOF name positions differed because the entry driver appends a
newline and the dependency loader does not. The fixtures now explicitly place the
brace on line 3 and EOF on line 4, retaining strict positions and all cases. No
assertion/exclusion was weakened. Restricted baseline could not locate Zig's SDK;
compiler checks used approved SDK/cache access without changing SDK/cache contents
or permissions. Restricted fetch failed DNS; approved fetch succeeded. P0011
[CI run 43022236461](https://github.com/Ddemon26/DreamCompiler/actions/runs/43022236461)
passed native Windows/Linux jobs. No intermittent SDK/cache errors recurred in
this pass's approved baseline or final checks.

The same 18 unannotated skips and existing module/graphics/concurrency exclusions
remain. Custom-type/array parameters and pointer emission/shape compatibility remain
unsupported/unqualified. Recovery does not qualify every malformed construct or
unbounded nesting. See [P0012](passes/P0012-parser-parameter-recovery.md) and
[STATE.md](STATE.md). Publication requires a matching remote SHA. Ignored evidence:
`build/P0012-*.log` includes all attempts and required checks.

---

## Historical P0011 checkpoint - November 11, 2026

**Date:** November 11, 2026 UTC

**Pass:** P0011 - primitive scalar constructor arguments and arity

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4

## Current measured state

- Unchanged baseline `zig build test`: **139 discovered, 121 passed, 18 skipped,
  zero failures/errors**, 47.96s; `test_results/test_results_20261111_052841_windows.json`.
- Pre-change constructor float-to-int rejection failed as intended: compiler
  returned 0 and generated C rather than diagnosing the mismatch.
- `zig build`: passed after implementation.
- Focused `go test -count=1 -v -run '^TestCompiler_ConstructorArguments$' .`:
  **80 rejection cases and two exact-output programs passed**, 2.830s (body 2.41s).
- First final `zig build test`: **141 discovered, 122 passed, 18 skipped, zero
  assertion failures, one compile error**, 44.34s. Unchanged `semantics/basic.dr`
  hit a Zig cache FileNotFound opening SDK
  `lib/libc/mingw/lib64/api-ms-win-crt-private-l1-1-0.def`. That path was absent
  when checked. Focused `python codex/test_cli.py run --pattern
  '**/semantics/basic.dr'` then passed **1/1**, 0.80s. Report:
  `test_results/test_results_20261111_053344_windows.json`.
- Full retry: **141 discovered, 122 passed, 18 skipped, zero assertion failures,
  one compile error**, 44.21s. Unchanged `inheritance_basic.dr` hit libzigc
  `file_open Unexpected` on SDK `lib/std/math/big.zig`; that path exists.
  Report: `test_results/test_results_20261111_053501_windows.json`.
- Windows Go quick from `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass
  -File ./build_and_test.ps1 quick`: **all nine groups passed**, 12.690s. Includes
  80 constructor rejections/two exact-output programs (1.85s), 82 method rejections/
  two programs, 38 diagnostic cases, arena growth, warnings, four loop settings,
  25 modules, and literal emission/failure checks. Compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.038s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; inspect native P0011 CI after publication.
- Final unchanged `zig build test` after quick/cross-build: **141 discovered,
  123 passed, 18 skipped, zero failures/errors**, 43.93s. Both new Dream entries
  passed. Report: `test_results/test_results_20261111_053712_windows.json`.
- Full diff review, local links, new constructor anchor, source paths, Go formatting,
  quick/CI selection, PowerShell BOM and `git diff --check` passed. Twelve pre-existing
  broken type-system links are unchanged from the parent.

The earlier full attempts did not pass; the changing SDK/cache failures did not
recur in the final run, and their cause remains unproven. No SDK/cache content or
permissions, assertions, exclusions, implementation or fixtures changed between
full attempts and the final pass. Restricted execution could not locate Zig's SDK;
compiler checks used approved installed-toolchain/cache access. Restricted fetch
failed DNS; approved fetch passed. P0010 Linux/Windows CI run 42816446088 passed.

The initial new focused fixtures exposed unsupported custom-type parameter syntax
that stalls parsing, and module/function/import name collisions. The nested test
now uses the supported primitive expression `new Child(1.5).value`; diagnostic and
once-only assertions remain intact. Positive names were corrected, with no weakened
assertions or exclusions. T007 records parser recovery as the next task.

The same 18 unannotated skips and existing module/graphics/concurrency exclusions
remain. Pointer/array shapes, custom-type compatibility, primitive pointer parameter
emission, field initializers and implicit inherited construction remain unqualified/
unsupported. See [P0011](passes/P0011-constructor-arguments.md) and [STATE.md](STATE.md).
Publication requires the pass commit and matching remote SHA. Ignored evidence:
`build/P0011-*.log`, including baseline, pre-change, focused attempts, full-suite
attempts, quick, Python and cross-build.

---

## Historical P0010 recovery checkpoint - November 10, 2026

**Date:** November 10, 2026 UTC

**Pass:** P0010 - primitive scalar method arguments; validation recovered

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0

## Current measured state

- `zig build test`: **139 discovered, 121 passed, 18 skipped, zero failures/errors**,
  47.48s. Report: `test_results/test_results_20261110_052829_windows.json`.
  Both new annotated method-argument entries passed.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all eight groups passed**, 10.964s. The protocol
  rebuilds the compiler and uses `-count=1`. It includes **82 method argument
  rejections and two exact-output programs** (2.19s focused group), 38 diagnostic
  cases, arena growth, nonfatal warnings, four loop optimization settings,
  25 module cases, and literal emission/failure checks.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.052s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; inspect native CI after publication.
- Full diff review and `git diff --check` passed. All **29 local link occurrences**
  resolve; 12 legacy broken links in the type-system guide are unchanged from HEAD.
  The method anchor, regression paths, quick/CI selection, PowerShell BOM, and Go
  formatting were verified. Publication requires a matching remote SHA.

This recovers the existing P0010 pass; no new feature or pass ID was started.
Restricted fetch failed DNS; approved fetch passed and the remote still matched
P0009. Restricted `zig build test` could not locate the installed SDK; compiler
checks used approved SDK/cache access. Earlier changing SDK/cache file-open errors
did not recur. Their cause remains unproven. No SDK/cache content or permissions,
tests, assertions, exclusions, or implementation changed during recovery.
Ignored evidence: `build/P0010-recovery-{full-suite,full-suite-approved,go-quick,
python,linux-cross-build}.log`.

The same 18 unannotated entries remain skipped/unverified; supporting module fixtures
and graphics/concurrency exclusions are unchanged. Pointer/array/custom method
argument shapes, implicit constructor checks, and pointer parameter C emission remain
unqualified. See [P0010](passes/P0010-method-arguments.md) and [STATE.md](STATE.md)
for historical failures, scope, and next steps. P0009 CI run 42481471439 remains
successful; P0010 native CI must be inspected after publication.

---

## Historical P0010 implementation checkpoint - November 9, 2026

**Date:** November 9, 2026 UTC

**Pass:** P0010 - primitive scalar method arguments; **validation/delivery blocked**

**Branch/platform:** `codex/compiler-stabilization`, primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0

## Current measured state

- Unchanged `zig build test`: **137 discovered, 119 passed, 18 skipped, zero
  failures/errors**, 47.31s.
- Pre-change focused Go float-to-int rejection failed: compiler returned 0 and
  created C instead of issuing a Dream argument diagnostic.
- `zig build`: **passed** after implementation.
- Focused `go test -count=1 -v -run '^TestCompiler_MethodArguments$' .`: **80
  rejections and two exact-output programs passed**, 2.925s. Final quick adds a
  nested-invalid-argument case in each mode for **82 rejections**.
- First final `zig build test`: **139 discovered, 120 passed, 18 skipped, zero
  assertion failures, one compile error**, 44.64s. Unchanged `test_debug_simple.dr`
  failed Zig mingw cache `file_open Unexpected` on SDK `misc/__winitenv.c`.
- Referenced SDK file exists. Focused `python codex/test_cli.py run --pattern
  '**/test_debug_simple.dr'`: **1 passed**, no skips/failures/errors, 0.81s.
- Full-suite retry: **139 discovered, 119 passed, 18 skipped, zero assertion
  failures, two compile errors**, 50.37s. Unchanged `string_array.dr` and
  `global_release.dr` hit the same mingw error on SDK `libsrc/power-uuid.c` and
  `gdtoa/g_xfmt.c`; both files exist. **Neither full run passed.** Latest report:
  `test_results/test_results_20261109_053831_windows.json`.
- Windows Go quick protocol: **failed**, 11.233s. Default loop compilation hit
  Zig `libzigc` cache `file_open Unexpected` on SDK `std/zig/string_literal.zig`.
  The `-O1`/`-O2`/`-O3` loop cases passed. Seven other groups passed: 38 diagnostics,
  arena growth, warnings, 25 module cases, literal emission/failures, and method
  arguments (**82 rejections and two exact-output programs**). All external
  compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.045s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; no P0010 commit/native CI exists.

Both new annotated entries passed in both complete runs. The initial new positive
fixture failed because existing `int*` method parameters emit C `int`, so null was
rejected by the host compiler. That deferred pointer assumption was removed from
the scalar fixture; a separate negative case with a pointer parameter and mismatched
second scalar requires exactly one scalar diagnostic. Pointer/array emission and
shape compatibility are not qualified by this pass. No existing test was removed,
excluded, or weakened.

Restricted baseline could not locate Zig's installation; compiler checks used
approved toolchain/cache access. SDK/cache contents and permissions were unchanged.
The changing file-open failures resemble earlier checkpoints but their cause is
unproven. Required full/quick validation must pass before P0010 commit/publication.
Preserve the uncommitted work; recover this pass instead of allocating another ID.
Ignored evidence: `build/P0010-{baseline,baseline-approved,rejection-before,build,
focused-go,focused-go-fixed,full-suite,focused-debug,full-suite-retry,python,
go-quick,linux-cross-build}.log`.

P0009 [CI run 42481471439](https://github.com/Ddemon26/DreamCompiler/actions/runs/42481471439)
passed on Linux and Windows before this pass. The same 18 entry skips, supporting
modules, and graphics/concurrency exclusions remain. See
[P0010](passes/P0010-method-arguments.md) for recovery and scope limits.

---

## Historical P0009 test checkpoint

**Date:** November 8, 2026 UTC

**Pass:** P0009 - checked literal arithmetic reaches emitted C/object code

**Branch/platform:** `codex/compiler-stabilization`, attached primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0

## Current measured state

- Unchanged baseline `zig build test`: **136 discovered, 118 passed, 18 skipped,
  zero failures/errors**, 42.56s.
- Final `zig build test`: **137 discovered, 119 passed, 18 skipped, zero failures/
  errors**, 39.12s. New annotated literal regression checks 49, 65, 2147483646, 0.
  Report: `test_results/test_results_20261108_053647_windows.json`.
- Focused `python codex/test_cli.py run --pattern '**/literal_arithmetic.dr'`:
  **1 passed**, zero skips/failures/errors, 0.79s.
- Focused Go `go test -count=1 -v -run 'TestCompiler_Literal(Emission|Failures)$' .`
  from `codex/go`: **passed**, 3.508s after the object header fix.
- Windows Go quick protocol from `codex/go`, `powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`: **all seven groups passed**, 9.343s,
  including 38 diagnostic cases, arena growth, warnings, four loop settings,
  25 module cases, literal emission, and literal internal failures.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.046s.
- `zig build` and `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`:
  **passed**. Linux executables were not run locally; P0009 native behavior awaits CI.

The emission matrix inspects generated C before host compilation and then compares
exact output at default/`-O1`/`-O2`/`-O3`. It covers nested/zero/boundary arithmetic,
free functions, methods, imported functions/source mapping, side effects, unsupported
whole-tree fallback, accepted/rejected operation/depth bounds, and object success/
nonzero host failure. Undefined overflow/division-by-zero examples are inspected
only. The UBSan C harness checks controlled SCCP/arena allocation failures, a
transformed-root mismatch, bounded nonconvergence, metadata, and atomic replacement.
Fault controls exist only in test code, with no production bypass paths.

Initial focused checks passed C/runtime settings and the allocation harness but
failed object compilation on missing `../libs/console.h` from temporary C. The common
emitter's header paths and object status propagation were fixed; final focused and
quick checks passed. No test exclusions or assertions were weakened.
Restricted baseline could not access Zig's installation directory; approved installed
toolchain/cache access passed. No SDK/cache repair or deletion was performed.

Ignored logs: `build/P0009-{baseline,baseline-approved,build,focused-go,
focused-go-fixed,focused-dream,full-suite,python,go-quick,linux-cross-build}.log`.

P0008 CI run [42373515160](https://github.com/Ddemon26/DreamCompiler/actions/runs/42373515160)
succeeded on Linux and Windows before this pass. P0009's local Linux cross-build
is separate evidence. The same 18 unannotated entries remain skipped/unverified;
supporting modules and graphics/concurrency exclusions are unchanged. General IR
emission/SSA and loop transformations remain unqualified. See
[P0009](passes/P0009-literal-emission.md) for limits and the delivery handoff.

---

## Historical P0007 test checkpoint

**Date:** November 6, 2026 UTC (local reports also use November 6)

**Pass:** P0007 - optimized loop compilation stability and one checked skipped entry

**Branch/platform:** `codex/compiler-stabilization`, attached primary checkout, Windows

**Toolchain:** Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0

## Current measured state

- Unchanged baseline `zig build test`: **136 discovered, 117 passed, 19 skipped,
  zero failures/errors** (49.32s). The loop fixture was skipped for missing metadata.
- Final `zig build test`: **136 discovered, 118 passed, 18 skipped, zero failures/
  errors** (42.32s). The same loop source now checks results 112 and 8 at `-O2`.
  Report: `test_results/test_results_20261106_053355_windows.json`.
- Focused `python codex/test_cli.py run --pattern '**/test_loop_opt.dr'`: **1 passed**,
  zero skips/failures/errors (0.85s).
- Focused Go `go test -count=1 -v -run '^TestCompiler_LoopsAtOptimizationLevels$' .`
  from `codex/go`: **default, -O1, -O2, -O3 all passed**, printing exactly 112 and 8
  (7.60s test body, 8.141s Go result).
- Windows Go quick protocol from `codex/go`, `powershell -NoProfile -ExecutionPolicy
  Bypass -File ./build_and_test.ps1 quick`: **all five groups passed**, including
  38 diagnostic cases, arena growth, nonfatal warnings, four loop optimization settings,
  and 25 module cases (`-count=1`, 7.065s).
- `python -m unittest discover -s codex/python -p test_test_manager.py`: **5 passed**
  (0.045s).
- `zig build` and `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally. P0007 native Linux behavior awaits CI.
- Diff review, local handoff links, quick/CI selection, PowerShell BOM preservation,
  and `git diff --check` checked before commit.

The new Go matrix initially failed at `-O1`/`-O2`/`-O3` with heap corruption exit
`0xc0000374`; default succeeded. SCCP negative table indices and LICM shared instruction
ownership were repaired. Intermediate `-O2` runs stalled because LICM/loop analysis
reported changes without transformations; only this run's identified processes were
stopped. An intermediate rebuild hit AccessDenied while its executable was still
running; it passed after that process stopped. The final checks above all passed.
No test exclusions or assertions were weakened. Full details and the remaining 18
unannotated entries are in [P0007](passes/P0007-loop-optimization-stability.md).

Restricted execution could not locate Zig's installation; compiler checks used approved
installed-toolchain/cache access. No SDK/cache repair or deletion was performed. Logs:
`build/P0007-{baseline,build,focused-go,focused-go-sccp,focused-go-fixed,focused-dream,
full-suite,go-quick,python,linux-cross-build}.log`. Intermediate failures are diagnostic
evidence, not successful validation. Optimized IR still does not feed C emission, so
these results do not prove optimizer transformations or performance gains.

Supporting modules remain fixtures; four graphics and four concurrency examples remain
outside the suite. Recursive legacy Go/cgo tests remain unsupported. P0006 CI run
42135769585 succeeded on Linux and Windows before this pass began. P0006 recovery
results were 117 passes, 19 skips, zero failures/errors (46.57s), all four then-current
Go quick groups, five runner tests, and a Linux cross-build; those historical recovery
results are recorded in [P0006](passes/P0006-member-access-diagnostics.md).

## Historical P0006 implementation checks - November 5, 2026

- Unchanged baseline `zig build test`: **133 discovered, 114 passed, 19 skipped,
  0 failed, 0 errors** (41.39s).
- First post-change `zig build test`: **136 discovered, 115 passed, 19 skipped,
  0 assertion failures, 2 compile errors** (37.85s). Unchanged `method.dr` and
  `multi_decl.dr` hit Zig libubsan/cache `file_open Unexpected` errors referencing
  SDK files `lib/std/os/linux/powerpc.zig` and `lib/std/array_hash_map.zig`.
- Full-suite retry: **136 discovered, 116 passed, 19 skipped, 0 assertion failures,
  1 compile error** (37.06s). Unchanged `bool_array.dr` hit a libubsan/cache file-open
  error referencing `lib/std/Build/Step/CheckFile.zig`. Report:
  `test_results/test_results_20261105_053133_windows.json`. **Neither attempt passed.**
- From `codex/go`, `go test -count=1 -v -run
  'TestCompiler_(DiagnosticExitStatus|Modules)$' .`: **38 diagnostic and 25 module
  cases passed**, including useful source locations and no artifacts for rejections.
- Windows Go quick protocol from `codex/go`,
  `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`:
  first run failed in unchanged module case `entry_class` with a mingw-w64 crt2.o
  cache file-open error referencing `lib/include/avx512vlbf16intrin.h`.
  **Retry passed all four groups** (38 diagnostics, arena growth, nonfatal warnings,
  25 modules; compiler tests use `-count=1`).
- `python codex/test_cli.py run --pattern '**/*member*.dr'`: **6 discovered, 6 passed,
  0 skipped, 0 failed, 0 errors** (1.51s).
- `python -m unittest discover -s codex/python -p test_test_manager.py`: **5 passed**.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **succeeded**.
  Linux executables were not run locally. P0005 CI run 41833228164 succeeded;
  P0006 has not been published or checked by CI.
- Diff review, local handoff link checks, and `git diff --check` passed.

Compiler checks used approved installed-toolchain access after the initial restricted
`zig build test` could not locate Zig's installation directory. Restricted fetch first
failed DNS resolution; approved fetch succeeded. The first three referenced SDK files
were present when checked. The changing file-open errors resemble P0005's earlier
errors; their root cause remains unproven. No SDK/cache repair, deletion, or override
was performed. No tests were excluded or assertions weakened.

Ignored logs: `build/P0006-baseline.log`, `build/P0006-focused-go.log`,
`build/P0006-full-suite.log`, `build/P0006-full-suite-retry.log`,
`build/P0006-go-quick.log`, `build/P0006-go-quick-retry.log`,
`build/P0006-focused-dream.log`, and `build/P0006-linux-cross-build.log`.

The same 19 unannotated entries remain skipped; supporting modules remain fixtures.
Four graphics and four concurrency fixtures remain outside the regular suite.
Legacy recursive cgo tests remain unsupported; use the Go quick protocol.
Method argument type/shape checking and class field declaration initializer analysis
remain incomplete. See [P0006](passes/P0006-member-access-diagnostics.md) and
[STATE.md](STATE.md) for the recovered validation and publication handoff.

## Historical Windows report - July 20, 2025

**Date:** July 20, 2025  
**Platform:** Windows  
**Test Runner:** Python test_cli.py  
**Total Tests Discovered:** 107  

## Test Summary

### Overall Results
- **Total Tests Run:** ~89 (excluding graphics timeout)
- **Pass Rate:** ~95% for completed tests
- **Test Categories:** 7 main categories
- **Notable Issues:** Graphics tests timeout after 30 seconds

### Test Categories Status

#### ✅ BASICS (All Passing)
- **Arithmetic:** 8/8 tests passing
  - arithmetic.dr, compound_assign.dr, division.dr, modulo.dr, multiplication.dr, negative_numbers.dr, subtraction.dr, unary_plus.dr
- **Bitwise:** 4/4 tests passing
  - bitnot.dr, bitwise_and_or.dr, shift.dr, xor.dr
- **Expressions:** 5/5 tests passing
  - grouping_precedence.dr, mixed_precedence.dr, parentheses.dr, power_assoc.dr, ternary.dr
- **I/O:** 3/3 tests passing
  - console.dr, readline.dr, write.dr
- **Memory:** 2/2 tests passing
  - global_release.dr, refcount.dr
- **Strings:** 4/4 tests passing
  - string_concat.dr, string_escape.dr, string_output.dr, string_var.dr
- **Types:** 3/3 tests passing
  - bool.dr, char.dr, float.dr
- **Variables:** 3/3 tests passing
  - multi_decl.dr, var_infer.dr, variables.dr

#### ✅ CONTROL FLOW (All Passing)
- **Comparisons:** 3/3 tests passing
  - comparison.dr, equality.dr, inequality.dr
- **Conditionals:** 4/4 tests passing
  - else_if.dr, if.dr, if_block.dr, if_else.dr
- **Exceptions:** 3/3 tests passing
  - finally.dr, nested.dr, try_catch.dr
- **Increment:** 2/2 tests passing
  - increment.dr, increment_expression.dr
- **Logical:** 2/2 tests passing
  - logical.dr, logical_not.dr
- **Loops:** 6/6 tests passing
  - break.dr, break_continue.dr, do_while.dr, for.dr, while.dr, while_block.dr
- **Switches:** 1/1 tests passing
  - switch.dr

#### ✅ FUNCTIONS (All Passing)
- **Definitions:** 3/3 tests passing
  - functions.dr, typed_return.dr, user_main.dr
- **Exceptions:** 1/1 tests passing
  - propagate.dr
- **Parameters:** 1/1 tests passing
  - function_params.dr

#### ✅ ADVANCED (High Pass Rate)
- **Concurrency:** 1/4 tests passing (async.dr passes, others have issues)
- **Data Structures:** 6/7 tests passing
  - array.dr, bool_array.dr, char_array.dr, float_array.dr, new_struct.dr, struct.dr (all pass)
  - string_array.dr has issues
- **OOP:** 12/12 tests passing
  - class.dr, constructor_call.dr, inheritance_assignment_compatibility.dr, inheritance_base_access.dr, inheritance_basic.dr, inheritance_multilevel.dr, method.dr, new_object.dr, object.dr, program_main.dr, static_member.dr, type.dr
- **SSA:** 1/1 tests passing
  - constant_prop.dr

#### ✅ DEBUG (Most Passing)
- **Line Directives:** 1/1 tests passing
- **Debug Tests:** 1/3 tests clearly passing
  - test_debug.dr passes
  - test_debug_complex.dr, test_debug_simple.dr have unclear status

#### ⚠️ GRAPHICS (Timeout Issues)
- **All 4 tests timeout after 30 seconds**
- Tests: vulkan_helpers.dr, vulkan_init.dr, vulkan_init_simple.dr, vulkan_pipeline.dr
- Issue: Compilation appears to hang during graphics test processing

#### ❓ ADDITIONAL CATEGORIES
- **Hottests:** 5 tests (status unclear)
- **Multifile:** 3 tests (status unclear)  
- **Optimization:** 1 test (status unclear)
- **Semantics:** 3 tests (status unclear)

## Detailed Test Execution Info

### Performance Metrics
- **Average compile time:** ~0.38-0.40 seconds per test
- **Average runtime:** ~0.01 seconds per test
- **Total test duration:** Varies by category (4.8s for OOP category of 12 tests)

### Known Issues
1. **Graphics Tests Timeout:** All vulkan-related tests timeout after 30 seconds during compilation
2. **Concurrency Issues:** 3/4 async-related tests show problems
3. **String Array Issues:** string_array.dr not passing reliably
4. **Some Debug Tests:** Mixed results in debug category

### Platform-Specific Notes
- **Windows Platform:** Tests generally perform well
- **Test Infrastructure:** Python-based runner working correctly
- **Compiler Build:** Zig build system functional
- **Runtime Integration:** Memory management and I/O systems working

## Test Environment Details
- **Compiler:** DreamCompiler (C frontend)
- **Build System:** Zig 0.15.0-dev.936+fc2c1883b
- **Python Version:** 3.13.5
- **PowerShell:** 5.1.26100.4652
- **Test Results Location:** `test_results/` directory with JSON files

## Recommendations for Post-Pull Comparison
1. Compare pass rates for each category
2. Monitor graphics test timeout behavior
3. Check if concurrency issues persist
4. Verify OOP functionality remains stable (currently 100% pass rate)
5. Track any new failures in basics/control_flow (currently 100% pass rate)

---
*Generated on July 20, 2025 at 23:40 by automated test analysis*
