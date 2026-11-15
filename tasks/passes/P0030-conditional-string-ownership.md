# P0030 - Conditional scalar string initializer ownership

- Date: 2026-11-14 (America/Vancouver)
- Task: T011 bounded conditional initializer slice; explicit compiler-pass resumption
- Parent checkpoint: `f1d61a75fcfe23c3cdb013809f0f55ffed705aff`
- Commit lookup: exact `Pass-ID: P0030` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Resume compiler work after completed pruning/website passes. Scalar string
conditional initializers previously emitted a borrowed selected pointer, then
released it as managed storage. Both ordinary/module-entry checked-runtime modes
reproduced the live-allocation assertion. `copies_string_initializer` now includes
`ND_COND`: a single copy surrounds the existing conditional expression. Its
selected result gains independent storage (or null) without evaluating both
branches or transferring source ownership. The same classification marks the
binding owned for existing replacement and break/continue cleanup.

Added `tests/basics/memory/conditional_string_copy.dr` and
`TestCompiler_ConditionalStrings`, selected by both Go quick protocols and CI.
The matrix checks nineteen conditional-copy sites, twenty-two nonnull releases
and twenty-four output lines per mode. Condition/call/index markers exclude
unselected or repeated branch effects. Literal/empty/borrowed/managed/typed-null,
nested/const forms, source replacement, independent snapshots, scalar replacement,
loop-header/body cleanup, continue/break, static/instance and top-level paths
are covered. The allocator is unchanged; its harness prints release-count
mismatches while preserving all assertions.

Updated ownership/feature/source-map/test docs and current handoff/backlog.
P0026-P0029 remain legitimate completed maintenance history. The daily prompt
explicitly follows repository state beyond stale scheduled-run memory, so the
next invocation continues compiler work. No schedule, website or Pages changes.

## Validation

- Approved checkout/attached branch/origin, clean start, one registered worktree;
  available project chats showed no other active writer. Fetch succeeded through
  normal approval controls after restricted `.git/FETCH_HEAD`/DNS denial.
  Remote SHA matched parent. History showed no unpublished pass to recover.
- Parent P0029 [CI 43661838320](https://github.com/Ddemon26/DreamCompiler/actions/runs/43661838320):
  **native Linux and Windows jobs passed** before implementation.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**,
  Windows. Restricted Zig invocation could not locate its installation; normal
  approved SDK access was used without toolchain/cache/permission changes.
- Baseline `zig build test`: **154 discovered, 136 passed, 18 skipped, zero
  failures/errors**, 51.37s, `test_results_20261114_184504_windows.json`.
- Initial fixture used literal null in conditional/function argument positions;
  existing scalar semantics rejected these. Typed null storage replaced those
  unsupported forms without changing compiler typing. On the unchanged compiler,
  `go test -count=1 -v -run '^TestCompiler_ConditionalStrings$' .`: **both modes
  failed** the checked live-allocation assertion, 2.639s. Initial reproduction
  omitted new-copy assertions to reach real cleanup; final assertions require them.
- `zig build`: **passed**. Initial generated-C checks were corrected to expect
  C boolean literals `1`/`0`. An initially miscounted release total included a
  top-level class reclaimed at shutdown; source audit corrected it to **22**
  (19 exercise releases, two method locals, replaced global backing).
- Final focused matrix: **both modes passed**, 3.533s (group 3.02s), including
  generated-C assertions, UBSan/live-registry/refcount/release/shutdown checks,
  Linux harness cross-linking, native checked execution and direct linked output.
- Final `zig build test`: **155 discovered, 137 passed, 18 skipped, zero failures/
  errors**, 52.30s; `test_results_20261114_184830_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.058s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all 23 groups passed**, 47.492s; conditional
  group 2.49s. Compiler-launching tests use `-count=1`; earlier ownership
  assertions remain intact.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed**. Linux binaries were not executed locally; native P0030 runtime
  evidence awaits CI. Platform-specific CI steps remain as configured.
- **118 local Markdown links resolved**; launcher/CI selector equality, PowerShell BOM,
  final scope/diff review and `git diff --check` checked before commit.

Ignored logs: `build/P0030-{baseline,before,build,focused,focused-final,full,quick,linux}.log`.

## Limits and handoff

Same eighteen unqualified skipped entries; no exclusions or weakened assertions.
Fresh managed branch results can remain registered until shutdown. Literal
null/string branch typing remains rejected; readable typed null values are covered.
This does not establish return transfer, general expression/field/container
ownership, class early-exit cleanup or exception/finally unwinding.

Publish this pass to the approved branch, verify matching remote SHA, then inspect
native CI. The next bounded T011 task is class-instance cleanup on break/continue:
reproduce retained body-local allocations with checked registry/pre-shutdown
release evidence in both modes, then qualify nested target/header preservation
and existing scalar-string behavior. Keep destructors/field ownership, returns
and exception/finally changes separate. Do not repeat completed pruning/site work.
