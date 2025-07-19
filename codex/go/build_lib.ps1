# Build script for DreamCompiler C library for Go testing (Windows PowerShell)
# This script handles the complex build process including lexer generation

param(
    [Parameter(Position=0)]
    [string]$Command = "build"
)

# Configuration
$RootDir = Split-Path -Parent $PSScriptRoot
$GoTestDir = $PSScriptRoot
$SrcDir = Join-Path $RootDir "src"
$LexerDir = Join-Path $SrcDir "lexer"

# Color functions
function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Blue
}

function Write-Success {
    param([string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARNING] $Message" -ForegroundColor Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

# Check dependencies
function Test-Dependencies {
    $MissingDeps = @()
    
    $RequiredTools = @("zig", "re2c")
    
    foreach ($Tool in $RequiredTools) {
        try {
            $null = Get-Command $Tool -ErrorAction Stop
        }
        catch {
            $MissingDeps += $Tool
        }
    }
    
    if ($MissingDeps.Count -gt 0) {
        Write-Error "Missing dependencies: $($MissingDeps -join ', ')"
        Write-Info "Please install missing tools:"
        Write-Info "  - zig: https://ziglang.org/download/"
        Write-Info "  - re2c: https://re2c.org/ or via chocolatey: choco install re2c"
        exit 1
    }
}

# Generate lexer if needed
function New-Lexer {
    Write-Info "Checking lexer generation..."
    
    $LexerRe = Join-Path $LexerDir "lexer.re"
    $TokensDef = Join-Path $LexerDir "tokens.def"
    $LexerC = Join-Path $LexerDir "lexer.c"
    
    if (-not (Test-Path $LexerRe) -or -not (Test-Path $TokensDef)) {
        Write-Error "Lexer source files not found:"
        Write-Error "  Expected: $LexerRe"
        Write-Error "  Expected: $TokensDef"
        exit 1
    }
    
    # Check if lexer.c needs regeneration
    $NeedsRegen = $false
    if (-not (Test-Path $LexerC)) {
        $NeedsRegen = $true
    } else {
        $LexerCTime = (Get-Item $LexerC).LastWriteTime
        $LexerReTime = (Get-Item $LexerRe).LastWriteTime
        $TokensDefTime = (Get-Item $TokensDef).LastWriteTime
        
        if ($LexerReTime -gt $LexerCTime -or $TokensDefTime -gt $LexerCTime) {
            $NeedsRegen = $true
        }
    }
    
    if ($NeedsRegen) {
        Write-Info "Generating lexer with re2c..."
        Push-Location $LexerDir
        try {
            re2c -c -o lexer.c lexer.re
            if ($LASTEXITCODE -eq 0) {
                Write-Success "Lexer generated successfully"
            } else {
                Write-Error "Lexer generation failed"
                exit 1
            }
        }
        finally {
            Pop-Location
        }
    } else {
        Write-Info "Lexer is up to date"
    }
}

# Build the C library
function Build-Library {
    Write-Info "Building DreamCompiler C library..."
    
    Push-Location $GoTestDir
    try {
        # Library configuration for Windows
        $LibName = "libdream.dll"
        $LibFlags = "-shared -Wl,--out-implib,libdream.lib"
        
        # Compiler flags
        $CFlags = "-std=c11 -Wall -Wextra -D_GNU_SOURCE -fPIC -O2"
        $Includes = "-I. -I$SrcDir -I$LexerDir"
        
        # Source files
        $Sources = @(
            "$SrcDir\parser\ast.c",
            "$SrcDir\parser\parser.c",
            "$SrcDir\parser\error.c",
            "$SrcDir\parser\diagnostic.c",
            "$SrcDir\sem\scope.c",
            "$SrcDir\sem\symbol.c",
            "$SrcDir\sem\type.c",
            "$SrcDir\sem\infer.c",
            "$SrcDir\sem\analysis.c",
            "$SrcDir\ir\ir.c",
            "$SrcDir\ir\lower.c",
            "$SrcDir\cfg\cfg.c",
            "$SrcDir\ssa\ssa.c",
            "$SrcDir\opt\pipeline.c",
            "$SrcDir\opt\sccp.c",
            "$SrcDir\opt\dce.c",
            "$SrcDir\opt\value_numbering.c",
            "$SrcDir\opt\licm.c",
            "$SrcDir\opt\copy_prop.c",
            "$SrcDir\opt\cse.c",
            "$SrcDir\opt\peephole.c",
            "$SrcDir\codegen\c_emit.c",
            "$SrcDir\codegen\context.c",
            "$SrcDir\codegen\expr.c",
            "$SrcDir\codegen\stmt.c",
            "$SrcDir\codegen\codegen.c",
            "$LexerDir\lexer.c",
            "dream_api.c"
        )
        
        # Check that all source files exist
        foreach ($Src in $Sources) {
            if (-not (Test-Path $Src)) {
                Write-Error "Source file not found: $Src"
                exit 1
            }
        }
        
        # Build command
        $SourcesStr = $Sources -join " "
        $BuildCmd = "zig cc $CFlags $Includes $LibFlags -o $LibName $SourcesStr"
        
        Write-Info "Build command: $BuildCmd"
        
        # Execute build
        Invoke-Expression $BuildCmd
        
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Library built successfully: $LibName"
            
            # Verify the library
            if (Test-Path $LibName) {
                $Size = (Get-Item $LibName).Length
                Write-Info "Library size: $Size bytes"
            } else {
                Write-Error "Library file not created: $LibName"
                exit 1
            }
        } else {
            Write-Error "Library build failed"
            exit 1
        }
    }
    finally {
        Pop-Location
    }
}

# Verify Go environment
function Test-GoEnvironment {
    Write-Info "Verifying Go environment..."
    
    try {
        $null = Get-Command go -ErrorAction Stop
    }
    catch {
        Write-Error "Go not found. Please install Go 1.21 or later."
        exit 1
    }
    
    $GoVersion = (go version) -replace '.*go(\d+\.\d+).*', '$1'
    Write-Info "Go version: go$GoVersion"
    
    # Check if go.mod exists
    $GoMod = Join-Path $GoTestDir "go.mod"
    if (-not (Test-Path $GoMod)) {
        Write-Warning "go.mod not found, initializing Go module..."
        Push-Location $GoTestDir
        try {
            go mod init dreamcompiler/tests
            go mod tidy
        }
        finally {
            Pop-Location
        }
    }
    
    Write-Success "Go environment verified"
}

# Run a quick test to verify the build
function Test-Build {
    Write-Info "Testing the built library..."
    
    Push-Location $GoTestDir
    try {
        # Try to run a simple Go test that uses the library
        $TestOutput = go test -run TestLexer_BasicTokens -v . 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Library test passed"
        } else {
            Write-Warning "Library test failed, but library was built"
            Write-Info "You may need to run 'go mod tidy' or check cgo configuration"
            Write-Info "Test output: $TestOutput"
        }
    }
    finally {
        Pop-Location
    }
}

# Clean build artifacts
function Remove-BuildArtifacts {
    Write-Info "Cleaning build artifacts..."
    
    Push-Location $GoTestDir
    try {
        $Artifacts = @("libdream.dll", "libdream.lib", "libdream.so", "libdream.dylib")
        
        foreach ($Artifact in $Artifacts) {
            if (Test-Path $Artifact) {
                Remove-Item $Artifact -Force
                Write-Info "Removed: $Artifact"
            }
        }
        
        go clean -testcache
        Write-Success "Clean completed"
    }
    finally {
        Pop-Location
    }
}

# Show help
function Show-Help {
    @"
DreamCompiler C Library Build Script for Windows

Usage: .\build_lib.ps1 [command]

Commands:
  build     Build the library (default)
  clean     Clean build artifacts
  help      Show this help

This script:
  1. Checks for required dependencies (zig, re2c)
  2. Generates the lexer if needed
  3. Verifies Go environment
  4. Builds the C library for Go testing
  5. Runs a quick verification test

Requirements:
  - Zig compiler (https://ziglang.org/download/)
  - re2c (https://re2c.org/ or 'choco install re2c')
  - Go 1.21+ (https://golang.org/dl/)

Example:
  .\build_lib.ps1          # Build the library
  .\build_lib.ps1 clean    # Clean build artifacts
  .\build_lib.ps1 help     # Show this help
"@
}

# Main function
function Main {
    Write-Info "DreamCompiler C Library Build Script for Windows"
    Write-Info "Root directory: $RootDir"
    Write-Info "Go test directory: $GoTestDir"
    
    Test-Dependencies
    New-Lexer
    Test-GoEnvironment
    Build-Library
    Test-Build
    
    Write-Success "Build completed successfully!"
    Write-Info "You can now run Go tests with: go test -v .\..."
}

# Handle command line arguments
switch ($Command.ToLower()) {
    "clean" {
        Remove-BuildArtifacts
    }
    "help" {
        Show-Help
    }
    "build" {
        Main
    }
    default {
        Main
    }
}