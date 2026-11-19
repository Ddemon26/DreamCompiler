# P0062 - Restore CI Python setup on Ubuntu 26.04

- Date: 2026-11-19 UTC
- Task: CI baseline recovery before T011 resumes
- Parent checkpoint: `9e257f5e40c3edbf12f7fb93bf24700bb6aeb675` (P0061)
- Commit lookup: exact `Pass-ID: P0062` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

In [P0061 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44376422082),
`ubuntu-latest` resolved to Ubuntu 26.04. The Python setup step could not
find Python 3.8 for x64 on that image. The Linux job failed before compiling,
and matrix fail-fast canceled the Windows job during Zig setup.

The [CI workflow](../../.github/workflows/ci.yml) now selects Python 3.13 for
both platforms. The official [Python versions manifest](https://raw.githubusercontent.com/actions/python-versions/main/versions-manifest.json)
listed stable Python 3.13 x64 builds for Ubuntu 26.04 and Windows. Local
workflow lint also flagged the legacy checkout and Python setup actions, so CI
now uses their Node 24-based v6 releases. The runner matrix and test commands
are preserved.

## Validation

- The active schedule uses the existing local checkout. The attached branch
  was clean, with the approved fetch and push URLs. Restricted fetch failed
  DNS. Approved fetch and `git ls-remote` confirmed P0061 at the remote tip.
- `actionlint` v1.7.11 passed on the updated workflow. All three Go quick
  selectors (CI, Makefile and PowerShell) matched and contained 43 groups.
- Windows local Python 3.13.17 passed all **5** tests in
  `python -m unittest discover -s codex/python -p test_test_manager.py` (0.047s).
- Restricted Zig could not resolve its installed standard library. The
  approved retry used Zig **0.17.1**. `zig build test` ran **181** Dream entries
  with **163 passed, 18 skipped, zero failures or errors** (58.35s suite).
  Report: `test_results_20261118_180634_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  passed **43 groups** (127.753s Go runtime, `-count=1`).
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed **15/15** build steps. Native Linux execution comes from CI.
- Reviewed the diff and checked `git diff --check`, six local links, the pass
  index and the Dream report path. Logs are in ignored `build/P0062-*.log`.

## Limits and handoff

Eighteen Dream entries remain unannotated. The ownership probes in P0061
remain in that record and were not rerun in this CI pass. Verify both
exact-commit CI jobs, then isolate `return (text += owned());` from a
synchronous string function with an owned local in both compilation modes.
Measure the factory operand and verify that the caller can read the join.
