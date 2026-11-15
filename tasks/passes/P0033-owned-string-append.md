# P0033 - Append owned scalar strings before releasing old storage

- Date: 2026-11-15 UTC (local reports November 14, America/Vancouver)
- Task: T011 bounded owned scalar string `+=` slice
- Parent checkpoint: `4f9584773eac163ad3486811a4094ef353741bd3`
- Commit lookup: exact `Pass-ID: P0033` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

String `+=` previously emitted generic C pointer addition. Both modes of the new
regression failed checked-harness C compilation on invalid operands; these programs
could not reach runtime lifetime checks. `src/codegen/expr.c` now routes a known-owned
scalar identifier with a string/null operand through `__dream_string_append`.
The emitted helper evaluates after the RHS, concatenates the slot's current value
and operand before releasing old storage, then stores and yields the new pointer.
Its `const char **` slot matches generated scalar string declarations.

Null contributes empty text on either side, including a nonnull managed empty
result for null/null. Self-append and nested updates are safe; nested updates read
the slot after the inner operation. `stmt.c` copies declaration initializer results
so snapshots have independent ownership and existing replacement/header/jump cleanup.
Assignment/append results classify as scalar strings for console and RHS emission.
Numeric compound operators preserve their existing C emission. Semantic analysis
rejects const identifier `+=` and non-string/non-null operands before artifacts;
assignment diagnostics now use the destination position instead of an unset binop
position. Other const compound operators remain outside this pass.

The annotated `appended_string_copy.dr`, two rejection entries, and
`TestCompiler_AppendedStrings` cover both ordinary/module-entry modes. Each positive
mode requires 49 nonnull releases (43 exercise, four method, two top-level replacements)
and 26 exact output lines, checked allocator/refcounts/shutdown, helper ordering,
independent snapshots, borrowed/member/index/fresh operands, self/nested updates,
once-only call/index effects, null/empty strings, methods, zero/multiple loop
iterations, header updates, break/continue, and numeric preservation. Ten negative
cases require source positions, exit 1 and no artifacts for const/string and
int/float/bool/char operand rejection. Existing harness assertions are unchanged.
Both quick protocols and CI select the group; feature, language, ownership and
source-map documentation describe the boundary. Nine pre-existing relative links
in the changed operator guide were repaired to existing pages.

## Validation

- Approved checkout/attached branch/origin verified clean at start; one registered
  worktree, no competing active project writer shown. Prior memory/history showed
  delivered P0032 and no unpublished pass. Schedule view/TOML already specifies
  local execution at the approved cwd; no configuration change needed.
- Restricted fetch failed DNS. Normal approved network fetch and `git ls-remote`
  succeeded, matching the parent. P0032 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43674621260)
  passed Linux and Windows before implementation.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  No toolchain/cache or permission configuration changes.
- Baseline `zig build test`: **157 discovered, 139 passed, 18 skipped, zero
  failures/errors**, 48.49s; `test_results_20261114_212342_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_AppendedStrings$' .`: **both modes failed** Linux harness
  cross-compilation with invalid C pointer `+=`, 0.975s (group 0.49s).
- `zig build`: **passed**, exit 0. Intermediate fixture work exposed a placeholder
  release count (49 actual versus 0), then incorrect console-emission/ANSI-position
  expectations. The independently counted release total and actual string console
  path are now asserted; no existing lifetime assertions were weakened.
- Focused group before final signature review: **both modes and ten rejection
  cases passed**, 3.084s (group 2.65s). Native checked UBSan harness, exact output,
  Linux harness cross-linking and direct compiler-linked output passed.
- Initial post-change full suite: **160 discovered, 142 passed, 18 skipped, zero
  failures/errors**, 51.07s; `test_results_20261114_213126_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`: **5 passed**,
  0.055s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **26 groups passed**, 50.863s initially.
- Initial Linux cross-build: **passed**. Linux executables were not run locally.
- Final `zig build test` after matching the helper slot type: **160 discovered,
  142 passed, 18 skipped, zero failures/errors**, 47.29s;
  `test_results/test_results_20261114_213348_windows.json`.
- Final supported Windows Go quick protocol: **all 26 groups passed**, 50.744s;
  append group **2.70s** includes both execution modes and ten rejection cases.
  Both protocols and CI select the same groups with `-count=1`.
- Final `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-building and harness cross-linking do not establish
  native Linux runtime behavior; P0033 native CI awaits publication.
- **146 local Markdown paths resolved**; 26-group selector equality, PowerShell UTF-8 BOM, gofmt,
  complete diff/scope review and `git diff --check` checked before commit.
  Precommit fetch still matched the parent; no remote divergence.

Ignored logs: `build/P0033-{baseline,before,build,focused-initial,focused,full,quick,linux}.log`
and final validation logs. Before-fix/scaffold failures are not passing evidence.

## Limits and handoff

Same eighteen skipped entries. Borrowed parameter/field/index destinations,
pointer/array and conditional operands, other compound operators, owning containers,
class aliases/destructors/fields, return transfer and exception/finally unwinding
remain unqualified. Fresh managed operands may remain registered until shutdown;
no temporary ownership transfer or new allocation failure/size policy is added.
The allocator and ordinary `+` null behavior are unchanged. Cross-build/cross-link
success does not establish native P0033 Linux runtime behavior.

Publish to the approved branch and verify the remote SHA, then inspect native CI
and repair failures first. Next reproduce owned scalar locals retained by bare
`return;` in void functions/methods. Qualify a bounded cleanup path using checked
pre-shutdown allocations, preserving borrowed parameters and nested/header scopes;
keep value-return transfer and exception/finally unwinding separate. Stop after
this pass; completed maintenance tasks must not be repeated.
