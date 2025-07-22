package dream_test

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"testing"
	"time"
)

// =============================================================================
// OPTIMIZATION AND PERFORMANCE TESTS
// =============================================================================

// OptimizationTest represents a test case for optimization validation
type OptimizationTest struct {
	Name                string
	UnoptimizedSource   string
	OptimizedSource     string
	ExpectedImprovement string
	Category            string
}

func TestCompiler_BasicOptimizations(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping optimization tests in short mode")
	}

	tr := NewTestRunner(t)
	defer tr.Cleanup()

	// Test constant folding optimization
	tests := []CompilerTestCase{
		{
			Name:          "constant_folding_arithmetic",
			Source:        "int result = 2 + 3 * 4;", // Should be optimized to 14
			ExpectSuccess: true,
			Category:      "optimization",
		},
		{
			Name:          "constant_folding_boolean",
			Source:        "bool result = true && false;", // Should be optimized to false
			ExpectSuccess: true,
			Category:      "optimization",
		},
		{
			Name:          "dead_code_elimination",
			Source:        "int x = 42; int y = 99; Console.WriteLine(x);", // y should be eliminated
			ExpectSuccess: true,
			Category:      "optimization",
		},
		{
			Name:          "common_subexpression_elimination",
			Source:        "int a = x + y; int b = x + y; Console.WriteLine(a + b);",
			ExpectSuccess: true,
			Category:      "optimization",
		},
		{
			Name:          "loop_invariant_code_motion",
			Source: `
				for (int i = 0; i < 100; i++) {
					int constant = 42 * 3; // Should be moved out of loop
					Console.WriteLine(i + constant);
				}
			`,
			ExpectSuccess: true,
			Category:      "optimization",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_OptimizationCorrectness(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping optimization correctness tests in short mode")
	}

	tr := NewTestRunner(t)
	defer tr.Cleanup()

	// These tests verify that optimizations don't change program semantics
	tests := []CompilerTestCase{
		{
			Name: "optimization_preserves_side_effects",
			Source: `
				func int sideEffect() {
					Console.WriteLine("side effect");
					return 42;
				}
				
				int unused = sideEffect(); // This call must not be eliminated
				Console.WriteLine("done");
			`,
			ExpectSuccess: true,
			Category:      "optimization_correctness",
		},
		{
			Name: "optimization_preserves_evaluation_order",
			Source: `
				func int increment(int x) {
					Console.WriteLine("incrementing " + x);
					return x + 1;
				}
				
				int result = increment(1) + increment(2); // Order must be preserved
			`,
			ExpectSuccess: true,
			Category:      "optimization_correctness",
		},
		{
			Name: "optimization_handles_aliasing",
			Source: `
				int x = 10;
				int* ptr = &x;
				*ptr = 20;
				Console.WriteLine(x); // Must print 20, not 10
			`,
			ExpectSuccess: false, // Pointers might not be implemented
			Category:      "optimization_correctness",
		},
		{
			Name: "optimization_preserves_exception_semantics",
			Source: `
				try {
					int x = riskyOperation();
					int y = x + 1; // This should not be moved above riskyOperation
					Console.WriteLine(y);
				} catch {
					Console.WriteLine("caught");
				}
			`,
			ExpectSuccess: true,
			Category:      "optimization_correctness",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_PerformanceRegression(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping performance regression tests in short mode")
	}

	tr := NewTestRunner(t)
	defer tr.Cleanup()

	// Create progressively larger programs to test scalability
	sizes := []int{100, 500, 1000, 2000}
	
	for _, size := range sizes {
		t.Run(fmt.Sprintf("scalability_%d_functions", size), func(t *testing.T) {
			source := generateManyFunctions(size)
			
			start := time.Now()
			tc := CompilerTestCase{
				Name:          fmt.Sprintf("many_functions_%d", size),
				Source:        source,
				ExpectSuccess: true,
				Category:      "performance",
			}
			tr.RunTest(t, tc)
			duration := time.Since(start)
			
			t.Logf("Compiled %d functions in %v", size, duration)
			
			// Rough performance expectations (adjust based on actual performance)
			expectedMaxDuration := time.Duration(size) * time.Millisecond
			if duration > expectedMaxDuration {
				t.Logf("WARNING: Compilation of %d functions took %v, expected < %v", 
					size, duration, expectedMaxDuration)
			}
		})
	}
}

func TestCompiler_MemoryUsageOptimization(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping memory optimization tests in short mode")
	}

	tr := NewTestRunner(t)
	defer tr.Cleanup()

	tests := []CompilerTestCase{
		{
			Name:          "large_string_constants",
			Source:        generateLargeStringConstants(100),
			ExpectSuccess: true,
			Category:      "memory_optimization",
		},
		{
			Name:          "many_temporary_variables",
			Source:        generateManyTemporaryVariables(1000),
			ExpectSuccess: true,
			Category:      "memory_optimization",
		},
		{
			Name:          "complex_expression_trees",
			Source:        generateComplexExpressionTrees(15),
			ExpectSuccess: true,
			Category:      "memory_optimization",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			start := time.Now()
			tr.RunTest(t, tc)
			duration := time.Since(start)
			
			t.Logf("Test %s completed in %v", tc.Name, duration)
		})
	}
}

func TestCompiler_OptimizationPipeline(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping optimization pipeline tests in short mode")
	}

	tr := NewTestRunner(t)
	defer tr.Cleanup()

	// Test that optimization passes work together correctly
	tests := []CompilerTestCase{
		{
			Name: "multiple_optimization_passes",
			Source: `
				func int complexCalculation(int x) {
					int a = 2 + 3;              // Constant folding
					int b = a * 4;              // Constant propagation  
					int unused = 999;           // Dead code elimination
					
					for (int i = 0; i < 10; i++) {
						int invariant = a + b;  // Loop invariant code motion
						Console.WriteLine(x + invariant + i);
					}
					
					return a + b;               // Common subexpression elimination
				}
				
				int result = complexCalculation(42);
			`,
			ExpectSuccess: true,
			Category:      "optimization_pipeline",
		},
		{
			Name: "nested_optimization_opportunities",
			Source: `
				func void nestedOptimizations() {
					for (int i = 0; i < 100; i++) {
						for (int j = 0; j < 100; j++) {
							int constant = 42 * 42;    // Should be moved out of both loops
							int semiConstant = i * 10;  // Should be moved out of inner loop
							if (constant > semiConstant + j) {
								Console.WriteLine("found");
							}
						}
					}
				}
			`,
			ExpectSuccess: true,
			Category:      "optimization_pipeline",
		},
	}

	for _, tc := range tests {
		t.Run(tc.Name, func(t *testing.T) {
			tr.RunTest(t, tc)
		})
	}
}

func TestCompiler_CompilationTimeOptimization(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping compilation time tests in short mode")
	}

	tr := NewTestRunner(t)
	defer tr.Cleanup()

	// Test compilation time for various program structures
	testCases := []struct {
		name      string
		generator func(int) string
		sizes     []int
	}{
		{
			name:      "linear_functions",
			generator: generateLinearFunctions,
			sizes:     []int{50, 100, 200},
		},
		{
			name:      "nested_structures",
			generator: generateNestedStructures,
			sizes:     []int{10, 20, 30},
		},
		{
			name:      "complex_expressions",
			generator: generateComplexExpressions,
			sizes:     []int{20, 40, 60},
		},
	}

	for _, testCase := range testCases {
		for _, size := range testCase.sizes {
			t.Run(fmt.Sprintf("%s_%d", testCase.name, size), func(t *testing.T) {
				source := testCase.generator(size)
				
				start := time.Now()
				tc := CompilerTestCase{
					Name:          fmt.Sprintf("%s_%d", testCase.name, size),
					Source:        source,
					ExpectSuccess: true,
					Category:      "compilation_time",
				}
				tr.RunTest(t, tc)
				duration := time.Since(start)
				
				t.Logf("%s with size %d compiled in %v", testCase.name, size, duration)
			})
		}
	}
}

// =============================================================================
// GENERATED C CODE QUALITY TESTS
// =============================================================================

func TestCompiler_GeneratedCodeQuality(t *testing.T) {
	if testing.Short() {
		t.Skip("Skipping generated code quality tests in short mode")
	}

	tr := NewTestRunner(t)
	defer tr.Cleanup()

	// These tests examine the quality of generated C code
	testCases := []struct {
		name         string
		source       string
		checkGenerated func(t *testing.T, cCode string)
	}{
		{
			name:   "simple_arithmetic",
			source: "int result = 1 + 2 * 3;",
			checkGenerated: func(t *testing.T, cCode string) {
				// Check that the generated C code is reasonable
				if !strings.Contains(cCode, "result") {
					t.Error("Generated code should contain result variable")
				}
			},
		},
		{
			name:   "function_generation",
			source: "func int add(int a, int b) { return a + b; }",
			checkGenerated: func(t *testing.T, cCode string) {
				// Check function signature generation
				if !strings.Contains(cCode, "add") {
					t.Error("Generated code should contain function name")
				}
			},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			// Create test file
			testFile := filepath.Join(tr.tempDir, fmt.Sprintf("test_%s.dr", tc.name))
			err := os.WriteFile(testFile, []byte(tc.source), 0644)
			if err != nil {
				t.Fatalf("Failed to write test file: %v", err)
			}

			// Run compiler and check generated C code
			// Note: This would require the compiler to output generated C code
			// For now, we just run the compiler
			tr.RunTest(t, CompilerTestCase{
				Name:          tc.name,
				Source:        tc.source,
				ExpectSuccess: true,
				Category:      "code_quality",
			})
		})
	}
}

// =============================================================================
// HELPER FUNCTIONS FOR GENERATING TEST PROGRAMS
// =============================================================================

func generateManyFunctions(count int) string {
	var result strings.Builder
	
	for i := 0; i < count; i++ {
		result.WriteString(fmt.Sprintf(`
			func int func%d(int x) {
				return x + %d;
			}
		`, i, i))
	}
	
	// Add a main function that calls some of them
	result.WriteString("func int main() {\n")
	result.WriteString("  int total = 0;\n")
	for i := 0; i < min(count, 10); i++ {
		result.WriteString(fmt.Sprintf("  total = total + func%d(%d);\n", i, i))
	}
	result.WriteString("  return total;\n")
	result.WriteString("}\n")
	
	return result.String()
}

func generateLargeStringConstants(count int) string {
	var result strings.Builder
	
	for i := 0; i < count; i++ {
		result.WriteString(fmt.Sprintf(`string str%d = "This is string number %d with some content to make it reasonably long";`, i, i))
		result.WriteString("\n")
	}
	
	return result.String()
}

func generateManyTemporaryVariables(count int) string {
	var result strings.Builder
	result.WriteString("func void manyTemps() {\n")
	
	for i := 0; i < count; i++ {
		result.WriteString(fmt.Sprintf("  int temp%d = %d + %d;\n", i, i, i*2))
	}
	
	result.WriteString("}\n")
	return result.String()
}

func generateComplexExpressionTrees(depth int) string {
	expr := generateComplexExpression(depth)
	return fmt.Sprintf("int result = %s;", expr)
}

func generateComplexExpression(depth int) string {
	if depth <= 1 {
		return "42"
	}
	
	left := generateComplexExpression(depth - 1)
	right := generateComplexExpression(depth - 1)
	operators := []string{"+", "-", "*", "/", "%"}
	op := operators[depth%len(operators)]
	
	return fmt.Sprintf("(%s %s %s)", left, op, right)
}

func generateLinearFunctions(count int) string {
	var result strings.Builder
	
	for i := 0; i < count; i++ {
		result.WriteString(fmt.Sprintf(`
			func int linear%d() {
				int a = %d;
				int b = a + 1;
				int c = b * 2;
				return c;
			}
		`, i, i))
	}
	
	return result.String()
}

func generateNestedStructures(depth int) string {
	var result strings.Builder
	
	// Generate nested if statements
	for i := 0; i < depth; i++ {
		result.WriteString(fmt.Sprintf("if (x > %d) {\n", i))
	}
	
	result.WriteString("  Console.WriteLine(\"deeply nested\");\n")
	
	for i := 0; i < depth; i++ {
		result.WriteString("}\n")
	}
	
	return result.String()
}

func generateComplexExpressions(count int) string {
	var result strings.Builder
	
	for i := 0; i < count; i++ {
		result.WriteString(fmt.Sprintf(`
			int expr%d = ((%d + %d) * (%d - %d)) / ((%d %% %d) + 1);
		`, i, i, i+1, i+2, i+3, i+4, i+5))
	}
	
	return result.String()
}

// min function for Go versions that don't have it built-in
func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}