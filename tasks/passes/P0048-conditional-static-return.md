# P0048 - Clean locals on conditional read-only static string-field returns

- Date: 2026-11-16 UTC
- Task: T011 conditional static scalar string-field return cleanup
- Parent checkpoint: `f06e6c84dfadfeeb93db39cd999145d126b8f16a`
- Commit lookup: exact `Pass-ID: P0048` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Extend the [static-field contract](../../docs/development/static-member-return-cleanup.md)
to conditional value leaves. `is_scoped_string_result` reuses
`borrowed_static_member_result` for each static leaf. The literal-only predicate,
parameter/instance guards and whole-body read-only allowlist are unchanged. Every
leaf must qualify, including unselected branches. Capture once before reverse
cleanup and preserve only selected owned storage through the existing cleanup
path. No result copy/retain, global release, field ownership or fake receiver
binding is added. Unsafe body/leaf/shape and async/exception exits keep prior emission.

The [fixture](../../tests/basics/memory/conditional_static_return.dr) and
[Go group](../../codex/go/conditional_static_return_test.go) check ordinary/module
lifetime, exact C/output, mixed static/instance/parameter/local/literal leaves,
inherited/hidden owner symbols, null/empty/literal/managed backing, snapshots,
free/static/instance readers, alternate/nested/unbraced/loop/header/switch paths,
later declarations, temporary collisions and no-local readers. Clear borrowed
globals before backing leaves scope. Shared body controls now also target
conditional static returns. Former eligible static fallback controls migrate to
positive exact C in earlier groups; no allocator/refcount assertion is removed.
Both quick protocols and CI select the new group. Feature, function, storage,
static contract, source-map and handoff docs reflect the resulting boundary.

## Validation

- Clean approved checkout/path/attached branch/origin verified. One registered
  worktree, all-ref history, preceding automation memory and available chats show
  no unfinished/unpublished pass or competing active writer. Schedule view/TOML
  already use local execution and approved cwd; unchanged. Restricted fetch failed
  DNS; approved fetch succeeded. HEAD equals the fetched parent, zero commits on
  either side. No alternate checkout or worktree was created.
- P0047 [native CI 43874515506](https://github.com/Ddemon26/DreamCompiler/actions/runs/43874515506)
  succeeded for the exact parent SHA. Linux job 133323936987 and Windows job
  133323937437 passed build, Python, quick and full-suite steps.
- Toolchain: Zig **0.17.1**, re2c **3.0**, Python **3.13.17**, Go **1.27.4**.
  Restricted baseline could not locate the Zig installation. Approved installed
  toolchain/cache access succeeded without cache/config/permission changes.
  Unchanged `zig build test`: **172 discovered, 154 passed, 18 skipped, zero
  failures/errors**, **46.94s**;
  `test_results/test_results_20261116_115736_windows.json`.
- Pre-fix `go test -count=1 -v -run
  '^TestCompiler_ConditionalStaticReturn/(ordinary|module_entry)$' .` from
  `codex/go`: both modes fail the exact capture/cleanup C assertion, **0.535s**.
  No pre-fix lifetime execution is claimed: the complete assertion stops before
  the checked allocator. Automatic review rejected a proposed temporary
  assertion-removal command before execution; the complete test was run unchanged
  instead. No assertion removal or approval bypass occurred.
- `zig build` passed, exit zero. Initial four-group focused run: prior direct
  member, conditional member and static groups pass (**7.93s**, **8.93s**, **9.28s**
  group times). New lifetime modes and other controls pass; one new module
  unselected-call C expectation fails because the existing free-function symbol
  is lowered. Corrected the exact module symbol without compiler/grammar changes
  or removal of lifetime assertions. Overall initial run failed, **36.343s**.
- Final `go test -count=1 -v -run '^TestCompiler_ConditionalStaticReturn$' .`:
  **246 subtests pass**, **9.247s**. Each lifetime mode checks **67 nonnull releases**
  (one receiver, 66 strings), **35 output lines**, **four original-reference owned
  survivors** before shutdown and an empty registry afterward. Native UBSan,
  live release/refcount validation, Linux harness cross-linking and directly linked
  exact output pass. Independent tally: 66 callee allocations plus five caller
  allocations minus four transferred locals = 67 releases. Remaining 244 controls:
  208 shared body/shape/guard checks and 36 expression/async/exception checks.
  Unsafe mutation/alias/shape programs are emission-only and never executed.
- `zig build test`: **173 discovered, 155 passed, 18 skipped, zero failures/errors**,
  **47.26s**; `test_results/test_results_20261116_120305_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, **0.036s**.
- Supported Windows Go quick protocol from `codex/go`, `powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`: **all 39 groups pass**,
  **93.816s**, external compiler invocations use `-count=1`. Conditional static
  group **7.61s**, conditional member **6.97s**, direct member **6.62s**, direct
  static **7.90s**. Prior lifetime/allocator/refcount assertions pass.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **15/15 steps pass**. No native P0048 Linux runtime claim from cross-building.
- Reviewed all 18 pass files and full implementation/test/documentation diff.
  **236 local Markdown paths**, **eight anchors**, UTF-8 in all 18 files,
  PowerShell BOM, 39-group quick/CI selector/actual equality, P0001-P0048 index
  continuity and 35 annotated outputs pass. `gofmt -l` is empty; `git diff --check`
  passes. Generated lexer, checked allocator harness and earlier Dream fixtures
  are unchanged. Precommit fetch and `git ls-remote` still equal the parent,
  zero commits either side; no unrelated work appeared.
  Logs and commit message: ignored `build/P0048-*.log` / `build/P0048-commit.txt`.

## Limits and handoff

The same eighteen skips, grammar/operator/shape limits and valid incoming backing
precondition persist. Synchronous single-threaded execution is required. Other
static qualifiers, static pointer/array declaration emission, class-parameter
syntax, local/struct/nested/base instance receivers, caller consumption/retained
owned-result accumulation, aliases/containers/destructors/fields, pointer/class
result transfer and async/exception unwinding remain separate. Four selected
owned results deliberately remain live until shutdown; caller consumption is not
claimed. Cross-building/cross-linking is not native Linux runtime verification.

Publish and verify remote SHA; inspect native CI and repair failures first. Next
audit caller consumption of owned string results and retained result accumulation,
distinguish borrowed/owned calls, reproduce the boundary with checked lifetime
evidence and define one bounded implementation contract. End after this pass.
