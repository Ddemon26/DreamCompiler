# Test and agent tooling

Follow the repository-root `AGENTS.md`. This file adds only tooling-specific guidance;
`docs/development/source-map.md` is the navigation map and
`docs/development/agent-workflow.md` defines the pass/delivery flow.

- `test_cli.py run` invokes the supported Python runner in `python/test_manager.py`.
  `quick` currently selects the full suite; use `run --pattern` for a small selection.
- `test_config.json` holds timeouts, categories, and explicit helper-module fixtures.
  A fixture exclusion must represent a supporting source file, not a hidden failure.
- `python -m unittest discover -s codex/python -p test_test_manager.py` checks runner
  outcomes, including timeouts and fixture discovery.
- Run the Go quick protocol from `codex/go`: `make quick-validate` on Linux or
  `powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
  on Windows. It builds the real compiler and runs driver/module regressions.
- Direct Go invocations that launch the compiler require `-count=1`; Go's cache does
  not track the external binary. The legacy recursive cgo suites are not supported.
- Preserve UTF-8 with BOM for `go/build_and_test.ps1`, because Windows PowerShell 5.1
  otherwise misreads its non-ASCII strings. Keep scripts usable on Windows and Linux.
- Keep setup scripts, README, and CI aligned with the pinned Zig 0.17.1 toolchain.
- Store substantial one-off analysis helpers under `codex/python/`; generated reports
  and commit-message files belong in ignored `build/` or `test_results/` directories.
- The reusable schedule prompt is `prompts/daily-progress.md`. Change it together with
  the root instructions and workflow when delivery or handoff behavior changes.

Compiler regressions require focused tests, the full Dream suite, the Go quick checks,
and runner tests as specified at the root. Documentation-only tooling instructions
need path/link checks and diff review; do not claim a new compiler run without one.
