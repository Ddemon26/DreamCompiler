# P0022 - Copy scalar string replacements before releasing old values

- Date: 2026-11-14 UTC
- Task: T011 (bounded plain scalar assignment slice)
- Parent checkpoint: `8d1897a9e91fdf35ca418e74d1e119e22b81e799`
- Commit lookup: exact `Pass-ID: P0022` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Plain C assignment lost a scalar string's old allocation and could leave block
cleanup releasing literal or borrowed storage. The backend now records ownership
for scalar declarations using the qualified copied initializer forms, managed
concatenation/read-line initializers, or no initializer. Uninitialized scalar
strings now start as null. Parameters remain borrowed; fields, indexed targets,
array/pointer bindings, compound assignment and other initializer forms are outside
this slice.

For a known-owned scalar identifier, plain `=` calls a generated static helper.
It evaluates the replacement once, copies its readable string (preserving null),
releases the slot's current value, stores the copy and yields that pointer. Copying
first preserves self-assignment and calls borrowing the old value. Chained and
nested same-slot assignments release the value current after inner evaluation.
Final normal block/header cleanup is retained; top-level final storage is reclaimed
at shutdown. Allocation failure stores null and releases the old value, following
the existing copy helper's contract. No runtime allocator or return ABI changes.

The new annotated `assigned_string_copy.dr` and `TestCompiler_AssignedStrings`
check generated copy/release ordering and exact output in ordinary/module-entry
programs, using the existing real checked allocator. Thirty-nine nonnull releases
and thirty-three output lines cover literal/borrowed/managed/member/index/call
sources, self/chained/nested assignment and its result, null/empty/null-default
values, nested source lifetime, once-only effects, free/static/instance methods,
loops, header updates and top-level replacement. Parameter/field/index writes
retain their existing borrowed emission. All allocator assertions remain.

The prior call fixture replaces its top-level backing string. Its exact nonnull
release expectation increases from fourteen to fifteen because that old allocation
is now released, with a new emission assertion; output is unchanged. This is a
stronger reclamation requirement, not a weakened assertion. Both quick protocols
and CI select the new group. Feature, ownership, variables, memory, source-map
and Go docs describe the bounded contract.

## Validation

Started clean in the approved local checkout/attached branch and exact origin,
one registered worktree, no unpublished pass or competing active project writer
in app inventory. Memory/history confirmed P0021 delivered. Restricted fetch
failed DNS; approved fetch succeeded and matched HEAD. Automation view/config
confirmed local execution and approved cwd; no schedule change was needed.
P0021 [CI 43569739018](https://github.com/Ddemon26/DreamCompiler/actions/runs/43569739018)
completed successfully for the exact parent SHA on Linux and Windows.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted Zig
SDK lookup failed; approved installed SDK/cache access was used without content
or permission changes. Compiler checks ran sequentially.

- Baseline `zig build test`: **150 discovered, 132 passed, 18 skipped, zero
  failures/errors**, 37.38s; `test_results/test_results_20261114_060342_windows.json`.
- Initial new fixture used unsupported direct `string nullStart = null` and was
  rejected in both modes. It now uses the existing typed borrowed-null path.
- Before implementation, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_AssignedStrings$' .`: **both modes failed live-allocation cleanup**,
  2.438s (body 2.01s), while cleaning up the first replaced local. Linux harness
  cross-links passed before native execution.
- `zig build`: **passed** after the fix. An intermediate fixture still failed
  live-registry cleanup because its local string array was released as scalar
  storage, a previously unqualified boundary. The fixture now receives the same
  backing as a borrowed `string*` from a top-level array, preserving indexed/null
  replacement checks. No existing test or allocator assertion was removed. This
  separate array/pointer-local cleanup defect is the concrete next T011 task.
- Initial ownership sweep caught the old call fixture's fourteen-release count;
  the expected fifteen now requires reclamation of its replaced top-level value.
- Final focused `go test -count=1 -v -run
  '^TestCompiler_(AssignedStrings|CallStrings|ForStrings|MemberStrings|IdentifierStrings|LiteralStrings|IndexedStrings)$' .`:
  **all fourteen modes passed**, 14.707s; assignment body 2.00s. Generated-C checks,
  Linux harness cross-links, native checked lifetime and exact direct-linked
  output passed. Previous allocator/refcount/shutdown assertions are retained.
- Final `zig build test`: **151 discovered, 133 passed, 18 skipped, zero
  failures/errors**, 33.90s; `test_results/test_results_20261114_060854_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all nineteen groups passed**, 28.582s;
  external compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.038s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0022 runtime awaits CI.

Ignored evidence: `build/P0022-*.log`. Full diff review and `git diff --check`,
85 local Markdown links, thirty-three-line fixture/Go/report output agreement,
gofmt, PowerShell BOM and quick/CI selection checks passed. Publication requires
push and matching `git ls-remote`; report that identity after delivery.

## Limits and handoff

Same 18 unannotated skips/exclusions remain. Replacements require readable input;
copying adds allocation cost. Fresh managed call/concatenation/read-line results
can remain registered until shutdown, without temporary or return transfer.
Other initializer forms, parameter/field/index destinations, compound assignment,
array/pointer-local cleanup, early return/exception cleanup and body locals skipped
by break/continue remain unqualified. Existing semantic shape/bounds/operator gaps,
generated-C warnings and allocation-failure behavior remain.

Verify P0022 delivery/native CI, repairing failures first. Then qualify normal
block cleanup for local string arrays and borrowed string pointers: the current
`emit_scope_cleanup` checks only the primitive string tag and can pass stack-array
or borrowed pointers to `dr_release`. Reproduce both with the live registry and
separate container/binding storage from scalar owned allocations, retaining scalar
cleanup and documenting element ownership. Do not start a second pass here.
