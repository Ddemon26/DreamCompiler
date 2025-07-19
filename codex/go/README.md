# DreamCompiler Go Test Suite

This directory contains a comprehensive Go test suite for the DreamCompiler project, providing unit tests, benchmarks, and fuzz tests that complement the existing Python-based end-to-end tests.

## Overview

The Go test suite provides:

- **Unit Tests** - Direct testing of C APIs via cgo
- **Benchmarks** - Performance testing for hot paths
- **Fuzz Tests** - Robustness testing with random inputs
- **Integration Tests** - End-to-end compilation and execution testing

## Architecture

### Files Structure

```
codex/go/
├── go.mod                 # Go module definition
├── dream.h               # C API header for Go integration
├── dream_api.c           # C API wrapper implementation
├── dream_test.go         # Unit and integration tests
├── dream_bench_test.go   # Performance benchmarks
├── dream_fuzz_test.go    # Fuzz tests for robustness
├── Makefile             # Build system for C library
└── README.md            # This file
```

### C API Wrapper

The Go tests interact with the DreamCompiler through a C API wrapper:

- **dream.h** - Defines the C API interface
- **dream_api.c** - Implements wrapper functions for Go cgo integration
- **libdream.so/.dll** - Shared library built from DreamCompiler sources

## Quick Start

### Prerequisites

- Go 1.21 or later
- Zig compiler (for building C library)
- re2c (for lexer generation)

### Installation

```bash
# Install Go dependencies
make deps

# Build the C library
make build

# Run all tests
make test
```

### Running Tests

```bash
# Unit tests only
go test -v ./...

# With benchmarks
make bench

# With fuzz testing (short duration)
make fuzz

# Complete test suite
make test-all
```

## Test Categories

### 1. Unit Tests (`dream_test.go`)

Direct testing of C APIs through cgo:

#### Lexer Tests
- **Token Recognition** - Verifies correct tokenization of various language constructs
- **Position Tracking** - Tests line/column position accuracy
- **Error Handling** - Tests lexer behavior with invalid input

```go
func TestLexer_BasicTokens(t *testing.T) {
    // Tests tokenization of "1 + 2"
    // Expects: TK_INT, TK_PLUS, TK_INT, TK_EOF
}
```

#### Parser Tests
- **AST Construction** - Verifies correct Abstract Syntax Tree generation
- **Error Recovery** - Tests parser behavior with syntax errors
- **Node Validation** - Validates AST node structure and relationships

```go
func TestParser_BasicExpressions(t *testing.T) {
    // Tests parsing of various expressions
    // Validates AST structure and error handling
}
```

#### Type System Tests
- **Type Creation** - Tests type system operations
- **Type Equality** - Verifies type comparison logic
- **Type Conversion** - Tests type string representations

#### Memory Management Tests
- **Arena Allocation** - Tests memory arena functionality
- **Resource Cleanup** - Verifies proper resource deallocation

### 2. Benchmarks (`dream_bench_test.go`)

Performance testing for compiler hot paths:

#### Lexer Benchmarks
```go
BenchmarkLexer_SimpleTokens         # Basic tokenization performance
BenchmarkLexer_ComplexExpression    # Complex expression tokenization
BenchmarkLexer_StringLiterals       # String processing performance
```

#### Parser Benchmarks
```go
BenchmarkParser_SimpleExpression    # Expression parsing performance
BenchmarkParser_ControlFlow         # Control structure parsing
BenchmarkParser_FunctionDeclaration # Function parsing performance
```

#### Code Generation Benchmarks
```go
BenchmarkCodegen_SimpleProgram      # Basic code generation
BenchmarkCodegen_ComplexProgram     # Complex program compilation
```

#### Scaling Benchmarks
- **Token Count Scaling** - Performance with varying input sizes
- **AST Depth Scaling** - Performance with nested structures
- **Memory Usage Profiling** - Memory allocation patterns

### 3. Fuzz Tests (`dream_fuzz_test.go`)

Robustness testing with random inputs:

#### Lexer Fuzzing
```go
func FuzzLexer(f *testing.F) {
    // Tests lexer with random byte sequences
    // Detects crashes, infinite loops, and memory issues
}
```

#### Parser Fuzzing
```go
func FuzzParser(f *testing.F) {
    // Tests parser with malformed syntax
    // Validates error handling and recovery
}
```

#### Code Generation Fuzzing
```go
func FuzzCodegen(f *testing.F) {
    // Tests code generation with edge cases
    // Ensures robust output generation
}
```

#### End-to-End Fuzzing
```go
func FuzzEndToEnd(f *testing.F) {
    // Tests complete compilation pipeline
    // Validates execution stability
}
```

## Usage Examples

### Running Specific Test Categories

```bash
# Run only lexer tests
go test -run TestLexer -v

# Run only benchmarks
go test -bench=BenchmarkLexer -benchmem

# Run specific fuzz test
go test -fuzz=FuzzParser -fuzztime=1m
```

### Performance Analysis

```bash
# Generate CPU profile during benchmarks
go test -bench=. -cpuprofile=cpu.prof

# Generate memory profile
go test -bench=. -memprofile=mem.prof

# Analyze profiles
go tool pprof cpu.prof
go tool pprof mem.prof
```

### Continuous Integration

```bash
# CI-friendly test run
make ci

# Generate test coverage report
make test-coverage
```

## Integration with Existing Tests

The Go test suite complements the existing Python tests:

### Python Tests (Existing)
- End-to-end `.dr` file execution
- Output comparison with expected results
- Cross-platform compatibility testing

### Go Tests (New)
- Direct C API testing
- Performance benchmarking
- Robustness and fuzz testing
- Memory management validation

### Combined Workflow

```bash
# Run Python tests
python codex/python/test_runner

# Run Go tests
cd codex/go && make test

# Run both with analysis
python test_cli.py analyze --format html
cd codex/go && make bench > go_bench_results.txt
```

## Performance Expectations

### Typical Benchmark Results

```
BenchmarkLexer_SimpleTokens-8           100000    10000 ns/op    1024 B/op    5 allocs/op
BenchmarkParser_SimpleExpression-8       50000    25000 ns/op    2048 B/op   15 allocs/op
BenchmarkCodegen_SimpleProgram-8         10000   100000 ns/op    8192 B/op   50 allocs/op
```

### Scaling Characteristics

- **Lexer**: Linear scaling with input size
- **Parser**: Roughly linear with some quadratic cases
- **Codegen**: Linear with AST size

## Troubleshooting

### Common Issues

#### Build Failures

```bash
# Check dependencies
make verify

# Clean and rebuild
make clean build

# Debug build configuration
make debug
```

#### Test Failures

```bash
# Run with verbose output
go test -v -x ./...

# Check C library
ldd libdream.so  # Linux
otool -L libdream.so  # macOS
```

#### Performance Issues

```bash
# Profile memory usage
go test -bench=. -memprofile=mem.prof

# Check for memory leaks
valgrind --tool=memcheck go test
```

### Platform-Specific Notes

#### Linux
- Uses `libdream.so` shared library
- Requires `ld` to find the library in current directory

#### Windows
- Uses `libdream.dll` dynamic library
- May require MinGW-w64 for compilation

#### macOS
- Uses `libdream.dylib` dynamic library
- May require Xcode command line tools

## Contributing

### Adding New Tests

1. **Unit Tests**: Add to `dream_test.go`
   ```go
   func TestNewFeature(t *testing.T) {
       // Test implementation
   }
   ```

2. **Benchmarks**: Add to `dream_bench_test.go`
   ```go
   func BenchmarkNewFeature(b *testing.B) {
       // Benchmark implementation
   }
   ```

3. **Fuzz Tests**: Add to `dream_fuzz_test.go`
   ```go
   func FuzzNewFeature(f *testing.F) {
       // Fuzz test implementation
   }
   ```

### Best Practices

- **Resource Management**: Always clean up C resources
- **Error Handling**: Test both success and failure cases
- **Performance**: Use `b.ResetTimer()` in benchmarks
- **Fuzzing**: Validate inputs and prevent infinite loops

### Code Style

- Follow Go conventions (`gofmt`, `golint`)
- Use table-driven tests for multiple cases
- Include comprehensive test documentation
- Add appropriate build tags for platform-specific tests

## Integration with CI/CD

### GitHub Actions Example

```yaml
- name: Setup Go
  uses: actions/setup-go@v4
  with:
    go-version: '1.21'

- name: Build C Library
  run: make build
  working-directory: tests/go

- name: Run Go Tests
  run: make test
  working-directory: tests/go

- name: Run Benchmarks
  run: make bench
  working-directory: tests/go
```

### Test Results Integration

The Go tests generate results that can be integrated with existing test reporting:

- **JUnit XML**: Use `go-junit-report` for CI integration
- **Coverage Reports**: Generate with `go test -cover`
- **Benchmark Results**: Parse with custom tools or `benchstat`

This comprehensive Go test suite provides robust validation of the DreamCompiler's core functionality while offering detailed performance insights and fuzz testing for reliability.