package dream_test

import (
	"fmt"
	"strings"
	"testing"
)

// =============================================================================
// EDGE CASES AND BOUNDARY CONDITION TESTS
// =============================================================================

func TestCompiler_LexerEdgeCases(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "empty_program",
			Source:        "",
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "whitespace_only",
			Source:        "   \n\t  \n  ",
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "comments_only",
			Source:        "// This is a comment\n/* Multi-line comment */",
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "unicode_identifiers",
			Source:        "int café = 42; // Unicode in identifier",
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "very_long_identifier",
			Source:        fmt.Sprintf("int %s = 42;", strings.Repeat("a", 1000)),
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "very_long_string_literal",
			Source:        fmt.Sprintf("string s = \"%s\";", strings.Repeat("x", 10000)),
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "escaped_characters",
			Source:        `string s = "Hello\nWorld\t\"Quote\"\\Backslash";`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "numeric_edge_cases",
			Source:        "int maxInt = 2147483647; int minInt = -2147483648; float pi = 3.14159265359;",
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_ParserEdgeCases(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "deeply_nested_blocks",
			Source:        generateDeeplyNestedBlocks(20),
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "many_parameters",
			Source:        generateFunctionWithManyParameters(50),
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "complex_expression_precedence",
			Source:        "int result = 1 + 2 * 3 - 4 / 2 % 3 + (5 - 6) * 7;",
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "nested_function_calls",
			Source:        generateNestedFunctionCalls(10),
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "chained_member_access",
			Source:        "obj.field1.field2.field3.method().property;",
			ExpectSuccess: false, // This should fail as it's not implemented
			Category:      "edge_cases",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_SemanticEdgeCases(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name: "recursive_function",
			Source: `
				func int factorial(int n) {
					if (n <= 1) return 1;
					return n * factorial(n - 1);
				}
				int result = factorial(5);
			`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name: "mutual_recursion",
			Source: `
				func bool isEven(int n);
				func bool isOdd(int n) {
					if (n == 0) return false;
					return isEven(n - 1);
				}
				func bool isEven(int n) {
					if (n == 0) return true;
					return isOdd(n - 1);
				}
			`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:           "circular_type_dependency",
			Source:         "struct A { B b; } struct B { A a; }",
			ExpectSuccess:  false,
			ExpectedErrors: []string{"circular", "dependency"},
			Category:       "edge_cases",
		},
		{
			Name: "shadowing_variables",
			Source: `
				int x = 1;
				{
					int x = 2;
					{
						int x = 3;
						Console.WriteLine(x); // Should print 3
					}
					Console.WriteLine(x); // Should print 2
				}
				Console.WriteLine(x); // Should print 1
			`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name: "function_overloading",
			Source: `
				func int add(int a, int b) { return a + b; }
				func float add(float a, float b) { return a + b; }
				int intResult = add(1, 2);
				float floatResult = add(1.5, 2.5);
			`,
			ExpectSuccess: false, // Function overloading might not be implemented
			Category:      "edge_cases",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_StringOperationEdgeCases(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "empty_string_concatenation",
			Source:        `string result = "" + "" + "";`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "null_string_handling",
			Source:        `string s = null; string result = s + "test";`,
			ExpectSuccess: false, // Should handle null gracefully
			Category:      "edge_cases",
		},
		{
			Name:          "very_large_string_concatenation",
			Source:        generateLargeStringConcatenation(1000),
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "mixed_type_complex_concatenation",
			Source:        `string result = "Number: " + 42 + ", Float: " + 3.14 + ", Bool: " + true;`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "nested_string_operations",
			Source:        `string result = ("a" + "b") + ("c" + ("d" + "e"));`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_ControlFlowEdgeCases(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name: "empty_switch",
			Source: `
				switch (x) {
				}
			`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name: "switch_without_default",
			Source: `
				switch (x) {
					case 1: Console.WriteLine("one"); break;
					case 2: Console.WriteLine("two"); break;
				}
			`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name: "switch_with_complex_fallthrough",
			Source: `
				switch (x) {
					case 1:
					case 2:
					case 3:
						Console.WriteLine("1, 2, or 3");
						break;
					case 4:
						Console.WriteLine("4");
						// Fallthrough to default
					default:
						Console.WriteLine("default");
				}
			`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name: "nested_loops_with_breaks",
			Source: `
				for (int i = 0; i < 10; i++) {
					for (int j = 0; j < 10; j++) {
						if (i * j > 20) {
							break; // Only breaks inner loop
						}
						Console.WriteLine(i + "," + j);
					}
				}
			`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name: "infinite_loop_with_break",
			Source: `
				while (true) {
					Console.WriteLine("once");
					break;
				}
			`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name: "do_while_edge_case",
			Source: `
				int i = 10;
				do {
					Console.WriteLine(i);
					i++;
				} while (i < 10); // Body executes once even though condition is false
			`,
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_EnumEdgeCases(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "empty_enum",
			Source:        "enum Empty { }",
			ExpectSuccess: false, // Should require at least one value
			Category:      "edge_cases",
		},
		{
			Name:          "single_value_enum",
			Source:        "enum Single { OnlyValue }",
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "enum_with_explicit_values",
			Source:        "enum Status { Success = 0, Warning = 1, Error = 2 }",
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "enum_with_mixed_explicit_implicit",
			Source:        "enum Mixed { First, Second = 5, Third, Fourth = 10 }",
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:           "enum_duplicate_names",
			Source:         "enum Bad { Value, Value }",
			ExpectSuccess:  false,
			ExpectedErrors: []string{"duplicate", "redefinition"},
			Category:       "edge_cases",
		},
		{
			Name:           "enum_duplicate_values",
			Source:         "enum Bad { First = 1, Second = 1 }",
			ExpectSuccess:  false,
			ExpectedErrors: []string{"duplicate", "value"},
			Category:       "edge_cases",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_MemoryAndPerformanceEdgeCases(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping memory/performance tests in short mode")
	}

	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "large_array_declaration",
			Source:        generateLargeArrayDeclaration(10000),
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "many_local_variables",
			Source:        generateManyLocalVariables(1000),
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
		{
			Name:          "deep_call_stack",
			Source:        generateDeepCallStack(100),
			ExpectSuccess: true,
			Category:      "edge_cases",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

// =============================================================================
// HELPER FUNCTIONS FOR GENERATING TEST CASES
// =============================================================================

func generateDeeplyNestedBlocks(depth int) string {
	var result strings.Builder
	
	// Open blocks
	for i := 0; i < depth; i++ {
		result.WriteString("{\n")
		result.WriteString(fmt.Sprintf("  int var%d = %d;\n", i, i))
	}
	
	// Close blocks
	for i := 0; i < depth; i++ {
		result.WriteString("}\n")
	}
	
	return result.String()
}

func generateFunctionWithManyParameters(count int) string {
	var params strings.Builder
	var args strings.Builder
	
	for i := 0; i < count; i++ {
		if i > 0 {
			params.WriteString(", ")
			args.WriteString(", ")
		}
		params.WriteString(fmt.Sprintf("int p%d", i))
		args.WriteString(fmt.Sprintf("%d", i))
	}
	
	return fmt.Sprintf(
		"func int manyParams(%s) { return p0; } int result = manyParams(%s);",
		params.String(),
		args.String(),
	)
}

func generateNestedFunctionCalls(depth int) string {
	if depth <= 1 {
		return "func int base() { return 42; } int result = base();"
	}
	
	var result strings.Builder
	result.WriteString("func int base() { return 42; }\n")
	
	for i := 1; i < depth; i++ {
		if i == 1 {
			result.WriteString(fmt.Sprintf("func int level%d() { return base(); }\n", i))
		} else {
			result.WriteString(fmt.Sprintf("func int level%d() { return level%d(); }\n", i, i-1))
		}
	}
	
	result.WriteString(fmt.Sprintf("int result = level%d();", depth-1))
	return result.String()
}

func generateLargeStringConcatenation(parts int) string {
	var result strings.Builder
	result.WriteString("string result = \"start\"")
	
	for i := 0; i < parts; i++ {
		result.WriteString(fmt.Sprintf(" + \"part%d\"", i))
	}
	
	result.WriteString(";")
	return result.String()
}

func generateLargeArrayDeclaration(size int) string {
	return fmt.Sprintf("int arr[%d];", size)
}

func generateManyLocalVariables(count int) string {
	var result strings.Builder
	result.WriteString("func void manyVars() {\n")
	
	for i := 0; i < count; i++ {
		result.WriteString(fmt.Sprintf("  int var%d = %d;\n", i, i))
	}
	
	result.WriteString("}\n")
	return result.String()
}

func generateDeepCallStack(depth int) string {
	var result strings.Builder
	
	// Generate recursive function
	result.WriteString(`
		func int deepRecursion(int n) {
			if (n <= 0) return 0;
			return 1 + deepRecursion(n - 1);
		}
	`)
	
	result.WriteString(fmt.Sprintf("int result = deepRecursion(%d);", depth))
	return result.String()
}

// =============================================================================
// REGRESSION TESTS FOR KNOWN ISSUES
// =============================================================================

func TestCompiler_RegressionTests(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name: "struct_followed_by_variable_decl",
			Source: `
				struct Point { int x; int y; }
				Point p;  // This was known to fail on Windows
			`,
			ExpectSuccess: true,
			Category:      "regression",
		},
		{
			Name: "string_array_concatenation",
			Source: `
				string[] words = {"Hello", "World"};
				string result = words[0] + " " + words[1];
			`,
			ExpectSuccess: true,
			Category:      "regression",
		},
		{
			Name: "switch_with_multiple_statements",
			Source: `
				switch (x) {
					case 1:
						Console.WriteLine("first");
						Console.WriteLine("statement");
						break;
					case 2:
						int temp = 42;
						Console.WriteLine(temp);
						break;
				}
			`,
			ExpectSuccess: true,
			Category:      "regression",
		},
		{
			Name: "nested_scope_variable_shadowing",
			Source: `
				int x = 1;
				if (true) {
					int x = 2;  // Should not conflict with outer x
					Console.WriteLine(x);
				}
			`,
			ExpectSuccess: true,
			Category:      "regression",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}