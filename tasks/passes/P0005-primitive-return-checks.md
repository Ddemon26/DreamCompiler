# P0005 - Check explicit primitive function returns

- Date: 2026-11-04
- Task: T001
- Status: recovery validation passed; commit identity is resolved by the trailer below
- Parent checkpoint: `bf5661f20ad46f1c98e3ea19e2a9ff761cb5df6b`
- Commit lookup: exact `Pass-ID: P0005` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Reject incompatible explicit primitive returns, missing values in primitive-returning
functions, and expressions in void returns before C emission. The same checks apply
to ordinary functions, static/instance methods, and imported functions. Each method
uses its declared return context. Top-level entry returns keep their prior behavior.

Semantic analysis now visits switch cases, try/catch/finally bodies, unary operands,
and both conditional branches. Catch parameters retain their own scope. Return and
conditional nodes preserve source positions; dependency regressions require the
actual dependency path and line, ignoring ANSI color codes only.

Return types match exactly (`int`, `float`, `char`, `bool`, `string`); the return
boundary does not widen or narrow numbers. Existing arithmetic promotion remains,
and mixed int/float conditional expressions have type float. Other conditional
branches must agree. The policy is documented in `docs/language-features/functions.md`
and `codex/FEATURES.md`.

## Initial validation and blocker history

Windows toolchain: Zig 0.17.1, Python 3.13.15, Go 1.27.0, re2c 3.0.
Previous checkpoint CI succeeded: GitHub Actions run 41758634937 (P0004).

- Initial `zig build test`: 129 discovered, 110 passed, 19 skipped, no failures/errors.
- Before implementation, three focused Go cases (return_int, return_bare,
  return_method) failed because invalid programs exited successfully and emitted C.
- Focused `go test -count=1 -v -run 'TestCompiler_(DiagnosticExitStatus|Modules)$' .`
  from `codex/go`: both groups passed, with 23 diagnostic and 20 module cases.
- `python codex/test_cli.py run --pattern '**/*return*.dr'`: 6 discovered,
  5 passed, 1 existing unannotated skip, no failures/errors.
- `python -m unittest discover -s codex/python -p test_test_manager.py`: 5 passed.
- Two full post-change runs with the shared Zig cache each reported 133 discovered,
  113 passed, 19 skipped, 0 failed, 1 compile error. The errors were different cache
  file-open failures: mmiscapi.h while compiling while.dr, then a missing
  api-ms-win-crt-stdio-l1-1-0.def reference while compiling inheritance_declared_fields.dr.
  Neither run is counted as passing. Logs remain under ignored `build/P0005-dream-tests*.log`.
- A third `zig build test` with a fresh task-specific global cache also failed:
  133 discovered, 113 passed, 19 skipped, 0 failed, 1 compile error. This time
  string_var.dr hit `file_open Unexpected` for the Zig SDK header processenv.h.
  The isolated-cache attempt does not establish a pass or a cache-only cause.
  Environment: `ZIG_GLOBAL_CACHE_DIR=C:/Users/damon/cApps/DreamCompiler/build/P0005-zig-cache`.
  A direct SDK-path check found mmiscapi.h present and the reported lib64 .def path
  absent. The cause of the changing SDK/cache file-open failures remains unresolved.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  with the isolated-cache environment: all four groups passed (23 diagnostic cases,
  arena growth, nonfatal warnings, and 20 module cases; compiler launches use `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux` with the same isolated
  global cache: succeeded. Linux executables were not run locally.
- Diff reviewed; `git diff --check` passed. Twelve local links across seven changed
  documents validated. The original project checkout remains clean.
- Final `git ls-remote origin refs/heads/codex/compiler-stabilization` still reported
  parent `bf5661f20ad46f1c98e3ea19e2a9ff761cb5df6b`. No P0005 commit or push exists.

An initial method execution fixture expected a declaration initializer to set its
string field, but printed `(null)`. The final fixture uses the established constructor
assignment path and keeps the same expected output. Instance declaration initializers
are recorded as existing follow-up work; no test assertion was relaxed.
An initial focused glob (`*returns*`) found no files; it is not counted as validation.
Source-line assertions initially failed on ANSI escapes; color removal preserves the
required dependency filename and line 3 checks. Conditional position 0:0 was fixed.

## Recovery validation and checkout policy

The user requested cleanup of the leftover worktree, a rule against separate
worktrees, and then commit/push delivery. Recovery on 2026-11-04 used the primary
checkout on the attached integration branch; the fetched remote still matched the
parent SHA. No compiler implementation or regression assertions changed in recovery.

- `zig version`: 0.17.1.
- Initial restricted `zig build test`: could not locate Zig's installation directory.
  Required toolchain access was then approved through the normal permission controls.
- `zig build test` with approved toolchain access: 133 discovered, 114 passed,
  19 skipped, 0 failed, 0 errors (42.83 seconds). Report:
  `test_results/test_results_20261104_055115_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  with approved toolchain access: all four groups passed, including 23 diagnostic
  cases, arena growth, nonfatal warnings, and 20 module cases (`-count=1`).
- `python -m unittest discover -s codex/python -p test_test_manager.py`: five passed.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux` with approved toolchain
  access: succeeded. Linux executables were not run locally.
- Recovery logs: `build/P0005-recovery-full-suite.log`,
  `build/P0005-recovery-go-quick.log`, `build/P0005-recovery-linux-cross-build.log`.
- Eighteen local documentation links and checkout-policy consistency checked;
  diff reviewed and `git diff --check` passed.

The earlier SDK/cache failures did not recur. Approved toolchain access allowed the
recovery validation to pass, but the original cause of the changing file-open errors
has not been established. No SDK or cache files were deleted or repaired, and no
new task-specific global-cache override was used for the successful recovery checks.

## Limits and handoff

This checks explicit scalar primitive returns, not all-path missing-return proofs,
pointer/array shape compatibility, or general operator operand validity. Async and
non-primitive return contracts remain outside the supported subset. The same 19
unannotated entry files remain skipped; supporting modules are fixtures.

The initial `zig build test` blocker occurred across three attempts, with no commit
or push at that time. The original changes were in
the detached automation worktree. On 2026-11-04, explicit user-requested cleanup copied
all 16 changed files and 11 logs/reports to `C:/Users/damon/cApps/DreamCompiler`, with
SHA256-verified backup copies under ignored `build/P0005-worktree-recovery/`, then
removed that detached worktree. The primary checkout remains attached to the approved
branch. The cleanup also updates the root instructions, workflow, and daily prompt to
require this existing checkout and forbid new worktrees or detached HEAD. The cleanup
itself did not rerun compiler checks; the subsequent recovery validation is above.
Cleanup validation: 18 local documentation links and checkout-policy consistency
checked; all 16 original-file backup hashes verified; recovered compiler/test files
remain byte-identical. `git diff --check` passed. `git worktree list --porcelain`
shows only the primary checkout on `codex/compiler-stabilization`. The existing
automation's saved prompt matches the revised daily prompt, uses local execution and
the primary project directory, and retains its schedule and notification preference.
Publish this validated P0005 checkpoint and verify its approved-branch SHA before
starting another pass. Then inspect CI and take T002: member-call and static/instance
diagnostics, unless CI fails. Commit/push identity and verification are reported after
publication; this record cannot contain its own final commit SHA.
