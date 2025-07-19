package dream_test

/*
#cgo CFLAGS: -I. -I../src -std=c11 -D_GNU_SOURCE
#cgo LDFLAGS: -L. -ldream
#include "dream.h"
#include <stdlib.h>
*/
import "C"

import (
	"strings"
	"testing"
	"unsafe"
)

// =============================================================================
// FUZZ TESTS - Robustness Testing with Random Inputs
// =============================================================================

// FuzzLexer tests the lexer with random byte sequences to detect crashes
func FuzzLexer(f *testing.F) {
	// Seed the fuzzer with known-good inputs
	seeds := []string{
		"int x = 42;",
		"Console.WriteLine(\"hello\");",
		"if (true) { return 1; }",
		"for (int i = 0; i < 10; i++) {}",
		"float y = 3.14 + 2.71;",
		"string s = \"test\" + \"string\";",
		"bool flag = true && false;",
		"// This is a comment\nint z = 0;",
		"/* Block comment */ int w = 1;",
		"class MyClass { int field; }",
		"struct Point { int x, y; }",
		"function add(int a, int b) { return a + b; }",
		"try { risky(); } catch { handle(); }",
		"switch (x) { case 1: break; default: break; }",
		"while (condition) { doSomething(); }",
		"do { action(); } while (check);",
		"x++; y--; z += 5;",
		"array[index] = value;",
		"object.property = newValue;",
		"result = condition ? trueValue : falseValue;",
	}

	// Add seeds to the fuzzer
	for _, seed := range seeds {
		f.Add(seed)
	}

	// Add some edge cases
	edgeCases := []string{
		"", // Empty input
		" ", // Whitespace only
		"\n\t\r", // Various whitespace
		"\"", // Unterminated string
		"/*", // Unterminated comment
		"123456789012345678901234567890", // Very long number
		strings.Repeat("a", 1000), // Very long identifier
		strings.Repeat("(", 100), // Many nested parentheses
		strings.Repeat("\"", 50), // Many quotes
		"int" + strings.Repeat(" ", 1000) + "x;", // Excessive whitespace
	}

	for _, edge := range edgeCases {
		f.Add(edge)
	}

	// The actual fuzz test
	f.Fuzz(func(t *testing.T, input string) {
		// Skip extremely long inputs to prevent timeouts
		if len(input) > 10000 {
			t.Skip("Input too long")
		}

		// Convert to C string
		cInput := C.CString(input)
		defer C.free(unsafe.Pointer(cInput))

		// Test lexer creation - should never crash
		lexer := C.lexer_create(cInput)
		if lexer == nil {
			// It's okay for lexer creation to fail, just don't crash
			return
		}
		defer C.lexer_destroy(lexer)

		// Tokenize the input - should never crash
		tokenCount := 0
		maxTokens := 10000 // Prevent infinite loops

		for tokenCount < maxTokens {
			token := C.lexer_next_token(lexer)
			tokenCount++

			// EOF should terminate the loop
			if token.kind == C.TK_EOF {
				break
			}

			// Error tokens are acceptable
			if token.kind == C.TK_ERROR {
				break
			}

			// Validate token position (should not be negative or extremely large)
			if token.pos.line > 100000 || token.pos.column > 100000 {
				t.Errorf("Token position out of reasonable range: line=%d, col=%d", 
					token.pos.line, token.pos.column)
			}

			// Validate token length (should not be negative or extremely large)
			if token.len > 10000 {
				t.Errorf("Token length unreasonably large: %d", token.len)
			}
		}

		// If we hit the token limit, that's suspicious
		if tokenCount >= maxTokens {
			t.Errorf("Lexer produced too many tokens (%d), possible infinite loop", tokenCount)
		}
	})
}

// FuzzParser tests the parser with random inputs to detect crashes and hangs
func FuzzParser(f *testing.F) {
	// Seed with syntactically valid programs
	seeds := []string{
		"int x = 42;",
		"Console.WriteLine(\"hello\");",
		"if (x > 0) { Console.WriteLine(x); }",
		"while (i < 10) { i++; }",
		"for (int j = 0; j < 5; j++) { Console.WriteLine(j); }",
		"int add(int a, int b) { return a + b; }",
		"class Test { int field; void method() {} }",
		"struct Point { int x; int y; }",
		"try { doSomething(); } catch { handleError(); }",
		"switch (value) { case 1: break; default: break; }",
		"{ int local = 1; Console.WriteLine(local); }",
		"return 42;",
		"break; continue;",
		"x = y + z * w;",
		"array[0] = 100;",
		"obj.field = value;",
		"result = test ? yes : no;",
		"new MyClass();",
		"throw exception;",
		"finally { cleanup(); }",
	}

	for _, seed := range seeds {
		f.Add(seed)
	}

	// Add some malformed inputs that should be handled gracefully
	malformedInputs := []string{
		"int x =", // Incomplete statement
		"if (", // Incomplete condition
		"while )", // Mismatched parentheses
		"{ { { { {", // Unmatched braces
		"} } } } }", // Extra closing braces
		"int int int", // Repeated keywords
		"123 abc 456", // Mixed tokens
		"\"unclosed string", // Unterminated string
		"/* unclosed comment", // Unterminated comment
		"function ()", // Missing name
		"class {}", // Missing name
		"if if if", // Repeated keywords
		"return return return;", // Multiple returns
		";;;;;;", // Multiple semicolons
		"......", // Multiple dots
		"++++++", // Multiple operators
		"((((((", // Nested parentheses
		"))))))", // Unmatched closing
		"[[[[[", // Nested brackets
		"]]]]]", // Unmatched closing
	}

	for _, malformed := range malformedInputs {
		f.Add(malformed)
	}

	f.Fuzz(func(t *testing.T, input string) {
		// Skip extremely long inputs
		if len(input) > 5000 {
			t.Skip("Input too long")
		}

		// Convert to C string
		cInput := C.CString(input)
		defer C.free(unsafe.Pointer(cInput))

		// Test parser creation - should never crash
		parser := C.parser_create(cInput)
		if parser == nil {
			// Parser creation failure is acceptable
			return
		}
		defer C.parser_destroy(parser)

		// Parse the program - should never crash
		program := C.parser_parse_program(parser)

		// Check if there were errors (this is fine)
		hasErrors := bool(C.parser_has_errors(parser))
		errorCount := int(C.parser_error_count(parser))

		// Validate error count is reasonable
		if errorCount > 1000 {
			t.Errorf("Excessive error count: %d", errorCount)
		}

		// If parsing succeeded, do some basic AST validation
		if !hasErrors && program != nil {
			validateASTNode(t, program, 0, 100) // Max depth of 100
		}

		// Test error message retrieval - should not crash
		for i := 0; i < errorCount && i < 10; i++ { // Limit to first 10 errors
			errorMsg := C.parser_get_error(parser, C.size_t(i))
			if errorMsg != nil {
				msg := C.GoString(errorMsg)
				// Error messages should not be excessively long
				if len(msg) > 1000 {
					t.Errorf("Error message too long: %d characters", len(msg))
				}
			}
		}
	})
}

// FuzzCodegen tests the code generation with random valid ASTs
func FuzzCodegen(f *testing.F) {
	// Seed with programs that should compile successfully
	seeds := []string{
		"Console.WriteLine(42);",
		"int x = 1 + 2 + 3; Console.WriteLine(x);",
		"if (true) Console.WriteLine(\"yes\");",
		"int i = 0; while (i < 3) { Console.WriteLine(i); i++; }",
		"for (int j = 0; j < 2; j++) Console.WriteLine(j);",
		"int add(int a, int b) { return a + b; } Console.WriteLine(add(1, 2));",
		"string s = \"hello\"; Console.WriteLine(s);",
		"bool flag = true; Console.WriteLine(flag);",
		"float f = 3.14; Console.WriteLine(f);",
		"char c = 'A'; Console.WriteLine(c);",
	}

	for _, seed := range seeds {
		f.Add(seed)
	}

	// Add some edge cases for code generation
	edgeCases := []string{
		"Console.WriteLine(0);", // Zero value
		"Console.WriteLine(-1);", // Negative value
		"Console.WriteLine(2147483647);", // Large int
		"Console.WriteLine(-2147483648);", // Small int
		"Console.WriteLine(3.14159265359);", // High precision float
		"Console.WriteLine(\"\");", // Empty string
		"Console.WriteLine(\"\\n\\t\\r\");", // Escape sequences
		"int x = 1; int y = x; Console.WriteLine(y);", // Variable reference
		"Console.WriteLine(1 + 2 * 3 - 4 / 2);", // Complex arithmetic
		"Console.WriteLine(true && false);", // Boolean logic
		"Console.WriteLine(!true);", // Unary operator
		"Console.WriteLine(5 % 3);", // Modulo operation
	}

	for _, edge := range edgeCases {
		f.Add(edge)
	}

	f.Fuzz(func(t *testing.T, input string) {
		// Skip very long inputs
		if len(input) > 2000 {
			t.Skip("Input too long")
		}

		// Convert to C string
		cInput := C.CString(input)
		defer C.free(unsafe.Pointer(cInput))

		// Test compilation - should never crash
		var cOutput *C.char
		var cErrors *C.char

		result := C.dream_compile_string(cInput, &cOutput, &cErrors)
		defer C.dream_free_string(cOutput)
		defer C.dream_free_string(cErrors)

		// Compilation can fail, that's fine
		if result != 0 {
			// But error messages should be reasonable
			if cErrors != nil {
				errorMsg := C.GoString(cErrors)
				if len(errorMsg) > 5000 {
					t.Errorf("Error message too long: %d characters", len(errorMsg))
				}
			}
			return
		}

		// If compilation succeeded, output should be reasonable
		if cOutput != nil {
			output := C.GoString(cOutput)
			if len(output) > 100000 {
				t.Errorf("Generated code too long: %d characters", len(output))
			}

			// Generated code should not contain null bytes (except at end)
			if strings.Contains(output[:len(output)-1], "\x00") {
				t.Error("Generated code contains null bytes")
			}
		}
	})
}

// FuzzEndToEnd tests complete compilation and execution pipeline
func FuzzEndToEnd(f *testing.F) {
	// Seed with simple programs that should execute successfully
	seeds := []string{
		"Console.WriteLine(42);",
		"Console.WriteLine(\"hello\");",
		"Console.WriteLine(1 + 2 + 3);",
		"int x = 42; Console.WriteLine(x);",
		"Console.WriteLine(true);",
		"Console.WriteLine(false);",
		"Console.WriteLine(3.14);",
		"Console.WriteLine('A');",
	}

	for _, seed := range seeds {
		f.Add(seed)
	}

	f.Fuzz(func(t *testing.T, input string) {
		// Skip very long inputs
		if len(input) > 1000 {
			t.Skip("Input too long")
		}

		// Convert to C string
		cInput := C.CString(input)
		defer C.free(unsafe.Pointer(cInput))

		// Test execution - should never crash
		var cOutput *C.char
		var cErrors *C.char

		result := C.dream_run_string(cInput, &cOutput, &cErrors)
		defer C.dream_free_string(cOutput)
		defer C.dream_free_string(cErrors)

		// Execution can fail, that's acceptable
		if result != 0 {
			// Error messages should be reasonable
			if cErrors != nil {
				errorMsg := C.GoString(cErrors)
				if len(errorMsg) > 2000 {
					t.Errorf("Error message too long: %d characters", len(errorMsg))
				}
			}
			return
		}

		// If execution succeeded, validate output
		if cOutput != nil {
			output := C.GoString(cOutput)
			
			// Output should not be excessively long
			if len(output) > 10000 {
				t.Errorf("Program output too long: %d characters", len(output))
			}

			// Output should not contain control characters (except newlines and tabs)
			for _, r := range output {
				if r < 32 && r != '\n' && r != '\t' && r != '\r' {
					t.Errorf("Output contains control character: %d", r)
					break
				}
			}
		}
	})
}

// FuzzTypeSystem tests the type system with random type operations
func FuzzTypeSystem(f *testing.F) {
	// This fuzzer tests type operations that shouldn't crash
	f.Fuzz(func(t *testing.T, typeKind1, typeKind2 uint8) {
		// Map random bytes to valid type kinds
		validTypes := []C.TypeKind{
			C.TY_INT, C.TY_FLOAT, C.TY_BOOL, C.TY_CHAR, 
			C.TY_STRING, C.TY_FUNC, C.TY_VAR, C.TY_ERROR,
		}

		kind1 := validTypes[typeKind1%uint8(len(validTypes))]
		kind2 := validTypes[typeKind2%uint8(len(validTypes))]

		// Create types - should never crash
		type1 := C.type_create(kind1)
		type2 := C.type_create(kind2)

		if type1 == nil || type2 == nil {
			// Type creation can fail, clean up what we have
			if type1 != nil {
				C.type_destroy(type1)
			}
			if type2 != nil {
				C.type_destroy(type2)
			}
			return
		}

		// Test type operations - should never crash
		_ = C.type_get_kind(type1)
		_ = C.type_get_kind(type2)
		_ = C.type_equals(type1, type2)
		_ = C.type_equals(type1, type1)
		_ = C.type_equals(type2, type2)

		// Test string conversion - should never crash
		str1 := C.type_to_string(type1)
		str2 := C.type_to_string(type2)

		if str1 != nil {
			s1 := C.GoString(str1)
			if len(s1) > 100 {
				t.Errorf("Type string too long: %d", len(s1))
			}
		}

		if str2 != nil {
			s2 := C.GoString(str2)
			if len(s2) > 100 {
				t.Errorf("Type string too long: %d", len(s2))
			}
		}

		// Clean up
		C.type_destroy(type1)
		C.type_destroy(type2)
	})
}

// =============================================================================
// HELPER FUNCTIONS FOR FUZZ TESTING
// =============================================================================

// validateASTNode recursively validates an AST node structure
func validateASTNode(t *testing.T, node *C.Node, depth, maxDepth int) {
	if node == nil {
		return
	}

	// Prevent infinite recursion
	if depth > maxDepth {
		t.Errorf("AST depth exceeded maximum: %d", depth)
		return
	}

	// Validate node kind is reasonable
	kind := C.node_get_kind(node)
	if int(kind) < 0 || int(kind) > 100 { // Assuming reasonable range
		t.Errorf("Invalid node kind: %d", kind)
		return
	}

	// Validate position
	pos := C.node_get_position(node)
	if pos.line > 100000 || pos.column > 100000 {
		t.Errorf("Node position out of range: line=%d, col=%d", pos.line, pos.column)
	}

	// Validate child count is reasonable
	childCount := C.node_count_children(node)
	if childCount > 1000 {
		t.Errorf("Too many child nodes: %d", childCount)
		return
	}

	// Recursively validate children
	for i := C.size_t(0); i < childCount; i++ {
		child := C.node_get_child(node, i)
		validateASTNode(t, child, depth+1, maxDepth)
	}

	// Validate text content if applicable
	text := C.node_get_text(node)
	if text != nil {
		textStr := C.GoString(text)
		if len(textStr) > 10000 {
			t.Errorf("Node text too long: %d characters", len(textStr))
		}
	}
}

// FuzzMemoryOperations tests memory management under stress
func FuzzMemoryOperations(f *testing.F) {
	f.Fuzz(func(t *testing.T, operations []uint8) {
		// Limit the number of operations to prevent timeouts
		if len(operations) > 100 {
			operations = operations[:100]
		}

		var arenas []*C.Arena
		var types []*C.Type
		var lexers []*C.Lexer
		var parsers []*C.Parser

		defer func() {
			// Clean up all resources
			for _, arena := range arenas {
				if arena != nil {
					C.arena_destroy(arena)
				}
			}
			for _, typ := range types {
				if typ != nil {
					C.type_destroy(typ)
				}
			}
			for _, lexer := range lexers {
				if lexer != nil {
					C.lexer_destroy(lexer)
				}
			}
			for _, parser := range parsers {
				if parser != nil {
					C.parser_destroy(parser)
				}
			}
		}()

		// Perform random operations
		for _, op := range operations {
			switch op % 8 {
			case 0: // Create arena
				arena := C.arena_create()
				if arena != nil {
					arenas = append(arenas, arena)
				}

			case 1: // Create type
				typeKinds := []C.TypeKind{C.TY_INT, C.TY_FLOAT, C.TY_BOOL, C.TY_STRING}
				kind := typeKinds[op%4]
				typ := C.type_create(kind)
				if typ != nil {
					types = append(types, typ)
				}

			case 2: // Create lexer
				source := "int x = 42;"
				cSource := C.CString(source)
				lexer := C.lexer_create(cSource)
				C.free(unsafe.Pointer(cSource))
				if lexer != nil {
					lexers = append(lexers, lexer)
				}

			case 3: // Create parser
				source := "int x = 42;"
				cSource := C.CString(source)
				parser := C.parser_create(cSource)
				C.free(unsafe.Pointer(cSource))
				if parser != nil {
					parsers = append(parsers, parser)
				}

			case 4: // Allocate from arena
				if len(arenas) > 0 {
					arena := arenas[int(op)%len(arenas)]
					if arena != nil {
						size := C.size_t(1 + (op % 100)) // 1-100 bytes
						C.arena_allocate(arena, size)
					}
				}

			case 5: // Compare types
				if len(types) >= 2 {
					idx1 := int(op) % len(types)
					idx2 := int(op/2) % len(types)
					if types[idx1] != nil && types[idx2] != nil {
						C.type_equals(types[idx1], types[idx2])
					}
				}

			case 6: // Tokenize with lexer
				if len(lexers) > 0 {
					lexer := lexers[int(op)%len(lexers)]
					if lexer != nil {
						// Get a few tokens
						for i := 0; i < 5; i++ {
							token := C.lexer_next_token(lexer)
							if token.kind == C.TK_EOF {
								break
							}
						}
					}
				}

			case 7: // Parse with parser
				if len(parsers) > 0 {
					parser := parsers[int(op)%len(parsers)]
					if parser != nil {
						program := C.parser_parse_program(parser)
						if program != nil {
							C.node_get_kind(program)
						}
					}
				}
			}
		}
	})
}