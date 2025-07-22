package dream_test

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"testing"
	"time"
)

// CompilerTestCase represents a single compiler test case
type CompilerTestCase struct {
	Name           string
	Source         string
	ExpectSuccess  bool
	ExpectedOutput string
	ExpectedErrors []string
	Timeout        time.Duration
	Category       string
}

// TestRunner handles execution of compiler tests
type TestRunner struct {
	compilerPath   string
	tempDir        string
	runtimeLibPath string
}

// NewTestRunner creates a new test runner
func NewTestRunner(t *testing.T) *TestRunner {
	// Find the compiler executable
	projectRoot, err := findProjectRoot()
	if err != nil {
		t.Fatalf("Failed to find project root: %v", err)
	}

	compilerPath := filepath.Join(projectRoot, "zig-out", "bin", "DreamCompiler")
	if runtime.GOOS == "windows" {
		compilerPath += ".exe"
	}

	// Check if compiler exists
	if _, err := os.Stat(compilerPath); os.IsNotExist(err) {
		t.Fatalf("Compiler not found at %s. Run 'zig build' first.", compilerPath)
	}

	// Create temp directory for test files
	tempDir, err := os.MkdirTemp("", "dream_test_*")
	if err != nil {
		t.Fatalf("Failed to create temp directory: %v", err)
	}

	runtimeLibPath := filepath.Join(projectRoot, "src", "runtime")

	return &TestRunner{
		compilerPath:   compilerPath,
		tempDir:        tempDir,
		runtimeLibPath: runtimeLibPath,
	}
}

// Cleanup removes temporary files
func (tr *TestRunner) Cleanup() {
	os.RemoveAll(tr.tempDir)
}

// RunTest executes a single test case
func (tr *TestRunner) RunTest(t *testing.T, tc CompilerTestCase) {
	// Create test file
	testFile := filepath.Join(tr.tempDir, fmt.Sprintf("test_%s.dr", tc.Name))
	err := os.WriteFile(testFile, []byte(tc.Source), 0644)
	if err != nil {
		t.Fatalf("Failed to write test file: %v", err)
	}

	// Run compiler
	cmd := exec.Command(tr.compilerPath, testFile)
	output, err := cmd.CombinedOutput()
	
	if tc.ExpectSuccess {
		if err != nil {
			t.Errorf("Test %s: Expected success but got error: %v\nOutput: %s", tc.Name, err, string(output))
			return
		}
		
		// Check expected output if specified
		if tc.ExpectedOutput != "" && !strings.Contains(string(output), tc.ExpectedOutput) {
			t.Errorf("Test %s: Expected output '%s' not found in: %s", tc.Name, tc.ExpectedOutput, string(output))
		}
	} else {
		if err == nil {
			t.Errorf("Test %s: Expected failure but compilation succeeded\nOutput: %s", tc.Name, string(output))
			return
		}
		
		// Check expected error messages
		for _, expectedErr := range tc.ExpectedErrors {
			if !strings.Contains(string(output), expectedErr) {
				t.Errorf("Test %s: Expected error '%s' not found in: %s", tc.Name, expectedErr, string(output))
			}
		}
	}
}

// findProjectRoot finds the Dream compiler project root
func findProjectRoot() (string, error) {
	dir, err := os.Getwd()
	if err != nil {
		return "", err
	}

	for {
		if _, err := os.Stat(filepath.Join(dir, "build.zig")); err == nil {
			return dir, nil
		}
		
		parent := filepath.Dir(dir)
		if parent == dir {
			break
		}
		dir = parent
	}

	return "", fmt.Errorf("project root not found (no build.zig)")
}

// =============================================================================
// LEXER AND PARSER TESTS
// =============================================================================

func TestCompiler_BasicLexing(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "basic_tokens",
			Source:        "int x = 42;",
			ExpectSuccess: true,
			Category:      "lexer",
		},
		{
			Name:          "string_literals",
			Source:        `string msg = "Hello, World!";`,
			ExpectSuccess: true,
			Category:      "lexer",
		},
		{
			Name:          "operators",
			Source:        "int result = 1 + 2 * 3 - 4 / 2;",
			ExpectSuccess: true,
			Category:      "lexer",
		},
		{
			Name:          "control_flow_keywords",
			Source:        "if (true) { return 42; }",
			ExpectSuccess: true,
			Category:      "lexer",
		},
		{
			Name:           "unterminated_string",
			Source:         `string bad = "unterminated string`,
			ExpectSuccess:  false,
			ExpectedErrors: []string{"unterminated"},
			Category:       "lexer",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_BasicParsing(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "variable_declaration",
			Source:        "int x = 42; float y = 3.14; bool flag = true;",
			ExpectSuccess: true,
			Category:      "parser",
		},
		{
			Name:          "function_declaration",
			Source:        "func int add(int a, int b) { return a + b; }",
			ExpectSuccess: true,
			Category:      "parser",
		},
		{
			Name:          "if_statement",
			Source:        "if (x > 0) { Console.WriteLine(\"positive\"); }",
			ExpectSuccess: true,
			Category:      "parser",
		},
		{
			Name:          "while_loop",
			Source:        "while (i < 10) { i = i + 1; }",
			ExpectSuccess: true,
			Category:      "parser",
		},
		{
			Name:          "for_loop",
			Source:        "for (int i = 0; i < 10; i++) { Console.WriteLine(i); }",
			ExpectSuccess: true,
			Category:      "parser",
		},
		{
			Name:           "syntax_error",
			Source:         "int x = ;",
			ExpectSuccess:  false,
			ExpectedErrors: []string{"syntax", "error"},
			Category:       "parser",
		},
		{
			Name:           "mismatched_braces",
			Source:         "if (true) { Console.WriteLine(\"test\");",
			ExpectSuccess:  false,
			ExpectedErrors: []string{"expected", "}"},
			Category:       "parser",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

// =============================================================================
// SEMANTIC ANALYSIS TESTS
// =============================================================================

func TestCompiler_SemanticAnalysis(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "valid_types",
			Source:        "int x = 42; string s = \"hello\"; bool b = true;",
			ExpectSuccess: true,
			Category:      "semantic",
		},
		{
			Name:          "function_call",
			Source:        "func int square(int n) { return n * n; } int result = square(5);",
			ExpectSuccess: true,
			Category:      "semantic",
		},
		{
			Name:           "type_mismatch",
			Source:         "int x = \"string\";",
			ExpectSuccess:  false,
			ExpectedErrors: []string{"type", "mismatch"},
			Category:       "semantic",
		},
		{
			Name:           "undefined_variable",
			Source:         "int x = undefinedVar;",
			ExpectSuccess:  false,
			ExpectedErrors: []string{"undefined", "variable"},
			Category:       "semantic",
		},
		{
			Name:           "undefined_function",
			Source:         "int result = undefinedFunc(42);",
			ExpectSuccess:  false,
			ExpectedErrors: []string{"undefined", "function"},
			Category:       "semantic",
		},
		{
			Name:           "duplicate_variable",
			Source:         "int x = 1; int x = 2;",
			ExpectSuccess:  false,
			ExpectedErrors: []string{"duplicate", "redefinition"},
			Category:       "semantic",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

// =============================================================================
// RECENT LANGUAGE FEATURES TESTS
// =============================================================================

func TestCompiler_RecentLanguageFeatures(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "string_concatenation",
			Source:        `string result = "Hello" + " " + "World";`,
			ExpectSuccess: true,
			Category:      "features",
		},
		{
			Name:          "mixed_string_concatenation",
			Source:        `string result = "Count: " + 42;`,
			ExpectSuccess: true,
			Category:      "features",
		},
		{
			Name:          "switch_statement",
			Source:        `switch (x) { case 1: Console.WriteLine("one"); break; case 2: Console.WriteLine("two"); break; default: Console.WriteLine("other"); }`,
			ExpectSuccess: true,
			Category:      "features",
		},
		{
			Name:          "switch_with_fallthrough",
			Source:        `switch (x) { case 1: Console.WriteLine("one or two"); case 2: Console.WriteLine("two"); break; default: Console.WriteLine("other"); }`,
			ExpectSuccess: true,
			Category:      "features",
		},
		{
			Name:          "enum_declaration",
			Source:        `enum Color { Red, Green, Blue }`,
			ExpectSuccess: true,
			Category:      "features",
		},
		{
			Name:          "enum_usage",
			Source:        `enum Color { Red, Green, Blue } Color c = Color.Red;`,
			ExpectSuccess: true,
			Category:      "features",
		},
		{
			Name:          "try_catch",
			Source:        `try { int x = riskyOperation(); } catch { Console.WriteLine("error"); }`,
			ExpectSuccess: true,
			Category:      "features",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

// =============================================================================
// CODE GENERATION TESTS
// =============================================================================

func TestCompiler_CodeGeneration(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "simple_program",
			Source:        `Console.WriteLine("Hello, World!");`,
			ExpectSuccess: true,
			Category:      "codegen",
		},
		{
			Name:          "arithmetic_expression",
			Source:        `int result = (1 + 2) * (3 - 4); Console.WriteLine(result);`,
			ExpectSuccess: true,
			Category:      "codegen",
		},
		{
			Name:          "function_with_parameters",
			Source:        `func int multiply(int a, int b) { return a * b; } Console.WriteLine(multiply(6, 7));`,
			ExpectSuccess: true,
			Category:      "codegen",
		},
		{
			Name:          "nested_function_calls",
			Source:        `func int square(int n) { return n * n; } func int add(int a, int b) { return a + b; } Console.WriteLine(add(square(3), square(4)));`,
			ExpectSuccess: true,
			Category:      "codegen",
		},
		{
			Name:          "control_flow_compilation",
			Source:        `for (int i = 1; i <= 5; i++) { if (i % 2 == 0) { Console.WriteLine("even"); } else { Console.WriteLine("odd"); } }`,
			ExpectSuccess: true,
			Category:      "codegen",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

// =============================================================================
// ERROR RECOVERY AND DIAGNOSTICS TESTS
// =============================================================================

func TestCompiler_ErrorRecovery(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:           "multiple_syntax_errors",
			Source:         `int x = ; float y = ;`,
			ExpectSuccess:  false,
			ExpectedErrors: []string{"syntax", "error"},
			Category:       "diagnostics",
		},
		{
			Name:           "cascading_semantic_errors",
			Source:         `int x = undefinedVar; int y = x + anotherUndefinedVar;`,
			ExpectSuccess:  false,
			ExpectedErrors: []string{"undefined"},
			Category:       "diagnostics",
		},
		{
			Name:           "helpful_error_messages",
			Source:         `int add(int a, int b) { return a + b; }`, // Missing func keyword
			ExpectSuccess:  false,
			ExpectedErrors: []string{"expected", "func"},
			Category:       "diagnostics",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

// =============================================================================
// PERFORMANCE AND SCALABILITY TESTS
// =============================================================================

func TestCompiler_Performance(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping performance tests in short mode")
	}

	tr := NewTestRunner(t)
	defer tr.Cleanup()

	// Generate a larger program to test scalability
	var largeProgram strings.Builder
	largeProgram.WriteString("// Large program test\n")
	
	// Generate many variable declarations
	for i := 0; i < 100; i++ {
		largeProgram.WriteString(fmt.Sprintf("int var%d = %d;\n", i, i))
	}
	
	// Generate many function declarations
	for i := 0; i < 50; i++ {
		largeProgram.WriteString(fmt.Sprintf("func int func%d(int x) { return x + %d; }\n", i, i))
	}
	
	// Generate a main function that uses everything
	largeProgram.WriteString("func int main() {\n")
	largeProgram.WriteString("  int total = 0;\n")
	for i := 0; i < 50; i++ {
		largeProgram.WriteString(fmt.Sprintf("  total = total + func%d(var%d);\n", i, i))
	}
	largeProgram.WriteString("  return total;\n")
	largeProgram.WriteString("}\n")

	tests := []CompilerTestCase{
		{
			Name:          "large_program",
			Source:        largeProgram.String(),
			ExpectSuccess: true,
			Category:      "performance",
			Timeout:       30 * time.Second,
		},
		{
			Name:          "deeply_nested_expressions",
			Source:        generateDeeplyNestedExpression(20),
			ExpectSuccess: true,
			Category:      "performance",
		},
		{
			Name:          "many_string_concatenations",
			Source:        generateManyStringConcatenations(50),
			ExpectSuccess: true,
			Category:      "performance",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			start := time.Now()
			tr.RunTest(t, tc)
			duration := time.Since(start)
			
			t.Logf("Test %s completed in %v", tc.Name, duration)
			
			// Warn if compilation takes too long
			if duration > 10*time.Second {
				t.Logf("WARNING: Test %s took %v, which may indicate performance issues", tc.Name, duration)
			}
		})
	}
}

// generateDeeplyNestedExpression creates a deeply nested arithmetic expression
func generateDeeplyNestedExpression(depth int) string {
	if depth <= 1 {
		return "Console.WriteLine(1);"
	}
	return fmt.Sprintf("Console.WriteLine((%s + %s));", 
		generateNestedArithmetic(depth-1), 
		generateNestedArithmetic(depth-1))
}

func generateNestedArithmetic(depth int) string {
	if depth <= 1 {
		return "1"
	}
	return fmt.Sprintf("(%s + %s)", 
		generateNestedArithmetic(depth-1), 
		generateNestedArithmetic(depth-1))
}

// generateManyStringConcatenations creates a program with many string concatenations
func generateManyStringConcatenations(count int) string {
	var result strings.Builder
	result.WriteString(`string result = "start"`)
	
	for i := 0; i < count; i++ {
		result.WriteString(fmt.Sprintf(` + "part%d"`, i))
	}
	
	result.WriteString("; Console.WriteLine(result);")
	return result.String()
}

// =============================================================================
// CROSS-FEATURE INTEGRATION TESTS
// =============================================================================

func TestCompiler_FeatureIntegration(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name: "functions_with_string_concat",
			Source: `
				func string greet(string name) {
					return "Hello, " + name + "!";
				}
				Console.WriteLine(greet("World"));
			`,
			ExpectSuccess: true,
			Category:      "integration",
		},
		{
			Name: "switch_with_enums",
			Source: `
				enum Status { Success, Error, Pending }
				Status s = Status.Success;
				switch (s) {
					case Status.Success: Console.WriteLine("OK"); break;
					case Status.Error: Console.WriteLine("Failed"); break;
					default: Console.WriteLine("Unknown");
				}
			`,
			ExpectSuccess: true,
			Category:      "integration",
		},
		{
			Name: "loops_with_functions",
			Source: `
				func bool isEven(int n) { return n % 2 == 0; }
				for (int i = 1; i <= 10; i++) {
					if (isEven(i)) {
						Console.WriteLine("Even: " + i);
					}
				}
			`,
			ExpectSuccess: true,
			Category:      "integration",
		},
		{
			Name: "complex_program",
			Source: `
				enum Color { Red, Green, Blue }
				
				func string colorName(Color c) {
					switch (c) {
						case Color.Red: return "Red";
						case Color.Green: return "Green";
						case Color.Blue: return "Blue";
						default: return "Unknown";
					}
				}
				
				func void printColors() {
					for (int i = 0; i < 3; i++) {
						Color c = (Color)i;
						Console.WriteLine("Color " + i + ": " + colorName(c));
					}
				}
				
				printColors();
			`,
			ExpectSuccess: true,
			Category:      "integration",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}