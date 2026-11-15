# P0007 - Compile and execute the skipped loop fixture at every optimization setting

- Date: 2026-11-06 UTC
- Task: T003 (one skipped entry; optimizer stability fixes required by its execution matrix)
- Parent checkpoint: `f32f95ec373ee9aabc31b6c83d0ed0779a37247a`
- Commit lookup: exact `Pass-ID: P0007` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Turn the skipped `tests/optimization/test_loop_opt.dr` into an honest execution
regression. Its two loops compute 4 * (0 + ... + 7) = 112 and 2 * 2 * 2 = 8.
The original source had no expected annotations, so the runner never compiled it.
The annotated entry now requires both lines and runs at `-O2`. Its loop bodies and
expected behavior are unchanged; misleading claims of actual unrolling/strength
reduction in emitted code were replaced with arithmetic explanations.

A new Go regression compiles the same fixture using default, `-O1`, `-O2`, and
`-O3`, executes each result, checks successful exits, and compares the entire
normalized output. It exposed real compiler defects rather than a fixture mismatch:

- SCCP indexed its value table with negative destination/constant IDs. It now
  skips destination-less instructions and looks up operands only for variable IDs;
  conditional successor indexing also requires two successors first.
- LICM shared the same instruction pointer between two owning blocks, changed both
  to NOP, and double-freed it during CFG cleanup. It now creates a separate hoisted
  instruction and leaves an independently owned NOP in the source block.
- LICM reported changes merely for finding a preheader. Loop strength reduction,
  fusion, and empty-loop analysis also reported candidate discoveries as changes
  without rewriting IR. These paths now report actual transformations only, allowing
  the repeated pipeline to converge on this fixture.

Changes use real `src/opt/{sccp,licm,loop_opt}.c` paths. The new regression is selected
by the Linux Makefile, Windows PowerShell quick protocol (UTF-8 BOM preserved), and
both CI jobs. `codex/FEATURES.md` and the Go README describe the measured boundary.

## Validation

Verified primary checkout, attached approved branch, clean worktree, approved origin,
single registered worktree, git history, preceding run memory, and app thread inventory.
No unfinished pass or competing project writer was found. Fetch of the approved branch
passed with normal approved network access; HEAD equaled the fetched parent. P0006
publication was verified by that ref and its trailer; CI run 42135769585 had successful
Linux and Windows jobs. The saved schedule already uses local execution and the primary
directory. Restricted CIM process inspection was unavailable; approved inspection later
identified only this pass's compiler/Go processes. No separate checkout was created.

Windows tools: Zig 0.17.1, Python 3.13.15, Go 1.27.0, re2c 3.0. Restricted
`zig build test` could not locate the Zig installation. Compiler checks below used
approved installed-toolchain/cache access; no SDK/cache repair or deletion was used.

- Unchanged `zig build test`: 136 discovered, 117 passed, 19 skipped, zero failures
  or errors (49.32s). The selected loop entry was SKIP. Log: `build/P0007-baseline.log`;
  report: `test_results/test_results_20261106_052850_windows.json` (local clock).
- Before optimizer fixes, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_LoopsAtOptimizationLevels$' .`: default passed with 112 and 8;
  `-O1`, `-O2`, and `-O3` each failed with heap corruption exit `0xc0000374`.
  Log: `build/P0007-focused-go.log`. These failures were not waived.
- With SCCP guards alone, `-O1` still crashed and `-O2` stalled. After LICM ownership
  and change reporting fixes, default and `-O1` passed but `-O2` still stalled on
  candidate-only loop changes. The two diagnostic runs were explicitly interrupted
  by stopping only their identified Go/test/compiler PIDs. One attempted rebuild
  failed installing the still-running executable with AccessDenied; rebuilding
  succeeded after stopping this pass's process. Log: `build/P0007-focused-go-sccp.log`.
- Final `zig build`: passed. Log: `build/P0007-build.log`.
- Final focused Go command above: all four settings passed (7.60s test body,
  8.141s Go result). Log: `build/P0007-focused-go-fixed.log`.
- `python codex/test_cli.py run --pattern '**/test_loop_opt.dr'`: 1 discovered,
  1 passed, zero skips/failures/errors (0.85s). Log: `build/P0007-focused-dream.log`.
- Final `zig build test`: 136 discovered, **118 passed, 18 skipped, zero failures
  or errors** (42.32s). Log: `build/P0007-full-suite.log`; report:
  `test_results/test_results_20261106_053355_windows.json` (local clock).
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: all five groups passed (38 diagnostic cases,
  arena growth, warnings, four loop settings, 25 module cases; `-count=1`, 7.065s).
  Log: `build/P0007-go-quick.log`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`: five
  passed (0.045s). Log: `build/P0007-python.log`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: passed. Log:
  `build/P0007-linux-cross-build.log`. Linux binaries were not executed locally.
- Implementation/test/documentation diff reviewed; 17 local links, protocol selection,
  PowerShell BOM, and `git diff --check` checked before commit. Raw reports/logs remain
  ignored. No assertions weakened, source programs excluded, or generated lexer edits.

## Limits and handoff

The other 18 entries remain unannotated skips, not passing feature claims. They are:
`advanced/data_structures/multidim_arrays.dr`, `basics/memory/complex_memory.dr`,
`basics/strings/string_advanced.dr`, `control_flow/exceptions/complex_exceptions.dr`,
`control_flow/test_loop.dr`, `functions/advanced/complex_functions.dr`,
`functions/returns/return.dr`, `hottests/{minimal_struct_test,struct_only,temp_test1}.dr`,
and `misc/{test_break,test_const,test_const_complete,test_simple,test_simple_const,
test_string_concat,test_switch,test_syntax_highlighting}.dr`, all under `tests/`.
Supporting modules remain fixtures; four graphics and four concurrency examples
remain outside the regular suite. Existing assignment-expression warnings are nonfatal;
this pass does not repair their source positions.

Optimized IR is still discarded and C is emitted from the AST. SSA is disabled.
This execution matrix proves this source compiles and runs through the optimization
flags, not that transformed IR is semantically sound, reaches C, or improves performance.
Non-SSA propagation, CFG rewrites, unrolling, and broader optimizer correctness need
qualification before backend integration. Fusion, empty-loop elimination, and derived
induction-variable recognition remain unimplemented transformations.

Publish this pass and verify its SHA against `git ls-remote` before allocating P0008;
the final run report records that evidence. Inspect P0007 CI next; current native Linux
runtime behavior awaits CI. Otherwise take T004: define one bounded optimization-to-C
design with supported lowering and explicit fallback/errors. Do not start a second pass
in this invocation.
