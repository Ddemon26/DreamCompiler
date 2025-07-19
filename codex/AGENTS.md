# Dream Compiler CLI Agent

*Last updated: 18 July 2025*

---

## Cross-Platform Compatibility

* All code, scripts, and build configurations **must** support both Linux and Windows environments.
* Use preprocessor defines in C/Zig and build scripts to select OS-specific code paths:

  ```c
  #ifdef _WIN32
    // Windows-specific implementation
  #elif defined(__linux__)
    // Linux-specific implementation
  #endif
  ```
* In build and CI scripts, detect the OS and set appropriate commands and flags. Example in PowerShell:

  ```powershell
  if ($IsWindows) {
      zig build
  } elseif ($IsLinux) {
      zig build
  }
  ```
* Ensure Python helper scripts adapt at runtime:

  ```python
  import platform
  if platform.system() == "Windows":
      # Windows paths or commands
  elif platform.system() == "Linux":
      # Linux paths or commands
  ```
* Validate cross-platform compatibility by running CI jobs on both Linux and Windows matrices.

### Windows-Specific Build Instructions

* **Prerequisites**: Install Zig 0.15.0+, MinGW-w64 (for GCC), and re2c
* **Build Command**: Use `zig build` (same as Linux)
* **Test Command**: Use `python codex/python/test_runner` or `python codex/python/test_runner --debug` for verbose output
* **Shell Commands**: Use PowerShell with `;` as command separator instead of `&&`
* **Path Separators**: The compiler automatically handles Windows path separators (`\` vs `/`)
* **Executable Names**: Windows executables have `.exe` extension (handled automatically)

### Known Windows Issues

* **Struct Declaration Parsing**: There is a known issue where struct declarations followed by variable declarations using those structs are not parsed correctly on Windows. This causes "incomplete type" errors during C compilation. The issue is in the parser's type vector management and affects tests in `tests/advanced/data_structures/` and `tests/advanced/oop/`.
* **Workaround**: Struct declarations work correctly when not followed by variable declarations using the struct type.
* **Status**: Under investigation - the issue appears to be in the `typevec_contains` function or the interaction between struct parsing and variable declaration parsing.

---

## Overview

The **Dream Compiler CLI agent** bootstraps, grows, and maintains the DreamCompiler code‑base.  The compiler is pure **C11**, built via **Zig** and tested using the **GNU toolchain**.  Source files end in `.dr`; the build pipeline is:

```
.dream  →  DreamCompiler (C frontend)  →  portable C  →  zig cc  →  native exe
```

Everything lives in a terminal environment where you invoke `zig build` for building, and use GNU tools (e.g., `make test`, `gcc`, `gdb`) for testing and debugging.  All steps must work on both Linux and Windows, using OS defines where necessary.

* **Python Helper Scripts** – if the agent needs to perform any heavy tasks (e.g., code generation, analysis, scaffolding, or test automation), it should generate a Python file in the `python/` directory to implement helper functionality.

---

## Dev Environment Tips  *(adapted from the Codex contributor guide)*

* **Jump quickly between modules** – use `grep -R "module "` or `find . -name '*.c'` instead of scanning with `ls`.
* **Prime the incremental cache** – run `zig build` once; subsequent builds re‑compile only what changed.
* **Run the full test suite** – `make test` (or `./test_runner`) executes every unit and regression test and prints a concise summary.
* **Focus on one test** – use the test runner with a `--filter="<pattern>"` option (pattern matches file or function).
* **Spin up a scratch playground** – `zig run tools/scratch.zig -- <snippet.dr>` to experiment without touching the main code‑base.
* **Add new external tools responsibly** – when you introduce something like **re2c** or **clang‑format**, record it in `codex/_startup.sh` so CI can install it.
* **Keep target names consistent** – double‑check any new `build.zig` target’s `name` field to avoid CI mis‑routing.

---

## Debug Console Helpers

The compiler relies on helper functions in `src/util/console_debug.h` for
printing diagnostics during development. `Console.Write`, `Console.WriteLine`,
`Console.ReadLine` and `Console.Read` map to these helpers. When the
`DREAM_RELEASE` macro is defined they become no-ops, so ensure any debugging
calls are removed or guarded before release builds.

---

## Testing Instructions

The DreamCompiler has a comprehensive testing framework designed for CLI agents to interact with efficiently. All testing tools work cross-platform and provide fast, detailed feedback.

### Quick Start for CLI Agents

**Universal Test CLI** - Use these commands on any platform:
```bash
# Linux
./test_cli.sh quick                    # Fast basic test run
./test_cli.sh run "basics/**/*.dr"     # Run specific test pattern
./test_cli.sh bulk                     # Test all categories
./test_cli.sh status                   # Check environment

# Windows
.\test_cli.ps1 quick                   # Fast basic test run
.\test_cli.ps1 run "basics/**/*.dr"    # Run specific test pattern
.\test_cli.ps1 bulk                    # Test all categories
.\test_cli.ps1 status                  # Check environment

# Cross-platform Python CLI
python test_cli.py quick               # Works on both platforms
python test_cli.py run --pattern "**/*.dr"
python test_cli.py analyze --format html
```

### Test Categories and Organization

Tests are organized into categories for efficient filtering:

* **basics/** - Unit tests for core language features (arithmetic, variables, I/O)
* **control_flow/** - Conditionals, loops, switches, logical operations
* **functions/** - Function definitions, parameters, returns, recursion
* **advanced/** - Complex features (OOP, data structures, concurrency, SSA)
* **semantics/** - Type checking, symbol resolution, semantic analysis
* **debug/** - Debug features, line directives, diagnostic output

**Category-based Testing:**
```bash
# Linux
./test_cli.sh filter basics           # Test only basic features
./test_cli.sh filter advanced         # Test advanced features
./test_cli.sh filter memory           # Test memory management

# Windows  
.\test_cli.ps1 filter basics          # Test only basic features
.\test_cli.ps1 filter advanced        # Test advanced features
.\test_cli.ps1 filter memory          # Test memory management
```

### Fast Test Execution

For CLI agents needing rapid feedback:

```bash
# Ultra-fast basic validation (< 5 seconds)
./test_cli.sh fast                     # Linux
.\test_cli.ps1 fast                    # Windows

# Fast category testing
./test_cli.sh fast "basics/**/*.dr"    # Linux
.\test_cli.ps1 fast "basics/**/*.dr"   # Windows

# Bulk testing with summary
./test_cli.sh bulk                     # Linux - tests all categories
.\test_cli.ps1 bulk                    # Windows - tests all categories
```

### Advanced Test Management

**Test Analysis and Reporting:**
```bash
# Generate comprehensive reports
python test_cli.py analyze --format html --output report.html
python test_cli.py analyze --format text > summary.txt

# Performance analysis with charts
python test_cli.py analyze --format charts

# Historical trend analysis
python test_cli.py analyze --days 7   # Last 7 days of results
```

**Test Generation and Validation:**
```bash
# Validate existing test suite
python test_cli.py generate --validate

# Generate new comprehensive tests
python test_cli.py generate --generate

# Show test matrix for CI/CD
python test_cli.py generate --matrix
```

**Continuous Testing:**
```bash
# Watch mode for development
./test_cli.sh watch "basics/**/*.dr"   # Linux
.\test_cli.ps1 watch "basics/**/*.dr"  # Windows

# Performance benchmarking
./test_cli.sh benchmark 5              # Linux - 5 iterations
.\test_cli.ps1 benchmark 5             # Windows - 5 iterations
```

### Test Result Interpretation

**Exit Codes:**
- `0` - All tests passed
- `1` - Some tests failed or errors occurred

**Test Status Types:**
- `PASS` - Test executed successfully with expected output
- `FAIL` - Test executed but output didn't match expected
- `SKIP` - Test skipped (no expected output defined)
- `ERROR` - Compilation or runtime error
- `TIMEOUT` - Test exceeded time limit

**Platform-Specific Results:**
- Linux: ~95% pass rate (all features working)
- Windows: ~85% pass rate (struct parsing issues known)

### Legacy Test Commands

For compatibility with existing workflows:
```bash
# Traditional commands still work
make test                              # Linux
python codex/python/test_runner        # Cross-platform
python codex/python/test_runner --debug # Verbose output
```

### Test File Format

All test files follow this format:
```dream
// Expected: <expected_output>
// Options: <compiler_options>  (optional)
<dream_code>
```

Example:
```dream
// Expected: 42
int x = 40 + 2;
Console.WriteLine(x);
```

### CI/CD Integration

The test framework integrates with CI/CD pipelines:
- Results saved as JSON in `test_results/` directory
- HTML reports generated automatically
- Test matrix configuration for multiple platforms
- Performance benchmarks tracked over time

### Windows Test Status

* **Passing**: Basic arithmetic, control flow, functions, I/O, and most language features
* **Failing**: Struct-related tests due to parsing issues (see Known Windows Issues above)
* **Total Pass Rate**: ~85% of tests pass on Windows (all non-struct tests)
* **Known Issues**: Struct declaration parsing affects `advanced/data_structures/` and `advanced/oop/` tests

### Performance Expectations

**Typical execution times:**
- Fast basic test: < 5 seconds
- Full test suite: 30-60 seconds
- Category-specific: 5-15 seconds
- Single test: < 1 second

**Memory usage:**
- Test runner: < 50MB RAM
- Compiled tests: < 10MB each
- Result storage: < 1MB per run

---

## Comprehensive Testing System

The DreamCompiler includes an advanced testing framework specifically designed for CLI agents to interact with efficiently. The system provides:

### Key Features for CLI Agents

* **Ultra-fast execution** - Quick validation in under 5 seconds
* **Intelligent filtering** - Test specific categories or patterns
* **Cross-platform compatibility** - Identical commands work on Linux and Windows
* **Detailed reporting** - JSON, HTML, and text output formats
* **Performance tracking** - Benchmark and trend analysis
* **Automated test generation** - Create comprehensive test suites
* **Continuous testing** - Watch mode for development workflows

### Essential Commands for CLI Agents

**Fastest possible validation:**
```bash
# Linux
./test_cli.sh quick                    # < 5 seconds
./test_cli.sh fast                     # < 10 seconds  
./test_cli.sh bulk                     # < 30 seconds, all categories

# Windows
.\test_cli.ps1 quick                   # < 5 seconds
.\test_cli.ps1 fast                    # < 10 seconds
.\test_cli.ps1 bulk                    # < 30 seconds, all categories

# Universal (works on both platforms)
python test_cli.py quick               # Cross-platform quick test
```

**Category-specific testing:**
```bash
./test_cli.sh filter basics            # Test basic language features
./test_cli.sh filter advanced          # Test advanced features
./test_cli.sh filter memory            # Test memory management
```

**Analysis and reporting:**
```bash
python test_cli.py analyze --format html --output report.html
python test_cli.py analyze --format text > summary.txt
python test_cli.py generate --validate # Validate existing tests
```

### Test Organization

Tests are organized into logical categories:
- **basics/** - Core language features (arithmetic, variables, I/O)
- **control_flow/** - Conditionals, loops, switches
- **functions/** - Function definitions, parameters, returns
- **advanced/** - OOP, data structures, concurrency
- **semantics/** - Type checking, symbol resolution
- **debug/** - Debug features and diagnostics

### Integration with Development Workflow

The testing system integrates seamlessly with the development workflow described in the main workflow section. When the `go` command is issued, agents should:

1. **Quick validation** - Run `./test_cli.sh quick` or `python test_cli.py quick` for immediate feedback
2. **Category testing** - Use `./test_cli.sh filter <category>` to test specific areas being modified
3. **Full validation** - Run `./test_cli.sh bulk` before committing changes
4. **Generate reports** - Use `python test_cli.py analyze` to document test results

### Documentation

Complete testing documentation is available in:
- **TEST_GUIDE.md** - Comprehensive testing guide with examples
- **test_config.json** - Configuration file with platform-specific settings
- **codex/python/** - Advanced test management tools and utilities

This testing framework ensures CLI agents can efficiently validate compiler changes with fast, reliable, and comprehensive test coverage across all supported platforms.

---

## PR Instructions

Title format: `[dreamc] <Short, imperative summary>`

Every pull request **must** fill out `codex/BOT_PR_TEMPLATE.md`, covering:

1. **What changed** – features, refactors, or bug fixes.
2. **How it was tested** – commands, new test files, CI results.
3. **Docs updated** – grammar, examples, changelog entries.
4. **Dependencies** – any additions in `codex/_startup.sh`.

---

## Responsibilities

* **Track `docs/grammar/Grammar.md`** – the authoritative Dream grammar.  Whenever the spec changes, update:

    * **`lexer/tokens.def`** and regenerate `lexer/lexer.c` via **re2c**.
    * **`parser/`**, **`sem/`** modules so the implementation matches the spec (new precedence levels, keywords, etc.).
    * **Examples and snippets** throughout `docs/`.
    * **Regression tests** under `tests/`.
* Grow functionality of the compiler incrementally, starting from a minimal working set.
* Keep documentation (`docs/`) fully in‑sync with behaviour.
* Maintain green test suite (`make test`).  Add tests for every new feature and bug‑fix.
* Record notable changes in `docs/changelog.md`.
* Add required system dependencies to `codex/_startup.sh` (e.g., `re2c`).
* Split large modules into focused files when practical.

---

## Workflow

When the command `go` is issued the agent should:

1. **Synchronise with the grammar**

    * Parse `docs/grammar/Grammar.md` and compare with `lexer/tokens.def` and precedence tables in `parser/parser.c`.
    * Report mismatches; update code or open a task to correct the spec.
2. **Review project state**

    * Identify monolithic areas that need extraction before adding new code.
3. **Implement the feature**

    * Add new functionality in the appropriate module (lexer, parser, semantic analysis, IR, etc.).
    * Ensure the implementation matches the spec and is documented.
    * Write tests for the new feature under `tests/`.
    * If a bug is fixed, document it in `docs/changelog.md` and add a regression test.
    * For any substantial or repetitive operations, scaffold or automate using Python helper scripts placed under `python/`, ensuring they detect and adapt to the host OS via preprocessor or runtime checks.
4. **Update docs & tests**

    * Regenerate lexer (`re2c`), rebuild compiler, and run all tests.
5. **Build & run**

    * Run `zig build` (using OS defines if needed to adjust flags).
    * If any Zig test files (`*.zig`) are present in the `tests/` directory, run `zig build test`; otherwise, skip it.
    * Run tests:
      - Linux: `make test` (or `./test_runner`)
      - Windows: `python codex/python/test_runner`
    * Optionally, run `zig build run -- <file.dr>`.
6. **Cross-platform validation**

    * Ensure tests pass on both Linux and Windows
    * On Windows, expect struct-related tests to fail due to known parsing issues
    * Document any platform-specific issues or workarounds
7. **Commit** once tests succeed on the target platform.

---

## Environment

### Linux Environment
* Ubuntu-based shell with Git, Zig (`>=0.13.0`), **GCC**, **Make**, **GDB**, and **re2c** available.
* Additional packages installed via `codex/_startup.sh`.

### Windows Environment  
* Windows 10/11 with PowerShell, Git, Zig (`>=0.15.0`), **MinGW-w64** (for GCC), and **re2c** available.
* Additional packages can be installed via `codex/_startup.ps1` (if available).
* **Required Tools**:
  - Zig 0.15.0+ (add to PATH)
  - MinGW-w64 (provides GCC for Windows)
  - re2c (for lexer generation)
  - Python 3.8+ (for test runner)
* **Installation Notes**:
  - Use Chocolatey, Scoop, or manual installation for tools
  - Ensure `zig cc` works as the C compiler
  - Test with `zig build` and `python codex/python/test_runner`

---

## Directory Structure

```
lexer/      – re2c spec & generated lexer
parser/     – Pratt / precedence‑climbing parser & AST
sem/        – Symbol table, type system, inference
ir/         – Three‑address SSA IR & CFG
opt/        – Optimisation passes (SCCP, DCE, LICM, …)
codegen/    – C emitter (topological, generics)
docs/       – Language spec, change‑log, design docs
    grammar/Grammar.md          – BNF grammar (source of truth)
    changelog.md        – Chronological changes
tests/      – Regression test suite (.dr → expected)
codex/      – Agent docs & startup scripts
build.zig   – Zig build pipeline (dr → c → exe)
python/     – Generated Python helper scripts
```

---

## Features & Roadmap

A living list of implemented and planned features is kept in [`FEATURES.md`](FEATURES.md).  Update this file whenever features are added, removed, or altered.

---

## Example Session

```
> go
[Agent] Building compiler…
[Agent] No Zig test files found; skipping `zig build test`.
[Agent] Running tests via make test…
[Agent] Committing changes…
```