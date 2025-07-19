#!/usr/bin/env python3
"""
DreamCompiler Universal Test CLI
Cross-platform test management interface for CLI agents
"""

import argparse
import json
import os
import platform
import subprocess
import sys
from pathlib import Path

# Add codex/python to path for imports
ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "codex" / "python"))

try:
    from test_manager import TestManager, TestCategory
    from test_analyzer import TestAnalyzer
    from test_generator import TestGenerator
except ImportError as e:
    print(f"Error importing test modules: {e}")
    print("Please ensure codex/python directory contains the test modules")
    sys.exit(1)

class UniversalTestCLI:
    """Universal CLI interface for DreamCompiler testing"""
    
    def __init__(self):
        self.platform = platform.system()
        self.is_windows = self.platform == "Windows"
        self.root = ROOT
        
    def run_command(self, cmd: str, shell: bool = True) -> int:
        """Execute platform-appropriate command"""
        if self.is_windows:
            # Use PowerShell for Windows
            if shell and not cmd.startswith("powershell"):
                cmd = f"powershell -Command \"{cmd}\""
        
        print(f"Executing: {cmd}")
        return os.system(cmd)
        
    def build_compiler(self) -> bool:
        """Build the DreamCompiler"""
        print("Building DreamCompiler...")
        
        if self.is_windows:
            result = self.run_command("zig build")
        else:
            result = self.run_command("zig build")
            
        return result == 0
        
    def run_tests(self, args) -> int:
        """Run tests with specified parameters"""
        manager = TestManager(debug=args.debug)
        
        try:
            suite = manager.run_test_suite(
                pattern=args.pattern,
                categories=args.categories
            )
            
            # Return appropriate exit code
            if suite.failed > 0 or suite.errors > 0:
                return 1
            return 0
            
        except Exception as e:
            print(f"Error running tests: {e}")
            return 1
            
    def analyze_results(self, args) -> int:
        """Analyze test results"""
        analyzer = TestAnalyzer()
        
        try:
            results = analyzer.load_results(args.pattern)
            
            if args.format == "text":
                report = analyzer.generate_summary_report(results)
                if args.output:
                    with open(args.output, 'w') as f:
                        f.write(report)
                    print(f"Report saved to {args.output}")
                else:
                    print(report)
                    
            elif args.format == "html":
                output_path = Path(args.output) if args.output else ROOT / "test_results" / "report.html"
                analyzer.generate_html_report(results, output_path)
                print(f"HTML report saved to {output_path}")
                
            elif args.format == "charts":
                analyzer.compare_platforms(results)
                
            return 0
            
        except Exception as e:
            print(f"Error analyzing results: {e}")
            return 1
            
    def generate_tests(self, args) -> int:
        """Generate test cases"""
        generator = TestGenerator()
        
        try:
            if args.validate:
                results = generator.validate_existing_tests()
                print(f"Validation complete:")
                print(f"  Valid: {len(results['valid_tests'])}")
                print(f"  Invalid: {len(results['invalid_tests'])}")
                print(f"  Missing expected: {len(results['missing_expected'])}")
                
            if args.matrix:
                matrix = generator.generate_test_matrix()
                print(json.dumps(matrix, indent=2))
                
            if args.generate:
                output_dir = Path(args.output) if args.output else None
                generator.generate_test_suite(output_dir)
                
            return 0
            
        except Exception as e:
            print(f"Error generating tests: {e}")
            return 1
            
    def quick_test(self, test_file: str = None) -> int:
        """Quick test execution for single file or basic suite"""
        if not self.build_compiler():
            print("Build failed!")
            return 1
            
        if test_file:
            # Test single file
            manager = TestManager()
            test_path = Path(test_file)
            if not test_path.exists():
                test_path = ROOT / "tests" / test_file
                
            if test_path.exists():
                result = manager.run_single_test(test_path)
                print(f"Result: {result.status.value}")
                return 0 if result.status.value == "PASS" else 1
            else:
                print(f"Test file not found: {test_file}")
                return 1
        else:
            # Run basic test suite
            return self.run_tests(argparse.Namespace(
                pattern="**/*.dr",
                categories=None,
                debug=False
            ))
            
    def list_tests(self, pattern: str = "**/*.dr") -> int:
        """List available tests"""
        manager = TestManager()
        tests = manager.discover_tests(pattern)
        
        print(f"Available tests ({len(tests)}):")
        for test in tests:
            category = manager.categorize_test(test)
            rel_path = test.relative_to(ROOT / "tests")
            print(f"  {rel_path} [{category.value}]")
            
        return 0
        
    def show_status(self) -> int:
        """Show current test environment status"""
        print("DreamCompiler Test Environment Status")
        print("=" * 40)
        print(f"Platform: {self.platform}")
        print(f"Root Directory: {self.root}")
        print(f"Python Version: {sys.version.split()[0]}")
        
        # Check for required tools
        tools = ["zig", "python"]
        if self.is_windows:
            tools.extend(["powershell"])
        else:
            tools.extend(["make", "gcc"])
            
        print("\nTool Availability:")
        for tool in tools:
            try:
                result = subprocess.run([tool, "--version"], 
                                      capture_output=True, text=True, timeout=5)
                if result.returncode == 0:
                    version = result.stdout.split('\n')[0]
                    print(f"  ✓ {tool}: {version}")
                else:
                    print(f"  ✗ {tool}: Not available")
            except:
                print(f"  ✗ {tool}: Not available")
                
        # Check test directories
        test_dir = self.root / "tests"
        if test_dir.exists():
            test_count = len(list(test_dir.rglob("*.dr")))
            print(f"\nTest Files: {test_count} found")
        else:
            print("\nTest Files: Test directory not found")
            
        # Check recent results
        results_dir = self.root / "test_results"
        if results_dir.exists():
            result_files = list(results_dir.glob("*.json"))
            print(f"Recent Results: {len(result_files)} result files")
        else:
            print("Recent Results: No results directory")
            
        return 0

def main():
    cli = UniversalTestCLI()
    
    parser = argparse.ArgumentParser(
        description="DreamCompiler Universal Test CLI",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Quick test run
  python test_cli.py quick
  
  # Run specific test
  python test_cli.py quick tests/basics/arithmetic/arithmetic.dr
  
  # Run full test suite
  python test_cli.py run
  
  # Run tests with filter
  python test_cli.py run --pattern "basics/**/*.dr"
  
  # Run specific categories
  python test_cli.py run --categories unit integration
  
  # Analyze results
  python test_cli.py analyze
  
  # Generate HTML report
  python test_cli.py analyze --format html --output report.html
  
  # List available tests
  python test_cli.py list
  
  # Show environment status
  python test_cli.py status
  
  # Generate new tests
  python test_cli.py generate --generate
  
  # Validate existing tests
  python test_cli.py generate --validate
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Quick command
    quick_parser = subparsers.add_parser("quick", help="Quick test execution")
    quick_parser.add_argument("file", nargs="?", help="Specific test file to run")
    
    # Run command
    run_parser = subparsers.add_parser("run", help="Run test suite")
    run_parser.add_argument("--pattern", default="**/*.dr", help="Test file pattern")
    run_parser.add_argument("--categories", nargs="+", help="Test categories to run")
    run_parser.add_argument("--debug", action="store_true", help="Enable debug output")
    
    # Analyze command
    analyze_parser = subparsers.add_parser("analyze", help="Analyze test results")
    analyze_parser.add_argument("--pattern", default="*.json", help="Result file pattern")
    analyze_parser.add_argument("--format", choices=["text", "html", "charts"], 
                               default="text", help="Output format")
    analyze_parser.add_argument("--output", help="Output file path")
    analyze_parser.add_argument("--days", type=int, help="Analyze last N days")
    
    # Generate command
    generate_parser = subparsers.add_parser("generate", help="Generate or validate tests")
    generate_parser.add_argument("--generate", action="store_true", help="Generate new tests")
    generate_parser.add_argument("--validate", action="store_true", help="Validate existing tests")
    generate_parser.add_argument("--matrix", action="store_true", help="Show test matrix")
    generate_parser.add_argument("--output", help="Output directory")
    
    # List command
    list_parser = subparsers.add_parser("list", help="List available tests")
    list_parser.add_argument("--pattern", default="**/*.dr", help="Test file pattern")
    
    # Status command
    subparsers.add_parser("status", help="Show environment status")
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
        
    # Execute command
    if args.command == "quick":
        return cli.quick_test(args.file)
    elif args.command == "run":
        return cli.run_tests(args)
    elif args.command == "analyze":
        return cli.analyze_results(args)
    elif args.command == "generate":
        return cli.generate_tests(args)
    elif args.command == "list":
        return cli.list_tests(args.pattern)
    elif args.command == "status":
        return cli.show_status()
    else:
        print(f"Unknown command: {args.command}")
        return 1

if __name__ == "__main__":
    sys.exit(main())