#!/usr/bin/env powershell
# Windows PowerShell script for DreamCompiler Go testing
# Provides agent-friendly testing protocols with proper error handling

param(
    [string]$Action = "test",
    [switch]$Verbose,
    [switch]$Coverage,
    [int]$FuzzTime = 30
)

$ErrorActionPreference = "Stop"

Write-Host "=== DreamCompiler Go Test Protocol ===" -ForegroundColor Cyan

# Check prerequisites
function Test-Prerequisites {
    Write-Host "Checking prerequisites..." -ForegroundColor Yellow
    
    # Check Go installation
    try {
        $goVersion = go version 2>$null
        Write-Host "✓ Go: $goVersion" -ForegroundColor Green
    } catch {
        Write-Host "✗ Go not installed. Install from https://golang.org/dl/" -ForegroundColor Red
        Write-Host "  Or use: choco install golang" -ForegroundColor Yellow
        exit 1
    }
    
    # Check Zig for C compilation
    try {
        $zigVersion = zig version 2>$null
        Write-Host "✓ Zig: $zigVersion" -ForegroundColor Green
    } catch {
        Write-Host "⚠ Zig not found - some tests may fail" -ForegroundColor Yellow
    }
    
    # Check if we're in the right directory
    if (!(Test-Path "go.mod")) {
        Write-Host "✗ Not in codex/go directory" -ForegroundColor Red
        exit 1
    }
    
    Write-Host "✓ Prerequisites check complete" -ForegroundColor Green
}

# Build C library
function Build-CLibrary {
    Write-Host "Building C library..." -ForegroundColor Yellow
    
    if ($IsWindows) {
        $libName = "libdream.dll"
        $cc = "zig cc"
        $cflags = "-std=c11 -Wall -Wextra -D_GNU_SOURCE -fPIC -O2"
        $ldflags = "-shared -Wl,--out-implib,libdream.lib"
    } else {
        $libName = "libdream.so"
        $cc = "zig cc"
        $cflags = "-std=c11 -Wall -Wextra -D_GNU_SOURCE -fPIC -O2"
        $ldflags = "-shared"
    }
    
    # Create source list (matching Makefile)
    $sources = @(
        "../../src/parser/ast.c",
        "../../src/parser/parser.c",
        "../../src/parser/error.c",
        "../../src/parser/diagnostic.c",
        "../../src/sem/scope.c",
        "../../src/sem/symbol.c",
        "../../src/sem/type.c",
        "../../src/sem/infer.c",
        "../../src/sem/analysis.c",
        "../../src/ir/ir.c",
        "../../src/ir/lower.c",
        "../../src/cfg/cfg.c",
        "../../src/ssa/ssa.c",
        "../../src/opt/pipeline.c",
        "../../src/opt/sccp.c",
        "../../src/opt/dce.c",
        "../../src/opt/value_numbering.c",
        "../../src/opt/licm.c",
        "../../src/opt/copy_prop.c",
        "../../src/opt/cse.c",
        "../../src/opt/peephole.c",
        "../../src/codegen/c_emit.c",
        "../../src/codegen/context.c",
        "../../src/codegen/expr.c",
        "../../src/codegen/stmt.c",
        "../../src/codegen/codegen.c",
        "dream_api.c"
    )
    
    $includes = "-I. -I../../src -I../../src/lexer"
    
    # Generate lexer if needed
    if (!(Test-Path "../../src/lexer/lexer.c") -or 
        (Get-Item "../../src/lexer/lexer.re").LastWriteTime -gt (Get-Item "../../src/lexer/lexer.c").LastWriteTime) {
        Write-Host "Generating lexer..." -ForegroundColor Yellow
        Push-Location "../../src/lexer"
        try {
            & re2c -c -o lexer.c lexer.re
        } catch {
            Write-Host "⚠ re2c not found - using existing lexer.c" -ForegroundColor Yellow
        }
        Pop-Location
    }
    
    $sources += "../../src/lexer/lexer.c"
    
    # Build command
    $buildCmd = "$cc $cflags $includes $ldflags -o $libName " + ($sources -join " ")
    
    if ($Verbose) {
        Write-Host "Build command: $buildCmd" -ForegroundColor Cyan
    }
    
    try {
        Invoke-Expression $buildCmd
        Write-Host "✓ C library built: $libName" -ForegroundColor Green
    } catch {
        Write-Host "✗ Library build failed: $($_.Exception.Message)" -ForegroundColor Red
        exit 1
    }
}

# Install Go dependencies
function Install-Dependencies {
    Write-Host "Installing Go dependencies..." -ForegroundColor Yellow
    
    try {
        go mod tidy
        go mod download
        $testifyVersion = go list -m github.com/stretchr/testify
        Write-Host "✓ Dependencies: $testifyVersion" -ForegroundColor Green
    } catch {
        Write-Host "✗ Dependency installation failed: $($_.Exception.Message)" -ForegroundColor Red
        exit 1
    }
}

# Run tests with different protocols
function Invoke-Tests {
    param([string]$TestType)
    
    switch ($TestType) {
        "quick" {
            Write-Host "=== QUICK VALIDATION (< 30s) ===" -ForegroundColor Cyan
            go test -v -run "TestLexer_BasicTokens|TestParser_BasicExpressions|TestTypes_BasicOperations" ./...
        }
        "unit" {
            Write-Host "=== UNIT TESTS ===" -ForegroundColor Cyan
            if ($Coverage) {
                go test -v -cover -coverprofile=coverage.out ./...
                go tool cover -html=coverage.out -o coverage.html
                Write-Host "✓ Coverage report: coverage.html" -ForegroundColor Green
            } else {
                go test -v ./...
            }
        }
        "feature" {
            Write-Host "=== FEATURE VALIDATION PROTOCOL ===" -ForegroundColor Cyan
            Write-Host "1. Running unit tests..." -ForegroundColor Yellow
            go test -v -run Test ./...
            Write-Host "2. Running benchmarks..." -ForegroundColor Yellow
            go test -v -bench=. -benchtime=5s ./...
            Write-Host "3. Running fuzz tests..." -ForegroundColor Yellow
            go test -fuzz=. -fuzztime="$($FuzzTime)s" ./...
            Write-Host "4. Generating coverage..." -ForegroundColor Yellow
            go test -v -cover -coverprofile=feature_coverage.out ./...
            go tool cover -html=feature_coverage.out -o feature_coverage.html
            Write-Host "✅ Feature validation complete!" -ForegroundColor Green
        }
        "comprehensive" {
            Write-Host "=== COMPREHENSIVE VALIDATION ===" -ForegroundColor Cyan
            go test -v ./...
            go test -v -bench=. -benchtime=10s ./...
            go test -fuzz=FuzzLexer -fuzztime=60s ./...
            go test -fuzz=FuzzParser -fuzztime=60s ./...
            go test -v -cover -coverprofile=comprehensive_coverage.out ./...
            go tool cover -html=comprehensive_coverage.out -o comprehensive_coverage.html
            Write-Host "✅ Comprehensive validation complete!" -ForegroundColor Green
        }
        "bench" {
            Write-Host "=== BENCHMARKS ===" -ForegroundColor Cyan
            go test -v -bench=. -benchmem ./...
        }
        "fuzz" {
            Write-Host "=== FUZZ TESTS ===" -ForegroundColor Cyan
            go test -fuzz=FuzzLexer -fuzztime="$($FuzzTime)s" ./...
            go test -fuzz=FuzzParser -fuzztime="$($FuzzTime)s" ./...
            go test -fuzz=FuzzCodegen -fuzztime="$($FuzzTime)s" ./...
        }
        default {
            Write-Host "=== STANDARD TESTS ===" -ForegroundColor Cyan
            go test -v ./...
        }
    }
}

# Main execution
function Main {
    Test-Prerequisites
    Install-Dependencies
    Build-CLibrary
    
    switch ($Action.ToLower()) {
        "quick" { Invoke-Tests "quick" }
        "unit" { Invoke-Tests "unit" }
        "feature" { Invoke-Tests "feature" }
        "comprehensive" { Invoke-Tests "comprehensive" }
        "bench" { Invoke-Tests "bench" }
        "fuzz" { Invoke-Tests "fuzz" }
        "all" {
            Invoke-Tests "unit"
            Invoke-Tests "bench"
            Invoke-Tests "fuzz"
        }
        default { Invoke-Tests "unit" }
    }
    
    Write-Host "=== Test Protocol Complete ===" -ForegroundColor Cyan
}

# Execute main function
Main