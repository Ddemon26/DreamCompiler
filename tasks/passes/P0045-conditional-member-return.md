# P0045 - Clean locals on conditional read-only instance-field returns

- Date: 2026-11-16 UTC
- Task: T011 conditional borrowed member-result cleanup
- Parent checkpoint: `106d95cd0db5a727e8a7f0a577670f0e81f91b93`
- Commit lookup: exact `Pass-ID: P0045` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Conditional string results containing `this.field` leaves formerly bypassed local
cleanup. Extend `is_scoped_string_result` in `src/codegen/stmt.c` to accept the
existing `borrowed_member_result` proof for each leaf. Trees may mix proven member
reads, literals, known-owned scalar locals and unchanged scalar parameters. Any
member leaf requires P0044's unchanged whole-body AST/operator read-only proof,
including conditions, unselected branches and later code. One unsupported leaf
preserves the entire prior return. The parameter-only proof remains unchanged.

Reuse one collision-safe result capture before reverse cleanup, preserving
inherited owning-class casts and only selected owned storage. Member results gain
no copy/retain, receiver release or field destruction. No-local methods keep direct
returns. Writes/calls/new/console/await, custom-type/pointer/array locals, exceptions
and unknown nodes/operators retain their barriers.

The new annotated Dream fixture and Go group check nested mixed values, two inherited
member fields, null/empty/literal/managed backing, selected local/empty/for-header
results, independent caller snapshots, unbraced/alternate/while/for/header/switch
paths, later declarations, temporary collisions and no-local returns. Each mode
requires 59 nonnull releases (one receiver, 58 strings), 27 output lines, four
original-reference selected owned results before shutdown and an empty registry
afterward. Exact C checks capture/cleanup order and original lifetime semantics.

Reuse the 196 direct-member emission controls on conditional trees, retaining the
original direct matrix. Fourteen more controls check effectful conditions and
unsupported unselected call/concat/assignment/index leaves; two nested-else
member controls qualify. Unsafe aliases are never executed. The former direct
group's now-eligible conditional example becomes a mixed instance/static result
that still fails qualification; separate positive lifetime/C checks cover conditional
members. No allocator/refcount/shutdown assertions are weakened. Quick protocols,
CI selectors, feature/user/ownership docs and the source map are aligned.

## Validation

- Verified approved path, attached branch, clean worktree, origin, all-ref history,
  preceding automation memory and one registered worktree. Available chats showed
  no competing active project writer. Schedule view/TOML already use local execution
  and the approved cwd; unchanged. Approved fetch matched the parent. Restricted
  fetch failed DNS; normal approved access worked.
- P0044 [native CI 43793738851](https://github.com/Ddemon26/DreamCompiler/actions/runs/43793738851)
  passed Linux/Windows build, Python, quick and full-suite steps before work (jobs
  133057747670 and 133057747437), matching the parent SHA.
- Actual versions: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted
  baseline could not locate Zig's installation; approved normal toolchain access
  succeeded without cache/configuration changes.
- Baseline `zig build test`: 170 discovered, 152 passed, 18 skipped, zero failures/
  errors, 50.92s; `test_results/test_results_20261115_212858_windows.json`.
- Before fix, `go test -count=1 -v -run '^TestCompiler_ConditionalMemberReturn$' .`
  from `codex/go`: both modes failed, 50 live allocations instead of four, 2.616s
  (group 2.17s). An earlier scaffolding check failed because a caller variable
  shared the forbidden parameter-release name; renamed the caller before lifetime
  reproduction, retaining the assertion.
- `zig build`: passed, exit 0. Initial lifetime group passed both modes, 3.184s
  (group 2.74s), with 57 releases/25 outputs before two final two-field calls.
- Five-group check passed prior direct/conditional parameter, conditional local
  and direct member groups, but failed the new index C expectation, 27.625s.
  Corrected missing existing parentheses around `values[0]`; no emitter change.
- Final focused new group: passed, 8.627s (group 8.18s), including final lifetime/
  exact-C checks, 196 shared controls, fourteen expression rejections and two
  nested-else positives. Native UBSan/live release/refcount checks, Linux harness
  cross-linking and directly linked output passed in ordinary/module-entry modes.
- `zig build test`: 171 discovered, 153 passed, 18 skipped, zero failures/errors,
  48.45s; `test_results/test_results_20261115_213338_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  five passed, 0.044s.
- Supported Windows Go quick protocol: all 37 groups passed, 82.677s; new group
  6.86s, direct member group 7.09s. External compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  passed, 15/15 steps. Cross-build/cross-linking does not prove native P0045 Linux
  runtime behavior; inspect CI after publication.
- 207 local Markdown paths, UTF-8 in all 17 pass files, PowerShell BOM,
  37-group quick/CI selector and actual-pass equality, 27 annotated outputs,
  P0001-P0045 index continuity, gofmt, full diff review and `git diff --check`
  passed. Ignored logs: `build/P0045-*.log`.

## Limits and handoff

Incoming receiver and field backing must already be live. Selected owned results
remain registered until runtime shutdown unless independently consumed; caller
consumption/transfer is unqualified. Static/local/struct/nested/base receivers,
broader field/alias/container/destructor ownership, pointer/class result transfer,
shape/operator checks and exception/async unwinding remain separate. Explicit class
parameters and async instance methods remain rejected by grammar; receiver shadowing
by semantics. The same eighteen skips, string-index return typing, custom-type for
headers and direct class null initializer limits persist.

Publish and verify the remote SHA, then inspect native CI; fix failures first.
Next audit static scalar string-field result lifetime and mutation/alias barriers,
then define a bounded implementation contract and both-mode acceptance matrix.
Do not relax current barriers or start that work in this invocation.
