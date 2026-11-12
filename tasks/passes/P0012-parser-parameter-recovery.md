# P0012 - Terminate malformed parameter parsing with useful errors

- Date: 2026-11-12 UTC
- Task: T007
- Parent checkpoint: `2e28d5661d54920236027176ed44d670ec07820a`
- Commit lookup: exact `Pass-ID: P0012` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

The unsupported declaration `func init(Child child) {}` stalled compilation in
ordinary and module-entry sources. `parse_func` returned without consuming the
parameter list, its body brace could close the containing class, and a subsequent
stray delimiter could leave statement parsing retrying the same token forever.

`src/parser/stmt.inc` now recovers from an invalid parameter type/name or missing
closing parenthesis by discarding the malformed signature and its balanced body.
Nested parentheses/braces are tracked, an enclosing brace or new declaration is
left for its caller, and the temporary parameter vector is released. Identifier
parameter types retain an `expected parameter type` error with a hint explaining
that custom-type parameters are unsupported. A statement progress guard consumes
an erroneous token left untouched by expression recovery, covering program,
block and switch statement loops without consuming a valid caller-owned brace.
No lexer, semantics, backend or runtime changes were made.

The real CLI matrix in `codex/go/parser_recovery_test.go` covers 25 inputs in
ordinary, module-entry and imported-dependency parsing (75 rejections). Each has
a ten-second bound, exit 1, exact expected source line/useful column, and a fresh
working directory containing only its input sources after rejection. It checks
custom/unknown/pointer parameter types, malformed names/separators, array syntax,
nested parentheses, EOF, incomplete headers, enclosing braces and stray expression
delimiters at program/block/switch scope. A nested-body case requires diagnostics
at subsequent methods and functions, proving recovery preserves declarations.
Custom-type cases require one type error and the explanatory hint.

One annotated Dream rejection checks the original trigger. Both quick protocols
and CI select the new group. Feature, function, grammar and tooling docs describe
this boundary and mark the existing array-parameter examples as future syntax.
Custom-type/array parameter support and arbitrary malformed-input termination are
not claimed.

## Validation

Starting state: clean approved primary checkout, attached branch and approved
origin, one registered worktree, no other active writer in the app inventory,
no unpublished agent pass in memory/history. Restricted fetch failed DNS;
approved `git fetch origin codex/compiler-stabilization` succeeded. HEAD matched
the fetched parent. P0011 [CI run 43022236461](https://github.com/Ddemon26/DreamCompiler/actions/runs/43022236461)
passed native Windows and Linux jobs, including stability and Dream suites.
The automation already has local execution and the approved cwd; unchanged.

Windows toolchain: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.
Restricted baseline could not locate Zig's installed SDK. Compiler checks used
approved SDK/cache access; SDK/cache content and permissions were not changed.

- Unchanged `zig build test`: 141 discovered, 123 passed, 18 skipped, zero
  failures/errors, 43.58s. Report:
  `test_results/test_results_20261112_041227_windows.json`.
- Pre-change `go test -count=1 -v -run
  '^TestCompiler_ParserRecovery/(ordinary|module_entry)/custom_constructor$' .`
  from `codex/go`: failed as intended; both compiler invocations timed out at
  ten seconds. Go total 21.348s (body 20.88s).
- `zig build`: passed after implementation.
- First focused matrix: all compilations terminated, but the enclosing-brace
  fixture expected line 3 when its erroneous brace was on line 4. Moved the
  brace to line 3, preserving the incomplete-header recovery assertion.
- Second focus: EOF parameter-name positions differed because the entry driver
  appends a newline and the module loader does not. The fixture now includes
  an explicit newline and requires line 4 in all modes. No assertion was relaxed,
  no case was excluded, and no compiler changes were needed for these corrections.
- Final `go test -count=1 -v -run '^TestCompiler_ParserRecovery$' .`:
  all 75 rejections passed, 1.907s.
- Final `zig build test`: 142 discovered, 124 passed, 18 skipped, zero failures/
  errors, 38.82s. New annotated rejection passed. Report:
  `test_results/test_results_20261112_041748_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  five passed, 0.044s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: all ten groups passed, 12.752s. Includes the
  75 parser rejections (0.99s), 80 constructor rejections/two programs, 82 method
  rejections/two programs, 38 diagnostics, arena growth, warnings, four loop
  settings, 25 module cases and literal emission/failure checks. External
  compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: passed.
  Linux binaries were not executed locally; inspect P0012 native CI after push.
- Complete diff review and `git diff --check` passed. All 36 local Markdown link
  occurrences in changed documents resolve. Go formatting, source/regression
  paths, both quick/CI selections and the PowerShell UTF-8 BOM were verified.

Ignored evidence: `build/P0012-*.log` records restricted/approved baseline,
pre-change reproduction, build, focused attempts, full suite, quick, Python and
cross-build. The previous pass's intermittent SDK/cache file errors did not recur
in this pass's approved baseline or final checks.

## Limits and handoff

The same 18 unannotated skips and existing module/graphics/concurrency exclusions
remain. Custom-type/array parameter support, pointer emission/compatibility,
field initializers, implicit inherited construction, all-path returns and general
CFG/SSA emission remain unqualified. Recovery does not qualify unbounded nesting
or every malformed grammar construct.

Required checks and review passed. Commit, push only the approved branch, and
verify its SHA before calling this delivered.
Resolve this pass's commit by its trailer. Inspect native CI before the next task;
repair failures first. Otherwise take T008: qualify and preserve primitive pointer
parameters in emitted method signatures, reproducing P0010's dropped `*` first.
Do not begin that work during this run.
