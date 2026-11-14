# P0025 - Use a Dream-written source normalizer in the compiler

- Date: 2026-11-14 UTC
- Task: explicit user request to begin production compiler dogfooding
- Parent checkpoint: `fe657146e26209db3936b52cbb6f413723d5558e`
- Commit lookup: exact `Pass-ID: P0025` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Production entry-file CRLF normalization now calls `src/driver/source_text.dr`
through `source_text.h`. The helper borrows writable NUL-terminated input without
allocation, uses pointer cursors to preserve the old input-size range, preserves
lone CR/high bytes, and retains the C implementation's single-pass behavior.

The unused Dream build hook passed unsupported driver arguments and built its
bootstrap for the target. It now builds a C-only host `dreamc-bootstrap`, including
`source_text_bootstrap.c` as its seed, then emits cached library C from tracked
Dream source inputs. The production target compiler links the generated helper
and runtime without the C seed. Both executables are installed. Cross-builds run
a host seed and compile generated C for the target.

`--emit-library-c OUTPUT SOURCE` uses the real parser, semantics and common AST
emitter. Synchronous functions have external C linkage and forward prototypes,
without main, host compilation or shared build output. Non-function top-level
nodes, main and async are rejected; module/object modes cannot be combined with
it. Invalid source preserves absent/existing output; open/write failures fail.
Program/object emission retains prior linkage and entry behavior.

The new Go group and C harness check 97,912 differential byte inputs, boundaries,
repeated calls, bootstrap/production helper C equality, separately linked pointer/
scalar/forward calls, artifact-free rejections, existing-output preservation,
output errors and production entry newline variants. Both quick protocols and CI
include it. README, feature/source maps, build guide and the
[contract](../../docs/development/compiler-dogfooding.md) describe the bounded
milestone. The user selected this pass ahead of the queued T011 initializer slice;
that work remains open.

## Validation

Started clean in the approved local checkout and attached branch, exact origin,
one registered worktree and no competing active project chat in app inventory.
Restricted fetch could not write `.git/FETCH_HEAD`; approved fetch succeeded and
HEAD matched the remote. P0024
[CI 43647310315](https://github.com/Ddemon26/DreamCompiler/actions/runs/43647310315)
passed native Linux/Windows checks and suites for the exact parent SHA.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
could not find its SDK; approved installed SDK/cache access was used without
changing contents or permissions. Compiler checks ran sequentially.

- Baseline `zig build test`: **154 discovered, 136 passed, 18 skipped, zero
  failures/errors**, 47.17s;
  `test_results/test_results_20261114_153454_windows.json`.
- Before rebuilding, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_Dogfood$' .`: **failed as expected**, 0.536s; the old compiler
  rejected the unsupported library request before creating its output.
- `zig build`: **passed** with host seed, generated Dream C and production link.
- Initial harness failed an incorrect idempotence assumption: the unchanged C
  seed also transforms CR CR LF into CR LF, then LF on the next call. The fixture
  now checks repeated-call equivalence, retaining independent first-pass expected
  output and complete-buffer/boundary checks. An initial Go helper also reported
  failures on its parent test; it now uses the active test. Existing regressions
  and assertions are unchanged.
- Focused `go test -count=1 -v -run '^TestCompiler_Dogfood$' .`: **passed**, 2.747s,
  including native differential execution and Linux cross-link. Subsequent added
  rejection/forward-call/newline coverage passed in the final quick protocol.
- Final `zig build test`: **154 discovered, 136 passed, 18 skipped, zero
  failures/errors**, 46.69s;
  `test_results/test_results_20261114_154100_windows.json`.
- From `codex/go`, Windows `build_and_test.ps1 quick`: **all twenty-two groups
  passed**, 45.446s; new group 1.62s with all final checks. External compiler tests
  use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.058s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**. The host
  seed ran on Windows and the Linux production compiler linked. Linux executables
  were not run locally; native P0025 runtime awaits CI.

Ignored logs: `build/P0025-{baseline,build,focused-go,full-suite,go-quick,python,linux-cross-build}.log`.
Diff review, `git diff --check`, added Markdown link/path checks, gofmt,
`zig fmt --check build.zig`, PowerShell BOM and quick/CI selection checks precede
commit. Publication requires push and matching `git ls-remote`; the delivery
report supplies the resulting commit identity.

## Limits and handoff

This is one production helper, not a self-hosted compiler. Lexer, parser, semantics
and C emitter remain C. The seed stays independent and equivalent. Only helper C
equality is checked, not successive whole-compiler rebuilds. Dependency loading
remains C. Embedding hosts own runtime lifecycle for library functions needing it;
the normalizer uses no runtime allocation. Library mode is not general C FFI and
does not qualify pointer shapes, return ownership or unsupported features.

Same 18 skips/exclusions and prior T011 ownership, return/unwinding, operator/shape,
allocation-failure and C-warning limits remain. Verify publication/native CI and
repair failures first. Then resume T011 conditional-string initializer ownership;
a future lexer migration needs a separate token/storage interface and equivalence
pass. Do not begin another feature in this pass.
