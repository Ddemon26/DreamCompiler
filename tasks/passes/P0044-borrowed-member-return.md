# P0044 - Release scoped strings before read-only instance-field returns

- Date: 2026-11-16 UTC
- Task: T011 direct borrowed member-result cleanup
- Parent checkpoint: `001c5223cc6d9e054b66f8da5f0b821f3731c007`
- Commit lookup: exact `Pass-ID: P0044` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Direct `return this.field;` formerly bypassed local cleanup. Implement P0043's
separate whole-body read-only proof in `src/codegen/stmt.c`, resolving the actual
nonstatic scalar string declaration and borrowed depth-1 class receiver. Inherited
field emission keeps its owning-class cast. Capture once before reverse cleanup
without result copy/retain, receiver release or field destruction. No-local methods
keep a direct return; failed qualification keeps the entire previous return.

The AST/operator allowlist scans all body/header/expression positions, including
later/unreachable code. Every write/inc/dec/call/new/console/await, exception,
custom-type/pointer/array local, receiver shadowing or unknown node/operator blocks
the proof. Class aliases in already-exited scopes therefore cannot free a borrowed
receiver on this path. P0041/P0042's parameter proof and conditional predicate are
unchanged. Static/local/struct/nested/base receivers and conditional member leaves
remain separate.

The annotated Dream fixture and new Go group cover literal/managed/empty/typed-null
backing, nested primitive locals/copies, alternate/unbraced paths, later declarations,
while/do/for/header/switch returns, nonmutating operations, inherited fields,
binding/lowered-name temporary collisions and direct no-local returns. Each mode
requires **38 nonnull releases** (one receiver, 37 strings), **21 exact output lines**,
empty pre/post-shutdown registries and checked live operands/reference counts.
The caller's independent snapshot survives backing/field replacement. Exact C
requires capture before reverse cleanup and forbids result retain/copy or receiver
release. **196 emission-only controls** cover 44 barriers before/after early returns
in both modes (176), eight result shapes in both modes (16), and active exception/
async guards in both modes (four). Unsafe alias examples are never executed.

P0043's conceptual class-parameter examples are not accepted source: today's
grammar rejects explicit class parameters and async instance methods; semantics
reject receiver-name shadowing. Executable qualification is explicit `this`.
Previously declared classes supply alias/constructor controls because self-type
declarations inside a class are not registered yet. An accepted free async function
with a static field checks existing task-result emission. Unknown AST/operators
fail closed in source; arbitrary unknown nodes cannot be constructed by source
syntax. No grammar or async runtime behavior is broadened.

Both quick protocols and CI select the new group. Feature/function/ownership docs,
P0043 contract, source map, state, backlog and pass index reflect the measured slice.

## Validation

- Verified approved path, attached branch, clean worktree, origin, one registered
  worktree, all-ref history and preceding automation memory. No unfinished pass or
  competing active project writer was found. Schedule view/TOML already use local
  execution and the approved cwd; unchanged. Approved fetch succeeded and matched
  the parent. Restricted fetch failed DNS; normal approved network access worked.
- P0043 [native CI 43784740261](https://github.com/Ddemon26/DreamCompiler/actions/runs/43784740261)
  passed Linux and Windows build, Python runner, quick regressions and full suite
  (jobs 133029744698 and 133029744478) before implementation.
- `zig version`: **0.17.1**. Restricted baseline could not locate Zig's installation;
  approved normal toolchain access worked without configuration/cache changes.
- Baseline `zig build test`: **169 discovered, 151 passed, 18 skipped, zero failures/
  errors**, 54.00s; `test_results/test_results_20261115_192459_windows.json`.
- Before fix, from `codex/go`, `go test -count=1 -v -run
  '^TestCompiler_BorrowedMemberReturn$' .`: **both modes failed** at the empty-before-
  shutdown assertion, 2.703s (group 2.24s). Initial test scaffolding had incorrect
  Go package declarations, then unsupported class parameters; corrected to the
  existing external test package and explicit-this fixture before this reproduction.
- `zig build`: **passed**, exit 0. Initial lifetime check: **both modes passed**,
  2.889s (group 2.44s), with 36 releases before adding two positive paths. Exact-C/
  fallback development exposed unsupported self-type declarations/shadowing/async
  method fixtures; replaced with grammar-valid controls and documented the limits.
  The broader five-group run passed prior return groups but failed the async-method
  fixture (20.723s); its accepted free async/static-field replacement now passes.
- Final `go test -count=1 -v -run '^TestCompiler_BorrowedMemberReturn$' .`:
  **passed**, 8.002s, including both final 38-release/21-line modes and all 196
  controls, exact C, native UBSan/live-operand/refcount checks, Linux harness
  cross-linking and directly linked output.
- `zig build test`: **170 discovered, 152 passed, 18 skipped, zero failures/errors**,
  52.40s; `test_results/test_results_20261115_193452_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **5 passed**, 0.057s.
- Supported Windows Go quick protocol: **all 36 groups passed**, 77.947s, including
  the new group's final lifetime/C/196-control matrix (8.57s) and prior return assertions.
  External compiler tests use `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. Cross-build/cross-linking does not prove native P0044
  Linux runtime behavior; inspect CI after publication.
- **201 local Markdown paths**, UTF-8 in all 16 pass files, PowerShell BOM,
  36-group quick/CI selector and actual-pass equality, 21 annotated outputs,
  P0001-P0044 index continuity, gofmt, full diff review and `git diff --check`
  passed. Precommit fetch and `git ls-remote` still matched the parent. An initial
  documentation patch with duplicate STATE operations was rejected atomically;
  corrected without partial edits. No unrelated files were changed.
  Logs are ignored `build/P0044-*.log`.

## Limits and handoff

Incoming receiver and backing must already be live. The conservative proof rejects
harmless effects and independent class locals too. Explicit class-parameter grammar,
conditional member leaves, static/local/struct/nested/base results, field ownership,
class alias retention/destructors/containers, caller consumption, pointer/class result
transfer, broader shape/operator checks and exception/async unwinding remain open.
The same eighteen skips remain unqualified. String-index return typing, custom-type
for headers and direct class null initialization remain limited.

Publish and verify the remote SHA, then inspect native CI; fix failures first.
Next qualify conditional result trees with read-only `this.field` leaves under the
same proof, starting both-mode retained-allocation reproduction and mixed owned/
literal/borrowed lifetime controls. Do not relax write/call/alias barriers in that
slice. End after this pass.
