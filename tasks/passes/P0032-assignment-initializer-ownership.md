# P0032 - Own scalar string assignment-initializer results

- Date: 2026-11-15 UTC (local reports November 14, America/Vancouver)
- Task: T011 bounded plain assignment-initializer ownership slice
- Parent checkpoint: `f028fc63f48cbc93723b9a11c3438e7619900189`
- Commit lookup: exact `Pass-ID: P0032` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

`string copy = (source = "next")` previously shared the assignment-result pointer
while normal cleanup treated the declaration as independently owned. Its binding
also missed owned replacement and jump cleanup. Both ordinary/module-entry modes
reproduced invalid cleanup through the existing checked live-allocation registry.

`copies_string_initializer` in `src/codegen/stmt.c` now includes `ND_BINOP` with
`TK_EQ`. The complete assignment evaluates once, then `dream_string_copy` gives
the scalar declaration independent storage and existing owned-scalar cleanup.
Source/copy replacement, self/nested assignments, borrowed parameter/field/index
destinations, null/empty values, managed operands, const bindings, once-only
call/index effects, methods, top-level snapshots and loop/header/jump paths are
covered. Destination assignment ownership is unchanged. Fresh managed operands
can remain registered until shutdown; no temporary ownership transfer is added.

Added the annotated `assignment_initializer_copy.dr` and
`TestCompiler_AssignmentInitializers`, selected by both Go quick protocols and CI.
Each mode requires seventeen emitted assignment-copy sites, thirty nonnull checked
releases and twenty-three exact output lines. Existing allocator/live-registry/
refcount/shutdown assertions and prior matrices are unchanged. Feature, user,
ownership, source-map, Go and handoff/backlog documentation reflect this boundary.
The schedule already uses local execution and the approved cwd; no change needed.

## Validation

- Clean approved checkout/attached branch/origin; one registered worktree.
  Git history and preceding memory show P0031 completed and published, with no
  unpublished pass. Available project chats show no competing active writer.
- Restricted fetch failed DNS; normal approved network access succeeded for
  `git fetch origin codex/compiler-stabilization` and `git ls-remote`, matching
  the parent. P0031 [native CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43666265892)
  completed successfully on Linux and Windows before implementation.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Checks used normal installed SDK/cache access without toolchain/cache or
  permission configuration changes.
- Baseline `zig build test`: **156 discovered, 138 passed, 18 skipped, zero
  failures/errors**, 59.43s; `test_results_20261114_205040_windows.json`.
- Before implementation, `go test -count=1 -v -run
  '^TestCompiler_AssignmentInitializers$' .`: **both modes failed** the live
  managed allocation cleanup assertion, 2.946s (group 2.49s). Emission assertions
  were added after this runtime reproduction.
- `zig build`: **passed**, exit 0. Focused command above after the fix:
  **both modes passed**, 3.798s (group 3.22s). Generated C, checked native UBSan
  harness, Linux harness cross-linking, exact output and direct linked output pass.
- Final `zig build test`: **157 discovered, 139 passed, 18 skipped, zero failures/
  errors**, 47.27s; `test_results/test_results_20261114_205330_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.040s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **25 groups passed**, 47.174s; new group 2.26s.
  Compiler-launching tests use `-count=1`; all prior ownership groups remain selected.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed**. Linux executables were not run locally; native P0032 runtime awaits
  CI after publication.
- **127 local Markdown links resolved**; 25-group quick/CI selector equality, PowerShell UTF-8 BOM,
  gofmt, complete scope/diff review and `git diff --check` checked before commit.

Ignored logs: `build/P0032-{baseline,before,build,focused,full,quick,linux}.log`.
The before-fix failures are regression evidence, not passing validation.

## Limits and handoff

Same eighteen skipped entries; no exclusions or weakened assertions. Compound
assignment, parameter/field/index destination ownership, owning containers,
class alias/container ownership, destructors/field destruction, return transfer
and exception/finally unwinding remain unqualified. Fresh managed assignment
operands can remain registered until shutdown. The allocator itself is unchanged.

Publish to the approved branch and verify the matching remote SHA, then inspect
native CI and repair failures first. Next reproduce T011 owned scalar string
`+=`, currently emitted as a generic C operator. Qualify a bounded string/string
replacement contract with once-only RHS effects, self-append and expression
results in both modes; preserve numeric compound operators and specify null/
temporary ownership policy. Stop after this pass; completed maintenance tasks
must not be repeated.
