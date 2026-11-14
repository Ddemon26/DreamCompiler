# DreamCompiler testing guide

Run commands from the repository root unless a different directory is specified.
Use Zig **0.17.1**, re2c, Python 3.8 or newer, and Go for the CLI stability checks.
Install tools outside the checkout; installer archives are not repository inputs.
The build and CI definitions are [build.zig](../build.zig) and
[ci.yml](../.github/workflows/ci.yml).

## Full validation

```sh
zig version
zig build test
python -m unittest discover -s codex/python -p test_test_manager.py
```

`zig build test` builds the compiler and runtime, then invokes
`codex/test_cli.py run`. The CLI imports `python/test_manager.py`; it is the
supported annotated Dream runner. Results are written to ignored `test_results/`.
See the [dated results](../tasks/MostRecentTestResults.md) for measured counts,
skips and platform limits. No fixed runtime or coverage percentage is promised.

From `codex/go`, run the supported quick protocol:

```sh
# Linux
make quick-validate
```

```powershell
# Windows
powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick
```

These build the real compiler and run the selected CLI stability groups. They
require no cgo library or downloaded Go modules. Use `-count=1` for direct Go
invocations because Go does not track changes to the external compiler binary.
See the [Go checks](go/README.md) for coverage and legacy exclusions.

Compiler tests share `build/bin/dream.c` and `dream`/`dream.exe`. Run them
sequentially; concurrent runs can overwrite each other's inputs and executables.

## Focused checks

Build first with `zig build`, then select existing source fixtures:

```sh
python codex/test_cli.py list --pattern "basics/**/*.dr"
python codex/test_cli.py run --pattern "basics/io/console.dr"
python codex/test_cli.py run --pattern "basics/memory/**/*.dr"
```

Patterns are relative to `tests/`. `quick` with no file selects the full suite;
it is not a smaller or faster acceptance gate. The optional shell/PowerShell CLI
wrappers delegate to the Python tooling; CI and agent acceptance use the commands
above. The analyzer/generator modules remain optional tools, not acceptance gates.

## Annotated Dream tests

```dream
// Expected: text printed by the program
// Expected exit: 8
// Expected error: diagnostic required from a rejected program
// Options: -Werror
```

`Expected exit` defaults to zero. `Expected error` checks a compiler rejection;
the program is not linked or executed. Use `// Expected: (no output)` for a
successful silent program. A source without expectations remains skipped, not
passing. Timeouts are errors and produce a failing suite exit status.

[Test configuration](test_config.json) defines categories, timeouts and supporting
module fixtures. The two module helpers are fixtures, not standalone programs.
Graphics/concurrency experiments under `not_implemented_tests/` are outside the
regular suite. Do not remove skipped files or weaken expectations to raise counts.
The runner regression tests cover rejection, exit status, timeouts and discovery.

## Platform evidence

On Windows, cross-build relevant C changes with:

```sh
zig build -Dtarget=x86_64-linux-gnu --prefix build/linux
```

A successful link does not demonstrate Linux execution. Native Linux and Windows
runs come from CI for the exact commit. Preserve the distinction in reports.
Documentation-only passes check links, paths and diffs and cite dated prior
compiler evidence without claiming to rerun it. Follow the
[delivery workflow](../docs/development/agent-workflow.md) for pass records.
