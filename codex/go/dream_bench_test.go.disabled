package dream_test

/*
#cgo CFLAGS: -I. -I../src -std=c11 -D_GNU_SOURCE
#cgo LDFLAGS: -L. -ldream
#include "dream.h"
#include <stdlib.h>
*/
import "C"

import (
	"fmt"
	"strings"
	"testing"
	"unsafe"
)

// =============================================================================
// BENCHMARK TESTS - Performance Testing for Hot Paths
// =============================================================================

// BenchmarkLexer_SimpleTokens benchmarks basic tokenization performance
func BenchmarkLexer_SimpleTokens(b *testing.B) {
	source := "int x = 42 + 3 * 7;"
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		lexer := C.lexer_create(cSource)
		if lexer == nil {
			b.Fatal("Failed to create lexer")
		}

		// Tokenize the entire input
		for {
			token := C.lexer_next_token(lexer)
			if token.kind == C.TK_EOF {
				break
			}
		}

		C.lexer_destroy(lexer)
	}
}

// BenchmarkLexer_ComplexExpression benchmarks tokenization of complex expressions
func BenchmarkLexer_ComplexExpression(b *testing.B) {
	source := "if (x > 0 && y < 100) { result = (a + b) * (c - d) / (e % f); }"
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		lexer := C.lexer_create(cSource)
		if lexer == nil {
			b.Fatal("Failed to create lexer")
		}

		tokenCount := 0
		for {
			token := C.lexer_next_token(lexer)
			tokenCount++
			if token.kind == C.TK_EOF {
				break
			}
		}

		C.lexer_destroy(lexer)
	}
}

// BenchmarkLexer_StringLiterals benchmarks string tokenization
func BenchmarkLexer_StringLiterals(b *testing.B) {
	source := `Console.WriteLine("Hello, World! This is a longer string with some content.");`
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		lexer := C.lexer_create(cSource)
		if lexer == nil {
			b.Fatal("Failed to create lexer")
		}

		for {
			token := C.lexer_next_token(lexer)
			if token.kind == C.TK_EOF {
				break
			}
		}

		C.lexer_destroy(lexer)
	}
}

// BenchmarkParser_SimpleExpression benchmarks parsing of simple expressions
func BenchmarkParser_SimpleExpression(b *testing.B) {
	source := "1 + 2 * 3;"
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		parser := C.parser_create(cSource)
		if parser == nil {
			b.Fatal("Failed to create parser")
		}

		program := C.parser_parse_program(parser)
		if program == nil {
			b.Fatal("Failed to parse program")
		}

		C.parser_destroy(parser)
	}
}

// BenchmarkParser_VariableDeclaration benchmarks variable declaration parsing
func BenchmarkParser_VariableDeclaration(b *testing.B) {
	source := "int x = 42, y = 84, z = 126;"
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		parser := C.parser_create(cSource)
		if parser == nil {
			b.Fatal("Failed to create parser")
		}

		program := C.parser_parse_program(parser)
		if program == nil {
			b.Fatal("Failed to parse program")
		}

		C.parser_destroy(parser)
	}
}

// BenchmarkParser_ControlFlow benchmarks control flow parsing
func BenchmarkParser_ControlFlow(b *testing.B) {
	source := `
		if (x > 0) {
			while (y < 10) {
				y = y + 1;
			}
		} else {
			for (int i = 0; i < 5; i++) {
				Console.WriteLine(i);
			}
		}
	`
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		parser := C.parser_create(cSource)
		if parser == nil {
			b.Fatal("Failed to create parser")
		}

		program := C.parser_parse_program(parser)
		if program == nil {
			b.Fatal("Failed to parse program")
		}

		C.parser_destroy(parser)
	}
}

// BenchmarkParser_FunctionDeclaration benchmarks function parsing
func BenchmarkParser_FunctionDeclaration(b *testing.B) {
	source := `
		int add(int a, int b) {
			return a + b;
		}
		
		void greet(string name) {
			Console.WriteLine("Hello, " + name);
		}
	`
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		parser := C.parser_create(cSource)
		if parser == nil {
			b.Fatal("Failed to create parser")
		}

		program := C.parser_parse_program(parser)
		if program == nil {
			b.Fatal("Failed to parse program")
		}

		C.parser_destroy(parser)
	}
}

// BenchmarkCodegen_SimpleProgram benchmarks code generation for simple programs
func BenchmarkCodegen_SimpleProgram(b *testing.B) {
	source := "int x = 1 + 2 + 3; Console.WriteLine(x);"
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		var cOutput *C.char
		var cErrors *C.char

		result := C.dream_compile_string(cSource, &cOutput, &cErrors)
		if result != 0 {
			b.Fatalf("Compilation failed: %s", C.GoString(cErrors))
		}

		C.dream_free_string(cOutput)
		C.dream_free_string(cErrors)
	}
}

// BenchmarkCodegen_ComplexProgram benchmarks code generation for complex programs
func BenchmarkCodegen_ComplexProgram(b *testing.B) {
	source := `
		int factorial(int n) {
			if (n <= 1) {
				return 1;
			}
			return n * factorial(n - 1);
		}
		
		int main() {
			int result = factorial(5);
			Console.WriteLine(result);
			return 0;
		}
	`
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		var cOutput *C.char
		var cErrors *C.char

		result := C.dream_compile_string(cSource, &cOutput, &cErrors)
		if result != 0 {
			b.Fatalf("Compilation failed: %s", C.GoString(cErrors))
		}

		C.dream_free_string(cOutput)
		C.dream_free_string(cErrors)
	}
}

// BenchmarkTypechecking_BasicTypes benchmarks type checking performance
func BenchmarkTypechecking_BasicTypes(b *testing.B) {
	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		// Create various types
		intType := C.type_create(C.TY_INT)
		floatType := C.type_create(C.TY_FLOAT)
		stringType := C.type_create(C.TY_STRING)
		boolType := C.type_create(C.TY_BOOL)

		// Perform type operations
		_ = C.type_equals(intType, intType)
		_ = C.type_equals(intType, floatType)
		_ = C.type_to_string(stringType)
		_ = C.type_get_kind(boolType)

		// Clean up
		C.type_destroy(intType)
		C.type_destroy(floatType)
		C.type_destroy(stringType)
		C.type_destroy(boolType)
	}
}

// BenchmarkTypechecking_FunctionTypes benchmarks function type operations
func BenchmarkTypechecking_FunctionTypes(b *testing.B) {
	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		// Create parameter and return types
		paramType := C.type_create(C.TY_INT)
		returnType := C.type_create(C.TY_BOOL)

		// Create function type (this would need to be implemented in the API)
		funcType := C.type_create(C.TY_FUNC)

		// Perform operations
		_ = C.type_get_kind(funcType)
		_ = C.type_to_string(funcType)

		// Clean up
		C.type_destroy(paramType)
		C.type_destroy(returnType)
		C.type_destroy(funcType)
	}
}

// BenchmarkMemoryManagement_Arena benchmarks arena allocation performance
func BenchmarkMemoryManagement_Arena(b *testing.B) {
	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		arena := C.arena_create()
		if arena == nil {
			b.Fatal("Failed to create arena")
		}

		// Allocate various sizes
		for j := 0; j < 100; j++ {
			size := C.size_t(8 + (j % 64)) // Vary allocation sizes
			ptr := C.arena_allocate(arena, size)
			if ptr == nil {
				b.Fatal("Failed to allocate memory")
			}
		}

		C.arena_destroy(arena)
	}
}

// BenchmarkEndToEnd_SimpleProgram benchmarks complete compilation and execution
func BenchmarkEndToEnd_SimpleProgram(b *testing.B) {
	source := "Console.WriteLine(42);"
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		var cOutput *C.char
		var cErrors *C.char

		result := C.dream_run_string(cSource, &cOutput, &cErrors)
		if result != 0 {
			b.Fatalf("Execution failed: %s", C.GoString(cErrors))
		}

		C.dream_free_string(cOutput)
		C.dream_free_string(cErrors)
	}
}

// BenchmarkEndToEnd_ArithmeticExpression benchmarks arithmetic evaluation
func BenchmarkEndToEnd_ArithmeticExpression(b *testing.B) {
	source := "Console.WriteLine(1 + 2 * 3 - 4 / 2);"
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		var cOutput *C.char
		var cErrors *C.char

		result := C.dream_run_string(cSource, &cOutput, &cErrors)
		if result != 0 {
			b.Fatalf("Execution failed: %s", C.GoString(cErrors))
		}

		C.dream_free_string(cOutput)
		C.dream_free_string(cErrors)
	}
}

// =============================================================================
// COMPARATIVE BENCHMARKS - Different Approaches
// =============================================================================

// BenchmarkLexer_vs_Parser compares lexing vs parsing performance
func BenchmarkLexer_vs_Parser(b *testing.B) {
	source := "int x = 42; Console.WriteLine(x);"
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.Run("Lexer_Only", func(b *testing.B) {
		b.ResetTimer()
		for i := 0; i < b.N; i++ {
			lexer := C.lexer_create(cSource)
			for {
				token := C.lexer_next_token(lexer)
				if token.kind == C.TK_EOF {
					break
				}
			}
			C.lexer_destroy(lexer)
		}
	})

	b.Run("Parser_Full", func(b *testing.B) {
		b.ResetTimer()
		for i := 0; i < b.N; i++ {
			parser := C.parser_create(cSource)
			program := C.parser_parse_program(parser)
			if program == nil {
				b.Fatal("Parse failed")
			}
			C.parser_destroy(parser)
		}
	})
}

// BenchmarkMemoryAllocation_Strategies compares different allocation strategies
func BenchmarkMemoryAllocation_Strategies(b *testing.B) {
	b.Run("Arena_Allocation", func(b *testing.B) {
		b.ResetTimer()
		for i := 0; i < b.N; i++ {
			arena := C.arena_create()
			for j := 0; j < 50; j++ {
				C.arena_allocate(arena, 64)
			}
			C.arena_destroy(arena)
		}
	})

	b.Run("Individual_Types", func(b *testing.B) {
		b.ResetTimer()
		for i := 0; i < b.N; i++ {
			var types []*C.Type
			for j := 0; j < 50; j++ {
				typ := C.type_create(C.TY_INT)
				types = append(types, typ)
			}
			for _, typ := range types {
				C.type_destroy(typ)
			}
		}
	})
}

// =============================================================================
// SCALING BENCHMARKS - Performance with Different Input Sizes
// =============================================================================

// BenchmarkScaling_TokenCount benchmarks performance with varying token counts
func BenchmarkScaling_TokenCount(b *testing.B) {
	sizes := []int{10, 50, 100, 500, 1000}

	for _, size := range sizes {
		b.Run(fmt.Sprintf("Tokens_%d", size), func(b *testing.B) {
			// Generate source with specified number of tokens
			source := generateTokenSequence(size)
			cSource := C.CString(source)
			defer C.free(unsafe.Pointer(cSource))

			b.ResetTimer()
			b.ReportAllocs()

			for i := 0; i < b.N; i++ {
				lexer := C.lexer_create(cSource)
				tokenCount := 0
				for {
					token := C.lexer_next_token(lexer)
					tokenCount++
					if token.kind == C.TK_EOF {
						break
					}
				}
				C.lexer_destroy(lexer)
			}
		})
	}
}

// BenchmarkScaling_ASTDepth benchmarks performance with varying AST depths
func BenchmarkScaling_ASTDepth(b *testing.B) {
	depths := []int{5, 10, 20, 50}

	for _, depth := range depths {
		b.Run(fmt.Sprintf("Depth_%d", depth), func(b *testing.B) {
			// Generate nested expressions
			source := generateNestedExpression(depth)
			cSource := C.CString(source)
			defer C.free(unsafe.Pointer(cSource))

			b.ResetTimer()
			b.ReportAllocs()

			for i := 0; i < b.N; i++ {
				parser := C.parser_create(cSource)
				program := C.parser_parse_program(parser)
				if program == nil {
					b.Fatal("Parse failed")
				}
				C.parser_destroy(parser)
			}
		})
	}
}

// =============================================================================
// HELPER FUNCTIONS FOR BENCHMARK DATA GENERATION
// =============================================================================

// generateTokenSequence creates a source string with approximately the specified number of tokens
func generateTokenSequence(tokenCount int) string {
	var result strings.Builder
	
	for i := 0; i < tokenCount/4; i++ { // Each iteration adds ~4 tokens
		result.WriteString(fmt.Sprintf("int x%d = %d; ", i, i*2))
	}
	
	return result.String()
}

// generateNestedExpression creates a deeply nested arithmetic expression
func generateNestedExpression(depth int) string {
	if depth <= 1 {
		return "1"
	}
	
	return fmt.Sprintf("(%s + %s)", generateNestedExpression(depth-1), generateNestedExpression(depth-1))
}

// =============================================================================
// MEMORY PROFILING BENCHMARKS
// =============================================================================

// BenchmarkMemoryProfile_LexerLifecycle profiles lexer memory usage
func BenchmarkMemoryProfile_LexerLifecycle(b *testing.B) {
	source := "int x = 42; float y = 3.14; string s = \"hello\"; bool flag = true;"
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		lexer := C.lexer_create(cSource)
		
		// Simulate typical usage pattern
		for j := 0; j < 3; j++ {
			// Reset and re-tokenize
			tempLexer := C.lexer_create(cSource)
			for {
				token := C.lexer_next_token(tempLexer)
				if token.kind == C.TK_EOF {
					break
				}
			}
			C.lexer_destroy(tempLexer)
		}
		
		C.lexer_destroy(lexer)
	}
}

// BenchmarkMemoryProfile_ParserLifecycle profiles parser memory usage
func BenchmarkMemoryProfile_ParserLifecycle(b *testing.B) {
	source := `
		int factorial(int n) {
			if (n <= 1) return 1;
			return n * factorial(n - 1);
		}
	`
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		parser := C.parser_create(cSource)
		program := C.parser_parse_program(parser)
		
		if program != nil {
			// Simulate AST traversal
			_ = C.node_get_kind(program)
			childCount := C.node_count_children(program)
			for j := C.size_t(0); j < childCount; j++ {
				child := C.node_get_child(program, j)
				if child != nil {
					_ = C.node_get_kind(child)
				}
			}
		}
		
		C.parser_destroy(parser)
	}
}

// =============================================================================
// NEW COMPILER FEATURES BENCHMARKS - Performance testing for recent additions  
// =============================================================================

// BenchmarkParser_StringConcatenation benchmarks string concatenation parsing
func BenchmarkParser_StringConcatenation(b *testing.B) {
	source := `string result = "Hello" + " " + "World" + "!";`
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		parser := C.parser_create(cSource)
		if parser == nil {
			b.Fatal("Failed to create parser")
		}

		program := C.parser_parse_program(parser)
		if program == nil {
			b.Fatal("Failed to parse program")
		}

		C.parser_destroy(parser)
	}
}

// BenchmarkParser_SwitchStatement benchmarks switch statement parsing
func BenchmarkParser_SwitchStatement(b *testing.B) {
	source := `
		switch (value) {
			case 1: Console.WriteLine("One"); break;
			case 2: Console.WriteLine("Two"); break;
			default: Console.WriteLine("Other");
		}
	`
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		parser := C.parser_create(cSource)
		if parser == nil {
			b.Fatal("Failed to create parser")
		}

		program := C.parser_parse_program(parser)
		if program == nil {
			b.Fatal("Failed to parse program")
		}

		C.parser_destroy(parser)
	}
}

// BenchmarkParser_EnumDeclaration benchmarks enum declaration parsing
func BenchmarkParser_EnumDeclaration(b *testing.B) {
	source := `enum Color { Red, Green, Blue, Yellow, Orange, Purple }`
	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		parser := C.parser_create(cSource)
		if parser == nil {
			b.Fatal("Failed to create parser")
		}

		program := C.parser_parse_program(parser)
		if program == nil {
			b.Fatal("Failed to parse program")
		}

		C.parser_destroy(parser)
	}
}