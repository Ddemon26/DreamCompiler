# P0019 - Give member string declarations independent storage

- Date: 2026-11-13 UTC
- Task: T011 (bounded scalar member initializer slice)
- Parent checkpoint: `8a8329f2869399b602abbbec25f3e3d0039f0f48`
- Commit lookup: exact `Pass-ID: P0019` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Scalar string member initializers previously emitted plain C assignment, borrowing
field storage that block cleanup then released. The new ordinary/module-entry
regression reproduced invalid literal field cleanup with the real allocator's
checked live registry in both modes.

The normal scalar declaration emitter now wraps `ND_FIELD` and `ND_BASE`
initializers in `dream_string_copy`, alongside literals/indexing/identifiers.
Each nonnull copy owns independent storage. The receiver runs once and source
ownership remains unchanged. Normal cleanup, the allocator, field assignment and
concatenation emission remain unchanged. Copying adds allocation and byte-copy cost.

The annotated Dream fixture and `TestCompiler_MemberStrings` check ten member-copy
sites, thirteen release sites, sixteen nonnull runtime releases and twenty-seven
output lines per mode. Literal/managed/null fields, inherited `this`/`base` access,
static fields, const/grouped declarations, nested cleanup, loops, top-level copies
and a temporary receiver are covered. Fields and managed backing remain readable
after nested copies are released. A top-level snapshot survives field replacement;
a constructor marker proves once-only receiver evaluation. Existing shared helper
checks prove a copy survives release of managed backing. All prior ownership
assertions remain; both quick protocols and CI select the new group. Feature,
memory, ownership, source-map and Go test documentation describes the boundary.

## Validation

Started clean in the approved primary checkout, attached branch and exact origin.
History/memory confirmed P0018 delivered; no unpublished pass, extra registered
worktree or competing active project writer was found. Restricted fetch failed DNS;
approved fetch succeeded and matched HEAD. P0018
[CI 43515951800](https://github.com/Ddemon26/DreamCompiler/actions/runs/43515951800)
passed both native Linux and Windows jobs, including stability checks/full suite.
Automation view/config confirmed local execution and approved cwd; unchanged.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
could not locate its installed SDK; approved SDK/cache access was used without
content or permission changes. Compiler checks ran sequentially.

- Unchanged `zig build test`: **147 discovered, 129 passed, 18 skipped, zero
  failures/errors**, 39.15s; `test_results/test_results_20261113_161704_windows.json`.
- Before implementation, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_MemberStrings$' .`: **both modes failed as intended** at the
  live-allocation cleanup assertion, exit `0xc0000409`, 3.405s (body 2.84s).
  Linux harness cross-links passed before native execution failed. The pre-change
  check required cleanup emission; new copy assertions were added after this run.
- `zig build`: **passed** after implementation.
- `go test -count=1 -v -run
  '^TestCompiler_(MemberStrings|IdentifierStrings|LiteralStrings|IndexedStrings)$' .`:
  **all eight modes passed**, 11.646s (member body 2.18s). Generated C, Linux
  cross-link, native checked lifetime and direct-linked exact output passed.
- Final `zig build test`: **148 discovered, 130 passed, 18 skipped, zero
  failures/errors**, 36.98s; `test_results/test_results_20261113_162034_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all sixteen groups passed**, 29.766s; member
  body 3.08s. External compiler checks use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.043s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0019 runtime awaits CI.

Ignored evidence: `build/P0019-*.log`. Diff review, checks of all 72 local Markdown
links, fixture/Go/report output agreement, copy/release assertions, Go formatting,
PowerShell BOM, quick/CI selection and `git diff --check` passed. Precommit fetch
again matched the parent. Publication requires push and matching remote SHA;
report that identity after delivery.

## Limits and handoff

The same 18 unannotated skips and existing exclusions remain. This qualifies
direct scalar member initialization with normal fall-through cleanup. Source
storage must remain readable while copying; it does not establish field ownership,
declaration initialization or destruction. The unbound temporary receiver remains
in the existing registry until shutdown. Call/for-header initializers,
reassignment, pointer/array-local cleanup, return transfer and early-exit/exception
cleanup remain unqualified. Allocation failure retains the helper's null contract.
General shape/bounds/operator checks and existing const-discard warnings remain.

Verify P0019 publication and inspect native CI first, repairing any failure.
Otherwise take a bounded T011 for-header declaration slice: reproduce string
binding scope/cleanup emission for zero and nonzero iterations in ordinary/module
entry modes, then qualify storage ownership and loop-exit cleanup. Do not begin
another pass in this invocation.
