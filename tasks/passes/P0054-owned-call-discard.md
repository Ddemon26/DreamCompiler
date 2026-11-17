# P0054 - Release discarded proven factory results

- Date: 2026-11-17 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `a1e09c068f5e059171459f7db20354c8cd759061`
- Commit lookup: exact `Pass-ID: P0054` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A complete standalone call to a proven string factory transferred an allocation
with no destination to reclaim it before shutdown. Under the
[P0053 contract](../../docs/development/owned-call-discard.md), the expression
statement emitter now wraps that call in `dr_release((void *)...)`. The existing
actual-definition proof remains the gate, including module lowering and imported
factories. Argument evaluation and the call finish before the single release.
The caller adds no copy, retain, temporary binding or cleanup entry.

Add one annotated Dream entry, a checked allocator harness and a Go group wired
into both quick protocols and CI. The existing initializer fallback matrix now
checks bare call statements for its 28 unsupported definitions per mode while
preserving its declaration and assignment assertions. Its eligible standalone
factory result now requires release. The audit migrates only that eligible
discard in the aggregate consumer case and adds an isolated case.

## Validation

- Approved existing checkout and attached branch, clean start and origin verified.
  Restricted fetch failed DNS. Approved fetch succeeded and HEAD equaled the
  fetched parent, zero commits either side. One registered worktree and no
  competing active project chat. The schedule already uses the local approved
  checkout. Exact-parent [CI 44155266595](https://github.com/Ddemon26/DreamCompiler/actions/runs/44155266595)
  passed Windows job 134294182025 and Linux job 134294182599, both with build,
  Python runner, Go/stability and full Dream tests.
- Zig 0.17.1 baseline `zig build test`: **175 discovered, 157 passed, 18 skipped,
  zero failures or errors** in 66.82s (`test_results_20261117_154217_windows.json`).
- Before the compiler edit, the checked isolated audit measured **zero releases
  and eight surviving originals** per mode (2.805s). The first probe's generated-C
  matcher omitted debug directives and failed before runtime. The corrected
  matcher anchors the bare call to the expression-statement debug comment.
  The focused pre-fix Go run failed all four subtests at the expected missing
  release gates (0.643s), before running the checked lifetime executables.
- `go test -count=1 -v -run '^TestCompiler_OwnedCall(Discard|Initializer|Assignment)$' .`
  from `codex/go`: **90 subtests pass** in 11.602s. Each mode executes 10,000
  isolated discards at peak live count one and checks the original pointer,
  reference count one and an empty registry after every call. Main checks
  16 allocations and 16 releases, empty shutdown, top-level/scoped/unbraced
  paths, loop/switch jumps, early return, literal/const/empty/escaped/absent/
  typed-null results, argument effects and backing/snapshot survival.
  Generated C distinguishes same-spelled borrowed/owned module functions,
  checks replacement before outer release and excludes for headers, compound/
  async results and nested owned arguments. Native UBSan, Windows-to-Linux
  harness cross-link, and normal linking with exact output pass in both modes.
- `python codex/python/owned_result_audit.py`: **12 probes pass** in 13.901s.
  Per mode isolated discard **8 releases/0 live**, other consumers **4/3**,
  isolated assignment **4/0**, owned initializer **6/0**, borrowed **7/0** and
  mixed **4/1**. Checks include native UBSan, cross-linking and exact C/output.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed** in 0.042s.
- Final `zig build test`: **176 discovered, 158 passed, 18 skipped, zero failures
  or errors** in 57.49s (`test_results_20261117_154619_windows.json`).
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`:
  **42 groups passed** in 122.767s, using `-count=1`.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **15/15 steps succeeded**.
- Reviewed the full diff. Checked 20 UTF-8 files, 272 local Markdown paths,
  54 consecutive pass IDs, all three quick/CI selectors, the PowerShell UTF-8
  BOM, Go formatting, Python syntax and `git diff --check`. Ignored logs are
  under `build/P0054-*.log`.

## Limits and handoff

Append, Console output and direct parameter use still retain factory results.
The discard slice consumes only the outer call's result on normal completion.
It adds no argument-ordering, parameter-escape, alias, field/container, general
result-flow or exception/async guarantees. Eighteen unannotated entries remain
skipped. Linux cross-build and cross-linking require native CI for runtime evidence.

Verify P0054 publication and inspect native CI before new work. Next audit the
retained `view(owned())` case for direct proven factory arguments to synchronous
string parameters, including parameter escape and evaluation order.
