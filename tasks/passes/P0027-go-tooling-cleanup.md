# P0027 - Retire unsupported Go library build entry points

- Date: 2026-11-14 UTC
- Task: T013; explicit user follow-up to prune unused files/scripts and docs
- Parent checkpoint: `57c8edf0edc7c05fc5385d2748ceb3011413fb49`
- Commit lookup: exact `Pass-ID: P0027` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

The Go directory still advertised unusable C library builders and recursive cgo,
benchmark and fuzz workflows despite supporting only the selected CLI quick tests.
The standalone builders require `codex/go/dream_api.c`, which is absent; the
PowerShell builder also computes its source root as `codex/`. No active repository
caller references either builder. The main PowerShell default previously entered
module installation and the broken C library build, while the Makefile default
also selected the unavailable library.

Removed `codex/go/build_lib.ps1` and `codex/go/build_lib.sh` (16,268 bytes total).
The main Makefile now exposes `quick-validate` and `help`, with quick as default.
Its actual build and Go commands are unchanged. The PowerShell launcher exposes
`quick` and `help`, defaults to quick, resolves build/test directories from its
own location, and preserves native failure codes. A build failure stops before
Go. Retired actions and switches are rejected; neither launcher installs modules,
builds cgo libraries, regenerates the lexer separately or purges caches.

The 22 quick groups remain identical across both launchers and CI. Current test
coverage descriptions remain in the Go README; its launcher contract and the main
testing guide now describe the supported defaults and failure behavior. About
1,000 net obsolete launcher lines were removed before adding pass metadata.

The audit also inspected documentation assets. GitHub repository/Pages metadata
reports a built public site at `https://ddemon26.github.io/DreamCompiler/`, sourced
from `main:/docs`. The HTML/CSS were therefore retained. The legacy cgo headers,
test fixtures, disabled benchmarks/fuzz sources and checksums remain because they
contain unique code or references requiring a replacement-coverage audit.

## Validation

Started clean in the approved local checkout, attached branch, exact origin, one
worktree and no competing active project chat in app inventory. Approved fetch
matched HEAD. P0026 [CI 43652638299](https://github.com/Ddemon26/DreamCompiler/actions/runs/43652638299)
was initially pending; both Linux and Windows jobs completed successfully before
edits began. No prior delivery was blocked.

- Before/after SHA-256 snapshots: **474 protected files unchanged**, including
  compiler/runtime/Dream sources, all active/deferred fixtures, plugin files,
  all `docs/` assets, all Go test/C/header/checksum files, build graph, supported
  Python runner/configuration and CI workflow.
- Both launchers and CI retain the exact original **22-group selector**. Windows
  PowerShell UTF-8 BOM preservation and PowerShell AST parsing **passed**.
- Controlled PowerShell probes: **seven passed**. Success/default/help work;
  build exit 17 propagates without calling Go, test exit 23 propagates, a retired
  action and missing Go fail before tools run. Help works without tools. Stubs
  record the actual build/test working directories and exact argument selection.
  These are launcher checks, not additional compiler regressions.
- GNU Make on Windows: `make help`, `make -n quick-validate` and default `make -n`
  **passed**; default/explicit plans match. `make -n build` **rejected as expected**.
  No native Linux Make execution is claimed. The first dry-run log redirection
  used a wrong relative location and failed before Make ran; it was corrected to
  run from `codex/go` with the log inside this checkout.
- `zig version`: **0.17.1**. `zig build test`: **154 discovered, 136 passed,
  18 skipped, zero failures/errors**, 48.74s;
  `test_results/test_results_20261114_163145_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.029s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all twenty-two groups passed**, 36.863s,
  through the pruned launcher with real tools and `-count=1`.
- Removed-entry reference searches and **95 local Markdown links** passed; final
  diff checks precede commit. No compiler/C/build inputs changed, so a cross-build was
  not required or rerun. Native P0027 CI awaits publication.

Compiler checks ran sequentially. Installed SDK/cache access used normal approval
without changing contents or permissions. Tools: Zig 0.17.1, re2c 3.0,
Python 3.13.17, Go 1.27.4. Ignored evidence: `build/P0027-*` (hashes, original
selector, controlled-probe JSON/stubs, Make plans, full/quick/Python logs).
Publication requires push and a matching `git ls-remote`; resolve this pass's
commit from the trailer instead of recording its own SHA here.

## Limits and handoff

Same 18 skips and existing compiler/language/ownership limits remain. No test
fixture or assertion was removed to improve results. The unique legacy cgo
sources and fixture-local Makefile are still unsupported; pruning them requires
an explicit coverage decision, not a claim that quick validates them. Checksums
remain because those legacy sources still reference their packages.

Pages assets are active publishing inputs and need a separate navigation/accuracy
repair. This pass does not delete them or claim the site's links now work.
Verify P0027 publication/native CI and repair failures first. Then resume T011
conditional-string initializer ownership unless the user selects another cleanup
slice. Do not start a second pass here.
