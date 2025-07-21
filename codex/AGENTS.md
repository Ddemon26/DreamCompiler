# Dream Compiler CLI Agent

*Last updated: 19 July 2025*

---

## Repository Source Layout & Navigation Map

**CRITICAL**: This map must be updated whenever files are moved, added, or removed. CLI agents should use this map to navigate the repository with speed and precision.

### Core Compiler Source (`src/`)
```
src/
├── lexer/           # Tokenization & lexical analysis
│   ├── lexer.re     # re2c specification (source of truth)
│   ├── lexer.c      # Generated lexer (DO NOT EDIT DIRECTLY)
│   ├── lexer.h      # Lexer interface
│   └── tokens.def   # Token definitions
├── parser/          # Syntax analysis & AST construction
│   ├── parser.c     # Parser entry point (includes submodules)
│   ├── diag.inc     # Diagnostic helpers
│   ├── expr.inc     # Expression parsing logic
│   ├── stmt.inc     # Statement parsing logic
│   ├── parser.h     # Parser interface
│   ├── ast.c        # AST node construction/manipulation
│   ├── ast.h        # AST data structures
│   ├── diagnostic.c # Parser diagnostics
│   ├── diagnostic.h # Diagnostic interface
│   └── error.c      # Error handling
├── sem/             # Semantic analysis & type system
│   ├── analysis.c   # Semantic analysis passes
│   ├── analysis.h   # Analysis interface
│   ├── infer.c      # Type inference engine
│   ├── infer.h      # Type inference interface
│   ├── scope.c      # Scope management
│   ├── scope.h      # Scope data structures
│   ├── symbol.c     # Symbol table operations
│   ├── symbol.h     # Symbol table interface
│   ├── type.c       # Type system implementation
│   └── type.h       # Type definitions
├── ir/              # Intermediate representation
│   ├── ir.c         # IR construction & manipulation
│   ├── ir.h         # IR data structures
│   ├── lower.c      # AST to IR lowering
│   └── lower.h      # Lowering interface
├── ssa/             # Static Single Assignment form
│   ├── ssa.c        # SSA construction & utilities
│   └── ssa.h        # SSA interface
├── cfg/             # Control Flow Graph
│   ├── cfg.c        # CFG construction & analysis
│   └── cfg.h        # CFG data structures
├── opt/             # Optimization passes
│   ├── pipeline.c   # Optimization pipeline orchestration
│   ├── pipeline.h   # Pipeline interface
│   ├── sccp.c       # Sparse Conditional Constant Propagation
│   ├── sccp.h       # SCCP interface
│   ├── dce.c        # Dead Code Elimination
│   ├── dce.h        # DCE interface
│   ├── copy_prop.c  # Copy Propagation
│   ├── copy_prop.h  # Copy Propagation interface
│   ├── cse.c        # Common Subexpression Elimination
│   ├── cse.h        # CSE interface
│   ├── licm.c       # Loop Invariant Code Motion
│   ├── licm.h       # LICM interface
│   ├── peephole.c   # Peephole optimizations
│   ├── peephole.h   # Peephole interface
│   ├── value_numbering.c # Global Value Numbering
│   └── value_numbering.h # GVN interface
├── codegen/         # Code generation (C backend)
│   ├── codegen.c    # Main code generation orchestration
│   ├── codegen.h    # Codegen interface
│   ├── c_emit.c     # C code emission
│   ├── c_emit.h     # C emission interface
│   ├── context.c    # Generation context management
│   ├── context.h    # Context interface
│   ├── expr.c       # Expression code generation
│   ├── expr.h       # Expression codegen interface
│   ├── stmt.c       # Statement code generation
│   └── stmt.h       # Statement codegen interface
├── driver/          # Compiler driver & main entry points
│   ├── main.c       # Main compiler driver
│   ├── lex_main.c   # Lexer-only mode
│   └── parse_main.c # Parser-only mode
└── util/            # Utility functions & platform abstraction
    ├── console_debug.h # Debug console helpers
    └── platform.h      # Platform-specific definitions
```

### Test Suite Organization (`tests/`)
```
tests/
├── basics/          # Core language features
│   ├── arithmetic/  # Math operations, operators
│   ├── bitwise/     # Bitwise operations
│   ├── comments/    # Comment parsing
│   ├── expressions/ # Expression evaluation
│   ├── io/          # Input/output operations
│   ├── memory/      # Memory management
│   ├── misc/        # Miscellaneous basic tests
│   ├── strings/     # String operations
│   ├── types/       # Basic type system
│   └── variables/   # Variable declarations
├── control_flow/    # Control structures
│   ├── comparisons/ # Comparison operators
│   ├── conditionals/# If/else statements
│   ├── exceptions/  # Exception handling
│   ├── increment/   # Increment/decrement
│   ├── logical/     # Logical operators
│   ├── loops/       # For/while loops
│   └── switches/    # Switch statements
├── functions/       # Function-related tests
│   ├── definitions/ # Function definitions
│   ├── exceptions/  # Function exception handling
│   ├── parameters/  # Parameter passing
│   └── returns/     # Return values
├── advanced/        # Complex language features
│   ├── concurrency/ # Concurrent programming
│   ├── data_structures/ # Complex data types
│   ├── oop/         # Object-oriented features
│   └── ssa/         # SSA-specific tests
├── semantics/       # Semantic analysis tests
│   └── basic.dr     # Basic semantic tests
├── debug/           # Debug feature tests
│   └── line_directives.dr # Line directive handling
├── graphics/        # Graphics and Vulkan tests
└── hottests/        # Active development tests
    ├── minimal_struct_test.dr
    ├── struct_only.dr
    └── test_struct_with_semicolon.dr
```

### Documentation (`docs/`)
```
docs/
├── grammar/         # Language specification
│   └── Grammar.md   # BNF grammar (AUTHORITATIVE SOURCE)
├── language/        # Language documentation
├── compiler/        # Compiler internals documentation
├── v1.0/           # Version 1.0 documentation
├── v1.1/           # Version 1.1 documentation
├── index.md        # Main documentation index
├── intro.md        # Introduction to Dream language
├── usage.md        # Usage instructions
├── index.html      # HTML documentation
├── index.css       # Documentation styling
└── index-dark.css  # Dark theme styling
```

### Build System & Configuration
```
build.zig           # Zig build configuration (MAIN BUILD FILE)
.clang-tidy         # Clang-tidy configuration
clang-format.txt    # Code formatting rules
LICENSE             # Project license
README.md           # Project overview
CODEOWNERS          # Code ownership rules
.gitignore          # Git ignore patterns
.gitattributes      # Git attributes
```

### CLI Agent Resources (`codex/`)
```
codex/
├── AGENTS.md       # This file - agent instructions
├── BOT_PR_TEMPLATE.md # PR template for agents
├── FEATURES.md     # Feature tracking
├── TEST_GUIDE.md   # Comprehensive testing guide
├── test_cli.sh     # Linux test CLI
├── test_cli.ps1    # Windows test CLI
├── test_cli.py     # Cross-platform test CLI
├── test_config.json # Test configuration
├── _startup.sh     # Linux environment setup
├── _startup.ps1    # Windows environment setup
├── go/             # Go testing suite
│   ├── dream_test.go      # Unit tests via cgo
│   ├── dream_bench_test.go # Performance benchmarks
│   ├── dream_fuzz_test.go  # Fuzz testing
│   ├── dream_api.c        # C API wrapper
│   ├── dream.h            # C API header
│   ├── Makefile           # Go test build system
│   └── README.md          # Go testing documentation
└── python/         # Python helper scripts (generated)
```

### Sub-Project: JetBrains Plugin (`assets/jetbrains/`)
```
assets/jetbrains/   # JetBrains IDE plugin (SEPARATE PROJECT)
├── AGENTS.md       # Plugin-specific agent instructions
├── build.gradle.kts # Gradle build configuration
├── settings.gradle.kts # Gradle settings
├── gradlew         # Gradle wrapper (Linux)
├── gradlew.bat     # Gradle wrapper (Windows)
├── tokens.json     # Token definitions for plugin
├── package-lock.json # NPM dependencies
├── src/            # Plugin source code
│   ├── main/       # Main plugin code
│   └── test/       # Plugin tests
├── idea/           # IntelliJ IDEA specific files
├── vscode/         # VS Code extension files
├── scripts/        # Build scripts
│   └── genFromTokens.js # Token generation script
└── .gradle/        # Gradle cache (generated)
```

### Runtime & Standard Library
```
runtime/            # Runtime system components
stdlib/             # Standard library implementation
```

### Generated & Cache Directories
```
.zig-cache/         # Zig build cache
zig-out/            # Zig build output
build/              # Build artifacts
.venv/              # Python virtual environment
.git/               # Git repository data
.idea/              # IntelliJ IDEA project files
.vscode/            # VS Code tasks and launch configs
```

### Navigation Commands for CLI Agents

**Quick file access patterns:**
```bash
# Core compiler components
src/lexer/lexer.re          # Lexer specification
src/parser/parser.c         # Parser entry point
src/parser/expr.inc         # Expression parsing
src/parser/stmt.inc         # Statement parsing
src/sem/type.c             # Type system
src/codegen/c_emit.c       # C code generation
docs/grammar/Grammar.md    # Language grammar

# Test categories
tests/basics/              # Basic language tests
tests/advanced/            # Advanced feature tests
tests/control_flow/        # Control structure tests

# Build and configuration
build.zig                  # Main build file
codex/test_cli.py         # Cross-platform testing
```

**Project context switching:**
- **Main compiler project**: Work in root directory, use `zig build` and test commands
- **JetBrains plugin project**: `cd assets/jetbrains/`, use Gradle commands (`./gradlew build`, `./gradlew test`)

**CRITICAL RULES:**
1. **Always update this map** when moving, adding, or removing files
2. **Use exact paths** from this map for navigation
3. **Check project context** before running commands (main vs sub-project)
4. **No `zig test` required** - the compiler is C-based, not Zig-based
5. **Switch test commands** based on project context

### Navigation Map Maintenance

**MANDATORY**: Update this navigation map immediately when:
- Adding new source files or directories
- Moving files between directories
- Removing files or directories
- Restructuring the project layout
- Adding new test categories
- Creating new modules or components

**Map Update Process**:
1. Make file system changes
2. Update the corresponding section in this navigation map
3. Verify all paths are accurate
4. Update any affected quick access patterns
5. Test navigation commands to ensure they work

**Quick Verification Commands**:
```bash
# Verify core paths exist
ls src/lexer/lexer.re
ls src/parser/parser.c
ls docs/grammar/Grammar.md
ls codex/test_cli.py

# Verify test structure
ls tests/basics/
ls tests/advanced/
ls tests/control_flow/

# Verify sub-project
ls assets/jetbrains/build.gradle.kts
```

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

## Go Testing Suite

The DreamCompiler includes a comprehensive Go testing suite that provides direct C API testing, performance benchmarking, and fuzz testing to complement the Python-based end-to-end tests.

### Key Features

* **Direct C API Testing** - Unit tests via cgo for lexer, parser, and type system
* **Performance Benchmarks** - Detailed performance analysis of hot paths
* **Fuzz Testing** - Robustness testing with random inputs to detect crashes
* **Memory Management Testing** - Arena allocation and resource cleanup validation
* **Cross-Platform Support** - Works on Linux, Windows, and macOS
* **Agent-Friendly Protocols** - Fast validation workflows for CLI agents

### Critical Testing Protocol for CLI Agents

**MANDATORY: All compiler changes MUST be validated through Go test protocols before proceeding**

### Prerequisites Check

**Before running any Go tests:**
```bash
# Verify Go installation
go version  # Should show Go 1.21+

# If Go not installed:
# Windows: choco install golang (run as Administrator)
# Linux: Download from https://golang.org/dl/
# Ensure 'go' command is in PATH
```

### Agent Testing Workflows

**1. RAPID DEVELOPMENT FEEDBACK (< 30 seconds):**
```bash
cd codex/go

# Linux/Unix
make quick-validate

# Windows
.\build_and_test.ps1 quick

# What it validates: Core lexer/parser/type functionality
# Use for: Frequent validation during development
```

**2. NEW FEATURE VALIDATION (< 2 minutes):**
```bash
cd codex/go

# Linux/Unix
make validate-feature

# Windows  
.\build_and_test.ps1 feature

# What it validates: Unit tests + benchmarks + fuzz tests + coverage
# Use for: Validating new language features or major changes
```

**3. COMPREHENSIVE VALIDATION (< 5 minutes):**
```bash
cd codex/go

# Linux/Unix
make comprehensive-validate

# Windows
.\build_and_test.ps1 comprehensive

# What it validates: Full test suite with extended fuzz testing
# Use for: Pre-commit validation and release preparation
```

**4. PERFORMANCE ANALYSIS:**
```bash
cd codex/go

# Linux/Unix
make bench

# Windows
.\build_and_test.ps1 bench

# What it provides: Performance metrics and regression detection
# Use for: Optimization work and performance monitoring
```

### Standard Testing Commands

**Build and test:**
```bash
# Build C library and run all tests
cd codex/go && make test

# Run performance benchmarks
cd codex/go && make bench

# Run fuzz tests (short duration)
cd codex/go && make fuzz

# Complete test suite with coverage
cd codex/go && make test-all
```

**Individual test categories:**
```bash
# Unit tests only
go test -v ./...

# Specific test patterns
go test -run TestLexer ./...
go test -run TestParser ./...

# Benchmarks with memory profiling
go test -bench=. -benchmem ./...

# Fuzz testing for specific components
go test -fuzz=FuzzLexer -fuzztime=30s ./...
```

### Test Categories

**Unit Tests (`dream_test.go`):**
- Lexer tokenization and position tracking
- Parser AST construction and error handling
- Type system operations and equality
- Memory arena allocation and cleanup
- End-to-end integration testing

**Benchmarks (`dream_bench_test.go`):**
- Lexer performance with various input types
- Parser performance scaling with complexity
- Code generation benchmarks
- Memory allocation profiling
- Comparative performance analysis

**Fuzz Tests (`dream_fuzz_test.go`):**
- Lexer robustness with random byte sequences
- Parser stability with malformed syntax
- Code generation with edge cases
- Type system stress testing
- Memory operation fuzzing

### Integration with Development Workflow

**CRITICAL: Go tests must be integrated into ALL development workflows**

### Complete Validation Workflow for Agents

**When implementing new compiler features:**
```bash
# Step 1: Build verification
zig build                              # Ensure compiler builds

# Step 2: Quick Go validation (< 30s)
cd codex/go && make quick-validate     # Linux
cd codex/go && .\build_and_test.ps1 quick  # Windows

# Step 3: Dream file validation (< 30s)  
cd .. && python test_cli.py quick     # Test .dr file compilation

# Step 4: If basic tests pass, run feature validation
cd codex/go && make validate-feature  # Linux
cd codex/go && .\build_and_test.ps1 feature  # Windows

# Step 5: Full Dream file validation
cd .. && python test_cli.py bulk      # Complete .dr file test suite
```

**Development Workflow Integration:**
1. **During development** - Use `quick-validate` for rapid feedback
2. **Feature completion** - Use `validate-feature` for comprehensive validation  
3. **Pre-commit** - Use `comprehensive-validate` + full Python test suite
4. **Performance monitoring** - Use bench protocol for optimization work
5. **CI/CD integration** - Automated testing with full protocols

**Expected Timeline:**
- Quick validation: < 30 seconds
- Feature validation: < 2 minutes  
- Comprehensive validation: < 5 minutes
- Full workflow: < 10 minutes total

**Protocol Failure Response:**
- If Go tests fail: Fix C API issues before proceeding
- If Dream file tests fail: Check code generation and runtime
- If both fail: Likely parser/semantic analysis issues

**Coverage Requirements:**
- Go unit tests: 100% pass for core features
- Go benchmarks: No significant performance regressions
- Dream file tests: Platform-specific pass rates (95% Linux, 90% Windows)

### Performance Expectations

**Typical execution times:**
- Unit tests: < 10 seconds
- Benchmarks: 30-60 seconds
- Fuzz tests: 30 seconds (configurable)
- Full suite: < 2 minutes

**Benchmark results provide:**
- Operations per second for each component
- Memory allocation patterns
- Performance scaling characteristics
- Regression detection over time

### Files and Structure

```
codex/go/
├── dream_test.go         # Unit and integration tests
├── dream_bench_test.go   # Performance benchmarks  
├── dream_fuzz_test.go    # Fuzz tests for robustness
├── dream.h              # C API header for cgo
├── dream_api.c          # C API wrapper implementation
├── Makefile             # Build system for C library
└── README.md            # Comprehensive documentation
```

The Go testing suite provides CLI agents with direct access to compiler internals for detailed validation, performance analysis, and robustness testing that complements the high-level Python test framework.

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

## Project Context Detection & Switching

**CRITICAL**: Always detect which project context you're working in before executing commands.

### Main Compiler Project (Root Directory)
**Detection**: Working in root directory or any subdirectory except `assets/jetbrains/`
**Build System**: Zig + C compilation
**Test Commands**:
```bash
# Linux
./codex/test_cli.sh quick
make test

# Windows  
.\codex\test_cli.ps1 quick
python codex/python/test_runner

# Cross-platform
python codex/test_cli.py quick
```
**Build Commands**:
```bash
zig build                    # Build compiler
zig build run -- file.dr    # Run compiler on file
```
**Key Files**: `build.zig`, `src/`, `tests/`, `docs/grammar/Grammar.md`

### JetBrains Plugin Sub-Project (`assets/jetbrains/`)
**Detection**: Working in `assets/jetbrains/` directory
**Build System**: Gradle + Kotlin/Java
**Test Commands**:
```bash
# Linux/macOS
./gradlew test
./gradlew check

# Windows
.\gradlew.bat test
.\gradlew.bat check
```
**Build Commands**:
```bash
# Linux/macOS
./gradlew build
./gradlew buildPlugin

# Windows
.\gradlew.bat build
.\gradlew.bat buildPlugin
```
**Key Files**: `build.gradle.kts`, `src/main/`, `src/test/`, `tokens.json`

### Context Switching Rules
1. **Before any command**: Check current directory and project context
2. **Main project changes**: Do NOT run JetBrains plugin tests
3. **Plugin changes**: Do NOT run compiler tests
4. **Cross-project changes**: Test both projects separately
5. **File moves**: Update this navigation map immediately

## Workflow

When the command `go` is issued the agent should:

1. **Detect project context**
    * Determine if working in main compiler project or JetBrains plugin sub-project
    * Use appropriate build system and test commands for detected context

2. **Synchronise with the grammar** (Main project only)
    * Parse `docs/grammar/Grammar.md` and compare with `src/lexer/tokens.def` and precedence tables in `src/parser/parser.c`
    * Report mismatches; update code or open a task to correct the spec

3. **Review project state**
    * Identify monolithic areas that need extraction before adding new code
    * Check if changes affect both main project and plugin (token definitions, grammar changes)

4. **Implement the feature**
    * **Main project**: Add functionality in appropriate module (`src/lexer/`, `src/parser/`, `src/sem/`, etc.)
    * **Plugin project**: Add functionality in `assets/jetbrains/src/main/`
    * Ensure implementation matches spec and is documented
    * Write tests in appropriate test directory
    * For substantial operations, create Python helpers in `codex/python/`

5. **Update docs & tests**
    * **Main project**: Regenerate lexer with `re2c`, rebuild with `zig build`
    * **Plugin project**: Update `tokens.json` if needed, rebuild with Gradle
    * Update navigation map in this file if files were moved/added/removed

6. **Build & test** (Context-specific)
    
    **Main Compiler Project**:
    ```bash
    # Build
    zig build
    
    # MANDATORY: Go test validation (C API testing)
    cd codex/go
    
    # Quick validation (< 30 seconds)
    # Linux
    make quick-validate
    # Windows
    .\build_and_test.ps1 quick
    
    # Feature validation for new features (< 2 minutes)  
    # Linux
    make validate-feature
    # Windows
    .\build_and_test.ps1 feature
    
    # Dream file testing (NO zig test - compiler is C-based)
    cd ..
    
    # Linux
    ./codex/test_cli.sh quick
    
    # Windows
    .\codex\test_cli.ps1 quick
    
    # Cross-platform
    python codex/test_cli.py quick
    ```
    
    **JetBrains Plugin Project**:
    ```bash
    # Change to plugin directory
    cd assets/jetbrains/
    
    # Build & test
    # Linux/macOS
    ./gradlew build test
    
    # Windows
    .\gradlew.bat build test
    ```

7. **Cross-platform validation**
    * **Main project**: Test on both Linux and Windows (expect struct test failures on Windows)
    * **Plugin project**: Test with Gradle on target platforms
    * Document platform-specific issues

8. **Commit** once tests succeed in the appropriate project context

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