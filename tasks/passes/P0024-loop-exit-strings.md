# P0024 - Release owned scalar strings before break and continue

- Date: 2026-11-14 UTC
- Task: T011 (bounded loop/switch jump cleanup slice)
- Parent checkpoint: `d8f1e38c60e1b58e86de0ae290e6debd568c214d`
- Commit lookup: exact `Pass-ID: P0024` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Bare C break/continue statements bypassed normal block cleanup, leaving owned
body-local strings in the registry until shutdown. The emitter now maintains
stack-local loop/switch target frames with binding boundaries. Before a jump,
it releases qualified owned strings in reverse order through the scopes being
left. Break selects the nearest loop/switch; continue searches past switches
for the nearest loop. Frames are restored after each construct without allocating
memory or discarding emitter binding metadata needed by other branches.

For targets begin after header declaration emission, so headers survive continue
and update, then receive their existing single loop-exit release. Outer locals,
later declarations, borrowed parameters, string arrays and explicit pointer
aliases are excluded. Jump cleanup uses the existing `owns_string` metadata;
class-instance early-exit ownership is separate. Compound statements keep cleanup
conditional for an unbraced if body. Normal cleanup, the runtime allocator,
string initialization and replacement behavior are unchanged.

The annotated `loop_exit_strings.dr` and `TestCompiler_LoopExitStrings` require
thirty-seven nonnull checked releases and sixteen exact output lines in ordinary
and module-entry modes. Exact generated-C sequences check reverse release order
and target boundaries. Coverage includes repeated while/do/for bodies, expression
for initializers, nested loops, switch break/continue and normal completion, a loop
inside a switch, null/empty locals, borrowed arrays/pointers, declarations after
jumps, unbraced conditional jumps, header replacement updates, free/static/instance
functions and top-level loops. The existing live-registry, single-reference and
empty-after-shutdown assertions and eighteen prior ownership modes are retained.
Both quick protocols and CI select the group. Feature, storage, memory, variable,
source-map and Go docs describe the boundary and limits.

## Validation

Started clean in the approved local checkout, attached branch and exact origin,
one registered worktree, no unpublished pass or competing active project writer
in app inventory. Memory/history confirmed P0023 delivery. Restricted fetch failed
DNS; approved fetch succeeded and matched HEAD. Automation view/config confirmed
local execution and approved cwd; no schedule change was needed. P0023
[CI 43630813439](https://github.com/Ddemon26/DreamCompiler/actions/runs/43630813439)
passed native Linux/Windows checks and full suites for the exact parent SHA.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
SDK discovery failed; approved installed SDK/cache access was used without content
or permission changes. Compiler checks ran sequentially.

- Baseline `zig build test`: **153 discovered, 135 passed, 18 skipped, zero
  failures/errors**, 43.62s; `test_results/test_results_20261114_150829_windows.json`.
- Before implementation, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_LoopExitStrings$' .`: **both modes failed the exact release total
  before shutdown**, 2.676s (body 2.25s). Linux harness cross-links passed first.
- `zig build`: **passed** after the fix.
- Initial focused new group: **both modes passed**, 3.144s (body 2.63s).
- Final focused `go test -count=1 -v -run
  '^TestCompiler_(LoopExitStrings|StringContainers|AssignedStrings|CallStrings|ForStrings|MemberStrings|IdentifierStrings|LiteralStrings|IndexedStrings)$' .`:
  **all twenty modes passed**, 24.888s; new group body 2.37s. Generated-C cleanup
  boundaries/order, Linux harness cross-links, checked native lifetime and exact
  direct-linked output passed. All prior allocator/refcount assertions remain.
- Final `zig build test`: **154 discovered, 136 passed, 18 skipped, zero
  failures/errors**, 42.93s; `test_results/test_results_20261114_151212_windows.json`.
- From `codex/go`, Windows `build_and_test.ps1 quick`: **all twenty-one groups
  passed**, 35.530s; compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.043s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0024 runtime awaits CI.

Ignored evidence: `build/P0024-*.log`. Diff review, `git diff --check`, local
Markdown link/path checks (94 links), sixteen-line fixture/Go/report output
agreement, gofmt, PowerShell BOM and twenty-one quick/CI selection checks passed.
Precommit approved fetch still matched the parent. Publication requires
push and matching `git ls-remote`; report that identity after delivery.

## Limits and handoff

Same 18 unannotated skips/exclusions remain. Only qualified owned scalar strings
receive the new jump cleanup. Class-instance cleanup on break/continue, unsupported
initializer forms, compound assignment, parameter/field/index destinations,
owning containers, return ownership and return/exception/finally unwinding remain
unqualified. In particular, jumps crossing exception contexts do not acquire
handler/finally unwinding. Managed temporaries can remain until shutdown. Existing
shape/operator gaps, C warnings, allocation-failure behavior and absence of bounds
or dangling-alias checks remain.

Verify P0024 publication and native CI, repairing failures first. Then qualify
T011 scalar conditional (`?:`) string initializer ownership: reproduce literal,
borrowed, managed and null branch cleanup with the live registry, copy the selected
value once and preserve branch side effects in ordinary/module-entry modes.
Keep return ownership and exception/finally unwinding separate. Do not start a
second pass here.
