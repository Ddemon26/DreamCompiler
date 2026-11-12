# P0014 - Give indexed string locals independent managed storage

- Date: 2026-11-12 UTC
- Task: T009
- Parent checkpoint: `36adec3f7569686806195ab55a3d70d9a950c961`
- Commit lookup: exact `Pass-ID: P0014` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

A scalar string local initialized by indexing borrowed the selected pointer, then
normal block cleanup passed it to `dr_release`. Elements may point to C literals
without a `DrRef` header or to managed backing owned elsewhere. P0014 wraps only
scalar string declarations initialized directly by an index in `dream_string_copy`.
The runtime helper copies valid string bytes/terminator into a new one-reference
managed allocation, preserves null and returns null on allocation failure. It does
not retain or inspect the input's allocation header. Existing cleanup and the
allocator are unchanged; array and pointer declarations are not wrapped.

The new annotated Dream entry and ordinary/module-entry Go matrix check literal,
dynamic and null backing, nested block cleanup, top-level copying and a once-only
index side effect. Before host compilation the Go test inspects emitted copy and
release calls. It then requires exact output with both the normal compiler linker
and a harness that validates cleanup operands against the real live allocator
registry before header access. Four nonnull local copies must be released, and
shutdown must leave an empty registry. Additional helper checks cover empty and
interior strings, unchanged backing reference counts, independent storage and
use after backing is freed. The harness adds no production allocator exemptions.

Both quick protocols and CI select `TestCompiler_IndexedStrings`. The new
[ownership checkpoint](../../docs/development/string-ownership.md), feature and
function docs describe the bounded contract. The memory guide now labels its old
pool/cycle/alias examples as historical design material rather than implemented
APIs. Broader string ownership is preserved as T011; T010 remains next.

## Validation

Started clean at the approved primary checkout on the attached branch/origin, one
registered worktree, no unpublished agent pass in memory/history and no competing
active writer in app inventory. Restricted fetch failed DNS; approved fetch passed,
and HEAD matched the fetched branch. P0013 [CI run 43265683253](https://github.com/Ddemon26/DreamCompiler/actions/runs/43265683253)
passed on Linux and Windows for the exact parent SHA. Automation tool/config
inspection confirmed existing local execution and the approved cwd; unchanged.

Windows: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted baseline
could not locate Zig's installed SDK; checks used approved SDK/cache access.
No SDK/cache content or permissions changed.

- Unchanged `zig build test`: 143 discovered, 125 passed, 18 skipped, zero
  failures/errors, 35.84s. Report:
  `test_results/test_results_20261112_160128_windows.json`.
- Initial Dream fixture used nested name shadowing, which was rejected as a
  redefinition. Renamed the inner local, preserving all output assertions.
- Uninstrumented pre-change Python focus and direct compiled execution happened
  to pass with exact output. These do not establish memory safety or reproduce
  the prior pass's particular misalignment panic. The checked registry is needed
  for deterministic invalid-release evidence, independent of allocator layout.
- First Go harness setup failed on duplicate installed/source `DrRef` headers and
  a module/local name conflict. Corrected header inclusion and renamed the fixture
  module before the actual ownership reproduction; no assertion was removed.
- Pre-change `go test -count=1 -v -run '^TestCompiler_IndexedStrings$' .` from
  `codex/go`: both modes failed at the live managed-allocation assertion before
  an invalid literal header read, 1.663s (body 1.22s).
- `zig build`: passed after implementation.
- Final focused command above: both modes passed generated-C, checked lifetime
  and direct linked exact-output assertions, 2.317s (body 1.82s).
- First full suite: 144 discovered, 125 passed, 18 skipped, one output assertion
  failure, zero errors, 32.50s. The new fixture's metadata placed an extra dynamic
  output line before the index value instead of at the end. Corrected metadata
  ordering to match the already-passing Go oracle; implementation and runtime
  output assertions were unchanged. Report:
  `test_results/test_results_20261112_160632_windows.json`.
- Final `zig build test`: **144 discovered, 126 passed, 18 skipped, zero failures/
  errors**, 33.45s. Report:
  `test_results/test_results_20261112_160736_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all twelve groups passed**, 15.706s;
  indexed-string group 1.52s. All previous groups remain selected; external
  compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.040s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  Linux binaries were not executed locally; native P0014 runtime awaits CI.

Ignored evidence: `build/P0014-*.log` contains baseline, fixture setup/reproduction,
focused, full-suite, quick, Python and cross-build output. Intermediate failures
above are not passing validation. No tests were excluded or assertions weakened.
Full diff review and `git diff --check` passed. All 57 local Markdown link
occurrences resolve; fixture metadata matches the independent Go oracle. Go
formatting, both quick/CI selections and the PowerShell UTF-8 BOM were checked.

## Limits and handoff

The same 18 unannotated skips and existing graphics/concurrency/helper exclusions
remain. Only scalar declarations initialized directly from indexing gain copied
ownership. For-header declarations, other initializer forms, reassignment, array/pointer-local cleanup,
function returns, early exits, exceptions, fields and compound-expression ownership
remain unqualified; no general lifetime, bounds, null-dereference or shape safety
is claimed. Copying adds an allocation and byte-copy cost. Scalar string indexing
and direct indexed float/char formatting are not fixed here.

Commit only this pass, push the approved branch and verify the remote SHA before
reporting delivery. Inspect native CI first next run and repair failures; otherwise
take T010, indexed primitive console formatting. Do not start another pass here.
