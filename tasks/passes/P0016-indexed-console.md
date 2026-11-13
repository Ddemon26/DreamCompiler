# P0016 - Preserve indexed primitive console formats

- Date: 2026-11-13 UTC
- Task: T010
- Parent checkpoint: `526f60c4e29de1252df629a523c588c01ee89364`
- Commit lookup: exact `Pass-ID: P0016` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Direct indexed float/char pointer parameters and local arrays previously selected
`%d`, while scalar string indexing was incorrectly classified as string output.
Backend bindings now retain array and explicit pointer layers for declarations,
parameters and for-header declarations. A recursive primitive storage classifier
consumes one layer per index, then treats scalar string indexing as a character.
The common int/float/string predicates and char format selection use this result.
Existing member lookup continues to resolve fields, inherited storage and calls.
This does not change parser/semantic compatibility, runtime allocation or cleanup.

The new annotated Dream entry and Go ordinary/module-entry matrix check all five
primitive element types, local arrays, a numeric alias, free functions/static
methods, inherited fields via this/base/objects, scalar string parameters/locals/
fields/literals/calls, nested string indexing and once-only index evaluation.
Both Write and WriteLine are exercised. Final checks require 18 generated C
snippets before linking and exactly 41 runtime output lines per mode. Quick
protocols and CI select the matrix; feature/function/test docs and source map
record the bounded behavior. No exclusions or existing assertions were weakened.

## Validation

Started clean in the approved primary checkout, attached branch and origin, with
one registered worktree and no competing active project writer in app inventory.
Memory and all-ref history confirmed P0015 delivery, no unpublished pass. Restricted
fetch failed DNS; normal approved fetch succeeded and matched HEAD. P0015 native
[CI 43389722344](https://github.com/Ddemon26/DreamCompiler/actions/runs/43389722344)
passed both Windows and Linux jobs, including stability checks and full suite.
Automation tool/config inspection confirmed local execution and the approved cwd;
no schedule changes were needed.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
could not locate its SDK; approved SDK/cache access was used without modifying
SDK, cache contents or permissions. Compiler checks ran sequentially.

- Unchanged `zig build test`: **144 discovered, 126 passed, 18 skipped, zero
  failures/errors**, 31.41s; `test_results/test_results_20261113_060253_windows.json`.
- Initial focused fixture failed semantic redefinition checks because it attempted
  unsupported local shadowing. Renamed only the new bindings, keeping all 41
  expected output lines. This setup failure was not the backend reproduction.
- Before implementation, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_IndexedConsole$' .`: **both modes failed as intended**, ten missing
  format assertions each, 0.466s (body 0.05s). No host compilation/execution was
  attempted after detecting incorrect generated formats.
- `zig build`: **passed** after implementation.
- Same focused command: **both modes passed**, 1.218s (body 0.78s), generated-C
  assertions and exact runtime output. Subsequently added five member format
  assertions; the final quick run below passed all 18 per mode.
- Final `zig build test`: **145 discovered, 127 passed, 18 skipped, zero failures/
  errors**, 29.57s; `test_results/test_results_20261113_060617_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all thirteen groups passed**, 15.509s; indexed
  console group 0.63s, both modes. External compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.041s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0016 runtime awaits CI.

Ignored logs: `build/P0016-*.log`. Full diff review, `git diff --check`, Go
formatting, all 57 local Markdown link occurrences, metadata/Go output agreement
and PowerShell UTF-8 BOM/quick/CI selections passed before commit. Publication requires push and matching remote SHA.

## Limits and handoff

Same 18 unannotated skips and existing exclusions remain. This qualifies direct
indexed console formatting, not general pointer/array compatibility, bounds, null
safety, operator typing, return shapes or string ownership. Semantic analysis
still does not distinguish every indexed string/array shape at argument/return
boundaries. String concatenation shares the backend predicates but broader
operator correctness is not qualified by this matrix. Existing ownership harness
const-discard warnings remain.

Inspect native P0016 CI and repair any failures first. Otherwise take one bounded
T011 ownership slice, starting with checked cleanup of scalar literal string
initializers. Preserve managed cleanup and prove ordinary/module lifetime behavior.
End after this pass; do not start T011 here.
