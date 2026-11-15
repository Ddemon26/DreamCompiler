# P0020 - Scope and release for-header string declarations

- Date: 2026-11-14 UTC
- Task: T011 (bounded for-header declaration scope and cleanup slice)
- Parent checkpoint: `dcec2a3f9a764b804e071039ba5b626ff0cc9272`
- Commit lookup: exact `Pass-ID: P0020` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

For-header declarations used a separate emitter that bypassed scalar string
copying and registered the header in the enclosing backend scope. Function/block
cleanup then referenced names outside their C lifetime; top-level headers were
left for shutdown. Semantic bindings also escaped the loop, preventing later
loops from reusing their names.

The semantic analyzer now gives each for loop a scope. Declaration-form loops
emit a wrapper block, use the normal declaration emitter, emit the C loop with
an empty initializer, and release header bindings before leaving the wrapper.
Block and header cleanup share the existing release logic. Literal/index/identifier/
member initializers gain the already-qualified copies; concatenation retains its
existing allocation. Initialization runs once, including zero iterations. Header
bindings remain visible in condition/body/update, disappear afterward, and can be
reused by later loops. Existing enclosing-name shadowing restrictions are unchanged.

The annotated `for_string_copy.dr` and `TestCompiler_ForStrings` qualify ordinary
and module-entry programs: nine copy sites, unchanged concatenation, ten release
sites, nine nonnull checked releases and thirteen exact output lines per mode.
Zero/multiple iterations, borrowed managed sources, member/index/null values,
once-only indexing, header references in condition/update, loop-targeting break,
continue, reused names and top-level release are covered. Generated C requires
cleanup immediately after each loop within its wrapper scope. Four driver cases
reject integer/string header uses after the loop at line 3 with no artifacts.
The real allocator's live registry and single-reference assertions remain intact;
all previous ownership groups remain selected. Both quick protocols/CI and feature,
grammar, ownership, memory, source-map and Go documentation are updated.

## Validation

Started clean in the approved primary checkout, attached branch and exact origin.
History/memory showed P0019 delivered, one registered worktree, no unpublished
pass and no competing active project writer in the app inventory. Restricted
fetch failed DNS; approved fetch succeeded and matched HEAD. Automation view and
config confirmed local execution in the approved cwd; no schedule change needed.
P0019 [CI 43526977205](https://github.com/Ddemon26/DreamCompiler/actions/runs/43526977205)
passed both native Linux and Windows jobs, including stability/full-suite checks.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
could not locate the installed SDK; approved SDK/cache access was used without
content or permission changes. Compiler checks ran sequentially.

- Unchanged `zig build test`: **148 discovered, 130 passed, 18 skipped, zero
  failures/errors**, 40.26s; `test_results/test_results_20261113_180507_windows.json`.
- Before implementation, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_ForStrings$' .`: **both modes failed semantic analysis** on
  redefinition of `repeated`, 0.535s (body 0.04s).
- After semantic scope repair and `zig build`, the same test **failed both modes**
  in Linux harness C compilation: nine out-of-scope release operands per mode,
  1.245s (body 0.79s). This failure precedes runtime lifetime checks; no claim of
  a pre-change allocator assertion failure is made. Copy/release-position checks
  were added after reproduction.
- `zig build`: **passed** after code generation repair. Initial focused pair
  **passed**, 3.007s (body 2.56s).
- Final focused `go test -count=1 -v -run
  '^TestCompiler_(ForStrings|DiagnosticExitStatus|MemberStrings|IdentifierStrings|LiteralStrings|IndexedStrings)$' .`:
  **all ten ownership modes and driver diagnostics passed**, 12.118s; for-header
  body 2.15s. Generated C, Linux harness cross-link, native checked lifetime and
  direct-linked exact output passed. All four new positioned/artifact-free
  scope rejection cases passed.
- Final `zig build test`: **149 discovered, 131 passed, 18 skipped, zero
  failures/errors**, 33.93s; `test_results/test_results_20261113_180919_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all seventeen groups passed**, 25.622s; for-header
  body 2.06s. External compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.043s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0020 runtime awaits CI.

Ignored evidence: `build/P0020-*.log`. Complete diff review, all 78 local Markdown
links, thirteen-line fixture/Go/report output agreement, Go formatting, PowerShell
BOM, quick/CI selection and `git diff --check` passed. Precommit fetch again
matched the parent. Publication requires push and a matching remote SHA; report
that identity after delivery.

## Limits and handoff

The same 18 unannotated skips and existing exclusions remain. This qualifies
supported primitive for-header declarations and their string ownership during
normal loop execution, continue, zero iterations and loop-targeting break. Return
and exception exits can bypass header cleanup; owned body locals skipped by
break/continue remain unqualified. Call/other initializers, reassignment,
return/field ownership, pointer/array-local cleanup and general shape/bounds/
operator checks remain open. Copies require readable input, add allocation cost,
and retain the helper's null-on-allocation-failure contract. Existing generated-C
const-discard and parentheses warnings remain.

Verify P0020 publication and inspect native CI first, repairing failures. Otherwise
next take a bounded T011 call-initializer slice: reproduce scalar declarations
from functions returning literal/borrowed versus managed strings in both modes,
then define and qualify their copy/transfer lifetime contract. Do not begin
another pass in this invocation.
