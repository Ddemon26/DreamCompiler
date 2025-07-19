# DreamCompiler Testing Guide

## Overview

The DreamCompiler testing framework provides comprehensive, cross-platform testing capabilities designed specifically for CLI agents and automated workflows. The system supports fast execution, detailed analysis, and intelligent filtering across Linux and Windows environments.

## Quick Start

### For CLI Agents - Ultra Fast Commands

```bash
# Linux - Fastest possible test validation
./test_cli.sh quick                    # < 5 seconds, basic validation
./test_cli.sh fast                     # < 10 seconds, core features  
./test_cli.sh bulk                     # < 30 seconds, all categories

# Windows - Fastest possible test validation  
.\test_cli.ps1 quick                   # < 5 seconds, basic validation
.\test_cli.ps1 fast                    # < 10 seconds, core features
.\test_cli.ps1 bulk                    # < 30 seconds, all categories

# Cross-platform Python (works everywhere)
python test_cli.py quick               # Universal quick test
python test_cli.py run --pattern "basics/**/*.dr"  # Filtered execution
```

### Installation & Setup

**Linux Requirements:**
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install zig python3 make gcc

# Make scripts executable
chmod +x test_cli.sh
chmod +x codex/python/test_runner

# Verify setup
./test_cli.sh status
```

**Windows Requirements:**
```powershell
# Install dependencies (using Chocolatey)
choco install zig python mingw

# Or install manually:
# - Zig 0.15.0+ from https://ziglang.org/download/
# - Python 3.8+ from https://python.org
# - MinGW-w64 for GCC

# Verify setup
.\test_cli.ps1 status
```

## Test Categories

### Core Categories
- **basics/** - Arithmetic, variables, I/O, strings, types
- **control_flow/** - If/else, loops, switches, comparisons
- **functions/** - Definitions, parameters, returns, recursion
- **advanced/** - OOP, data structures, concurrency, SSA
- **semantics/** - Type checking, symbol resolution
- **debug/** - Debug features, diagnostics

### Special Categories
- **cross_platform/** - Platform compatibility tests
- **performance/** - Benchmark and performance tests
- **regression/** - Tests for known issues and fixes
- **error_cases/** - Expected compilation/runtime failures

## Command Reference

### Universal Commands (All Platforms)

```bash
# Quick validation (< 5 seconds)
python test_cli.py quick [test_file]

# Full test suite
python test_cli.py run [--pattern "**/*.dr"] [--categories unit integration]

# Analysis and reporting
python test_cli.py analyze [--format text|html|charts] [--output file]

# Test management
python test_cli.py list [--pattern "**/*.dr"]
python test_cli.py generate --validate
python test_cli.py generate --generate
```

### Linux-Specific Commands

```bash
# Fast execution with shell optimizations
./test_cli.sh quick [test_file]         # Quick test
./test_cli.sh run [pattern] [debug]     # Full run with optional debug
./test_cli.sh fast [pattern]            # Fast execution, minimal output
./test_cli.sh bulk                      # All categories with summary
./test_cli.sh filter <category>         # Category-specific testing

# Advanced features
./test_cli.sh watch [pattern]           # Continuous testing mode
./test_cli.sh benchmark [iterations]    # Performance benchmarking
./test_cli.sh analyze [format] [output] # Result analysis
```

### Windows-Specific Commands

```powershell
# Fast execution with PowerShell optimizations
.\test_cli.ps1 quick [test_file]        # Quick test
.\test_cli.ps1 run [pattern] [-debug]   # Full run with optional debug
.\test_cli.ps1 fast [pattern]           # Fast execution, minimal output
.\test_cli.ps1 bulk                     # All categories with summary
.\test_cli.ps1 filter <category>        # Category-specific testing

# Advanced features
.\test_cli.ps1 watch [pattern]          # Continuous testing mode
.\test_cli.ps1 benchmark [iterations]   # Performance benchmarking
.\test_cli.ps1 analyze [format] [output] # Result analysis
```

## Test Patterns and Filters

### Predefined Patterns
```bash
# Quick patterns for common use cases
"basics/**/*.dr"                       # All basic language features
"control_flow/**/*.dr"                 # Control structures
"functions/**/*.dr"                    # Function-related tests
"advanced/**/*.dr"                     # Advanced features
"**/memory/*.dr"                       # Memory management tests
"**/data_structures/*.dr"              # Struct and array tests
```

### Category Filters
```bash
# Use with filter command
./test_cli.sh filter basics            # Basic language features
./test_cli.sh filter advanced          # Advanced features  
./test_cli.sh filter control           # Control flow
./test_cli.sh filter functions         # Function tests
./test_cli.sh filter memory            # Memory tests
./test_cli.sh filter struct            # Structure tests
```

## Performance and Timing

### Expected Execution Times
- **Quick test**: < 5 seconds (basic validation)
- **Fast test**: < 10 seconds (core features)
- **Category test**: 5-15 seconds (specific area)
- **Bulk test**: < 30 seconds (all categories)
- **Full suite**: 30-60 seconds (comprehensive)

### Performance Optimization
```bash
# For maximum speed in CI/CD
./test_cli.sh fast "basics/**/*.dr"    # Fastest core validation
python test_cli.py quick               # Cross-platform quick test

# For comprehensive validation
./test_cli.sh bulk                     # All categories with summary
python test_cli.py run                 # Full detailed run
```

## Result Analysis

### Real-time Results
All test commands provide immediate feedback:
- ✓ **PASS** - Test executed successfully
- ✗ **FAIL** - Output mismatch
- ⚠ **SKIP** - No expected output defined
- ❌ **ERROR** - Compilation/runtime error
- ⏱ **TIMEOUT** - Exceeded time limit

### Detailed Analysis
```bash
# Generate comprehensive reports
python test_cli.py analyze --format html --output report.html
python test_cli.py analyze --format text > summary.txt

# Performance analysis
python test_cli.py analyze --format charts

# Historical trends
python test_cli.py analyze --days 7    # Last week's results
```

### Result Storage
- Results saved in `test_results/` directory
- JSON format for programmatic access
- HTML reports for human review
- Performance metrics tracked over time

## Platform-Specific Information

### Linux Platform
- **Pass Rate**: ~95% (all features working)
- **Known Issues**: None currently
- **Performance**: Optimal (native compilation)
- **Tools**: Full GNU toolchain support

### Windows Platform  
- **Pass Rate**: ~85% (struct parsing issues)
- **Known Issues**: Struct declaration parsing
- **Performance**: Good (MinGW-w64 compilation)
- **Tools**: PowerShell and MinGW-w64 required

### Known Windows Issues
The following tests are expected to fail on Windows due to struct parsing issues:
- `tests/advanced/data_structures/struct.dr`
- `tests/advanced/data_structures/new_struct.dr`
- `tests/advanced/oop/class.dr`
- All tests in `tests/advanced/oop/` directory

## CI/CD Integration

### GitHub Actions Example
```yaml
- name: Run Tests (Linux)
  run: ./test_cli.sh bulk
  
- name: Run Tests (Windows)  
  run: .\test_cli.ps1 bulk
  shell: powershell

- name: Generate Report
  run: python test_cli.py analyze --format html --output test-report.html
```

### Exit Codes
- `0` - All tests passed
- `1` - Some tests failed or errors occurred

### Automated Reporting
```bash
# Generate CI-friendly reports
python test_cli.py analyze --format text > test-summary.txt
python test_cli.py generate --matrix > test-matrix.json
```

## Advanced Features

### Continuous Testing
```bash
# Watch mode for development
./test_cli.sh watch "basics/**/*.dr"   # Linux
.\test_cli.ps1 watch "basics/**/*.dr"  # Windows
```

### Performance Benchmarking
```bash
# Run performance benchmarks
./test_cli.sh benchmark 5              # 5 iterations
.\test_cli.ps1 benchmark 5             # 5 iterations
```

### Test Generation
```bash
# Validate existing tests
python test_cli.py generate --validate

# Generate comprehensive test suite
python test_cli.py generate --generate

# Show test matrix for planning
python test_cli.py generate --matrix
```

## Troubleshooting

### Common Issues

**Build Failures:**
```bash
# Check environment
./test_cli.sh status                   # Linux
.\test_cli.ps1 status                  # Windows

# Manual build
zig build
```

**Missing Dependencies:**
```bash
# Linux
sudo apt-get install zig python3 make gcc

# Windows (Chocolatey)
choco install zig python mingw
```

**Permission Issues (Linux):**
```bash
chmod +x test_cli.sh
chmod +x codex/python/test_runner
```

### Debug Mode
```bash
# Enable verbose output
./test_cli.sh run "**/*.dr" true       # Linux debug
.\test_cli.ps1 run "**/*.dr" -debug    # Windows debug
python test_cli.py run --debug         # Cross-platform debug
```

### Environment Variables
```bash
# Linux/macOS
export TEST_DEBUG=1
export TEST_TIMEOUT=60

# Windows
$env:TEST_DEBUG = "1"
$env:TEST_TIMEOUT = "60"
```

## Best Practices for CLI Agents

1. **Start with quick validation**: Use `quick` command for fast feedback
2. **Use category filters**: Test specific areas with `filter` command
3. **Leverage bulk testing**: Use `bulk` for comprehensive validation
4. **Monitor performance**: Use `benchmark` to track performance changes
5. **Generate reports**: Use `analyze` for detailed insights
6. **Validate before changes**: Always run tests before committing code

## File Structure

```
DreamCompiler/
├── test_cli.py          # Universal Python CLI
├── test_cli.sh          # Linux shell script
├── test_cli.ps1         # Windows PowerShell script
├── test_config.json     # Test configuration
├── TEST_GUIDE.md        # This guide
├── tests/               # Test files organized by category
├── test_results/        # Generated test results
├── codex/python/        # Advanced test management tools
│   ├── test_manager.py  # Core test execution engine
│   ├── test_analyzer.py # Result analysis and reporting
│   └── test_generator.py # Test generation and validation
└── codex/AGENTS.md      # Agent-specific documentation
```

This testing framework provides CLI agents with fast, reliable, and comprehensive testing capabilities across all supported platforms.