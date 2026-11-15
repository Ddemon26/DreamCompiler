# P0023 - Preserve borrowed string array and pointer storage at block exit

- Date: 2026-11-14 UTC
- Task: T011 (bounded normal block cleanup slice)
- Parent checkpoint: `1c8a517043b4645040641a89155340f3404412f9`
- Commit lookup: exact `Pass-ID: P0023` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Block cleanup treated every binding tagged `string` as a scalar allocation,
passing stack arrays and borrowed `string*` locals to `dr_release`. The emitter
now checks the existing binding indirection count before releasing a string.
Scalar string and class-instance cleanup remain intact; the runtime allocator,
declaration initialization and assignment paths are unchanged.

Local string arrays contain borrowed pointers; explicit string pointers borrow
target storage. Normal block exit releases neither a container/alias nor its
elements. Element writes and pointer rebinding do not transfer ownership. Managed
backing must remain live through alias use. Scalar declarations copied from
elements still own independent allocations. Owning containers, element destruction,
heap-array allocation and pointer-result ownership remain outside this boundary.

Two annotated fixtures and `TestCompiler_StringContainers` cover arrays and
pointers separately in ordinary/module-entry programs. Generated C must omit
container/alias releases while retaining scalar and class-instance releases.
The unchanged real allocator harness requires eight/seven nonnull releases and
thirteen/eleven exact output lines respectively. Nested and unused storage,
literal/managed/null elements, alias chains and rebinding, null pointers, loop
bodies, free functions and static/instance methods are included. Backing survives
container exit and element replacement; scalar copies keep one reference and
are reclaimed. Both quick protocols and CI select the group. Feature, ownership,
variables, memory, source-map and Go docs describe the borrowed storage contract.

## Validation

Started clean in the approved local checkout, attached branch and exact origin,
one registered worktree, no unpublished pass or competing active project writer
in app inventory. Memory/history confirmed P0022 delivery. Restricted fetch failed
DNS; approved fetch succeeded and matched HEAD. Automation view/config confirmed
local execution and the approved cwd; no schedule change was needed.
P0022 [CI 43598710643](https://github.com/Ddemon26/DreamCompiler/actions/runs/43598710643)
passed native Linux/Windows checks and the full suite for the exact parent SHA.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
SDK discovery failed; approved installed SDK/cache access was used without content
or permission changes. Compiler tests ran sequentially.

- Baseline `zig build test`: **151 discovered, 133 passed, 18 skipped, zero
  failures/errors**, 34.07s; `test_results/test_results_20261114_120336_windows.json`.
- Before implementation, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_StringContainers$' .`: **all four modes failed the live-allocation
  release assertion**, 3.717s (body 3.31s). Linux harness cross-links passed first.
- `zig build`: **passed** after the fix.
- Focused `go test -count=1 -v -run
  '^TestCompiler_(StringContainers|AssignedStrings|CallStrings|ForStrings|MemberStrings|IdentifierStrings|LiteralStrings|IndexedStrings)$' .`:
  **all eighteen modes passed**, 18.221s; new group body 3.93s. Generated-C
  positive/negative checks, Linux harness cross-links, checked native lifetime and
  exact direct-linked output passed. Prior allocator/refcount/shutdown assertions
  and all fourteen existing ownership modes are retained.
- Final `zig build test`: **153 discovered, 135 passed, 18 skipped, zero
  failures/errors**, 35.23s; `test_results/test_results_20261114_120718_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all twenty groups passed**, 32.398s;
  external compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.032s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0023 runtime awaits CI.

Ignored evidence: `build/P0023-*.log`. Full/staged diff review and `git diff --check`,
90 local Markdown links, thirteen/eleven-line fixture/Go/report output agreement,
gofmt, PowerShell BOM and twenty quick/CI selection/passing-group checks passed.
Publication requires push and matching `git ls-remote`; report that identity after
delivery.

## Limits and handoff

Same 18 unannotated skips/exclusions remain. Borrowed containers do not keep managed
backing alive and must not outlive it; no bounds, dangling-pointer or initialization
checks are added. Owning containers and automatic element destruction, other scalar
initializers, compound assignment, parameter/field/index destination ownership,
return transfer, early return/exception cleanup and body locals skipped by
break/continue remain unqualified. Managed temporaries can remain until shutdown.
Existing C warnings, shape/operator gaps and allocation-failure behavior remain.

Verify P0023 publication and native CI, repairing failures first. Then qualify
T011 owned scalar body-local cleanup on loop-targeting break/continue with checked
release totals, preserving for-header lifetime through continue/update and
distinguishing nested loop/switch targets. Keep return/exception cleanup separate.
Do not start a second pass here.
