# P0018 - Give identifier string declarations independent storage

- Date: 2026-11-13 UTC
- Task: T011 (bounded scalar identifier initializer slice)
- Parent checkpoint: `f15eb03d52203e02bdd6c1d7417412b36c435cf4`
- Commit lookup: exact `Pass-ID: P0018` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Scalar string identifier initializers previously emitted plain C assignment.
Nested block cleanup could free the source allocation, leaving outer bindings
dangling and causing a subsequent invalid release. The new ordinary/module-entry
regression reproduced this with the real allocator's checked live registry.

The normal scalar declaration emitter now wraps identifier initializers in
`dream_string_copy`, alongside literal/indexed initializers. Local, top-level and
borrowed parameter sources remain owned by their original owner; each nonnull
copy gets independent managed storage. Const/grouped bindings use the same path.
The allocator, normal cleanup, concatenation allocation and other initializer
paths remain unchanged. Copying adds allocation and byte-copy cost.

The annotated Dream fixture and `TestCompiler_IdentifierStrings` check nine
identifier-copy sites, two unchanged concatenations, ten release sites, twelve
nonnull runtime releases and nineteen output lines per mode. Coverage includes
nested alias chains, literal/managed borrowed parameters, free/static/instance
functions, local backing used after nested cleanup, loop bodies, top-level
snapshots and a null identifier. The existing shared harness also checks helper
copies survive managed backing release. All prior indexed/literal checks remain.
Both quick protocols and CI select the new matrix. Feature, storage, memory,
test and source-map documentation describes the bounded ownership result.

## Validation

Started clean in the approved primary checkout on the attached branch and exact
approved origin; one registered worktree and only this active project writer in
app inventory. Memory/history confirmed P0017 delivered with no unpublished pass.
Restricted fetch failed DNS; approved fetch succeeded and matched HEAD. P0017
[CI 43489193804](https://github.com/Ddemon26/DreamCompiler/actions/runs/43489193804)
passed Linux and Windows jobs, including stability checks and full suite.
Automation tool/config confirmed local execution and the approved cwd; unchanged.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
could not locate its installed SDK; normal approved SDK/cache access was used
without content or permission changes. Compiler checks ran sequentially.

- Unchanged `zig build test`: **146 discovered, 128 passed, 18 skipped, zero
  failures/errors**, 41.44s; `test_results/test_results_20261113_145511_windows.json`.
- Before implementation, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_IdentifierStrings$' .`: **both modes failed as intended** at the
  live-allocation cleanup assertion, exit `0xc0000409`, 2.703s (body 2.25s).
  Both Linux harness cross-links passed before native execution failed.
- `zig build`: **passed** after implementation.
- `go test -count=1 -v -run
  '^TestCompiler_(IdentifierStrings|LiteralStrings|IndexedStrings)$' .`:
  **all six modes passed**, 9.195s (identifier body 2.87s). Generated C, Linux
  cross-link, native checked lifetime and direct-linked exact output passed.
- Final `zig build test`: **147 discovered, 129 passed, 18 skipped, zero
  failures/errors**, 47.33s; `test_results/test_results_20261113_145812_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all fifteen groups passed**, 23.940s; identifier
  body 2.41s. External compiler checks use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.047s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0018 runtime awaits CI.

Ignored evidence: `build/P0018-*.log`. Diff review, all 68 local Markdown link
occurrences, fixture/Go/report output agreement, nine copy-site assertions, Go
formatting, PowerShell BOM, quick/CI selection and `git diff --check` passed.
Precommit fetch again matched the parent. Publication requires push and matching
remote SHA; report that after delivery.

## Limits and handoff

The same 18 unannotated skips and existing exclusions remain. This qualifies
direct scalar identifier initialization with normal fall-through cleanup, not
general alias retention. Source storage must remain readable while copying.
Call/member/for-header initializers, reassignment, pointer/array-local cleanup,
return transfer, early-exit/exception cleanup and fields remain unqualified.
Allocation failure retains the helper's existing null result contract. General
shape/bounds/operator checks and existing harness const-discard warnings remain.

Verify P0018 delivery and inspect native CI first; repair any failure. Otherwise
continue T011 with a bounded member-expression initializer slice: reproduce a
local borrowing literal/managed field storage, retain cleanup and prove source
and copy lifetimes in ordinary/module-entry modes. Do not begin another pass
in this invocation.
