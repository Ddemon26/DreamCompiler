#!/usr/bin/env python3
"""
DreamCompiler Test Generator
Automatically generates comprehensive test cases for language features
"""

import argparse
import json
from pathlib import Path
from typing import Dict, List, Tuple
import re

# Configuration
ROOT = Path(__file__).resolve().parent.parent.parent
TEST_DIR = ROOT / "tests"
GRAMMAR_FILE = ROOT / "docs" / "grammar" / "Grammar.md"

class TestGenerator:
    """Generate comprehensive test cases for DreamCompiler"""
    
    def __init__(self):
        self.test_templates = self._load_test_templates()
        
    def _load_test_templates(self) -> Dict:
        """Load test case templates for different language features"""
        return {
            "arithmetic": {
                "basic": [
                    ("int x = 5 + 3;", "8"),
                    ("int x = 10 - 4;", "6"),
                    ("int x = 6 * 7;", "42"),
                    ("int x = 15 / 3;", "5"),
                    ("int x = 17 % 5;", "2"),
                ],
                "precedence": [
                    ("int x = 2 + 3 * 4;", "14"),
                    ("int x = (2 + 3) * 4;", "20"),
                    ("int x = 2 * 3 + 4;", "10"),
                    ("int x = 2 * (3 + 4);", "14"),
                ],
                "negative": [
                    ("int x = -5;", "-5"),
                    ("int x = -(-5);", "5"),
                    ("int x = 10 + (-3);", "7"),
                ]
            },
            "variables": {
                "declaration": [
                    ("int x = 42;", "42"),
                    ("float y = 3.14;", "3.14"),
                    ("bool z = true;", "true"),
                    ("string s = \"hello\";", "hello"),
                ],
                "inference": [
                    ("var x = 42;", "42"),
                    ("var y = 3.14;", "3.14"),
                    ("var z = true;", "true"),
                    ("var s = \"hello\";", "hello"),
                ],
                "multiple": [
                    ("int x = 1, y = 2;", "1\n2"),
                    ("var a = 10, b = 20;", "10\n20"),
                ]
            },
            "control_flow": {
                "if_statements": [
                    ("if (true) Console.WriteLine(\"yes\");", "yes"),
                    ("if (false) Console.WriteLine(\"no\"); else Console.WriteLine(\"yes\");", "yes"),
                    ("int x = 5; if (x > 3) Console.WriteLine(\"big\");", "big"),
                ],
                "loops": [
                    ("for (int i = 0; i < 3; i++) Console.WriteLine(i);", "0\n1\n2"),
                    ("int i = 0; while (i < 3) { Console.WriteLine(i); i++; }", "0\n1\n2"),
                ],
                "switch": [
                    ("int x = 2; switch (x) { case 1: Console.WriteLine(\"one\"); break; case 2: Console.WriteLine(\"two\"); break; }", "two"),
                ]
            },
            "functions": {
                "basic": [
                    ("int add(int a, int b) { return a + b; } Console.WriteLine(add(3, 4));", "7"),
                    ("void greet() { Console.WriteLine(\"hello\"); } greet();", "hello"),
                ],
                "recursion": [
                    ("int factorial(int n) { if (n <= 1) return 1; return n * factorial(n - 1); } Console.WriteLine(factorial(5));", "120"),
                ]
            },
            "data_structures": {
                "arrays": [
                    ("int[] arr = {1, 2, 3}; Console.WriteLine(arr[1]);", "2"),
                    ("string[] names = {\"alice\", \"bob\"}; Console.WriteLine(names[0]);", "alice"),
                ],
                "structs": [
                    ("struct Point { int x, y; } Point p = {3, 4}; Console.WriteLine(p.x);", "3"),
                ]
            },
            "error_cases": {
                "syntax_errors": [
                    ("int x = ;", "compile_error"),
                    ("if (true Console.WriteLine(\"test\");", "compile_error"),
                    ("int x = 5 +;", "compile_error"),
                ],
                "type_errors": [
                    ("int x = \"string\";", "type_error"),
                    ("bool b = 42;", "type_error"),
                ],
                "runtime_errors": [
                    ("int[] arr = {1, 2}; Console.WriteLine(arr[5]);", "runtime_error"),
                    ("int x = 5 / 0;", "runtime_error"),
                ]
            }
        }
        
    def generate_basic_tests(self, category: str, subcategory: str = None) -> List[Tuple[str, str, str]]:
        """Generate basic test cases for a category"""
        tests = []
        
        if category not in self.test_templates:
            return tests
            
        if subcategory:
            if subcategory in self.test_templates[category]:
                template_list = self.test_templates[category][subcategory]
            else:
                return tests
        else:
            template_list = []
            for subcat in self.test_templates[category].values():
                template_list.extend(subcat)
                
        for i, (code, expected) in enumerate(template_list):
            test_name = f"{category}_{subcategory}_{i:03d}" if subcategory else f"{category}_{i:03d}"
            full_code = f"{code}\nConsole.WriteLine({code.split('=')[0].strip().split()[-1]});" if '=' in code and 'Console.WriteLine' not in code else code
            tests.append((test_name, full_code, expected))
            
        return tests
        
    def generate_edge_case_tests(self, category: str) -> List[Tuple[str, str, str]]:
        """Generate edge case tests for a category"""
        edge_cases = {
            "arithmetic": [
                ("int x = 2147483647; Console.WriteLine(x);", "2147483647"),  # INT_MAX
                ("int x = -2147483648; Console.WriteLine(x);", "-2147483648"),  # INT_MIN
                ("float x = 0.0; Console.WriteLine(x);", "0"),
                ("int x = 0 * 1000000; Console.WriteLine(x);", "0"),
            ],
            "strings": [
                ("string s = \"\"; Console.WriteLine(s.length);", "0"),
                ("string s = \"a\\nb\\tc\"; Console.WriteLine(s);", "a\nb\tc"),
                ("string s = \"\\\"quoted\\\"\"; Console.WriteLine(s);", "\"quoted\""),
            ],
            "control_flow": [
                ("for (int i = 0; i < 0; i++) Console.WriteLine(i);", ""),  # No output
                ("int x = 0; while (false) { x++; } Console.WriteLine(x);", "0"),
            ]
        }
        
        tests = []
        if category in edge_cases:
            for i, (code, expected) in enumerate(edge_cases[category]):
                test_name = f"{category}_edge_{i:03d}"
                tests.append((test_name, code, expected))
                
        return tests
        
    def generate_cross_platform_tests(self) -> List[Tuple[str, str, str]]:
        """Generate tests that verify cross-platform compatibility"""
        tests = [
            ("path_separator", 
             "string path = \"dir/file.txt\"; Console.WriteLine(path);", 
             "dir/file.txt"),
            ("line_endings", 
             "Console.WriteLine(\"line1\"); Console.WriteLine(\"line2\");", 
             "line1\nline2"),
            ("unicode_basic", 
             "string s = \"café\"; Console.WriteLine(s);", 
             "café"),
            ("large_numbers", 
             "long x = 9223372036854775807; Console.WriteLine(x);", 
             "9223372036854775807"),
        ]
        
        return tests
        
    def generate_performance_tests(self) -> List[Tuple[str, str, str]]:
        """Generate performance benchmark tests"""
        tests = [
            ("loop_performance", 
             "int sum = 0; for (int i = 0; i < 10000; i++) sum += i; Console.WriteLine(sum);", 
             "49995000"),
            ("recursion_depth", 
             "int countdown(int n) { if (n <= 0) return 0; return n + countdown(n-1); } Console.WriteLine(countdown(100));", 
             "5050"),
            ("string_concat", 
             "string s = \"\"; for (int i = 0; i < 100; i++) s += \"a\"; Console.WriteLine(s.length);", 
             "100"),
        ]
        
        return tests
        
    def generate_regression_tests(self) -> List[Tuple[str, str, str]]:
        """Generate regression tests for known issues"""
        tests = [
            ("struct_parsing_windows", 
             "struct Point { int x; int y; } Point p; Console.WriteLine(\"ok\");", 
             "ok"),
            ("semicolon_handling", 
             "int x = 5;; Console.WriteLine(x);", 
             "5"),
            ("comment_parsing", 
             "int x = 5; /* comment */ Console.WriteLine(x);", 
             "5"),
        ]
        
        return tests
        
    def write_test_file(self, test_name: str, code: str, expected: str, category_path: Path):
        """Write a single test file"""
        category_path.mkdir(parents=True, exist_ok=True)
        
        test_file = category_path / f"{test_name}.dr"
        
        # Format the test file content
        content = f"// Expected: {expected}\n"
        if expected in ["compile_error", "type_error", "runtime_error"]:
            content += f"// This test should fail during compilation or runtime\n"
        content += f"{code}\n"
        
        test_file.write_text(content, encoding='utf-8')
        return test_file
        
    def generate_test_suite(self, output_dir: Path = None):
        """Generate comprehensive test suite"""
        if output_dir is None:
            output_dir = TEST_DIR / "generated"
            
        print(f"Generating test suite in {output_dir}")
        
        # Generate basic tests
        for category in self.test_templates:
            category_dir = output_dir / category
            
            if category == "error_cases":
                # Handle error cases separately
                for subcategory in self.test_templates[category]:
                    subcat_dir = category_dir / subcategory
                    tests = self.generate_basic_tests(category, subcategory)
                    
                    for test_name, code, expected in tests:
                        self.write_test_file(test_name, code, expected, subcat_dir)
            else:
                # Regular test categories
                tests = self.generate_basic_tests(category)
                for test_name, code, expected in tests:
                    self.write_test_file(test_name, code, expected, category_dir)
                    
                # Add edge cases
                edge_tests = self.generate_edge_case_tests(category)
                for test_name, code, expected in edge_tests:
                    self.write_test_file(test_name, code, expected, category_dir / "edge_cases")
                    
        # Generate special test categories
        cross_platform_tests = self.generate_cross_platform_tests()
        for test_name, code, expected in cross_platform_tests:
            self.write_test_file(test_name, code, expected, output_dir / "cross_platform")
            
        performance_tests = self.generate_performance_tests()
        for test_name, code, expected in performance_tests:
            self.write_test_file(test_name, code, expected, output_dir / "performance")
            
        regression_tests = self.generate_regression_tests()
        for test_name, code, expected in regression_tests:
            self.write_test_file(test_name, code, expected, output_dir / "regression")
            
        print("Test suite generation complete!")
        
    def generate_test_matrix(self) -> Dict:
        """Generate test matrix for CI/CD"""
        matrix = {
            "platforms": ["linux", "windows"],
            "categories": [
                "arithmetic",
                "variables", 
                "control_flow",
                "functions",
                "data_structures",
                "cross_platform",
                "performance",
                "regression"
            ],
            "test_types": [
                "unit",
                "integration", 
                "edge_cases",
                "error_cases"
            ],
            "expected_results": {
                "linux": {
                    "total_tests": 150,
                    "expected_pass_rate": 95.0,
                    "known_failures": []
                },
                "windows": {
                    "total_tests": 150,
                    "expected_pass_rate": 85.0,
                    "known_failures": [
                        "data_structures/structs",
                        "regression/struct_parsing_windows"
                    ]
                }
            }
        }
        
        return matrix
        
    def validate_existing_tests(self) -> Dict:
        """Validate existing test files for completeness and correctness"""
        validation_results = {
            "valid_tests": [],
            "invalid_tests": [],
            "missing_expected": [],
            "syntax_issues": []
        }
        
        for test_file in TEST_DIR.rglob("*.dr"):
            try:
                content = test_file.read_text(encoding='utf-8')
                
                # Check for expected output
                expected_matches = re.findall(r"//\s*Expected:\s*(.*)", content)
                if not expected_matches:
                    validation_results["missing_expected"].append(str(test_file.relative_to(ROOT)))
                    continue
                    
                # Basic syntax validation
                if self._validate_syntax(content):
                    validation_results["valid_tests"].append(str(test_file.relative_to(ROOT)))
                else:
                    validation_results["syntax_issues"].append(str(test_file.relative_to(ROOT)))
                    
            except Exception as e:
                validation_results["invalid_tests"].append({
                    "file": str(test_file.relative_to(ROOT)),
                    "error": str(e)
                })
                
        return validation_results
        
    def _validate_syntax(self, content: str) -> bool:
        """Basic syntax validation for test content"""
        # Remove comments and expected output lines
        lines = [line for line in content.split('\n') 
                if not line.strip().startswith('//')]
        code = '\n'.join(lines).strip()
        
        if not code:
            return False
            
        # Basic checks
        open_braces = code.count('{')
        close_braces = code.count('}')
        if open_braces != close_braces:
            return False
            
        open_parens = code.count('(')
        close_parens = code.count(')')
        if open_parens != close_parens:
            return False
            
        return True

def main():
    parser = argparse.ArgumentParser(description="DreamCompiler Test Generator")
    parser.add_argument("--generate", action="store_true", help="Generate new test suite")
    parser.add_argument("--validate", action="store_true", help="Validate existing tests")
    parser.add_argument("--matrix", action="store_true", help="Generate test matrix")
    parser.add_argument("--output", help="Output directory for generated tests")
    parser.add_argument("--category", help="Generate tests for specific category only")
    
    args = parser.parse_args()
    
    generator = TestGenerator()
    
    if args.validate:
        print("Validating existing tests...")
        results = generator.validate_existing_tests()
        
        print(f"Valid tests: {len(results['valid_tests'])}")
        print(f"Invalid tests: {len(results['invalid_tests'])}")
        print(f"Missing expected output: {len(results['missing_expected'])}")
        print(f"Syntax issues: {len(results['syntax_issues'])}")
        
        if results['invalid_tests']:
            print("\nInvalid tests:")
            for test in results['invalid_tests']:
                print(f"  {test['file']}: {test['error']}")
                
        if results['missing_expected']:
            print("\nTests missing expected output:")
            for test in results['missing_expected']:
                print(f"  {test}")
                
    if args.matrix:
        matrix = generator.generate_test_matrix()
        print(json.dumps(matrix, indent=2))
        
    if args.generate:
        output_dir = Path(args.output) if args.output else None
        generator.generate_test_suite(output_dir)

if __name__ == "__main__":
    main()