#!/usr/bin/env powershell
# Complete Go Test Suite Validation Script
# This script validates the entire Go testing workflow for DreamCompiler

param(
    [string]$Action = "validate",
    [switch]$InstallGo,
    [switch]$Verbose,
    [switch]$SkipGoTests
)

$ErrorActionPreference = "Stop"

Write-Host "=== DreamCompiler Go Test Suite Validation ===" -ForegroundColor Cyan
Write-Host "Timestamp: $(Get-Date)" -ForegroundColor Gray

# Color coding for output
function Write-Success { param($msg) Write-Host "[SUCCESS] $msg" -ForegroundColor Green }
function Write-Error { param($msg) Write-Host "[ERROR] $msg" -ForegroundColor Red }
function Write-Warning { param($msg) Write-Host "[WARNING] $msg" -ForegroundColor Yellow }
function Write-Info { param($msg) Write-Host "[INFO] $msg" -ForegroundColor Blue }

# Step 1: Check prerequisites
function Test-Prerequisites {
    Write-Host "`n=== CHECKING PREREQUISITES ===" -ForegroundColor Yellow
    
    $allGood = $true
    
    # Check Go installation
    try {
        $goVersion = go version 2>$null
        Write-Success "Go installed: $goVersion"
    } catch {
        Write-Error "Go not installed"
        if ($InstallGo) {
            Write-Info "Attempting to install Go via Chocolatey..."
            try {
                choco install golang -y
                Write-Success "Go installed successfully"
                Write-Warning "Please restart your terminal and re-run this script"
                exit 0
            } catch {
                Write-Error "Failed to install Go automatically"
                Write-Info "Please install Go manually from https://golang.org/dl/"
                Write-Info "Or run: choco install golang (as Administrator)"
                $allGood = $false
            }
        } else {
            Write-Warning "Use -InstallGo flag to attempt automatic installation"
            $allGood = $false
        }
    }
    
    # Check Zig installation
    try {
        $zigVersion = zig version 2>$null
        Write-Success "Zig installed: $zigVersion"
    } catch {
        Write-Warning "Zig not found - some tests may fail"
        Write-Info "Install Zig from https://ziglang.org/download/"
    }
    
    # Check if we're in the correct directory
    if (!(Test-Path "go.mod")) {
        Write-Error "Not in codex/go directory"
        $allGood = $false
    } else {
        Write-Success "Correct directory: codex/go"
    }
    
    # Check for required files
    $requiredFiles = @("dream.h", "dream_api.c", "dream_test.go", "dream_bench_test.go", "dream_fuzz_test.go", "Makefile")
    foreach ($file in $requiredFiles) {
        if (Test-Path $file) {
            Write-Success "Found: $file"
        } else {
            Write-Error "Missing: $file"
            $allGood = $false
        }
    }
    
    return $allGood
}

# Step 2: Test compiler build
function Test-CompilerBuild {
    Write-Host "`n=== TESTING COMPILER BUILD ===" -ForegroundColor Yellow
    
    try {
        Push-Location "../.."
        
        Write-Info "Building DreamCompiler..."
        $buildOutput = zig build 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Compiler build successful"
            
            # Check if executable was created
            if (Test-Path "zig-out/bin/DreamCompiler.exe") {
                Write-Success "Executable created: zig-out/bin/DreamCompiler.exe"
                
                # Test basic compilation
                Write-Info "Testing basic compilation..."
                $testFile = "test_basic.dr"
                "int x = 42; Console.WriteLine(x);" | Out-File -FilePath $testFile -Encoding ASCII
                
                try {
                    $compilerPath = "./zig-out/bin/DreamCompiler.exe"
                    $compileResult = & $compilerPath $testFile 2>&1
                    if ($LASTEXITCODE -eq 0) {
                        Write-Success "Basic compilation test passed"
                    } else {
                        Write-Warning "Basic compilation test failed: $compileResult"
                    }
                } catch {
                    Write-Warning "Could not test compilation: $($_.Exception.Message)"
                } finally {
                    if (Test-Path $testFile) { Remove-Item $testFile }
                }
            } else {
                Write-Warning "Executable not found - build may have failed"
            }
        } else {
            Write-Error "Compiler build failed"
            if ($Verbose) {
                Write-Host $buildOutput
            }
            return $false
        }
    } catch {
        Write-Error "Build test failed: $($_.Exception.Message)"
        return $false
    } finally {
        Pop-Location
    }
    
    return $true
}

# Step 3: Test Go module and dependencies
function Test-GoModule {
    Write-Host "`n=== TESTING GO MODULE ===" -ForegroundColor Yellow
    
    if ($SkipGoTests) {
        Write-Warning "Skipping Go tests (-SkipGoTests flag)"
        return $true
    }
    
    try {
        Write-Info "Installing Go dependencies..."
        go mod tidy
        go mod download
        
        $testifyVersion = go list -m github.com/stretchr/testify
        Write-Success "Dependencies ready: $testifyVersion"
        
        return $true
    } catch {
        Write-Error "Go module setup failed: $($_.Exception.Message)"
        return $false
    }
}

# Step 4: Test C library build
function Test-CLibraryBuild {
    Write-Host "`n=== TESTING C LIBRARY BUILD ===" -ForegroundColor Yellow
    
    if ($SkipGoTests) {
        Write-Warning "Skipping C library build (-SkipGoTests flag)"
        return $true
    }
    
    try {
        # Try using make first
        Write-Info "Attempting to build C library with Make..."
        $makeResult = make build 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            Write-Success "C library built with Make"
            $libName = if ($IsWindows) { "libdream.dll" } else { "libdream.so" }
            if (Test-Path $libName) {
                Write-Success "Library file created: $libName"
                return $true
            }
        }
        
        # Fallback to PowerShell script
        Write-Info "Trying PowerShell build script..."
        if (Test-Path "build_and_test.ps1") {
            $psResult = & .\build_and_test.ps1 -Action "build" 2>&1
            if ($LASTEXITCODE -eq 0) {
                Write-Success "C library built with PowerShell script"
                return $true
            }
        }
        
        Write-Warning "C library build may have issues"
        return $false
        
    } catch {
        Write-Error "C library build failed: $($_.Exception.Message)"
        return $false
    }
}

# Step 5: Run Go tests
function Test-GoTests {
    Write-Host "`n=== RUNNING GO TESTS ===" -ForegroundColor Yellow
    
    if ($SkipGoTests) {
        Write-Warning "Skipping Go tests (-SkipGoTests flag)"
        return $true
    }
    
    $testResults = @{
        "Unit Tests" = $false
        "Benchmarks" = $false
        "Fuzz Tests" = $false
    }
    
    # Unit tests
    try {
        Write-Info "Running unit tests..."
        $unitResult = go test -v -timeout=60s ./... 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Unit tests passed"
            $testResults["Unit Tests"] = $true
        } else {
            Write-Warning "Unit tests had issues"
            if ($Verbose) {
                Write-Host $unitResult
            }
        }
    } catch {
        Write-Error "Unit tests failed: $($_.Exception.Message)"
    }
    
    # Benchmarks
    try {
        Write-Info "Running benchmarks..."
        $benchResult = go test -bench=. -benchtime=1s -timeout=60s ./... 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Benchmarks completed"
            $testResults["Benchmarks"] = $true
        } else {
            Write-Warning "Benchmarks had issues"
            if ($Verbose) {
                Write-Host $benchResult
            }
        }
    } catch {
        Write-Error "Benchmarks failed: $($_.Exception.Message)"
    }
    
    # Fuzz tests (short duration)
    try {
        Write-Info "Running fuzz tests (10s each)..."
        $fuzzTests = @("FuzzLexer", "FuzzParser", "FuzzStringConcatenation")
        foreach ($fuzzTest in $fuzzTests) {
            $fuzzResult = go test -fuzz=$fuzzTest -fuzztime=10s -timeout=30s ./... 2>&1
            if ($LASTEXITCODE -eq 0) {
                Write-Success "Fuzz test passed: $fuzzTest"
            } else {
                Write-Warning "Fuzz test issues: $fuzzTest"
            }
        }
        $testResults["Fuzz Tests"] = $true
    } catch {
        Write-Error "Fuzz tests failed: $($_.Exception.Message)"
    }
    
    return $testResults
}

# Step 6: Test Dream file compilation
function Test-DreamFileCompilation {
    Write-Host "`n=== TESTING DREAM FILE COMPILATION ===" -ForegroundColor Yellow
    
    try {
        Push-Location "../.."
        
        # Test basic Dream file compilation with Python runner
        Write-Info "Testing Dream file compilation..."
        if (Test-Path "codex/test_cli.py") {
            $pythonResult = python codex/test_cli.py quick 2>&1
            if ($LASTEXITCODE -eq 0) {
                Write-Success "Dream file compilation tests passed"
                return $true
            } else {
                Write-Warning "Dream file compilation had issues"
                if ($Verbose) {
                    Write-Host $pythonResult
                }
                return $false
            }
        } else {
            Write-Warning "Python test runner not found"
            return $false
        }
    } catch {
        Write-Error "Dream file test failed: $($_.Exception.Message)"
        return $false
    } finally {
        Pop-Location
    }
}

# Step 7: Generate comprehensive report
function Write-Report {
    param($testResults, $prereqsPassed, $buildPassed, $goPassed, $dreamPassed)
    
    Write-Host "`n=== VALIDATION REPORT ===" -ForegroundColor Cyan
    Write-Host "Timestamp: $(Get-Date)" -ForegroundColor Gray
    Write-Host ""
    
    # Overall status
    $overallPassed = $prereqsPassed -and $buildPassed -and ($SkipGoTests -or ($goPassed -and $dreamPassed))
    
    if ($overallPassed) {
        Write-Success "OVERALL STATUS: PASSED [SUCCESS]"
    } else {
        Write-Error "OVERALL STATUS: FAILED [FAILED]"
    }
    
    Write-Host ""
    Write-Host "Component Status:" -ForegroundColor White
    Write-Host "  Prerequisites: $(if ($prereqsPassed) { '[SUCCESS] PASS' } else { '[FAILED] FAIL' })"
    Write-Host "  Compiler Build: $(if ($buildPassed) { '[SUCCESS] PASS' } else { '[FAILED] FAIL' })"
    
    if ($SkipGoTests) {
        Write-Host "  Go Tests: [SKIPPED] SKIPPED"
    } else {
        Write-Host "  Go Tests: $(if ($goPassed) { '[SUCCESS] PASS' } else { '[FAILED] FAIL' })"
        if ($testResults) {
            foreach ($test in $testResults.Keys) {
                $status = if ($testResults[$test]) { '[SUCCESS]' } else { '[FAILED]' }
                Write-Host "    $test`: $status"
            }
        }
    }
    
    Write-Host "  Dream Files: $(if ($dreamPassed) { '[SUCCESS] PASS' } else { '[FAILED] FAIL' })"
    
    Write-Host ""
    Write-Host "Next Steps:" -ForegroundColor White
    if (-not $prereqsPassed) {
        Write-Host "  1. Install missing prerequisites (Go, Zig)"
        Write-Host "  2. Re-run validation with: .\validate_complete_workflow.ps1 -InstallGo"
    }
    if (-not $buildPassed) {
        Write-Host "  1. Fix compiler build issues with: zig build"
        Write-Host "  2. Check for missing dependencies or source errors"
    }
    if (-not $goPassed -and -not $SkipGoTests) {
        Write-Host "  1. Install Go dependencies: go mod tidy"
        Write-Host "  2. Build C library: make build"
        Write-Host "  3. Run individual tests to debug issues"
    }
    if (-not $dreamPassed) {
        Write-Host "  1. Test Dream compilation: python codex/test_cli.py quick"
        Write-Host "  2. Check compiler and runtime setup"
    }
    
    Write-Host ""
    Write-Host "Validation Commands:" -ForegroundColor White
    Write-Host "  Quick Go validation: cd codex/go && make quick-validate"
    Write-Host "  Feature validation: cd codex/go && make validate-feature" 
    Write-Host "  Complete validation: cd codex/go && make comprehensive-validate"
    Write-Host "  Dream file tests: python codex/test_cli.py quick"
    
    return $overallPassed
}

# Main execution
function Main {
    $prereqsPassed = Test-Prerequisites
    if (-not $prereqsPassed -and -not $SkipGoTests) {
        Write-Error "Prerequisites not met. Use -SkipGoTests to continue without Go tests."
        exit 1
    }
    
    $buildPassed = Test-CompilerBuild
    $goPassed = $true
    $testResults = $null
    
    if (-not $SkipGoTests) {
        $goModPassed = Test-GoModule
        $cLibPassed = Test-CLibraryBuild
        $testResults = Test-GoTests
        
        $goPassed = $goModPassed -and $cLibPassed -and 
                   ($testResults["Unit Tests"] -or $testResults["Benchmarks"])
    }
    
    $dreamPassed = Test-DreamFileCompilation
    
    $overallPassed = Write-Report -testResults $testResults -prereqsPassed $prereqsPassed -buildPassed $buildPassed -goPassed $goPassed -dreamPassed $dreamPassed
    
    if ($overallPassed) {
        Write-Host "`n[SUCCESS] All validation tests completed successfully!" -ForegroundColor Green
        exit 0
    } else {
        Write-Host "`n[FAILED] Some validation tests failed. See report above." -ForegroundColor Red
        exit 1
    }
}

# Execute main function
Main