# P0017 - Give literal string declarations managed storage

- Date: 2026-11-13 UTC
- Task: T011 (bounded literal initializer slice)
- Parent checkpoint: `08cfbed6bb06a236eea6c307d46abfaad72d016d`
- Commit lookup: exact `Pass-ID: P0017` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Scalar string declarations initialized directly from literals previously borrowed
C static storage, then passed it to `dr_release` on normal block exit. Both
ordinary/module-entry checked-allocator runs reproduced the invalid release.
The normal declaration emitter now copies literal initializers with the existing
`dream_string_copy`, alongside P0014's indexed copies. Block cleanup and allocator
behavior remain intact. This includes empty/escaped/parenthesized literals,
`const` bindings, nested blocks, loop bodies, free/static/instance methods and
top-level declarations. It adds an allocation per executed literal declaration.
Concatenation initializers retain their existing allocation without another copy.

The new annotated Dream entry and `TestCompiler_LiteralStrings` check seven
generated copy sites, unchanged concatenation emission, seven release sites
(eight nonnull runtime releases) and exactly ten output lines per mode. The Go
test shares the existing ownership harness/linking path; the original indexed
fixture, dependency substitution, four-release oracle and output are preserved.
The harness now takes an explicit expected release count and also checks each
released allocation has exactly one reference. Windows cross-links both harness
modes for Linux. Quick protocols and CI include the new group. Feature, ownership,
memory, test and source-map docs describe the resulting boundary and limits.

## Validation

Started clean in the approved primary checkout, attached branch and origin, with
one registered worktree and no competing active project writer in app inventory.
Memory/history confirmed P0016 delivery and no unpublished pass. Restricted fetch
failed DNS; approved fetch succeeded and matched HEAD. P0016 native
[CI 43431180404](https://github.com/Ddemon26/DreamCompiler/actions/runs/43431180404)
passed both Linux and Windows jobs, including stability checks and full suite.
Automation tool/config inspection confirmed local execution and approved cwd;
no schedule change was needed.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
could not locate its installed SDK; approved SDK/cache access was used without
content/permission changes. Compiler checks ran sequentially.

- Unchanged `zig build test`: **145 discovered, 127 passed, 18 skipped, zero
  failures/errors**, 33.24s; `test_results/test_results_20261113_120005_windows.json`.
- Initial new fixture included unsupported direct `string absent = null`, which
  semantics rejected in both modes. Removed that unrelated line before ownership
  reproduction; no output assertion changed. Indexed null remains covered by P0014.
- Before implementation, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_LiteralStrings$' .`: **both modes failed as intended** at the
  harness live-allocation assertion, exit `0xc0000409`, 2.367s (body 1.96s).
  Linux harness cross-links had succeeded before each native failure.
- `zig build`: **passed** after the emitter change.
- First combined focused run: indexed modes passed; literal modes failed a new
  emitted-C assertion because it omitted existing concatenation parentheses.
  Corrected the assertion to require the actual unchanged parenthesized expression.
- `go test -count=1 -v -run '^TestCompiler_(LiteralStrings|IndexedStrings)$' .`:
  **all four modes passed**, 5.214s (literal body 2.29s, indexed 2.37s), generated
  code, Linux cross-link, checked lifetime and directly linked exact output.
- Final `zig build test`: **146 discovered, 128 passed, 18 skipped, zero failures/
  errors**, 36.68s; `test_results/test_results_20261113_120323_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all fourteen groups passed**, 18.349s; literal
  group 2.05s, indexed group 1.91s. This run includes the final single-reference
  harness assertion in both modes. External compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.056s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0017 runtime awaits CI.

Ignored logs: `build/P0017-*.log`. Full/staged diff review, all 64 local Markdown
link occurrences, metadata/output agreement, Go formatting, PowerShell BOM,
quick/CI selections and `git diff --check` passed before commit. Precommit fetch
again matched the parent. Publication requires push and matching remote SHA.

## Limits and handoff

The same 18 unannotated skips and existing exclusions remain. This qualifies
normal declaration/fall-through cleanup for direct literals, not a complete
ownership system. Identifier/call/for-header initializers, reassignment, aliases,
array/pointer locals, return transfer, early exits, exception cleanup and fields
remain unqualified. Allocation failure preserves the existing helper's null
result contract; no new failure policy is introduced. Existing ownership-harness
const-discard warnings remain. General shapes/bounds/operators remain separate.

Verify delivery and inspect native P0017 CI, repairing failures first. Otherwise
continue T011 with scalar identifier initializers: reproduce alias cleanup across
nested scopes using the live registry and prove backing/copy lifetimes without
removing releases. End after P0017; do not start that slice in this invocation.
