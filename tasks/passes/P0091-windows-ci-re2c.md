# P0091 - Provision re2c in a required Windows CI step

- Date: 2026-11-26 UTC
- Task: CI baseline recovery before T011
- Parent checkpoint: `b75051c2acc2f8235176b777f16246eb2e71874a` (P0090)
- Commit lookup: exact `Pass-ID: P0091` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

[P0090 CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/45691819027)
passed on Ubuntu. In the Windows job, the Chocolatey community feed returned
HTTP 503 for re2c 3.0. The dependency step continued, and `zig build` failed
with `failed to spawn and capture stdio from re2c: FileNotFound` before tests ran.

The [workflow](../../.github/workflows/ci.yml) now downloads the
[re2c 3.0 release archive](https://github.com/skvadrik/re2c/releases/tag/3.0),
checks its SHA-256, and builds the C lexer generator with the Windows runner's
CMake and Visual Studio tools. It runs `re2c.exe --version` before publishing
the executable's directory through `GITHUB_PATH`. Required setup failures stop
the job. Optional Windows Python packages keep their own best-effort step.

## Validation

- The checkout was clean and attached to the approved branch at the fetched
  P0090 SHA. The active schedule already uses that local checkout.
- `actionlint` 1.7.12 accepted the edited workflow. The Windows PowerShell step
  downloaded and verified the archive, built it using CMake 4.4.4 and Visual
  Studio 18, and put the directory of the executable reporting `re2c 3.0` in
  `GITHUB_PATH`. The first local trial exposed that upstream places the EXE in
  the build root. The corrected step passed.
- A corrupt archive failed the checksum before extraction, build, or PATH
  publication. A simulated native CMake exit of 7 halted the step after
  extraction and before PATH publication.
- Zig 0.17.1 resolved the newly built re2c first on PATH. `zig build --cache-dir
  build/P0091-zig-cache --summary all` passed 15/15 steps and ran lexer generation
  in a fresh local cache.
- `zig build test` discovered 194 Windows entries. **176 passed**, **18 skipped**,
  and zero failed or errored (63.27s). Report `test_results_20261125_190358_windows.json`
  matched P0090 on every entry status, passing output, and skip reason.
- The Windows Go quick protocol from `codex/go`, `powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`, passed all 43 groups
  (166.996s Go suite time, `-count=1`). The hosted build and test steps are unchanged.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`
  passed 15/15 steps using cached build artifacts. Linux binaries were not run locally.
- `python -m unittest discover -s codex/python -p test_test_manager.py`
  passed 5/5 (0.043s).
- Six changed local links, all 91 pass index entries, diff review, and
  `git diff --check` passed. Setup and compiler logs are in `build/P0091-*.log`.

## Limits and handoff

The recovery changes CI dependency setup. Eighteen unannotated Dream entries
remain skipped. Native GitHub Actions runs are the check for hosted runner
behavior. Verify both P0091 jobs, then measure retained factory joins in
`Console.Write(text += deeper());` and `Console.WriteLine(text += deeper());`.
Check once-only operand evaluation, release of the third join, and output from
the current destination when an argument replaces it.
