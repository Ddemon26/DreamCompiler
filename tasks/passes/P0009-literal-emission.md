# P0009 - Emit checked literal arithmetic from isolated SCCP IR

- Date: 2026-11-08 UTC
- Task: T004 first implementation slice
- Parent checkpoint: `2d9c6ea191cb7af8b2164aa735cb170c88cd3c3b`
- Commit lookup: exact `Pass-ID: P0009` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Implement the P0008 design so a real, qualified IR transformation reaches emitted
C. At `-O1`/`-O2`/`-O3`, `(6 * 7) + (10 - 3)` emits literal `49`, while default
emission retains arithmetic. The replacement comes from SCCP's transformed root,
verified against checked preflight. This completes T004's first bounded slice.

`src/opt/literal_fold.c` accepts only canonical decimal integer `+`/`-`/`*` trees,
with every intermediate in `0..INT_MAX-1`, at most 256 operations, and depth 64
(root zero). It builds fresh postorder IDs in one stack-owned block, runs only
checked SCCP to bounded convergence, validates the root, then commits arena-owned
text while preserving source metadata. Ineligible maximal binary/unary expressions
remain whole. Explicit child traversal covers function/method bodies, statements,
control-flow containers, calls, and imported functions after module expansion.

The driver no longer runs the discarded, unqualified whole-program CFG pipeline.
`sccp_checked` reports allocation failure; `arena_try_alloc` provides a recoverable
replacement allocation without changing old `arena_alloc` callers. All bridge
errors are explicit and return before opening output artifacts. General mutable
IR, SSA, loops, division, negative/overflow arithmetic, and unknown containers are
not qualified for emission.

Object-mode testing exposed missing runtime headers in temporary generated C.
The common emitter now selects object header names with project runtime include
directories. Object write/emission/host failures propagate nonzero to the CLI.
Both modes still require the project working directory/runtime sources.

New `tests/optimization/literal_arithmetic.dr` checks exact values 49, 65,
2147483646, and 0. The focused Go matrix and real-implementation C fault harness
are selected by both quick protocols and Linux/Windows CI. Feature documentation,
the implementation contract, source map, backlog, state, and index are updated.

## Validation

Starting state: approved primary checkout/attached branch/origin, clean worktree,
one registered worktree, and no competing writer in the app thread inventory.
Git history and preceding memory showed no unfinished pass. The schedule already
uses local execution and the primary directory; no change was needed. Restricted
fetch failed DNS; approved `git fetch origin codex/compiler-stabilization` passed,
and HEAD equaled the fetched parent. P0008 CI run
[42373515160](https://github.com/Ddemon26/DreamCompiler/actions/runs/42373515160)
completed successfully on Linux and Windows, including stability and Dream suites.

Windows toolchain: Zig **0.17.1**, re2c 3.0, Python 3.13.15, Go 1.27.0.
Restricted baseline could not access Zig's installation directory; approved
installed-toolchain/cache access ran compiler checks. No SDK/cache changes were made.

- Unchanged `zig build test`: **136 discovered, 118 passed, 18 skipped, zero
  failures/errors**, 42.56s.
- `zig build`: passed after bridge implementation and after the object fix.
- Initial focused Go matrix: all four C/runtime settings and allocation harness
  passed; object emission failed finding `../libs/console.h`. This was a real
  pre-existing object path defect, fixed rather than excluded.
- Focused `go test -count=1 -v -run 'TestCompiler_Literal(Emission|Failures)$' .`
  from `codex/go`: **passed**, 3.508s after the fix. Later resource-boundary and
  invariant-fault extensions also passed in the final quick protocol below.
- Final `zig build test`: **137 discovered, 119 passed, 18 skipped, zero
  failures/errors**, 39.12s. Report:
  `test_results/test_results_20261108_053647_windows.json`.
- Focused `python codex/test_cli.py run --pattern '**/literal_arithmetic.dr'`:
  **1 passed**, zero skips/failures/errors, 0.79s.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.046s.
- Windows Go quick, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick` from `codex/go`: **all seven groups passed**,
  9.343s. Includes 38 diagnostic cases, arena growth, nonfatal warnings, four
  loop settings, 25 module cases, literal emission, and literal internal failures.
  All external-compiler Go checks use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux executables were not run locally; P0009 native Linux behavior awaits CI.

The emission matrix inspects named initializers and function/method/module returns
in C immediately after `--dev`, before any host compilation. Exact outputs match
at all four settings with `CC=zig cc`. It checks nested propagation, zero,
`INT_MAX-1`, calls/increment/assignment side effects, identifiers, unary/negative
intermediates, leading-zero literals, division/remainder/shifts, float/string
fallback, module source mapping, and object success/nonzero host failure. Undefined
overflow/zero-divisor source is inspected only, never executed. Depth/operation
limits include accepted boundaries and rejected whole-tree fallback.

The UBSan C harness uses the real bridge/SCCP/arena with controlled allocation
failures, a transformed-root mismatch, and nonconvergence. It verifies nonzero
status/diagnostics, candidate atomicity, source metadata, and root/range guards.
Fault flags exist only in test code. No tests were skipped, excluded, or weakened.
All 39 local Markdown links across eight changed/new documents, source references,
BOM/quick/CI selection, full diff review, and `git diff --check` passed before committing.

Ignored evidence: `build/P0009-{baseline,baseline-approved,build,focused-go,
focused-go-fixed,focused-dream,full-suite,python,go-quick,linux-cross-build}.log`.

## Limits and handoff

This qualifies only literal arithmetic, with no performance claim or general IR
correctness claim. SSA remains disabled; the legacy optimizer sources remain for
future qualification. Existing undefined arithmetic is preserved on fallback, not
given new semantics. The same 18 unannotated entries remain unverified; module
fixtures and graphics/concurrency exclusions are unchanged. Object emission was
compiled locally but does not provide a general standalone installation workflow.

Publish and verify the remote SHA before another pass; inspect P0009 native
Linux/Windows CI, repairing any failure first. Then take T005's primitive method
argument compatibility slice with diagnostics/artifact checks. Do not begin that
second pass in this invocation.
