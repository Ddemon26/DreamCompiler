# Most Recent Test Results - DreamCompiler

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
