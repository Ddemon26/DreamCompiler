# DreamCompiler Test CLI for Windows PowerShell
# Provides fast test execution and management for CLI agents

param(
    [Parameter(Position=0)]
    [string]$Command,
    
    [Parameter(Position=1, ValueFromRemainingArguments=$true)]
    [string[]]$Arguments
)

# Configuration
$RootDir = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$TestDir = Join-Path $RootDir "tests"
$ResultsDir = Join-Path $RootDir "test_results"
$PythonCLI = Join-Path $RootDir "codex" | Join-Path -ChildPath "test_cli.py"

# Color functions for output
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
    
    # Check required tools
    $RequiredTools = @("zig", "python")
    
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
        Write-Info "Please install missing tools and try again"
        exit 1
    }
    
    # Check Python modules
    try {
        python -c "import json, pathlib, subprocess" 2>$null
        if ($LASTEXITCODE -ne 0) {
            throw "Python modules check failed"
        }
    }
    catch {
        Write-Error "Required Python modules not available"
        exit 1
    }
}

# Build compiler
function Build-Compiler {
    Write-Info "Building DreamCompiler..."
    
    try {
        zig build
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Build completed successfully"
            return $true
        }
        else {
            Write-Error "Build failed"
            return $false
        }
    }
    catch {
        Write-Error "Build failed: $($_.Exception.Message)"
        return $false
    }
}

# Quick test functions
function Invoke-QuickTest {
    param([string]$TestFile)
    
    Write-Info "Running quick test..."
    
    if (-not (Build-Compiler)) {
        return 1
    }
    
    if ($TestFile) {
        python $PythonCLI quick $TestFile
    }
    else {
        python $PythonCLI quick
    }
    
    return $LASTEXITCODE
}

# Run test suite
function Invoke-TestSuite {
    param(
        [string]$Pattern = "**/*.dr",
        [string[]]$Categories,
        [switch]$Debug
    )
    
    Write-Info "Running test suite with pattern: $Pattern"
    
    $CmdArgs = @("run", "--pattern", $Pattern)
    
    if ($Categories) {
        $CmdArgs += "--categories"
        $CmdArgs += $Categories
    }
    
    if ($Debug) {
        $CmdArgs += "--debug"
    }
    
    python $PythonCLI @CmdArgs
    return $LASTEXITCODE
}

# Filter tests by category
function Get-TestPattern {
    param([string]$Category)
    
    switch ($Category.ToLower()) {
        { $_ -in @("basics", "basic") } { return "basics/**/*.dr" }
        "advanced" { return "advanced/**/*.dr" }
        { $_ -in @("control", "control_flow") } { return "control_flow/**/*.dr" }
        { $_ -in @("functions", "func") } { return "functions/**/*.dr" }
        { $_ -in @("memory", "mem") } { return "**/memory/*.dr" }
        { $_ -in @("struct", "structs") } { return "**/data_structures/*.dr" }
        default {
            Write-Warning "Unknown category: $Category"
            return "**/*.dr"
        }
    }
}

# Bulk test execution
function Invoke-BulkTest {
    $Categories = @("basics", "control_flow", "functions", "advanced")
    $Results = @()
    
    Write-Info "Running bulk tests across all categories..."
    
    if (-not (Build-Compiler)) {
        return 1
    }
    
    foreach ($Category in $Categories) {
        Write-Info "Testing category: $Category"
        
        $Pattern = Get-TestPattern $Category
        
        python $PythonCLI run --pattern $Pattern > $null 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            $Results += "$Category`:PASS"
            Write-Success "$Category tests passed"
        }
        else {
            $Results += "$Category`:FAIL"
            Write-Error "$Category tests failed"
        }
    }
    
    # Summary
    Write-Host ""
    Write-Info "Bulk test summary:"
    foreach ($Result in $Results) {
        $Cat, $Status = $Result -split ':'
        
        if ($Status -eq "PASS") {
            Write-Success "$Cat`: PASSED"
        }
        else {
            Write-Error "$Cat`: FAILED"
        }
    }
}

# Fast test execution with minimal output
function Invoke-FastTest {
    param([string]$Pattern = "basics/**/*.dr")
    
    Write-Info "Fast test execution..."
    
    # Build silently
    zig build > $null 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed"
        return 1
    }
    
    # Run tests with minimal output
    $StartTime = Get-Date
    
    python $PythonCLI run --pattern $Pattern > $null 2>&1
    
    if ($LASTEXITCODE -eq 0) {
        $EndTime = Get-Date
        $Duration = ($EndTime - $StartTime).TotalSeconds
        
        Write-Success "Tests passed in $([math]::Round($Duration, 2))s"
        return 0
    }
    else {
        Write-Error "Tests failed"
        return 1
    }
}

# Analyze results
function Invoke-AnalyzeResults {
    param(
        [string]$Format = "text",
        [string]$Output
    )
    
    Write-Info "Analyzing test results..."
    
    $CmdArgs = @("analyze", "--format", $Format)
    
    if ($Output) {
        $CmdArgs += "--output"
        $CmdArgs += $Output
    }
    
    python $PythonCLI @CmdArgs
    return $LASTEXITCODE
}

# List available tests
function Get-TestList {
    param([string]$Pattern = "**/*.dr")
    
    python $PythonCLI list --pattern $Pattern
    return $LASTEXITCODE
}

# Show environment status
function Show-Status {
    python $PythonCLI status
    return $LASTEXITCODE
}

# Generate tests
function Invoke-GenerateTests {
    param([string]$Action)
    
    switch ($Action.ToLower()) {
        { $_ -in @("new", "generate") } {
            python $PythonCLI generate --generate
        }
        { $_ -in @("validate", "check") } {
            python $PythonCLI generate --validate
        }
        "matrix" {
            python $PythonCLI generate --matrix
        }
        default {
            Write-Error "Unknown generate action: $Action"
            Write-Info "Available actions: new, validate, matrix"
            return 1
        }
    }
    
    return $LASTEXITCODE
}

# Watch mode for continuous testing
function Start-WatchMode {
    param([string]$Pattern = "basics/**/*.dr")
    
    Write-Info "Starting watch mode for pattern: $Pattern"
    Write-Info "Press Ctrl+C to stop"
    
    try {
        while ($true) {
            Clear-Host
            Write-Host "=== DreamCompiler Test Watch Mode ===" -ForegroundColor Cyan
            Write-Host "Pattern: $Pattern"
            Write-Host "Time: $(Get-Date)"
            Write-Host ""
            
            if ((Invoke-FastTest $Pattern) -eq 0) {
                Write-Success "All tests passing"
            }
            else {
                Write-Error "Some tests failing"
            }
            
            Write-Host ""
            Write-Host "Waiting 5 seconds... (Ctrl+C to stop)"
            Start-Sleep -Seconds 5
        }
    }
    catch {
        Write-Info "Watch mode stopped"
    }
}

# Performance benchmark
function Invoke-Benchmark {
    param([int]$Iterations = 3)
    
    Write-Info "Running performance benchmark ($Iterations iterations)..."
    
    $TotalTime = 0
    $SuccessfulRuns = 0
    
    for ($i = 1; $i -le $Iterations; $i++) {
        Write-Info "Benchmark run $i/$Iterations"
        
        $StartTime = Get-Date
        
        if ((Invoke-FastTest) -eq 0) {
            $EndTime = Get-Date
            $Duration = ($EndTime - $StartTime).TotalMilliseconds
            
            $TotalTime += $Duration
            $SuccessfulRuns++
            
            Write-Success "Run $i completed in $([math]::Round($Duration, 0))ms"
        }
        else {
            Write-Error "Run $i failed"
        }
    }
    
    if ($SuccessfulRuns -gt 0) {
        $AvgTime = [math]::Round($TotalTime / $SuccessfulRuns, 0)
        Write-Success "Benchmark complete: ${AvgTime}ms average ($SuccessfulRuns/$Iterations successful)"
    }
    else {
        Write-Error "All benchmark runs failed"
        return 1
    }
}

# Help function
function Show-Help {
    @"
DreamCompiler Test CLI for Windows PowerShell

Usage: .\test_cli.ps1 <command> [options]

Commands:
  quick [file]              Quick test execution (single file or basic suite)
  run [pattern] [debug]     Run test suite with optional pattern and debug mode
  bulk                      Run tests across all categories
  fast [pattern]            Fast test execution with minimal output
  filter <category>         Run tests for specific category
  analyze [format] [output] Analyze test results (text/html/charts)
  list [pattern]            List available tests
  status                    Show environment status
  generate <action>         Generate or validate tests (new/validate/matrix)
  watch [pattern]           Continuous testing mode
  benchmark [iterations]    Performance benchmark
  build                     Build compiler only
  help                      Show this help

Categories for filter:
  basics, advanced, control, functions, memory, struct

Examples:
  .\test_cli.ps1 quick                           # Quick test run
  .\test_cli.ps1 quick tests\basics\arithmetic.dr # Test specific file
  .\test_cli.ps1 run "basics/**/*.dr"           # Run basic tests
  .\test_cli.ps1 run "**/*.dr" -debug           # Run all tests with debug
  .\test_cli.ps1 bulk                           # Test all categories
  .\test_cli.ps1 fast                           # Fast basic test
  .\test_cli.ps1 filter basics                 # Test basic category
  .\test_cli.ps1 analyze html report.html      # Generate HTML report
  .\test_cli.ps1 list "advanced/**/*.dr"       # List advanced tests
  .\test_cli.ps1 generate validate             # Validate existing tests
  .\test_cli.ps1 watch "basics/**/*.dr"        # Watch mode for basics
  .\test_cli.ps1 benchmark 5                   # Run 5 benchmark iterations

Environment Variables:
  `$env:TEST_DEBUG = "1"                        # Enable debug output
  `$env:TEST_TIMEOUT = "30"                     # Set test timeout in seconds
"@
}

# Main command dispatcher
function Main {
    # Check dependencies first
    Test-Dependencies
    
    if (-not $Command) {
        Show-Help
        return 1
    }
    
    switch ($Command.ToLower()) {
        "quick" {
            return Invoke-QuickTest $Arguments[0]
        }
        "run" {
            $Pattern = if ($Arguments[0]) { $Arguments[0] } else { "**/*.dr" }
            $Debug = $Arguments -contains "-debug" -or $Arguments -contains "--debug"
            return Invoke-TestSuite -Pattern $Pattern -Debug:$Debug
        }
        "bulk" {
            Invoke-BulkTest
            return $LASTEXITCODE
        }
        "fast" {
            $Pattern = if ($Arguments[0]) { $Arguments[0] } else { "basics/**/*.dr" }
            return Invoke-FastTest $Pattern
        }
        "filter" {
            if (-not $Arguments[0]) {
                Write-Error "Category required for filter command"
                return 1
            }
            $Pattern = Get-TestPattern $Arguments[0]
            return Invoke-TestSuite -Pattern $Pattern
        }
        "analyze" {
            $Format = if ($Arguments[0]) { $Arguments[0] } else { "text" }
            $Output = $Arguments[1]
            return Invoke-AnalyzeResults -Format $Format -Output $Output
        }
        "list" {
            $Pattern = if ($Arguments[0]) { $Arguments[0] } else { "**/*.dr" }
            return Get-TestList $Pattern
        }
        "status" {
            return Show-Status
        }
        "generate" {
            if (-not $Arguments[0]) {
                Write-Error "Action required for generate command"
                return 1
            }
            return Invoke-GenerateTests $Arguments[0]
        }
        "watch" {
            $Pattern = if ($Arguments[0]) { $Arguments[0] } else { "basics/**/*.dr" }
            Start-WatchMode $Pattern
            return 0
        }
        "benchmark" {
            $Iterations = if ($Arguments[0]) { [int]$Arguments[0] } else { 3 }
            Invoke-Benchmark $Iterations
            return $LASTEXITCODE
        }
        "build" {
            if (Build-Compiler) { return 0 } else { return 1 }
        }
        { $_ -in @("help", "--help", "-h") } {
            Show-Help
            return 0
        }
        default {
            Write-Error "Unknown command: $Command"
            Write-Host ""
            Show-Help
            return 1
        }
    }
}

# Execute main function
exit (Main)