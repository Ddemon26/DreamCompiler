# P0049 - Measure owned call-result accumulation and define initializer adoption

- Date: 2026-11-17 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `7f356d9ab1e2145f0f10fe31142f3db34a09072b`
- Commit lookup: exact `Pass-ID: P0049` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Audit actual call-result consumers and distinguish transferred owned storage from
literal/parameter/field borrowing and mixed returns. The
[contract](../../docs/development/owned-call-result-consumption.md) maps AST result
metadata, backend function/method lookup, module lowering, declaration copies,
qualified returns, other consumers and runtime allocation/release paths.

The reproducible [audit helper](../../codex/python/owned_result_audit.py) adds four
complete programs in ordinary and module-entry modes. It inspects generated C,
uses the unchanged checked allocator harness with real runtime/UBSan, cross-links
for Linux and checks directly linked exact output. Every release operand must be
a live managed base with one reference; remaining references and shutdown registry
are checked. Owned initializer originals accumulate despite caller scope cleanup;
borrowed controls remain safe. The counts explicitly describe debt, not adoption
acceptance. No existing assertion, fixture, compiler or runner is changed.

The first implementation contract permits adoption only for a direct free-function
call initializing a scalar string declaration. Require an actual synchronous
definition with exactly one scalar string local declaration and its terminal
direct local return. Initializers are limited to literal/null/absent or scalar
string parameters; parameters have scalar primitive shapes. Resolve actual lowered
names and binding identity; reject caller shadowing and every unsupported body,
shape, receiver or effect. Keep current fallback copies and return proofs intact.
Original allocation identity evidence is required in the implementation pass.
Methods, mixed results, other consumers and general return-flow inference remain
separate. Compiler behavior and C ABI are unchanged.

State, backlog, pass index, source map, storage documentation and measured snapshot
carry the evidence and next step. No editor-plugin work is included.

## Validation

- Approved path, attached branch, origin and clean worktree verified; all-ref
  history, preceding run memory and one registered worktree show no unfinished or
  unpublished pass. Available chats show no competing active project writer.
  Schedule view/TOML already use local execution and approved cwd; unchanged.
- Restricted fetch failed DNS; approved `git fetch origin
  codex/compiler-stabilization` succeeded. HEAD equals fetched parent with zero
  commits on either side. P0048 publication is confirmed.
- Exact-parent [native CI 43935983779](https://github.com/Ddemon26/DreamCompiler/actions/runs/43935983779)
  has head SHA equal to the parent above and conclusion success. Linux job
  133540564284 and Windows job 133540563606 pass build, Python, Go quick and Dream
  full-suite steps. Restricted CLI metadata failed network; approved metadata and
  connected job-step reads succeeded.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted baseline could not locate Zig's installation. Approved installed
  toolchain/cache access passed without changing configuration/caches/permissions.
- `zig build test`: **173 discovered, 155 passed, 18 skipped, zero failures/errors**,
  **49.46s**; `test_results/test_results_20261116_180209_windows.json` (local date
  in the filename). Compiler sources are unchanged throughout this pass, so this
  run also validates the final compiler checkpoint; no redundant rerun is claimed.
- `python codex/python/owned_result_audit.py`: **eight probes pass, 9.439s**. Per
  mode: owned initializers **6 releases / 5 live**, borrowed initializers
  **7 / 0**, mixed initializers **4 / 1**, other consumers **3 / 5**. All original
  survivor refcounts are one; shutdown empties every registry. Generated C, checked
  native UBSan output, Linux harness cross-linking and directly linked output pass.
  The contract explains independent allocation tallies; identity/peak bytes are
  not measured. An initial helper launch used the wrong compiler path and failed
  before compiler execution; corrected to the supported `zig-out/bin/DreamCompiler`.
  No assertion or expectation was weakened; all initially specified counts/C/output
  checks passed unchanged on the corrected invocation.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five pass, 0.056s**. An initial combined validation command used a root-relative
  log path from `codex/go`; redirection failed before tests ran. Correct root and
  Go-directory invocations below were run separately; the shell's zero exit for
  that failed launch is not test evidence.
- Supported Windows Go quick protocol from `codex/go`, `powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`: **all 39 groups pass,
  101.704s**. External compiler tests use `-count=1`. Existing lifetime/refcount
  assertions are unchanged and pass.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **15/15 steps pass**. Cross-build/cross-link success does not prove native Linux
  execution of the audit helper/programs.
- Final review: **212 local Markdown paths**, **29 source-symbol references in
  eight source files**, P0001-P0049 index continuity, UTF-8 in all nine pass files
  and Python syntax checks pass. These documents contain no local fragment links
  requiring anchor checks. Reviewed the complete audit/contract/handoff diff
  against implementation and measured counts; `git diff --check` passes.
  `python -m py_compile
  codex/python/owned_result_audit.py` passes. Tracked `src`, `tests`, `codex/go`,
  `codex/c`, `.github` and `build.zig` are unchanged, including generated-code
  predicates and checked allocator assertions. Logs: ignored `build/P0049-*.log`.
  Precommit fetch and `git ls-remote` still equal the parent SHA, zero commits
  either side; no unrelated work appeared.

## Limits and handoff

The same eighteen skips and grammar/operator/shape limits persist. This pass
measures accumulation and defines a conservative implementation contract; no
caller adoption or general result cleanup is implemented. Incoming borrowed
backing must already be live. Other consumer contexts, methods, mixed provenance,
general/recursive return flow, pointer/class result transfer, aliases/containers/
destructors/fields and async/exception unwinding remain separate. The eight audit
probes are manual baseline analysis, not a new quick/CI feature regression group.

Publish and verify remote SHA; inspect native CI and repair failures first. Next
implement the contract's direct proven-owned free-function initializer adoption
with intact pre-fix regressions, original allocation identity checks and bounded
repeated-call storage. Explain genuinely eligible prior fixture/count migrations,
preserving all borrowed/mixed/method/shape/effect fallback and lifetime assertions.
End after this pass.
