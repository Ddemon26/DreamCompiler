# P0026 - Remove obsolete tooling and consolidate operational documentation

- Date: 2026-11-14 UTC
- Task: T012; explicit user request to prune unused files/scripts and clean docs
- Parent checkpoint: `52de683a1752e1a6b22b5ecfe673fdc72f23cc76`
- Commit lookup: exact `Pass-ID: P0026` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

One repository hygiene pass removes proven obsolete entry points and replaces
misleading operational documentation with references to the actual build and
test paths. No compiler behavior, fixtures, exclusions, plugin code, supported
runner or CI selection changes. The user selected this priority ahead of T011.

Eleven tracked files, totaling 66,516,306 working-tree bytes, were removed:

| Removed files | Evidence and replacement |
| --- | --- |
| `go1.21.0.linux-amd64.tar.gz` | 66,479,500-byte downloaded installer, not an input to builds, CI or startup scripts. Tools belong outside the checkout. A narrow root Go-installer ignore rule prevents reintroduction. |
| `test.c`, `test_debug.c`, `test_validation.dr` | Root scratch main/lexer/print samples, unreferenced by the build, test discovery or active scripts. The lexer tool and annotated fixtures remain. |
| `BuildWithLinks.bat` | Unreferenced manual link wrapper with obsolete runtime include layout. Normal compiler linking and the supported runner replace it. |
| `codex/python/test_runner.py` | Superseded standalone runner, not imported by CLI/build/CI. It hardcodes `libdreamrt.a` and the old include layout. The supported CLI imports `test_manager.py`, whose regression tests remain. |
| `codex/test_runtime_integration.ps1`, `codex/test_runtime_integration.sh` | Unreferenced scripts invoke generated `dream` as the compiler, check obsolete object locations and leave temporary unannotated fixtures. Existing annotated and Go tests cover real runtime paths. |
| `codex/go/validate_complete_workflow.ps1` | Unreferenced legacy wrapper requires absent `dream_api.c`, enabled benchmark/fuzz files and unsupported cgo workflow. Supported quick protocols remain. |
| `codex/go/GO_SETUP.md`, `codex/WINDOWS_TESTING_IMPROVEMENTS.md` | Unlinked duplicate operational notes with obsolete paths/counts or recursive cgo commands. Current prerequisites, optional tools and validation live in the maintained guides. |

Reference searches covered build/CI/tooling/docs and the rest of the repository;
only the removed wrapper's own usage and stale guide instructions needed pruning.
All removals were explicit tracked files inside the approved checkout. No recursive
delete, cache purge, git clean, history rewrite or plugin edit was performed.
The installer remains recoverable in Git history; this is not a history purge.

The maintained testing reference is `codex/TEST_GUIDE.md`. Compiler testing links
there rather than embedding an invented runner. Build/architecture/development
guides and the Markdown documentation index now use real source paths, executable
names, supported commands, current pipeline limits and dated evidence. The current
Go coverage descriptions remain; their stale cgo tutorials were removed. README
points to Markdown and replaces the contradictory isolated-worktree instruction
with the existing-checkout policy. Around 5,600 net text lines were removed before
adding this pass's handoff; no new maintenance script was introduced.

## Validation

Started clean in the approved primary checkout, attached branch, exact origin,
one worktree and no competing active project chat in app inventory. Approved fetch
matched HEAD. P0025 [CI 43649932934](https://github.com/Ddemon26/DreamCompiler/actions/runs/43649932934)
passed both native Linux/Windows jobs, full suites, stability regressions and the
Linux-host Windows cross-build for the exact parent SHA.

- SHA-256 snapshots before/after: **367 protected files unchanged**, including
  compiler/runtime/Dream sources, active and deferred fixtures, standard library,
  plugins, `build.zig`, supported Python CLI, test configuration and CI workflow.
- `python codex/test_cli.py list` before/after: **same 154 entries and categories**.
- `python codex/test_cli.py --help`: **passed**.
- `zig version`: **0.17.1**. Full `zig build test`: **154 discovered, 136 passed,
  18 skipped, zero failures/errors**, 51.72s;
  `test_results/test_results_20261114_161430_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.044s.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all twenty-two groups passed**, 41.955s,
  with external compiler tests using `-count=1`.
- Revised operational Markdown: **101 local links resolved** before the pass
  metadata was added; **184** across all revised/new Markdown in the final check.
  Added handoff links, inbound fragment searches, literal
  source/tooling paths, narrow installer ignore rule and deletion-reference
  searches were also checked. No current operational references to retired entry
  points remain; this record intentionally inventories their names.
- Diff review and `git diff --check` precede commit. No C/build inputs changed,
  so no new cross-build was required or claimed. Native P0026 CI awaits publication.

Compiler checks ran sequentially with approved installed SDK/cache access. Tools:
Zig 0.17.1, re2c 3.0, Python 3.13.17 and Go 1.27.4. No SDK/cache contents or
permissions changed. Ignored evidence: `build/P0026-{protected-files.json,
discovery-before.log,discovery-after.log,cli-help.log,full-suite.log,python.log,go-quick.log}`.
Publication requires push and matching `git ls-remote`; the report supplies the
commit identity rather than putting a self-referential SHA in this record.

## Limits and handoff

Same 18 skips and language/ownership limits remain. Historical pass/test evidence,
versioned language references, design examples, supported CLI wrappers, unique
legacy cgo fixtures and experimental compiler code remain. A missing reference
alone does not establish safe deletion. The HTML documentation viewer and broader
language/API tutorials still need their own path/accuracy audit; README now directs
readers to the maintained Markdown index. This is the first bounded cleanup pass,
not a claim that every dormant file has been eliminated.

Verify P0026 publication/native CI, repairing failures first. Then resume the
queued T011 conditional-string initializer ownership work unless the user selects
another cleanup slice. Do not start a second pass here.
