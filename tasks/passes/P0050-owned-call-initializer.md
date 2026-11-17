# P0050 - Adopt proven owned free-function string initializers

- Date: 2026-11-17 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `f31b174eb5bad57334c915b769d3c7f63cb3ff66`
- Commit lookup: exact `Pass-ID: P0050` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Implement the bounded [P0049 contract](../../docs/development/owned-call-result-consumption.md):
a complete scalar string declaration adopts a proven free-function factory's
original allocation instead of copying it and retaining the original until shutdown.
`adopts_string_call` resolves the actual registered lowered definition, excludes
active caller bindings and requires a synchronous string function whose entire
body is exactly one scalar nonstatic string declaration and its terminal direct
local return. Only literal, absent/null AST or scalar string-parameter sources
qualify; parameters have scalar primitive shapes. All other definitions and
consumers retain their existing emission. Destination ownership, callee return
proofs, runtime ABI, grammar and semantics are unchanged.

The new Dream fixture, Go group and separate checked C harness cover forward and
imported factories, same-spelled imported borrowed/entry-owned definitions,
literal/escaped/empty/const/grouped calls, typed-null/absent results, independent
snapshots and replacement, nested/header/body/jump cleanup, early bare/primitive
returns, string transfer, name collisions and top-level storage. Ten thousand
scoped calls in each mode allocate once, release that exact original base with
reference count one and have peak live count one. A separate transfer probe
returns the original to the harness for one release. No generated ownership
statement is removed. Top-level storage correctly retains program lifetime.

The existing audit helper supersedes only eligible initializer C and live counts;
all prior lifetime fixtures, allocator/refcount assertions and fallback programs
are unchanged. State, backlog, index, feature/ownership/source-map and Go docs
record the scope. Quick selectors in both launchers and CI include the new group.
No editor-plugin or generated lexer work is included.

## Validation

- Approved checkout, attached branch, clean starting worktree and public origin
  verified; preceding run memory, all-ref history, one registered worktree and
  available chats show no unfinished pass or competing active project writer.
  Schedule view/TOML already use local execution and the approved cwd; unchanged.
- Restricted fetch failed DNS; approved `git fetch origin
  codex/compiler-stabilization` succeeded. Parent equals fetched branch with zero
  commits on either side. Exact-parent [CI 43993419794](https://github.com/Ddemon26/DreamCompiler/actions/runs/43993419794)
  has matching head SHA and success; native Linux job 133739469239 and Windows
  job 133739469591 pass build, Python, quick and full-suite steps.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Normal installed toolchain/cache access used approved execution, without
  changing sandbox settings, permissions, caches or toolchain configuration.
- Baseline `zig build test`: **173 discovered, 155 passed, 18 skipped, zero
  failures/errors**, **61.36s**; `test_results/test_results_20261116_192137_windows.json`
  (local date). The corrected pre-fix regression fails caller-copy C assertions
  in both modes, **0.702s** package time. The C gate stops before runtime; no new
  pre-fix runtime/identity result is claimed. P0049 separately measured retention.
- Initial new fixtures encountered existing literal-null semantic rejection,
  async-result typing and pointer string equality. Supported typed-null/Console
  paths and artifact-free rejection controls replace those assumptions. Existing
  return temporary, boolean lowering and module-shadow diagnostic spellings were
  reflected in new expectations. The new harness originally assumed top-level
  scope release; source review corrected it to require exactly the original global
  at shutdown. These are explained fixture corrections, with no existing test,
  compiler diagnostic or lifetime assertion weakened. One gofmt invocation used
  a root-relative path from `codex/go`; it was rerun successfully from the root.
- Final `go test -count=1 -v -run '^TestCompiler_OwnedCallInitializer$' .`
  from `codex/go`: **74 subtests pass**, **6.829s** package time. Both lifetime
  modes check exact C/output, native UBSan, Linux harness cross-link and directly
  linked output. Ordinary main fixture: **16 copies/15 scoped releases**; module:
  **17/16** (one independent borrowed import copy). Each leaves exactly one
  original program-lifetime global with one reference before shutdown, none after.
  Independent tally: exercise first/escaped/empty/backing/snapshot/replacement/
  nested/zero-header/multi-header = nine, three loop bodies = twelve, bare/scalar/
  collision = fifteen, global = sixteen. Null adds neither allocation nor release.
  The separate 10,000-call identity and string-transfer probes are not in that tally.
  Fifty-six unsafe/unsupported definition controls preserve caller copies, four
  subtests check other consumers and primitive parameter positives, and twelve
  artifact-free rejections cover unresolved/shadowed/binding/null/async cases.
- Supported Windows Go quick protocol from `codex/go`, `powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`: **all 40 groups pass,
  151.212s**. External compiler tests use `-count=1`. The final focused rerun above
  follows a test-only addition distinguishing same-spelled imported borrowing;
  compiler and all other quick-group inputs were unchanged. No P0037-P0048
  assertion/count/fixture migration was necessary: their complex factories remain
  outside this proof. All their existing checked lifetime assertions pass intact.
- Final `zig build test`: **174 discovered, 156 passed, 18 skipped, zero failures/
  errors**, **80.83s**; `test_results/test_results_20261116_193319_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five pass, 0.081s**.
- `python codex/python/owned_result_audit.py`: **eight probes pass, 12.184s**.
  Per mode, owned initializers **6 releases/0 live** (P0049: 6/5); borrowed
  **7/0**, mixed **4/1**, other consumers **3/5**, unchanged. The six releases
  now reclaim original factory allocations plus backing, rather than caller
  copies plus backing. No probe program or other-consumer assertion is weakened.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **15/15 steps pass**. Cross-build/cross-link success does not establish native
  Linux execution of P0050; inspect the new commit's CI after publication.
- Final review: **243 local Markdown paths**, UTF-8 in all **18 pass files**,
  P0001-P0050 record/index continuity, all **40** identical quick/CI selector
  groups matched to passing output, preserved PowerShell BOM, gofmt and Python
  syntax checks pass. New links have no fragment targets needing anchor checks.
  Reviewed implementation, new regressions and complete documentation diff;
  `git diff --check` passes. Existing ownership fixtures/harness/assertions are
  unchanged. Precommit fetch and `git ls-remote` still equal the parent, zero
  commits either side; no unrelated work appeared. Staged diff is reviewed before
  commit. Logs: ignored `build/P0050-*.log`.

## Limits and handoff

The eighteen skips and previous grammar/operator/shape limitations persist.
Direct literal-null local initialization, custom-type/array parameter syntax and
async-result string declarations remain rejected. Caller shadowing is already
rejected before codegen; the proof also guards it defensively. Unsafe shape,
recursive, effect and exception controls are emission-only and never executed.
Top-level storage remains live until shutdown. Other consumers, nested owned
argument temporaries, methods, mixed/general return flow, pointer/class transfer,
aliases/containers/destructors/fields and async/exception unwinding remain separate.

Publish and verify the exact remote SHA, then inspect native CI and repair failures
first. Next audit plain assignment into existing owned scalar slots using the same
factory proof: once-only evaluation, old-slot release and unsupported fallbacks.
Do not implement another consumer or start a second pass in this invocation.
