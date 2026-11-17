# P0052 - Adopt complete-statement owned factory assignments

- Date: 2026-11-17 UTC
- Task: T011 caller consumption of owned string results
- Parent checkpoint: `d39aefd84c2a349cff0e093e2416dbbdeba8c759`
- Commit lookup: exact `Pass-ID: P0052` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Complete P0051's assignment slice: complete expression statements assigning a direct
proven factory call to an identifier owned scalar string slot now store the original
allocation. Reuse the unchanged `adopts_string_call` proof inside `stmt.c`. Capture
RHS once, then release the current slot and replace it; self-snapshot and nested
argument replacement finish before release. Existing destination cleanup is unchanged.

Use a statement-local block and temporary rather than adding a global helper. Check
the temporary against active bindings and registered functions. This realizes the
contract's helper operation without introducing a globally colliding emitted name;
the proof already lives in the same emitter, so no duplicate/external proof is needed.
General expression emission, runtime, ABI and grammar are unchanged. Borrowed/mixed/
method/shape/body barriers and excluded assignment expressions retain prior behavior.

Add a Dream entry, both-mode Go checks and checked allocator harness, and wire the
new group into equal Makefile/Windows/CI selectors. Extend all 28 existing unsupported
factory definition controls with assignment fallback checks. Supersede only eligible
assignment C in the previous consumer test and aggregate audit; preserve all previous
initializer/lifetime counts and refcount assertions. Update feature/ownership docs,
source map, state, backlog and index.

## Validation

- Approved clean checkout, attached branch/origin and fetched parent verified;
  zero commits either side. History/memory show P0051 delivered, one worktree,
  available chats show no competing writer. Schedule already local/approved cwd.
- P0051 native [CI 44036460739](https://github.com/Ddemon26/DreamCompiler/actions/runs/44036460739)
  passed Linux job 133880403851 and Windows job 133880404255, including required checks.
  Restricted DNS and Zig installation access initially failed; normal approved
  fetch/toolchain execution succeeded without configuration/cache changes.
- Windows Zig **0.17.1**. Baseline `zig build test`: **174 discovered, 156 passed,
  18 skipped, zero failures/errors**, **48.11s**,
  `test_results/test_results_20261117_060416_windows.json`.
- Pre-fix `python codex/python/owned_result_audit.py`: **ten probes pass, 10.564s**.
  Each isolated three-assignment case retains **three** originals with **four**
  nonnull releases. Aggregate other consumers retain five. The new focused test
  fails both lifetime modes at the adoption C gate, **0.793s** package time;
  all ten context controls pass. No pre-fix identity execution is claimed.
- Final `go test -count=1 -v -run '^TestCompiler_OwnedCall(Assignment|Initializer)$' .`
  from `codex/go`: both groups pass, **7.771s**. Assignment: **12 subtests**;
  initializer: **74**, with 56 added assignment checks inside its definition controls.
  Per mode, ten replacements in each of 10,000 calls require eleven original
  allocations/releases in order, refcount one, peak live count two and zero survivors.
  Returned-result identity and old-slot release are checked separately. Main fixture:
  **19 copies/19 releases**, exact four-line output, self-snapshot/nested replacement,
  once-only effects, null/empty/escaped/const factories, loops/jumps/unbraced scopes,
  temporary collision and program-lifetime global. Native UBSan, Linux harness
  cross-link and normally linked output pass. Existing initializer assertions pass.
- Final `python codex/python/owned_result_audit.py`: **ten probes pass, 10.197s**.
  Per mode isolated assignment **4 releases/0 live**; other consumers **3/4**;
  owned initializer **6/0**, borrowed **7/0**, mixed **4/1** unchanged.
- Final `zig build test`: **175 discovered, 157 passed, 18 skipped, zero failures/
  errors**, **44.56s**, `test_results/test_results_20261117_060815_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five pass, 0.032s**.
- Required Windows Go quick protocol: **41 groups pass, 96.283s**; external
  compiler tests use `-count=1`. Linux cross-build
  `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`: **15/15
  steps pass**. All four required check exit codes are zero.
- Final documentation/path, encoding, selector, index and diff review pass (counts
  recorded in the current test snapshot). A first review helper assumed UTF-16 logs;
  corrected to the actual UTF-8 log encoding. Removed a trailing blank line found
  by `git diff --check`; final diff check passes.
  Initial documentation helper used the Windows default encoding and stopped on
  an existing UTF-8 file; corrected to explicit UTF-8 and removed a duplicate added
  paragraph. Initial post-fix focused run failed only the old eligible assignment
  C expectation in the previous consumer control; migrated it to exact adoption C.
  No failure was excluded or assertion weakened.

## Limits and handoff

The same eighteen skips and grammar/operator/shape limits persist. Only complete
assignment statements qualify. General assignment expressions, discarded calls,
append/console/argument consumers, mixed/method results, nested owned arguments,
pointer/class transfer, aliases/containers/destructors/fields and async/exception
unwinding remain separate. Cross-build/cross-linking is not native Linux execution.

Publish and verify the exact remote SHA, then inspect native CI when available.
Next audit discarded complete factory-call statements and define the actual-
definition, side-effect and lifetime contract before implementing another consumer.
End after this pass; do not start a second consumer in this invocation.
