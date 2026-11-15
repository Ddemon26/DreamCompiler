# P0015 - Restore Linux indexed-string harness builds

- Date: 2026-11-13 UTC
- Task: failed P0014 CI repair (before T010)
- Parent checkpoint: `d156d5cab31304eb2cec695d83ab0f4083acac35`
- Commit lookup: exact `Pass-ID: P0015` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

P0014 was published, but [CI run 43326685373](https://github.com/Ddemon26/DreamCompiler/actions/runs/43326685373)
failed on Linux in both indexed-string harness modes: strict C11 compilation of
`exception.c` could not see the POSIX `strdup` declaration. Windows was cancelled
before completing its build. The harness omitted `-D_GNU_SOURCE`, already present
in the normal `build.zig` runtime flags.

The Go harness now uses that same macro while retaining C11, undefined-behavior
sanitization, generated-code assertions, checked live-allocation cleanup and exact
output through both linking paths. On Windows, each ordinary/module-entry mode
also cross-links the complete harness for `x86_64-linux-gnu` before native linking
and execution. This detects the Linux header/link defect on the development host;
it does not execute Linux binaries. Compiler/runtime behavior is unchanged.
The Go README and ownership checkpoint describe this qualification boundary.
T010 remains untouched; no test exclusions or weakened assertions were added.

## Validation

Started clean in the approved primary checkout, attached branch and origin. One
registered worktree, only this active project writer in app inventory. Memory,
all-ref history and records confirmed P0014 delivery with no unpublished pass.
Restricted fetch failed DNS; approved fetch succeeded and matched HEAD. Automation
inspection confirmed local execution and the approved cwd; no schedule change.
Read the failed Linux job logs before selecting this repair instead of T010.

Windows tools: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Approved SDK/cache
access was used after restricted Zig could not locate its installation. No SDK,
cache content or permissions changed.

- Initial `zig build test`: 144 discovered, 126 passed, 18 skipped, zero failures/
  errors, 38.93s; `test_results/test_results_20261113_005521_windows.json`.
  Its last seconds overlapped the first focused reproduction due to polling the
  process before completion. Treat this as provisional; the final suite below
  ran alone after the focused process completed. All subsequent compiler checks
  ran sequentially.
- Pre-change `zig cc -target x86_64-linux-gnu -std=c11 -c
  src/runtime/exceptions/exception.c -o build/P0015-exception-before.o`:
  failed with the same four `strdup`/pointer conversion errors as CI.
- Added Linux cross-link regression before fixing flags, then from `codex/go`
  ran `go test -count=1 -v -run '^TestCompiler_IndexedStrings$' .`:
  both modes failed with those same four errors, 0.868s (body 0.38s).
- After adding the macro, focused command above: both modes passed Linux
  cross-link, native checked-lifetime execution and direct-linked exact output,
  2.846s (body 2.39s).
- Final `zig build test`: **144 discovered, 126 passed, 18 skipped, zero failures/
  errors**, 34.21s; `test_results/test_results_20261113_005631_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: **all twelve groups passed**, 17.419s;
  indexed-string group 2.19s. External compiler tests use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, 0.045s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: **passed**.
  No Linux binaries were executed locally; native runtime awaits P0015 CI.

Ignored evidence: `build/P0015-*.log`. Full diff review and `git diff --check`
passed; all 53 local Markdown link occurrences resolve and Go formatting passed.
Publication requires a push and matching remote SHA.

## Limits and handoff

The same 18 unannotated skips and existing exclusions remain. This restores the
harness build contract and adds Linux cross-link coverage on Windows; no exception
ownership, string lifetime, general shape compatibility or indexed formatting fix
is claimed. Existing const-discard warnings in the harness remain. Native Linux
and Windows P0015 CI must be inspected after publication; repair failures first.
Otherwise take T010, indexed primitive console formatting. End after this pass.
