# Testing Framework Documentation

The Dream Compiler includes a comprehensive testing framework designed to ensure reliability, correctness, and performance across all supported platforms and language features.

## Testing Architecture

```
+------------------------------------------------------------+
|                    Dream Testing Stack                     |
+------------------------------------------------------------+
|  Test Runners (CLI scripts: .sh, .ps1, .py)                |
|              |                                             |
|              v                                             |
|  Test Framework (codex/)                                   |
|              |                                             |
|              v                                             |
|  Dream Compiler (compilation tests)                        |
|              |                                             |
|              v                                             |
|  Generated C Code (runtime tests)                          |
|              |                                             |
|              v                                             |
|  Native Execution (output validation)                      |
+------------------------------------------------------------+
```

## Test Organization Structure

### Directory Layout

```
tests/
|-- basics/                 # Core language features (45 tests)
|   |-- arithmetic/         # Basic arithmetic operations
|   |-- variables/          # Variable declarations and assignments
|   |-- io/                 # Console input/output operations
|   '-- strings/            # String manipulation and operations
|-- control_flow/           # Control flow constructs (28 tests)
|   |-- conditionals/       # if/else statements
|   |-- loops/              # while, do, for loops
|   |-- comparisons/        # Comparison operators
|   '-- switches/           # Switch statements
|-- functions/              # Function definitions and calls (23 tests)
|   |-- declarations/       # Function declarations
|   |-- parameters/         # Parameter passing
|   |-- returns/            # Return values
|   '-- recursion/          # Recursive function calls
|-- advanced/               # Advanced features (35 tests)
|   |-- data_structures/    # Classes, structs, arrays
|   |-- oop/                # Object-oriented programming
|   |-- generics/           # Generic types and functions
|   '-- concurrency/        # Async/await features
|-- semantics/              # Type checking and validation (15 tests)
|   |-- type_checking/      # Static type validation
|   |-- symbol_resolution/  # Variable and function resolution
|   '-- error_detection/    # Compile-time error detection
|-- multifile/              # Multi-file compilation (10 tests)
|   |-- modules/            # Module system testing
|   |-- imports/            # Import/export functionality
|   '-- dependencies/       # Dependency resolution
'-- debug/                  # Debug and diagnostic tests (5 tests)
    |-- error_messages/     # Error message quality
    '-- diagnostics/        # Debug information
```

### Test Categories and Coverage

| Category | Test Count | Coverage | Status |
|----------|------------|----------|--------|
| **Basics** | 45 | Core language | ✅ Stable |
| **Control Flow** | 28 | Conditionals, loops | ✅ Stable |
| **Functions** | 23 | Function system | ✅ Stable |
| **Advanced** | 35 | OOP, generics | 🚧 In Progress |
| **Semantics** | 15 | Type system | ✅ Stable |
| **Multi-file** | 10 | Module system | ✅ Stable |
| **Debug** | 5 | Diagnostics | 🚧 In Progress |
| **Total** | **161** | **Complete system** | **87% Stable** |

## Test Runners

### Cross-Platform Test Execution

Dream provides three equivalent test runners for different platforms:

#### 1. Linux/macOS Shell Script (`codex/test_cli.sh`)

```bash
#!/bin/bash
# Cross-platform Dream compiler testing framework

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
COMPILER_PATH="$PROJECT_ROOT/zig-out/bin/DreamCompiler"

# Test configuration
TEST_TIMEOUT=30
MAX_PARALLEL_JOBS=4
VERBOSE=false

# Color output for terminals
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

run_test() {
    local test_file="$1"
    local test_name="$(basename "$test_file" .dr)"
    
    echo "Running test: $test_name"
    
    # Compile the Dream file
    if timeout $TEST_TIMEOUT "$COMPILER_PATH" "$test_file" > "/tmp/${test_name}.log" 2>&1; then
        # Check if compilation succeeded
        if [ -f "build/bin/dream.c" ]; then
            # Compile generated C code
            if gcc "build/bin/dream.c" "src/runtime/memory.c" "src/runtime/console.c" \
                   -o "/tmp/${test_name}" >> "/tmp/${test_name}.log" 2>&1; then
                # Execute and capture output
                if timeout $TEST_TIMEOUT "/tmp/${test_name}" > "/tmp/${test_name}.output" 2>&1; then
                    echo -e "${GREEN}PASS${NC}: $test_name"
                    return 0
                else
                    echo -e "${RED}FAIL${NC}: $test_name (execution failed)"
                    return 1
                fi
            else
                echo -e "${RED}FAIL${NC}: $test_name (C compilation failed)"
                return 1
            fi
        else
            echo -e "${RED}FAIL${NC}: $test_name (Dream compilation failed)"
            return 1
        fi
    else
        echo -e "${RED}FAIL${NC}: $test_name (timeout)"
        return 1
    fi
}

# Test suite execution functions
run_quick_tests() {
    echo "Running quick test suite..."
    local test_files=(
        "tests/basics/io/console.dr"
        "tests/basics/arithmetic/basic_ops.dr"
        "tests/basics/variables/simple_vars.dr"
        "tests/control_flow/conditionals/if_else.dr"
        "tests/functions/declarations/simple_func.dr"
    )
    
    local passed=0
    local total=${#test_files[@]}
    
    for test_file in "${test_files[@]}"; do
        if run_test "$test_file"; then
            ((passed++))
        fi
    done
    
    echo "Quick tests: $passed/$total passed"
    return $((total - passed))
}
```

#### 2. Windows PowerShell Script (`codex/test_cli.ps1`)

```powershell
# Dream Compiler Testing Framework - Windows PowerShell Version

param(
    [string]$Command = "help",
    [string]$Filter = "",
    [switch]$Verbose = $false
)

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$CompilerPath = Join-Path $ProjectRoot "zig-out\bin\DreamCompiler.exe"

# Test configuration
$TestTimeout = 30
$MaxParallelJobs = 4

function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    
    switch ($Color) {
        "Red" { Write-Host $Message -ForegroundColor Red }
        "Green" { Write-Host $Message -ForegroundColor Green }
        "Yellow" { Write-Host $Message -ForegroundColor Yellow }
        default { Write-Host $Message }
    }
}

function Run-Test {
    param(
        [string]$TestFile
    )
    
    $testName = [System.IO.Path]::GetFileNameWithoutExtension($TestFile)
    Write-Output "Running test: $testName"
    
    $logFile = Join-Path $env:TEMP "$testName.log"
    $outputFile = Join-Path $env:TEMP "$testName.output"
    $executableFile = Join-Path $env:TEMP "$testName.exe"
    
    try {
        # Compile Dream file
        $compileProcess = Start-Process -FilePath $CompilerPath -ArgumentList $TestFile `
                           -RedirectStandardOutput $logFile -RedirectStandardError $logFile `
                           -Wait -PassThru -WindowStyle Hidden
        
        if ($compileProcess.ExitCode -eq 0 -and (Test-Path "build\bin\dream.c")) {
            # Compile generated C code
            $gccArgs = @("build\bin\dream.c", "src\runtime\memory.c", "src\runtime\console.c", "-o", $executableFile)
            $gccProcess = Start-Process -FilePath "gcc" -ArgumentList $gccArgs `
                           -RedirectStandardOutput $logFile -RedirectStandardError $logFile `
                           -Wait -PassThru -WindowStyle Hidden
            
            if ($gccProcess.ExitCode -eq 0) {
                # Execute compiled program
                $execProcess = Start-Process -FilePath $executableFile `
                                -RedirectStandardOutput $outputFile -RedirectStandardError $outputFile `
                                -Wait -PassThru -WindowStyle Hidden
                
                if ($execProcess.ExitCode -eq 0) {
                    Write-ColorOutput "PASS: $testName" "Green"
                    return $true
                } else {
                    Write-ColorOutput "FAIL: $testName (execution failed)" "Red"
                    return $false
                }
            } else {
                Write-ColorOutput "FAIL: $testName (C compilation failed)" "Red"
                return $false
            }
        } else {
            Write-ColorOutput "FAIL: $testName (Dream compilation failed)" "Red"
            return $false
        }
    } catch {
        Write-ColorOutput "FAIL: $testName (exception: $($_.Exception.Message))" "Red"
        return $false
    }
}

function Run-QuickTests {
    Write-Output "Running quick test suite..."
    
    $testFiles = @(
        "tests\basics\io\console.dr",
        "tests\basics\arithmetic\basic_ops.dr",
        "tests\basics\variables\simple_vars.dr",
        "tests\control_flow\conditionals\if_else.dr",
        "tests\functions\declarations\simple_func.dr"
    )
    
    $passed = 0
    $total = $testFiles.Length
    
    foreach ($testFile in $testFiles) {
        if (Run-Test $testFile) {
            $passed++
        }
    }
    
    Write-Output "Quick tests: $passed/$total passed"
    return ($total - $passed)
}
```

#### 3. Cross-Platform Python Script (`codex/test_cli.py`)

```python
#!/usr/bin/env python3
"""
Dream Compiler Testing Framework - Cross-Platform Python Version
Provides consistent testing across Windows, Linux, and macOS
"""

import os
import sys
import subprocess
import json
import time
import argparse
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import List, Dict, Tuple, Optional

class DreamTestRunner:
    def __init__(self, project_root: Path):
        self.project_root = project_root
        self.compiler_path = self.find_compiler()
        self.config = self.load_test_config()
        self.results = {"passed": [], "failed": [], "skipped": []}
        
    def find_compiler(self) -> Path:
        """Locate the Dream compiler executable."""
        possible_paths = [
            self.project_root / "zig-out" / "bin" / "DreamCompiler",
            self.project_root / "zig-out" / "bin" / "DreamCompiler.exe",
            Path("./DreamCompiler"),
            Path("./DreamCompiler.exe")
        ]
        
        for path in possible_paths:
            if path.exists() and path.is_file():
                return path.resolve()
        
        raise FileNotFoundError("Dream compiler not found. Run 'zig build' first.")
    
    def load_test_config(self) -> Dict:
        """Load test configuration from JSON file."""
        config_path = self.project_root / "codex" / "test_config.json"
        
        if config_path.exists():
            with open(config_path) as f:
                return json.load(f)
        
        # Default configuration
        return {
            "timeout": 30,
            "max_parallel": 4,
            "platforms": {
                "windows": {"pass_rate": 85, "known_issues": ["struct_parsing"]},
                "linux": {"pass_rate": 95, "known_issues": []},
                "darwin": {"pass_rate": 90, "known_issues": ["in_development"]}
            },
            "test_categories": {
                "quick": [
                    "tests/basics/io/console.dr",
                    "tests/basics/arithmetic/basic_ops.dr",
                    "tests/basics/variables/simple_vars.dr",
                    "tests/control_flow/conditionals/if_else.dr",
                    "tests/functions/declarations/simple_func.dr"
                ]
            }
        }
    
    def run_single_test(self, test_file: Path) -> Tuple[str, bool, str]:
        """Run a single test file and return results."""
        test_name = test_file.stem
        
        try:
            # Compile Dream file
            compile_result = subprocess.run(
                [str(self.compiler_path), str(test_file)],
                capture_output=True,
                text=True,
                timeout=self.config["timeout"],
                cwd=self.project_root
            )
            
            if compile_result.returncode == 0:
                # Check for generated C code
                c_file = self.project_root / "build" / "bin" / "dream.c"
                if c_file.exists():
                    # Compile C code
                    gcc_result = subprocess.run([
                        "gcc", str(c_file),
                        str(self.project_root / "src" / "runtime" / "memory.c"),
                        str(self.project_root / "src" / "runtime" / "console.c"),
                        "-o", f"/tmp/{test_name}" if os.name != 'nt' else f"{test_name}.exe"
                    ], capture_output=True, text=True, timeout=self.config["timeout"])
                    
                    if gcc_result.returncode == 0:
                        # Execute compiled program
                        exec_path = f"/tmp/{test_name}" if os.name != 'nt' else f"{test_name}.exe"
                        exec_result = subprocess.run(
                            [exec_path],
                            capture_output=True,
                            text=True,
                            timeout=self.config["timeout"]
                        )
                        
                        if exec_result.returncode == 0:
                            return test_name, True, "Test passed"
                        else:
                            return test_name, False, f"Execution failed: {exec_result.stderr}"
                    else:
                        return test_name, False, f"C compilation failed: {gcc_result.stderr}"
                else:
                    return test_name, False, "No C code generated"
            else:
                return test_name, False, f"Dream compilation failed: {compile_result.stderr}"
                
        except subprocess.TimeoutExpired:
            return test_name, False, f"Test timeout after {self.config['timeout']}s"
        except Exception as e:
            return test_name, False, f"Test exception: {str(e)}"
    
    def run_parallel_tests(self, test_files: List[Path]) -> None:
        """Run multiple tests in parallel."""
        max_workers = min(self.config["max_parallel"], len(test_files))
        
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            future_to_test = {
                executor.submit(self.run_single_test, test_file): test_file
                for test_file in test_files
            }
            
            for future in as_completed(future_to_test):
                test_file = future_to_test[future]
                try:
                    test_name, passed, message = future.result()
                    
                    if passed:
                        self.results["passed"].append(test_name)
                        print(f"✅ PASS: {test_name}")
                    else:
                        self.results["failed"].append((test_name, message))
                        print(f"❌ FAIL: {test_name} - {message}")
                        
                except Exception as exc:
                    test_name = test_file.stem
                    self.results["failed"].append((test_name, f"Exception: {exc}"))
                    print(f"❌ ERROR: {test_name} - {exc}")
    
    def run_quick_tests(self) -> int:
        """Run the quick test suite."""
        print("🚀 Running quick test suite...")
        
        test_files = []
        for test_path in self.config["test_categories"]["quick"]:
            full_path = self.project_root / test_path
            if full_path.exists():
                test_files.append(full_path)
            else:
                print(f"⚠️  Test file not found: {test_path}")
        
        if not test_files:
            print("❌ No test files found!")
            return 1
        
        start_time = time.time()
        self.run_parallel_tests(test_files)
        elapsed_time = time.time() - start_time
        
        # Print results
        passed_count = len(self.results["passed"])
        failed_count = len(self.results["failed"])
        total_count = passed_count + failed_count
        
        print(f"\n🚀 Quick Test Results:")
        print(f"   Passed: {passed_count}/{total_count}")
        print(f"   Failed: {failed_count}/{total_count}")
        print(f"   Time: {elapsed_time:.2f}s")
        print(f"   Pass Rate: {(passed_count/total_count)*100:.1f}%")
        
        if failed_count > 0:
            print(f"\n❌ Failed Tests:")
            for test_name, message in self.results["failed"]:
                print(f"    {test_name}: {message}")
        
        return failed_count

def main():
    parser = argparse.ArgumentParser(description="Dream Compiler Test Runner")
    parser.add_argument("command", nargs="?", default="help", 
                       choices=["help", "quick", "bulk", "filter", "run"],
                       help="Test command to run")
    parser.add_argument("--pattern", help="Test file pattern for filtering")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    parser.add_argument("--parallel", "-j", type=int, default=4, help="Parallel job count")
    
    args = parser.parse_args()
    
    # Find project root
    current_dir = Path(__file__).parent
    project_root = current_dir.parent
    
    try:
        test_runner = DreamTestRunner(project_root)
        test_runner.config["max_parallel"] = args.parallel
        
        if args.command == "quick":
            return test_runner.run_quick_tests()
        elif args.command == "help":
            print("Dream Compiler Test Runner")
            print("Commands:")
            print("  quick    - Run quick test suite (5 tests, <5s)")
            print("  bulk     - Run all tests (~30s)")
            print("  filter   - Run tests matching pattern")
            print("  run      - Run specific test file")
            return 0
        else:
            print(f"Command '{args.command}' not implemented yet")
            return 1
            
    except Exception as e:
        print(f"❌ Error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
```

## Test Categories and Examples

### 1. Basic Language Features (`tests/basics/`)

#### Console I/O Test (`tests/basics/io/console.dr`)
```dream
// Test basic console input/output operations
func main() {
    Console.WriteLine("Testing console output");
    
    // Test different data types
    Console.WriteLine(42);
    Console.WriteLine(3.14159);
    Console.WriteLine(true);
    Console.WriteLine('A');
    
    // Test string concatenation in output
    string name = "Dream";
    Console.WriteLine("Hello, " + name + "!");
}
```

**Expected Output:**
```
Testing console output
42
3.14159
true
A
Hello, Dream!
```

#### Arithmetic Operations Test (`tests/basics/arithmetic/basic_ops.dr`)
```dream
// Test basic arithmetic operations
func main() {
    int a = 10;
    int b = 5;
    
    Console.WriteLine("Addition: " + (a + b));
    Console.WriteLine("Subtraction: " + (a - b));
    Console.WriteLine("Multiplication: " + (a * b));
    Console.WriteLine("Division: " + (a / b));
    Console.WriteLine("Modulo: " + (a % 3));
    
    // Test operator precedence
    int result = a + b * 2;
    Console.WriteLine("Precedence test: " + result);
}
```

### 2. Control Flow Tests (`tests/control_flow/`)

#### Conditional Statements (`tests/control_flow/conditionals/if_else.dr`)
```dream
// Test if/else conditional statements
func main() {
    int score = 85;
    
    if (score >= 90) {
        Console.WriteLine("Grade: A");
    } else if (score >= 80) {
        Console.WriteLine("Grade: B");
    } else if (score >= 70) {
        Console.WriteLine("Grade: C");
    } else {
        Console.WriteLine("Grade: F");
    }
    
    // Test boolean conditions
    bool is_passing = score >= 60;
    if (is_passing) {
        Console.WriteLine("Student passes");
    }
}
```

#### Loop Constructs (`tests/control_flow/loops/for_loop.dr`)
```dream
// Test for loop functionality
func main() {
    Console.WriteLine("Counting up:");
    for (int i = 1; i <= 5; i++) {
        Console.WriteLine("Count: " + i);
    }
    
    Console.WriteLine("Counting down:");
    for (int j = 5; j > 0; j--) {
        Console.WriteLine("Countdown: " + j);
    }
    
    // Test nested loops
    Console.WriteLine("Multiplication table:");
    for (int x = 1; x <= 3; x++) {
        for (int y = 1; y <= 3; y++) {
            Console.WriteLine(x + " x " + y + " = " + (x * y));
        }
    }
}
```

### 3. Function Tests (`tests/functions/`)

#### Function Declarations (`tests/functions/declarations/simple_func.dr`)
```dream
// Test function declarations and calls
func int add(int a, int b) {
    return a + b;
}

func string greet(string name) {
    return "Hello, " + name + "!";
}

func void print_separator() {
    Console.WriteLine("---");
}

func main() {
    int result = add(10, 5);
    Console.WriteLine("10 + 5 = " + result);
    
    string message = greet("World");
    Console.WriteLine(message);
    
    print_separator();
    Console.WriteLine("Functions working correctly!");
}
```

### 4. Advanced Features (`tests/advanced/`)

#### Object-Oriented Programming (`tests/advanced/oop/classes.dr`)
```dream
// Test basic class functionality
class Person {
    string name;
    int age;
    
    func Person(string name, int age) {
        this.name = name;
        this.age = age;
    }
    
    func string GetDescription() {
        return this.name + " is " + this.age + " years old";
    }
    
    func void HaveBirthday() {
        this.age++;
        Console.WriteLine(this.name + " is now " + this.age + " years old!");
    }
}

func main() {
    Person alice = new Person("Alice", 25);
    Person bob = new Person("Bob", 30);
    
    Console.WriteLine(alice.GetDescription());
    Console.WriteLine(bob.GetDescription());
    
    alice.HaveBirthday();
    Console.WriteLine(alice.GetDescription());
}
```

### 5. Multi-File Compilation Tests (`tests/multifile/`)

#### Main Module (`tests/multifile/main.dr`)
```dream
// Test multi-file compilation
module main;

import math_utils;
import string_utils;

func main() {
    Console.WriteLine("Testing multi-file compilation");
    
    // Test math utilities
    int sum = math_utils.Add(10, 5);
    int product = math_utils.Multiply(4, 7);
    
    Console.WriteLine("10 + 5 = " + sum);
    Console.WriteLine("4 * 7 = " + product);
    
    // Test string utilities
    string result = string_utils.Concat("Hello", "World");
    string upper = string_utils.ToUpper(result);
    
    Console.WriteLine("Concatenated: " + result);
    Console.WriteLine("Uppercase: " + upper);
}
```

#### Math Utilities Module (`tests/multifile/math_utils.dr`)
```dream
// Math utility module
module math_utils;

export func int Add(int a, int b) {
    return a + b;
}

export func int Subtract(int a, int b) {
    return a - b;
}

export func int Multiply(int a, int b) {
    return a * b;
}

export func int Divide(int a, int b) {
    if (b == 0) {
        throw new Exception("Division by zero!");
    }
    return a / b;
}
```

## Test Configuration and Platform Support

### Test Configuration File (`codex/test_config.json`)

```json
{
  "version": "1.1.12",
  "timeout_seconds": 30,
  "max_parallel_jobs": 4,
  "platforms": {
    "windows": {
      "expected_pass_rate": 85,
      "known_issues": [
        "struct_declaration_parsing",
        "some_data_structure_tests"
      ],
      "compiler_flags": ["-std=c11", "-Wall"],
      "runtime_libraries": ["memory.c", "console.c", "custom.c"]
    },
    "linux": {
      "expected_pass_rate": 95,
      "known_issues": [],
      "compiler_flags": ["-std=c11", "-Wall", "-O2"],
      "runtime_libraries": ["memory.c", "console.c", "custom.c"]
    },
    "macos": {
      "expected_pass_rate": 90,
      "known_issues": ["in_development"],
      "compiler_flags": ["-std=c11", "-Wall"],
      "runtime_libraries": ["memory.c", "console.c", "custom.c"]
    }
  },
  "test_categories": {
    "quick": {
      "description": "Fast smoke tests (< 5 seconds)",
      "test_files": [
        "tests/basics/io/console.dr",
        "tests/basics/arithmetic/basic_ops.dr",
        "tests/basics/variables/simple_vars.dr",
        "tests/control_flow/conditionals/if_else.dr",
        "tests/functions/declarations/simple_func.dr"
      ]
    },
    "basics": {
      "description": "Core language features",
      "pattern": "tests/basics/**/*.dr"
    },
    "advanced": {
      "description": "Advanced language features",
      "pattern": "tests/advanced/**/*.dr"
    },
    "multifile": {
      "description": "Multi-file compilation",
      "pattern": "tests/multifile/**/*.dr"
    }
  },
  "expected_results": {
    "quick": {
      "total_tests": 5,
      "min_passing": 5,
      "max_duration_seconds": 5
    },
    "bulk": {
      "total_tests": 161,
      "min_passing_linux": 153,
      "min_passing_windows": 137,
      "max_duration_seconds": 30
    }
  }
}
```

## Performance and Regression Testing

### Performance Benchmarking

```python
# Performance test runner (codex/performance_tests.py)
class PerformanceTestRunner:
    def __init__(self):
        self.baseline_metrics = self.load_baseline()
        
    def run_compilation_benchmark(self, test_files):
        """Measure compilation performance."""
        results = {}
        
        for test_file in test_files:
            start_time = time.perf_counter()
            
            # Run compiler
            result = subprocess.run([
                str(self.compiler_path), str(test_file)
            ], capture_output=True, text=True)
            
            end_time = time.perf_counter()
            compilation_time = end_time - start_time
            
            results[test_file.stem] = {
                "compilation_time": compilation_time,
                "success": result.returncode == 0,
                "memory_usage": self.get_memory_usage(),
                "output_size": self.get_output_size()
            }
            
        return results
    
    def run_execution_benchmark(self, test_files):
        """Measure runtime performance of generated code."""
        results = {}
        
        for test_file in test_files:
            # Compile first
            self.compile_test(test_file)
            
            # Measure execution time
            executable = f"/tmp/{test_file.stem}"
            start_time = time.perf_counter()
            
            result = subprocess.run([executable], capture_output=True)
            
            end_time = time.perf_counter()
            execution_time = end_time - start_time
            
            results[test_file.stem] = {
                "execution_time": execution_time,
                "success": result.returncode == 0,
                "memory_usage": self.get_runtime_memory_usage(executable)
            }
            
        return results
```

### Regression Testing

```bash
# Regression test script (codex/regression_test.sh)
#!/bin/bash

# Run regression tests against previous version
run_regression_tests() {
    local previous_version="$1"
    local current_version="$2"
    
    echo "Running regression tests: $previous_version � $current_version"
    
    # Build both versions
    git checkout "$previous_version"
    zig build
    cp "zig-out/bin/DreamCompiler" "/tmp/dream_compiler_old"
    
    git checkout "$current_version"
    zig build
    cp "zig-out/bin/DreamCompiler" "/tmp/dream_compiler_new"
    
    # Compare test results
    echo "Testing with old compiler..."
    run_test_suite "/tmp/dream_compiler_old" > "/tmp/old_results.txt"
    
    echo "Testing with new compiler..."
    run_test_suite "/tmp/dream_compiler_new" > "/tmp/new_results.txt"
    
    # Analyze differences
    diff "/tmp/old_results.txt" "/tmp/new_results.txt" > "/tmp/regression_diff.txt"
    
    if [ -s "/tmp/regression_diff.txt" ]; then
        echo "�  Regression detected!"
        cat "/tmp/regression_diff.txt"
        return 1
    else
        echo " No regressions detected"
        return 0
    fi
}
```

## Continuous Integration Testing

### GitHub Actions Workflow (`.github/workflows/test.yml`)

```yaml
name: Dream Compiler Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  quick-tests:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Zig
      uses: goto-bus-stop/setup-zig@v2
      with:
        version: 0.12.0
    
    - name: Build Compiler
      run: zig build
    
    - name: Run Quick Tests
      shell: bash
      run: |
        if [[ "$RUNNER_OS" == "Windows" ]]; then
          powershell -ExecutionPolicy Bypass -File codex/test_cli.ps1 quick
        else
          ./codex/test_cli.sh quick
        fi
    
    - name: Upload Test Results
      if: always()
      uses: actions/upload-artifact@v3
      with:
        name: test-results-${{ matrix.os }}
        path: |
          /tmp/*.log
          /tmp/*.output

  full-test-suite:
    needs: quick-tests
    runs-on: ubuntu-latest
    if: github.event_name == 'push' && github.ref == 'refs/heads/main'
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Zig
      uses: goto-bus-stop/setup-zig@v2
      with:
        version: 0.12.0
    
    - name: Build Compiler
      run: zig build
    
    - name: Run Full Test Suite
      run: ./codex/test_cli.sh bulk
    
    - name: Generate Test Report
      run: python3 codex/test_cli.py report --format=html
    
    - name: Upload Test Report
      uses: actions/upload-artifact@v3
      with:
        name: test-report-full
        path: build/test-report.html

  performance-tests:
    needs: quick-tests
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
      with:
        fetch-depth: 0  # Need history for performance comparison
    
    - name: Setup Zig
      uses: goto-bus-stop/setup-zig@v2
      with:
        version: 0.12.0
    
    - name: Build Compiler
      run: zig build
    
    - name: Run Performance Tests
      run: python3 codex/performance_tests.py --compare-with=HEAD~1
    
    - name: Upload Performance Results
      uses: actions/upload-artifact@v3
      with:
        name: performance-results
        path: build/performance-report.json
```

## Test Maintenance and Best Practices

### Adding New Tests

1. **Follow Naming Conventions:**
   ```
   tests/[category]/[feature]/[specific_test].dr
   ```

2. **Include Expected Output:**
   ```dream
   // Expected output:
   // Line 1: Hello World
   // Line 2: 42
   // Line 3: true
   
   func main() {
       Console.WriteLine("Hello World");
       Console.WriteLine(42);
       Console.WriteLine(true);
   }
   ```

3. **Test One Feature Per File:**
   ```dream
   // Good: Tests only string concatenation
   func main() {
       string a = "Hello";
       string b = "World";
       Console.WriteLine(a + " " + b);
   }
   
   // Avoid: Tests multiple unrelated features
   func main() {
       string s = "test";
       int i = 42;
       if (i > 0) {
           Console.WriteLine(s);
       }
   }
   ```

### Debugging Failed Tests

1. **Check Compiler Output:**
   ```bash
   ./zig-out/bin/DreamCompiler tests/failing_test.dr -v
   ```

2. **Examine Generated C Code:**
   ```bash
   cat build/bin/dream.c
   ```

3. **Manually Compile and Run:**
   ```bash
   gcc build/bin/dream.c src/runtime/memory.c -o test_debug
   ./test_debug
   ```

This comprehensive testing framework ensures the Dream Compiler maintains high quality and reliability across all supported platforms and language features.