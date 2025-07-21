# Go Testing Suite Setup

This document provides setup instructions for the DreamCompiler Go testing suite.

## Prerequisites

### Go Installation

**Required:** Go version 1.21 or higher

#### Windows Installation

**Option 1: Chocolatey (Recommended)**
```powershell
# Run PowerShell as Administrator
choco install golang
```

**Option 2: Manual Installation**
1. Download Go from: https://golang.org/dl/
2. Install the MSI package
3. Add Go to your PATH (usually automatic)
4. Restart your terminal

**Option 3: Scoop**
```powershell
scoop install go
```

#### Linux Installation

**Ubuntu/Debian:**
```bash
# Install from package manager (may be older version)
sudo apt install golang-go

# Or install latest from snap
sudo snap install go --classic

# Or download latest binary
wget https://golang.org/dl/go1.21.0.linux-amd64.tar.gz
sudo rm -rf /usr/local/go
sudo tar -C /usr/local -xzf go1.21.0.linux-amd64.tar.gz
export PATH=$PATH:/usr/local/go/bin
```

### Verification

```bash
# Verify installation
go version  # Should show Go 1.21+

# Verify environment
go env GOOS GOARCH
```

## Dependencies

The Go test suite uses these libraries (automatically installed):

- **github.com/stretchr/testify v1.8.4** - Testing framework with assertions
- Standard library packages for cgo and testing

Dependencies are managed in `go.mod` and installed automatically via:
```bash
go mod tidy
go mod download
```

## Build Dependencies

The Go tests require the DreamCompiler C library to be built. This requires:

- **Zig** - C compiler (zig cc)
- **re2c** - Lexer generator (optional, for regenerating lexer)

### Zig Installation

**Windows:**
```powershell
choco install zig
# Or download from https://ziglang.org/download/
```

**Linux:**
```bash
# Snap (recommended)
sudo snap install zig --classic --beta

# Or download binary from https://ziglang.org/download/
```

## Directory Structure

```
codex/go/
├── go.mod                  # Go module definition
├── dream_test.go          # Unit and integration tests  
├── dream_bench_test.go    # Performance benchmarks
├── dream_fuzz_test.go     # Fuzz tests for robustness
├── dream_api.c           # C API wrapper for cgo
├── dream.h               # C API header
├── Makefile              # Unix/Linux build system
├── build_and_test.ps1    # Windows PowerShell script
└── GO_SETUP.md          # This file
```

## Quick Start

1. **Install Go** (see above)
2. **Verify installation:** `go version`
3. **Navigate to test directory:** `cd codex/go`
4. **Run quick validation:**
   - Linux: `make quick-validate`
   - Windows: `.\build_and_test.ps1 quick`

## Testing Protocols

### For CLI Agents

**Development Workflow:**
```bash
# 1. Quick validation during development (< 30s)
cd codex/go
make quick-validate              # Linux
.\build_and_test.ps1 quick      # Windows

# 2. Feature validation for new compiler features (< 2 minutes)
make validate-feature            # Linux  
.\build_and_test.ps1 feature    # Windows

# 3. Comprehensive validation before commits (< 5 minutes)
make comprehensive-validate     # Linux
.\build_and_test.ps1 comprehensive  # Windows
```

### Standard Commands

```bash
# Install dependencies
make deps

# Build C library and run tests
make test

# Run benchmarks
make bench

# Run fuzz tests
make fuzz

# Clean build artifacts  
make clean

# Show help
make help
```

### Windows PowerShell Script

The `build_and_test.ps1` script provides the same functionality on Windows:

```powershell
# Basic usage
.\build_and_test.ps1 unit         # Unit tests
.\build_and_test.ps1 bench        # Benchmarks  
.\build_and_test.ps1 fuzz         # Fuzz tests

# With options
.\build_and_test.ps1 unit -Coverage    # With coverage report
.\build_and_test.ps1 fuzz -FuzzTime 60 # 60 second fuzz testing
.\build_and_test.ps1 feature -Verbose  # Verbose output
```

## Troubleshooting

### Common Issues

**"go: command not found"**
- Go is not installed or not in PATH
- Restart terminal after installation
- On Windows, check Environment Variables

**"zig: command not found"** 
- Zig is not installed or not in PATH
- Install Zig from https://ziglang.org/download/
- The Go tests will try to build the C library

**"re2c: command not found"**
- This is optional - tests will use existing lexer.c
- Install re2c if you need to regenerate the lexer

**Build failures**
- Check that all C source files exist in src/
- Verify Zig can compile C code: `zig cc --version`
- Check compiler include paths in Makefile

**Test failures**
- Some tests may fail if core compiler functionality is broken
- Check that `zig build` succeeds first
- Review test output for specific failure reasons

### Getting Help

1. Check `make help` for available targets
2. Run `make debug` to show configuration
3. Use `-Verbose` flag with PowerShell script for detailed output
4. Review test output and error messages
5. Ensure all prerequisites are properly installed

## Integration with Main Test Suite

The Go tests complement the Python-based Dream file tests:

- **Go tests** validate the C API directly via cgo
- **Python tests** validate .dr file compilation end-to-end

Both should be run as part of the development workflow:
```bash
# Complete validation workflow
cd codex/go && make quick-validate     # C API validation
cd .. && python test_cli.py quick     # Dream file validation
```