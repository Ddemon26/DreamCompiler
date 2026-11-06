# P0006 - Diagnose invalid member receivers and instance access

- Date: 2026-11-05; recovery 2026-11-06 UTC
- Task: T002 (bounded receiver/access slice)
- Status: validation recovered; verify publication through the commit trailer and remote ref
- Parent checkpoint: `fbd5458a7c5889a453fcb8d63549e151dc331f07`
- Commit lookup: exact `Pass-ID: P0006` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Analyze member-call receivers and arguments before C emission. Diagnose undefined
and non-object receivers, unknown methods/fields, and type-qualified instance fields
or methods. Preserve source positions for member and base access instead of reporting
0:0. Keep inherited instance/base calls, struct field access, static members through
types or simple object variables, and imported function calls working.

Changes are in `src/sem/analysis.c` and `src/parser/expr.inc`. Fifteen new ordinary
Go diagnostic cases require line 3 and a positive column, exit 1, and no generated
build directory. Five module cases cover a valid inherited entry class, three entry
rejections with source filenames/positions, and a dependency receiver rejection.
Three annotated Dream files add executable member access and two rejections.
The type-system guide and feature inventory describe the checked boundary.

## Validation

Existing primary checkout, attached approved branch, clean start, approved origin,
and only one registered worktree verified. Restricted fetch could not resolve GitHub;
normal approved escalation succeeded. HEAD and fetched remote matched the parent.
P0005 CI run 41833228164 completed successfully. Existing schedule configuration uses
local execution and `C:/Users/damon/cApps/DreamCompiler`; no schedule changes needed.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0.
Compiler checks below used approved installed-toolchain access. The initial restricted
`zig build test` could not locate the Zig installation directory.

- Unchanged baseline `zig build test`: 133 discovered, 114 passed, 19 skipped,
  0 failed, 0 errors (41.39s). Log: `build/P0006-baseline.log`.
- Before implementation, focused Go cases `member_missing_receiver` and
  `member_type_field_read` failed because invalid sources exited 0 and generated C;
  `member_type_method` failed its position assertion with 0:0.
- `zig build`: passed after implementation.
- From `codex/go`, `go test -count=1 -v -run
  'TestCompiler_(DiagnosticExitStatus|Modules)$' .`: passed, 38 diagnostic cases and
  25 module cases. Log: `build/P0006-focused-go.log`.
- First post-change `zig build test`: 136 discovered, 115 passed, 19 skipped,
  0 assertion failures, 2 compile errors (37.85s). Unchanged `method.dr` and
  `multi_decl.dr` hit Zig libubsan cache `file_open Unexpected` errors referencing
  SDK files `lib/std/os/linux/powerpc.zig` and `lib/std/array_hash_map.zig`.
  Log: `build/P0006-full-suite.log`.
- Full-suite retry: 136 discovered, 116 passed, 19 skipped, 0 assertion failures,
  1 compile error (37.06s). Unchanged `bool_array.dr` hit the same libubsan/cache
  error referencing `lib/std/Build/Step/CheckFile.zig`. Log:
  `build/P0006-full-suite-retry.log`; report:
  `test_results/test_results_20261105_053133_windows.json`. Neither run passed.
- Windows Go quick protocol from `codex/go`,
  `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`:
  first run failed in unchanged module case `entry_class` with a Zig mingw-w64 crt2.o
  cache file-open error for `lib/include/avx512vlbf16intrin.h`. Retry passed all four
  groups: 38 diagnostics, arena growth, nonfatal warnings, 25 modules (`-count=1`).
  Logs: `build/P0006-go-quick.log`, `build/P0006-go-quick-retry.log`.
- `python codex/test_cli.py run --pattern '**/*member*.dr'`: 6 discovered,
  6 passed, no skips/failures/errors (1.51s). Log: `build/P0006-focused-dream.log`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`: 5 passed.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: passed.
  Log: `build/P0006-linux-cross-build.log`. Linux executables were not run locally.
- The first three referenced SDK files were present when checked. Changing errors
  resemble P0005's intermittent failures; their cause remains unproven. No SDK/cache
  files were deleted, overridden, or repaired; no assertions or exclusions changed.
- Implementation, regressions, and documentation reviewed; local handoff links checked;
  `git diff --check` passed. Raw logs/reports remain ignored.

## Recovery validation - 2026-11-06 UTC

Resumed these documented edits in the primary attached checkout without allocating a
new pass. Approved fetch confirmed HEAD and remote still equal the parent. Git history,
the previous automation memory, single worktree, app thread inventory, and process
inspection showed no competing DreamCompiler writer or unrelated changes. P0005 CI
run 41833228164 was rechecked and remains successful. The saved schedule already uses
local execution and the primary project directory; no schedule change was needed.

Tool versions remained Zig 0.17.1, Python 3.13.15, Go 1.27.0, re2c 3.0. Restricted
`zig build test` could not locate the installation directory. With normal approved
toolchain/cache access, all required checks passed:

- `zig build test`: 136 discovered, 117 passed, 19 skipped, 0 failed, 0 errors
  (46.57s). Log: `build/P0006-recovery-full-suite.log`; report:
  `test_results/test_results_20261105_171508_windows.json` (local clock date).
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: all four groups passed (38 diagnostic cases,
  arena growth, nonfatal warnings, 25 module cases; `-count=1`, Go test 5.320s).
  Log: `build/P0006-recovery-go-quick.log`. This reruns the focused member matrix.
- `python -m unittest discover -s codex/python -p test_test_manager.py`: 5 passed
  (0.051s). Log: `build/P0006-recovery-python.log`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: passed. Log:
  `build/P0006-recovery-linux-cross-build.log`. Linux binaries were not run locally.

The earlier intermittent SDK file-open errors did not recur. Their root cause remains
unproven; no SDK/cache deletion, repair, override, test exclusion, or assertion change
was used. Implementation/regression/documentation diff and local links were reviewed;
`git diff --check` passed before commit.

## Limits and handoff

The original full-suite validation blocker is cleared by the recovery results above.
Publication still requires the `[dreamc][P0006]` commit to be pushed to the approved
branch and its SHA verified with `git ls-remote`; the final run report records that
evidence. Inspect P0006 CI after publication; pending CI is not Linux runtime proof.

The same 19 entry files remain skipped. Method argument type/shape compatibility,
class field declaration initializer analysis, all-path returns, virtual dispatch,
access control, imported types/globals, async, and graphics remain incomplete.
After delivering this receiver/access slice, take T003: turn one skipped entry into
a meaningful execution or rejection regression, unless baseline or CI failures intervene.
