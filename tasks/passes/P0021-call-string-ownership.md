# P0021 - Give call-initialized scalar strings independent storage

- Date: 2026-11-14 UTC
- Task: T011 (bounded scalar call-initializer ownership slice)
- Parent checkpoint: `daeb57fb8bffe7bd7b3729a8fdc330a60cac9620`
- Commit lookup: exact `Pass-ID: P0021` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Scalar string call initializers used plain assignment, allowing block cleanup to
release literal or borrowed storage. `src/codegen/stmt.c` now includes `ND_CALL`
in scalar declaration copying. Each call is evaluated once and its readable
result is copied with `dream_string_copy`, preserving null. Existing cleanup
releases the declaration's independent allocation; the callee/result owner is
unchanged. No runtime allocator or return-emission behavior changes.

The annotated `call_string_copy.dr` and `TestCompiler_CallStrings` qualify literal,
empty, borrowed managed, fresh managed, local-returned and null results in ordinary
and module-entry programs. Free/static/instance/base methods, nested copies,
const/grouped declarations, zero/multiple loop iterations, for headers and top-level
snapshots are covered. Fifteen emitted copy sites, fourteen release sites,
fourteen nonnull checked releases and nineteen output lines are required per mode.
Borrowed backing survives nested cleanup, a global snapshot survives source
replacement, and a marker proves once-only call evaluation. Existing real
live-registry/single-reference/shutdown assertions remain intact. The shared
emission checker substitutes entry-function prefixes to check module lowering.
Both quick protocols and CI select the new group. Feature, ownership, memory,
function, source-map and Go documentation describe the copy-without-transfer rule.

## Validation

Started clean in the approved checkout with attached branch and exact origin.
Memory/history showed P0020 delivered, one registered worktree and no unpublished
pass; app inventory showed only this active project writer. Restricted fetch
failed DNS; approved fetch succeeded and matched HEAD. The existing automation
is already local with the approved cwd; no schedule change was needed.
P0020 [CI 43539278761](https://github.com/Ddemon26/DreamCompiler/actions/runs/43539278761)
passed native Linux and Windows stability checks and full suite.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
SDK lookup failed; approved installed SDK/cache access was used without content
or permission changes. Compiler checks ran sequentially.

- Baseline `zig build test`: **149 discovered, 131 passed, 18 skipped, zero
  failures/errors**, 34.69s; `test_results/test_results_20261114_000133_windows.json`.
- Before implementation, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_CallStrings$' .`: **both modes failed the live-allocation cleanup
  assertion**, 2.511s (body 2.08s). Linux harness cross-links passed first.
- `zig build`: **passed** after adding call copying. Initial runtime check
  exposed a fixture's pointer equality against an empty literal; copied strings
  have different addresses. Following the existing literal fixture, output now
  writes the empty result followed by a marker, checking its bytes without
  assuming value equality. No compiler equality behavior or existing assertion
  was changed. Emission assertions were then matched to actual receiver casts
  and module function prefixes. The final focused pair **passed**, 2.519s.
- Final focused `go test -count=1 -v -run
  '^TestCompiler_(CallStrings|ForStrings|MemberStrings|IdentifierStrings|LiteralStrings|IndexedStrings)$' .`:
  **all twelve modes passed**, 12.745s; call body 1.97s. Generated-C checks,
  Linux harness cross-links, native checked lifetime and exact direct-linked
  output passed. All previous ownership assertions are retained.
- Final `zig build test`: **150 discovered, 132 passed, 18 skipped, zero
  failures/errors**, 33.46s; `test_results/test_results_20261114_000525_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all eighteen groups passed**, 27.077s;
  external compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.046s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0021 runtime awaits CI.

Ignored evidence: `build/P0021-*.log`. Full/staged diff review and diff checks,
all 82 local Markdown links, nineteen-line fixture/Go/report output agreement,
formatting, PowerShell BOM and quick/CI selection passed.
Publication requires push and matching `git ls-remote`;
report that identity after delivery.

## Limits and handoff

The same 18 unannotated skips and exclusions remain. Calls copy readable results
without inferring or transferring ownership. Fresh managed results and returned
locals whose return bypasses cleanup can stay registered until shutdown; copying
adds allocation cost. Callees must return readable storage. Return transfer,
dangling-result handling, early-return/exception cleanup, body-local cleanup
skipped by break/continue, reassignment, field ownership, array/pointer-local
cleanup, compound initializer forms and general shape/bounds/operator checks
remain open. Read-line expressions retain their existing result path. Existing
generated-C const-discard and parentheses warnings remain.

Verify P0021 publication and native CI first, repairing failures. Otherwise next
take a bounded T011 scalar string reassignment slice: audit/reproduce literal,
borrowed and managed replacement on normal paths with checked lifetime evidence,
then define old-value release and replacement-copy rules in both modes. Return
transfer remains a separate follow-up. Do not begin a second pass in this run.
