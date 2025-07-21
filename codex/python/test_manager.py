#!/usr/bin/env python3
"""
DreamCompiler Advanced Test Management System
Provides comprehensive testing capabilities for CLI agents across Linux and Windows
"""

import argparse
import json
import logging
import os
import platform
import re
import subprocess
import sys
import time
from dataclasses import dataclass, asdict
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple, Union
from enum import Enum

# Configuration
ROOT = Path(__file__).resolve().parent.parent.parent
TEST_DIR = ROOT / "tests"
RUNTIME_DIR = ROOT / "runtime"
RESULTS_DIR = ROOT / "test_results"
CONFIG_FILE = ROOT / "codex" / "test_config.json"

class TestStatus(Enum):
    PASS = "PASS"
    FAIL = "FAIL"
    SKIP = "SKIP"
    ERROR = "ERROR"
    TIMEOUT = "TIMEOUT"

class TestCategory(Enum):
    UNIT = "unit"
    INTEGRATION = "integration"
    REGRESSION = "regression"
    PERFORMANCE = "performance"
    CROSS_PLATFORM = "cross_platform"
    MEMORY = "memory"
    SYNTAX = "syntax"
    SEMANTIC = "semantic"

@dataclass
class TestResult:
    """Detailed test result with comprehensive metadata"""
    name: str
    path: str
    status: TestStatus
    category: TestCategory
    duration: float
    expected_output: str
    actual_output: str
    compile_time: float
    runtime: float
    memory_usage: Optional[int] = None
    error_message: Optional[str] = None
    platform: str = platform.system()
    compiler_version: Optional[str] = None
    timestamp: float = 0.0
    
    def __post_init__(self):
        if self.timestamp == 0.0:
            self.timestamp = time.time()

@dataclass
class TestSuite:
    """Collection of test results with summary statistics"""
    name: str
    results: List[TestResult]
    total_duration: float
    platform: str
    timestamp: float
    
    @property
    def passed(self) -> int:
        return sum(1 for r in self.results if r.status == TestStatus.PASS)
    
    @property
    def failed(self) -> int:
        return sum(1 for r in self.results if r.status == TestStatus.FAIL)
    
    @property
    def skipped(self) -> int:
        return sum(1 for r in self.results if r.status == TestStatus.SKIP)
    
    @property
    def errors(self) -> int:
        return sum(1 for r in self.results if r.status == TestStatus.ERROR)
    
    @property
    def total(self) -> int:
        return len(self.results)
    
    @property
    def pass_rate(self) -> float:
        return (self.passed / self.total * 100) if self.total > 0 else 0.0

class TestManager:
    """Advanced test management system for DreamCompiler"""
    
    def __init__(self, debug: bool = False):
        self.debug = debug
        self.setup_logging()
        self.ensure_directories()
        self.load_config()
        
    def setup_logging(self):
        """Configure logging with appropriate levels"""
        level = logging.DEBUG if self.debug else logging.INFO
        logging.basicConfig(
            level=level,
            format="%(asctime)s [%(levelname)s] %(message)s",
            datefmt="%H:%M:%S"
        )
        self.logger = logging.getLogger(__name__)
        
    def ensure_directories(self):
        """Create necessary directories"""
        RESULTS_DIR.mkdir(exist_ok=True)
        
    def load_config(self):
        """Load test configuration"""
        default_config = {
            "timeout": 30,
            "parallel_jobs": 1,
            "memory_limit_mb": 512,
            "categories": {
                "basics": TestCategory.UNIT.value,
                "control_flow": TestCategory.UNIT.value,
                "functions": TestCategory.UNIT.value,
                "advanced": TestCategory.INTEGRATION.value,
                "semantics": TestCategory.SEMANTIC.value,
                "debug": TestCategory.REGRESSION.value
            },
            "platform_specific": {
                "windows": {
                    "known_failures": [
                        "tests/advanced/data_structures/struct.dr",
                        "tests/advanced/data_structures/new_struct.dr",
                        "tests/advanced/oop/class.dr"
                    ]
                },
                "linux": {
                    "known_failures": []
                }
            }
        }
        
        if CONFIG_FILE.exists():
            with open(CONFIG_FILE, 'r') as f:
                self.config = json.load(f)
        else:
            self.config = default_config
            self.save_config()
            
    def save_config(self):
        """Save current configuration"""
        with open(CONFIG_FILE, 'w') as f:
            json.dump(self.config, f, indent=2)
            
    def categorize_test(self, test_path: Path) -> TestCategory:
        """Determine test category based on path"""
        # Convert to absolute path and then make relative to TEST_DIR
        try:
            if test_path.is_absolute():
                path_str = str(test_path.relative_to(TEST_DIR))
            else:
                # Assume it's relative to root and convert to absolute
                abs_path = ROOT / test_path
                path_str = str(abs_path.relative_to(TEST_DIR))
        except ValueError:
            # Fallback: just use the path as-is and extract the category part
            path_parts = test_path.parts
            if len(path_parts) > 0:
                # Use the first directory as category indicator
                path_str = path_parts[0] if path_parts[0] != "tests" else (path_parts[1] if len(path_parts) > 1 else "")
            else:
                path_str = str(test_path)
        
        for category_path, category_name in self.config["categories"].items():
            if path_str.startswith(category_path):
                return TestCategory(category_name)
                
        return TestCategory.UNIT
        
    def is_known_failure(self, test_path: Path) -> bool:
        """Check if test is a known failure on current platform"""
        platform_name = platform.system().lower()
        known_failures = self.config.get("platform_specific", {}).get(platform_name, {}).get("known_failures", [])
        
        # Convert path to string for comparison, handling relative/absolute paths
        try:
            if test_path.is_absolute():
                test_str = str(test_path.relative_to(ROOT))
            else:
                test_str = str(test_path)
        except ValueError:
            test_str = str(test_path)
            
        return test_str in known_failures
        
    def run_command(self, cmd: List[str], cwd: Path = ROOT, timeout: int = None) -> subprocess.CompletedProcess:
        """Execute command with detailed logging and timeout"""
        timeout = timeout or self.config["timeout"]
        
        self.logger.debug(f"Running: {' '.join(cmd)}")
        self.logger.debug(f"Working directory: {cwd}")
        
        try:
            result = subprocess.run(
                cmd, 
                cwd=cwd, 
                text=True, 
                capture_output=True, 
                timeout=timeout
            )
            
            self.logger.debug(f"Return code: {result.returncode}")
            if result.stdout:
                self.logger.debug(f"stdout: {result.stdout}")
            if result.stderr:
                self.logger.debug(f"stderr: {result.stderr}")
                
            return result
            
        except subprocess.TimeoutExpired as e:
            self.logger.error(f"Command timed out after {timeout}s: {' '.join(cmd)}")
            raise
            
    def parse_test_file(self, test_path: Path) -> Tuple[List[str], List[str]]:
        """Parse test file for expected output and options"""
        content = test_path.read_text(encoding='utf-8')
        
        expected_matches = re.findall(r"//\s*Expected:\s*(.*)", content)
        option_matches = re.findall(r"//\s*Options:\s*(.*)", content)
        
        expected_output = []
        for match in expected_matches:
            match = match.strip()
            if match != "(no output)":
                expected_output.append(match)
                
        options = []
        for match in option_matches:
            options.extend(match.split())
            
        return expected_output, options
        
    def compile_test(self, test_path: Path, options: List[str]) -> Tuple[bool, float, str]:
        """Compile a single test file and link with runtime library"""
        start_time = time.time()

        # Ensure compiler and runtime library are built
        if self.run_command(["zig", "build"]).returncode != 0:
            return False, 0.0, "Build failed"

        is_windows = platform.system() == "Windows"
        compiler_name = "DreamCompiler.exe" if is_windows else "DreamCompiler"

        possible_paths = [
            ROOT / "zig-out" / "bin" / compiler_name,
            ROOT / "build" / "bin" / compiler_name,
            ROOT / compiler_name,
        ]

        compiler_path = None
        for path in possible_paths:
            if path.exists():
                compiler_path = path
                break

        if not compiler_path:
            return False, 0.0, "DreamCompiler executable not found"

        cmd = [str(compiler_path), "--dev"] + options + [str(test_path)]
        result = self.run_command(cmd)
        if result.returncode != 0:
            return False, time.time() - start_time, result.stderr

        exe = "dream.exe" if is_windows else "dream"
        exe_path = ROOT / exe
        
        # Link with individual runtime source files for better compatibility
        # Runtime sources moved into subdirectories; use new paths
        runtime_files = [
            "src/runtime/memory/memory.c",
            "src/runtime/io/console.c",
            "src/runtime/extensions/custom.c",
            "src/runtime/exceptions/exception.c",
            "src/runtime/system/task.c",
        ]
        
        cc_cmd = [
            "zig",
            "cc",
            "-Isrc/runtime",
            "build/bin/dream.c"
        ] + runtime_files + [
            "-o",
            str(exe_path),
        ]
        link_res = self.run_command(cc_cmd)
        compile_time = time.time() - start_time
        if link_res.returncode != 0:
            return False, compile_time, link_res.stderr

        return True, compile_time, link_res.stdout
            
    def execute_test(self, test_path: Path) -> Tuple[bool, float, str]:
        """Run the compiled test executable"""
        _ = test_path
        start_time = time.time()

        exe = "dream.exe" if platform.system() == "Windows" else "dream"
        exe_path = ROOT / exe
        if not exe_path.exists():
            return False, 0.0, "Executable not found"

        res = self.run_command([str(exe_path)])
        runtime = time.time() - start_time
        if res.returncode != 0:
            return False, runtime, res.stderr

        return True, runtime, res.stdout.strip()
            
    def run_single_test(self, test_path: Path) -> TestResult:
        """Execute a single test with comprehensive result tracking"""
        start_time = time.time()
        category = self.categorize_test(test_path)
        
        # Safe relative path display
        try:
            if test_path.is_absolute():
                display_path = test_path.relative_to(TEST_DIR)
            else:
                display_path = test_path
        except ValueError:
            display_path = test_path
        self.logger.info(f"Running {display_path}")
        
        # Parse test file
        expected_output_lines, options = self.parse_test_file(test_path)
        expected_output = "\n".join(expected_output_lines)
        
        # Skip if no expected output
        if not expected_output_lines:
            duration = time.time() - start_time
            # Safe path conversion for skip result
            try:
                if test_path.is_absolute():
                    skip_path = str(test_path.relative_to(ROOT))
                else:
                    abs_path = ROOT / test_path
                    skip_path = str(abs_path.relative_to(ROOT))
            except ValueError:
                skip_path = str(test_path)
                
            return TestResult(
                name=test_path.name,
                path=skip_path,
                status=TestStatus.SKIP,
                category=category,
                duration=duration,
                expected_output="",
                actual_output="",
                compile_time=0.0,
                runtime=0.0
            )
            
        # Check if known failure
        if self.is_known_failure(test_path):
            self.logger.info(f"[KNOWN FAILURE] {test_path.name}")
            
        # Compile test
        compile_success, compile_time, compile_error = self.compile_test(test_path, options)
        
        if not compile_success:
            duration = time.time() - start_time
            # Safe path conversion for error result
            try:
                if test_path.is_absolute():
                    error_path = str(test_path.relative_to(ROOT))
                else:
                    abs_path = ROOT / test_path
                    error_path = str(abs_path.relative_to(ROOT))
            except ValueError:
                error_path = str(test_path)
                
            return TestResult(
                name=test_path.name,
                path=error_path,
                status=TestStatus.ERROR,
                category=category,
                duration=duration,
                expected_output=expected_output,
                actual_output="",
                compile_time=compile_time,
                runtime=0.0,
                error_message=f"Compile error: {compile_error}"
            )
            
        # Execute test
        exec_success, runtime, actual_output = self.execute_test(test_path)
        
        if not exec_success:
            duration = time.time() - start_time
            # Safe path conversion for runtime error result
            try:
                if test_path.is_absolute():
                    runtime_error_path = str(test_path.relative_to(ROOT))
                else:
                    abs_path = ROOT / test_path
                    runtime_error_path = str(abs_path.relative_to(ROOT))
            except ValueError:
                runtime_error_path = str(test_path)
                
            return TestResult(
                name=test_path.name,
                path=runtime_error_path,
                status=TestStatus.ERROR,
                category=category,
                duration=duration,
                expected_output=expected_output,
                actual_output="",
                compile_time=compile_time,
                runtime=runtime,
                error_message=f"Runtime error: {actual_output}"
            )
            
        # Compare output
        status = TestStatus.PASS if actual_output == expected_output else TestStatus.FAIL
        duration = time.time() - start_time
        
        # Safe path conversion for result
        try:
            if test_path.is_absolute():
                result_path = str(test_path.relative_to(ROOT))
            else:
                # Convert relative path to absolute and then back to relative to ROOT
                abs_path = ROOT / test_path
                result_path = str(abs_path.relative_to(ROOT))
        except ValueError:
            result_path = str(test_path)
            
        result = TestResult(
            name=test_path.name,
            path=result_path,
            status=status,
            category=category,
            duration=duration,
            expected_output=expected_output,
            actual_output=actual_output,
            compile_time=compile_time,
            runtime=runtime
        )
        
        # Log result
        if status == TestStatus.PASS:
            self.logger.info(f"[PASS] {test_path.name}")
        else:
            self.logger.error(f"[FAIL] {test_path.name} - Expected: '{expected_output}', Got: '{actual_output}'")
            
        return result
        
    def discover_tests(self, pattern: str = "**/*.dr") -> List[Path]:
        """Discover all test files matching pattern"""
        tests = sorted(TEST_DIR.glob(pattern))
        self.logger.info(f"Discovered {len(tests)} test files")
        return tests
        
    def run_test_suite(self, pattern: str = "**/*.dr", categories: List[str] = None) -> TestSuite:
        """Run complete test suite with filtering"""
        start_time = time.time()
        
        tests = self.discover_tests(pattern)
        results = []
        
        # Filter by categories if specified
        if categories:
            category_enums = [TestCategory(cat) for cat in categories]
            filtered_tests = []
            for test in tests:
                if self.categorize_test(test) in category_enums:
                    filtered_tests.append(test)
            tests = filtered_tests
            
        self.logger.info(f"Running {len(tests)} tests...")
        
        for test_path in tests:
            result = self.run_single_test(test_path)
            results.append(result)
            
        total_duration = time.time() - start_time
        
        suite = TestSuite(
            name=f"DreamCompiler Tests ({pattern})",
            results=results,
            total_duration=total_duration,
            platform=platform.system(),
            timestamp=time.time()
        )
        
        self.save_results(suite)
        self.print_summary(suite)
        
        return suite
        
    def save_results(self, suite: TestSuite):
        """Save test results to JSON file"""
        timestamp_str = time.strftime("%Y%m%d_%H%M%S", time.localtime(suite.timestamp))
        filename = f"test_results_{timestamp_str}_{suite.platform.lower()}.json"
        filepath = RESULTS_DIR / filename
        
        # Convert to serializable format
        suite_dict = asdict(suite)
        suite_dict['results'] = [asdict(r) for r in suite.results]
        
        # Convert enums to strings
        for result in suite_dict['results']:
            result['status'] = result['status'].value if hasattr(result['status'], 'value') else result['status']
            result['category'] = result['category'].value if hasattr(result['category'], 'value') else result['category']
            
        with open(filepath, 'w') as f:
            json.dump(suite_dict, f, indent=2)
            
        self.logger.info(f"Results saved to {filepath}")
        
    def print_summary(self, suite: TestSuite):
        """Print comprehensive test summary"""
        print("\n" + "="*80)
        print(f"TEST SUITE SUMMARY: {suite.name}")
        print(f"Platform: {suite.platform}")
        print(f"Duration: {suite.total_duration:.2f}s")
        print("="*80)
        
        print(f"Total Tests: {suite.total}")
        print(f"Passed:      {suite.passed} ({suite.pass_rate:.1f}%)")
        print(f"Failed:      {suite.failed}")
        print(f"Skipped:     {suite.skipped}")
        print(f"Errors:      {suite.errors}")
        
        # Category breakdown
        categories = {}
        for result in suite.results:
            cat = result.category.value if hasattr(result.category, 'value') else result.category
            if cat not in categories:
                categories[cat] = {'pass': 0, 'fail': 0, 'skip': 0, 'error': 0}
            
            status = result.status.value if hasattr(result.status, 'value') else result.status
            if status == 'PASS':
                categories[cat]['pass'] += 1
            elif status == 'FAIL':
                categories[cat]['fail'] += 1
            elif status == 'SKIP':
                categories[cat]['skip'] += 1
            else:
                categories[cat]['error'] += 1
                
        print("\nCategory Breakdown:")
        for cat, stats in categories.items():
            total = sum(stats.values())
            pass_rate = (stats['pass'] / total * 100) if total > 0 else 0
            print(f"  {cat:12} {stats['pass']:3}/{total:3} ({pass_rate:5.1f}%)")
            
        # Failed tests
        failed_tests = [r for r in suite.results if r.status == TestStatus.FAIL]
        if failed_tests:
            print(f"\nFailed Tests ({len(failed_tests)}):")
            for result in failed_tests:
                print(f"  {result.path}")
                if result.error_message:
                    print(f"    Error: {result.error_message}")
                    
        print("="*80)

def main():
    parser = argparse.ArgumentParser(description="DreamCompiler Advanced Test Manager")
    parser.add_argument("--pattern", default="**/*.dr", help="Test file pattern")
    parser.add_argument("--categories", nargs="+", help="Test categories to run")
    parser.add_argument("--debug", action="store_true", help="Enable debug logging")
    parser.add_argument("--list-tests", action="store_true", help="List discovered tests")
    parser.add_argument("--list-categories", action="store_true", help="List available categories")
    parser.add_argument("--config", action="store_true", help="Show current configuration")
    
    args = parser.parse_args()
    
    manager = TestManager(debug=args.debug)
    
    if args.config:
        print(json.dumps(manager.config, indent=2))
        return
        
    if args.list_categories:
        print("Available test categories:")
        for cat in TestCategory:
            print(f"  {cat.value}")
        return
        
    if args.list_tests:
        tests = manager.discover_tests(args.pattern)
        print(f"Discovered tests ({len(tests)}):")
        for test in tests:
            category = manager.categorize_test(test)
            try:
                display_path = test.relative_to(TEST_DIR)
            except ValueError:
                display_path = test
            print(f"  {display_path} [{category.value}]")
        return
        
    # Run tests
    suite = manager.run_test_suite(args.pattern, args.categories)
    
    # Exit with error code if tests failed
    if suite.failed > 0 or suite.errors > 0:
        sys.exit(1)

if __name__ == "__main__":
    main()