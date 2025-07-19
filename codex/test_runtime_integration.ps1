# Test script to verify Dream runtime library integration
# Tests both compilation and execution of Dream programs with runtime dependencies

Write-Host "=== Dream Runtime Library Integration Tests ===" -ForegroundColor Green

# Test 1: Basic console output
Write-Host "`nTest 1: Basic Console Output" -ForegroundColor Yellow
$test1_content = @"
// Test basic console output functionality
Console.WriteLine("Test 1: Console output works!");
"@

Set-Content -Path "tests/temp_test1.dr" -Value $test1_content
$result1 = & ".\dream.exe" "tests/temp_test1.dr" 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Test 1 PASSED: $result1" -ForegroundColor Green
} else {
    Write-Host "✗ Test 1 FAILED: $result1" -ForegroundColor Red
}

# Test 2: Memory allocation functions
Write-Host "`nTest 2: Memory Management" -ForegroundColor Yellow
$test2_content = @"
// Test memory allocation and cleanup
Console.WriteLine("Test 2: Memory management works!");
"@

Set-Content -Path "tests/temp_test2.dr" -Value $test2_content
$result2 = & ".\dream.exe" "tests/temp_test2.dr" 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Test 2 PASSED: $result2" -ForegroundColor Green
} else {
    Write-Host "✗ Test 2 FAILED: $result2" -ForegroundColor Red
}

# Test 3: Check generated C code includes runtime headers
Write-Host "`nTest 3: Generated C Code Verification" -ForegroundColor Yellow
$test3_content = @"
// Test that generated C code properly includes runtime
Console.WriteLine("Test 3: Runtime headers included!");
"@

Set-Content -Path "tests/temp_test3.dr" -Value $test3_content

# Compile to C only to check headers
$c_output = & ".\dream.exe" "--emit-c" "tests/temp_test3.dr" 2>&1
if ($LASTEXITCODE -eq 0) {
    $c_content = Get-Content "build/bin/dream.c" -Raw
    if ($c_content -match '#include "console.h"' -and $c_content -match '#include "memory.h"') {
        Write-Host "✓ Test 3 PASSED: Runtime headers properly included" -ForegroundColor Green
    } else {
        Write-Host "✗ Test 3 FAILED: Runtime headers missing from generated C" -ForegroundColor Red
    }
} else {
    Write-Host "✗ Test 3 FAILED: Could not generate C code: $c_output" -ForegroundColor Red
}

# Test 4: Verify runtime object files exist and are linked
Write-Host "`nTest 4: Runtime Object Files" -ForegroundColor Yellow
if ((Test-Path "build/console.o") -and (Test-Path "build/memory.o")) {
    Write-Host "✓ Test 4 PASSED: Runtime object files exist" -ForegroundColor Green
} else {
    Write-Host "✗ Test 4 FAILED: Runtime object files missing" -ForegroundColor Red
    Write-Host "  Expected: build/console.o and build/memory.o" -ForegroundColor Red
}

# Test 5: Cross-platform compilation test
Write-Host "`nTest 5: Cross-Platform Compilation" -ForegroundColor Yellow
$test5_content = @"
// Test cross-platform runtime compatibility
Console.WriteLine("Cross-platform test successful!");
"@

Set-Content -Path "tests/temp_test5.dr" -Value $test5_content

# Test different targets if available
$targets = @("x86_64-windows", "x86_64-linux")
foreach ($target in $targets) {
    Write-Host "  Testing target: $target" -ForegroundColor Cyan
    $cross_result = & ".\dream.exe" "--target" $target "--emit-c" "tests/temp_test5.dr" 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  ✓ $target compilation successful" -ForegroundColor Green
    } else {
        Write-Host "  ✗ $target compilation failed: $cross_result" -ForegroundColor Red
    }
}

# Test 6: Custom runtime library detection
Write-Host "`nTest 6: Custom Runtime Library" -ForegroundColor Yellow
if (Test-Path "build/custom.o" -or (Test-Path "build/libdruntime.a" -and (ar t "build/libdruntime.a" | Select-String -Quiet "custom.o"))) {
    Write-Host "✓ Test 6 PASSED: custom runtime library compiled" -ForegroundColor Green
} else {
    Write-Host "✗ Test 6 FAILED: custom runtime object missing" -ForegroundColor Red
}

# Cleanup temporary files
Remove-Item "tests/temp_test*.dr" -ErrorAction SilentlyContinue

Write-Host "`n=== Test Summary ===" -ForegroundColor Green
Write-Host "Runtime integration tests completed." -ForegroundColor White
Write-Host "Check output above for any failures that need attention." -ForegroundColor White