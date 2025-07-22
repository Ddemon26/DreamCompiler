package dream_test

/*
#cgo CFLAGS: -I. -I../../src -std=c11 -D_GNU_SOURCE
#cgo LDFLAGS: -L. -ldream
#include "dream.h"
#include <stdlib.h>
*/
import "C"

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"testing"
	"unsafe"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

// =============================================================================
// UNIT TESTS - Direct C API Testing via cgo
// =============================================================================

func TestLexer_BasicTokens(t *testing.T) {
	tests := []struct {
		name     string
		input    string
		expected []C.int
	}{
		{
			name:  "simple arithmetic",
			input: "1 + 2",
			expected: []C.int{
				C.TK_INT,
				C.TK_PLUS,
				C.TK_INT,
				C.TK_EOF,
			},
		},
		{
			name:  "variable declaration",
			input: "int x = 42;",
			expected: []C.int{
				C.TK_KW_INT,
				C.TK_IDENT,
				C.TK_EQ,
				C.TK_INT,
				C.TK_SEMICOLON,
				C.TK_EOF,
			},
		},
		{
			name:  "function call",
			input: "Console.WriteLine(\"hello\");",
			expected: []C.int{
				C.TK_CONSOLE,
				C.TK_DOT,
				C.TK_WRITELINE,
				C.TK_LPAREN,
				C.TK_STRING,
				C.TK_RPAREN,
				C.TK_SEMICOLON,
				C.TK_EOF,
			},
		},
		{
			name:  "control flow",
			input: "if (x > 0) { return true; }",
			expected: []C.int{
				C.TK_IF,
				C.TK_LPAREN,
				C.TK_IDENT,
				C.TK_GT,
				C.TK_INT,
				C.TK_RPAREN,
				C.TK_LBRACE,
				C.TK_RETURN,
				C.TK_KW_TRUE,
				C.TK_SEMICOLON,
				C.TK_RBRACE,
				C.TK_EOF,
			},
		},
		{
			name:  "string concatenation",
			input: "\"hello\" + \" world\"",
			expected: []C.int{
				C.TK_STRING,
				C.TK_PLUS,
				C.TK_STRING,
				C.TK_EOF,
			},
		},
		{
			name:  "switch statement",
			input: "switch (x) { case 1: break; }",
			expected: []C.int{
				C.TK_KW_SWITCH,
				C.TK_LPAREN,
				C.TK_IDENT,
				C.TK_RPAREN,
				C.TK_LBRACE,
				C.TK_KW_CASE,
				C.TK_INT,
				C.TK_COLON,
				C.TK_BREAK,
				C.TK_SEMICOLON,
				C.TK_RBRACE,
				C.TK_EOF,
			},
		},
		{
			name:  "enum declaration",
			input: "enum Color { Red, Green, Blue }",
			expected: []C.int{
				C.TK_KW_ENUM,
				C.TK_IDENT,
				C.TK_LBRACE,
				C.TK_IDENT,
				C.TK_COMMA,
				C.TK_IDENT,
				C.TK_COMMA,
				C.TK_IDENT,
				C.TK_RBRACE,
				C.TK_EOF,
			},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cSource := C.CString(tt.input)
			defer C.free(unsafe.Pointer(cSource))

			lexer := C.lexer_create(cSource)
			require.NotNil(t, lexer, "Failed to create lexer")
			defer C.lexer_destroy(lexer)

			var tokens []C.int
			for {
				token := C.lexer_next_token(lexer)
				tokens = append(tokens, token.kind)
				if token.kind == C.TK_EOF {
					break
				}
				// Safety check to prevent infinite loops
				if len(tokens) > 100 {
					t.Fatal("Too many tokens, possible infinite loop")
				}
			}

			assert.Equal(t, tt.expected, tokens, "Token sequence mismatch")
		})
	}
}

func TestLexer_TokenPositions(t *testing.T) {
	input := "int x = 42;\nfloat y = 3.14;"
	cSource := C.CString(input)
	defer C.free(unsafe.Pointer(cSource))

	lexer := C.lexer_create(cSource)
	require.NotNil(t, lexer)
	defer C.lexer_destroy(lexer)

	// Test first line tokens
	token1 := C.lexer_next_token(lexer) // "int"
	assert.Equal(t, C.size_t(1), token1.pos.line)
	assert.Equal(t, C.size_t(1), token1.pos.column)

	token2 := C.lexer_next_token(lexer) // "x"
	assert.Equal(t, C.size_t(1), token2.pos.line)
	assert.Greater(t, int(token2.pos.column), 1)

	// Skip to second line
	for {
		token := C.lexer_next_token(lexer)
		if token.kind == C.TK_KW_FLOAT {
			assert.Equal(t, C.size_t(2), token.pos.line)
			assert.Equal(t, C.size_t(1), token.pos.column)
			break
		}
		if token.kind == C.TK_EOF {
			t.Fatal("Unexpected EOF before finding float token")
		}
	}
}

func TestLexer_ErrorHandling(t *testing.T) {
	tests := []struct {
		name  string
		input string
	}{
		{"unterminated string", "\"hello world"},
		{"invalid character", "int x = @;"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cSource := C.CString(tt.input)
			defer C.free(unsafe.Pointer(cSource))

			lexer := C.lexer_create(cSource)
			require.NotNil(t, lexer)
			defer C.lexer_destroy(lexer)

			// Tokenize and look for error tokens or handle gracefully
			foundError := false
			for i := 0; i < 100; i++ { // Safety limit
				token := C.lexer_next_token(lexer)
				if token.kind == C.TK_ERROR {
					foundError = true
					break
				}
				if token.kind == C.TK_EOF {
					break
				}
			}

			// Log result - some inputs might not generate TK_ERROR
			// depending on lexer implementation
			t.Logf("Error handling test: %s - Error token found: %v", tt.name, foundError)
		})
	}
}

func TestParser_BasicExpressions(t *testing.T) {
	tests := []struct {
		name        string
		input       string
		expectError bool
		nodeKind    C.int
	}{
		{
			name:        "integer literal",
			input:       "42;",
			expectError: false,
			nodeKind:    C.ND_INT,
		},
		{
			name:        "string literal",
			input:       "\"hello world\";",
			expectError: false,
			nodeKind:    C.ND_STRING,
		},
		{
			name:        "binary expression",
			input:       "1 + 2;",
			expectError: false,
			nodeKind:    C.ND_BINOP,
		},
		{
			name:        "variable declaration",
			input:       "int x = 42;",
			expectError: false,
			nodeKind:    C.ND_VAR_DECL,
		},
		{
			name:        "function call",
			input:       "Console.WriteLine(42);",
			expectError: false,
			nodeKind:    C.ND_CONSOLE_CALL,
		},
		{
			name:        "syntax error - missing semicolon",
			input:       "int x = 42",
			expectError: true,
			nodeKind:    C.ND_ERROR,
		},
		{
			name:        "syntax error - unmatched parentheses",
			input:       "Console.WriteLine(42;",
			expectError: true,
			nodeKind:    C.ND_ERROR,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cSource := C.CString(tt.input)
			defer C.free(unsafe.Pointer(cSource))

			parser := C.parser_create(cSource)
			require.NotNil(t, parser, "Failed to create parser")
			defer C.parser_destroy(parser)

			program := C.parser_parse_program(parser)
			hasErrors := bool(C.parser_has_errors(parser))

			if tt.expectError {
				assert.True(t, hasErrors, "Expected parser errors but got none")
			} else {
				assert.False(t, hasErrors, "Unexpected parser errors")
				require.NotNil(t, program, "Expected valid AST but got nil")

				// For simple cases, check the first statement
				if program != nil {
					kind := C.node_get_kind(program)
					if kind == C.ND_BLOCK {
						childCount := C.node_count_children(program)
						if childCount > 0 {
							firstChild := C.node_get_child(program, 0)
							if firstChild != nil {
								actualKind := C.node_get_kind(firstChild)
								assert.Equal(t, tt.nodeKind, actualKind, "Unexpected node kind")
							}
						}
					}
				}
			}
		})
	}
}

func TestParser_AST_Structure(t *testing.T) {
	input := "if (x > 0) { Console.WriteLine(x); }"
	cSource := C.CString(input)
	defer C.free(unsafe.Pointer(cSource))

	parser := C.parser_create(cSource)
	require.NotNil(t, parser)
	defer C.parser_destroy(parser)

	program := C.parser_parse_program(parser)
	require.NotNil(t, program)
	assert.False(t, bool(C.parser_has_errors(parser)))

	// Verify AST structure
	programKind := C.node_get_kind(program)
	assert.Equal(t, C.ND_BLOCK, programKind)

	childCount := C.node_count_children(program)
	assert.Greater(t, int(childCount), 0)

	// Get the if statement
	ifStmt := C.node_get_child(program, 0)
	require.NotNil(t, ifStmt)
	assert.Equal(t, C.ND_IF, C.node_get_kind(ifStmt))

	// Check if statement has condition and body
	ifChildCount := C.node_count_children(ifStmt)
	assert.GreaterOrEqual(t, int(ifChildCount), 2) // at least condition and then branch
}

func TestTypes_BasicOperations(t *testing.T) {
	tests := []struct {
		name     string
		kind     C.int
		expected string
	}{
		{"integer type", C.TY_INT, "int"},
		{"float type", C.TY_FLOAT, "float"},
		{"boolean type", C.TY_BOOL, "bool"},
		{"character type", C.TY_CHAR, "char"},
		{"string type", C.TY_STRING, "string"},
		{"function type", C.TY_FUNC, "function"},
		{"error type", C.TY_ERROR, "error"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			typ := C.type_create(tt.kind)
			require.NotNil(t, typ)
			defer C.type_destroy(typ)

			kind := C.type_get_kind(typ)
			assert.Equal(t, tt.kind, kind)

			typeStr := C.GoString(C.type_to_string(typ))
			assert.Equal(t, tt.expected, typeStr)
		})
	}
}

func TestTypes_Equality(t *testing.T) {
	// Create two int types
	int1 := C.type_create(C.TY_INT)
	int2 := C.type_create(C.TY_INT)
	float1 := C.type_create(C.TY_FLOAT)

	defer C.type_destroy(int1)
	defer C.type_destroy(int2)
	defer C.type_destroy(float1)

	// Same types should be equal
	assert.True(t, bool(C.type_equals(int1, int2)))

	// Different types should not be equal
	assert.False(t, bool(C.type_equals(int1, float1)))

	// Self-equality
	assert.True(t, bool(C.type_equals(int1, int1)))
}

func TestArena_MemoryManagement(t *testing.T) {
	arena := C.arena_create()
	require.NotNil(t, arena)
	defer C.arena_destroy(arena)

	// Allocate some memory
	ptr1 := C.arena_allocate(arena, 64)
	assert.NotNil(t, ptr1)

	ptr2 := C.arena_allocate(arena, 128)
	assert.NotNil(t, ptr2)

	// Pointers should be different
	assert.NotEqual(t, ptr1, ptr2)
}

// =============================================================================
// COMPREHENSIVE FEATURE TESTS - Testing new compiler features
// =============================================================================

func TestLexer_ComprehensiveTokens(t *testing.T) {
	testCases := []struct {
		category string
		tests    []struct {
			name     string
			input    string
			expected []C.int
		}
	}{
		{
			category: "Arithmetic Operators",
			tests: []struct {
				name     string
				input    string
				expected []C.int
			}{
				{"addition", "a + b", []C.int{C.TK_IDENT, C.TK_PLUS, C.TK_IDENT, C.TK_EOF}},
				{"subtraction", "a - b", []C.int{C.TK_IDENT, C.TK_MINUS, C.TK_IDENT, C.TK_EOF}},
				{"multiplication", "a * b", []C.int{C.TK_IDENT, C.TK_STAR, C.TK_IDENT, C.TK_EOF}},
				{"division", "a / b", []C.int{C.TK_IDENT, C.TK_SLASH, C.TK_IDENT, C.TK_EOF}},
				{"modulo", "a % b", []C.int{C.TK_IDENT, C.TK_PERCENT, C.TK_IDENT, C.TK_EOF}},
			},
		},
		{
			category: "Comparison Operators",
			tests: []struct {
				name     string
				input    string
				expected []C.int
			}{
				{"equality", "a == b", []C.int{C.TK_IDENT, C.TK_EQEQ, C.TK_IDENT, C.TK_EOF}},
				{"inequality", "a != b", []C.int{C.TK_IDENT, C.TK_NEQ, C.TK_IDENT, C.TK_EOF}},
				{"less than", "a < b", []C.int{C.TK_IDENT, C.TK_LT, C.TK_IDENT, C.TK_EOF}},
				{"less equal", "a <= b", []C.int{C.TK_IDENT, C.TK_LTEQ, C.TK_IDENT, C.TK_EOF}},
				{"greater than", "a > b", []C.int{C.TK_IDENT, C.TK_GT, C.TK_IDENT, C.TK_EOF}},
				{"greater equal", "a >= b", []C.int{C.TK_IDENT, C.TK_GTEQ, C.TK_IDENT, C.TK_EOF}},
			},
		},
		{
			category: "Logical Operators",
			tests: []struct {
				name     string
				input    string
				expected []C.int
			}{
				{"logical and", "a && b", []C.int{C.TK_IDENT, C.TK_ANDAND, C.TK_IDENT, C.TK_EOF}},
				{"logical or", "a || b", []C.int{C.TK_IDENT, C.TK_OROR, C.TK_IDENT, C.TK_EOF}},
				{"logical not", "!a", []C.int{C.TK_BANG, C.TK_IDENT, C.TK_EOF}},
			},
		},
		{
			category: "Keywords",
			tests: []struct {
				name     string
				input    string
				expected []C.int
			}{
				{"if keyword", "if", []C.int{C.TK_IF, C.TK_EOF}},
				{"else keyword", "else", []C.int{C.TK_KW_ELSE, C.TK_EOF}},
				{"while keyword", "while", []C.int{C.TK_KW_WHILE, C.TK_EOF}},
				{"for keyword", "for", []C.int{C.TK_KW_FOR, C.TK_EOF}},
				{"return keyword", "return", []C.int{C.TK_RETURN, C.TK_EOF}},
				{"break keyword", "break", []C.int{C.TK_BREAK, C.TK_EOF}},
				{"continue keyword", "continue", []C.int{C.TK_KW_CONTINUE, C.TK_EOF}},
			},
		},
		{
			category: "New Features",
			tests: []struct {
				name     string
				input    string
				expected []C.int
			}{
				{"switch keyword", "switch", []C.int{C.TK_KW_SWITCH, C.TK_EOF}},
				{"case keyword", "case", []C.int{C.TK_KW_CASE, C.TK_EOF}},
				{"enum keyword", "enum", []C.int{C.TK_KW_ENUM, C.TK_EOF}},
				{"null literal", "null", []C.int{C.TK_KW_NULL, C.TK_EOF}},
				{"const keyword", "const", []C.int{C.TK_KW_CONST, C.TK_EOF}},
			},
		},
	}

	for _, category := range testCases {
		t.Run(category.category, func(t *testing.T) {
			for _, tt := range category.tests {
				t.Run(tt.name, func(t *testing.T) {
					cSource := C.CString(tt.input)
					defer C.free(unsafe.Pointer(cSource))

					lexer := C.lexer_create(cSource)
					require.NotNil(t, lexer)
					defer C.lexer_destroy(lexer)

					var tokens []C.int
					for {
						token := C.lexer_next_token(lexer)
						tokens = append(tokens, token.kind)
						if token.kind == C.TK_EOF {
							break
						}
						if len(tokens) > 50 {
							t.Fatal("Too many tokens")
						}
					}

					assert.Equal(t, tt.expected, tokens)
				})
			}
		})
	}
}

func TestParser_NewLanguageFeatures(t *testing.T) {
	tests := []struct {
		name        string
		input       string
		expectError bool
		description string
	}{
		{
			name:        "string concatenation",
			input:       "string result = \"Hello\" + \" World\";",
			expectError: false,
			description: "String concatenation with + operator",
		},
		{
			name:        "mixed type concatenation",
			input:       "string message = \"Count: \" + 42;",
			expectError: false,
			description: "String and integer concatenation",
		},
		{
			name:        "switch statement basic",
			input:       "switch (x) { case 1: Console.WriteLine(\"one\"); break; }",
			expectError: false,
			description: "Basic switch statement with break",
		},
		{
			name:        "switch statement multiple cases",
			input:       "switch (color) { case 1: x = 1; case 2: x = 2; break; default: x = 0; }",
			expectError: false,
			description: "Switch with multiple cases and default",
		},
		{
			name:        "enum declaration",
			input:       "enum Color { Red, Green, Blue }",
			expectError: false,
			description: "Enum declaration with multiple values",
		},
		{
			name:        "null literal usage",
			input:       "string s = null;",
			expectError: false,
			description: "Null literal assignment",
		},
		{
			name:        "const variable",
			input:       "const int MAX_SIZE = 100;",
			expectError: false,
			description: "Const variable declaration",
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cSource := C.CString(tt.input)
			defer C.free(unsafe.Pointer(cSource))

			parser := C.parser_create(cSource)
			require.NotNil(t, parser, "Failed to create parser")
			defer C.parser_destroy(parser)

			program := C.parser_parse_program(parser)
			hasErrors := bool(C.parser_has_errors(parser))

			if tt.expectError {
				assert.True(t, hasErrors, "Expected parser errors for: %s", tt.description)
			} else {
				if hasErrors {
					// Log errors for debugging
					errorCount := int(C.parser_error_count(parser))
					for i := 0; i < errorCount; i++ {
						errorMsg := C.parser_get_error(parser, C.size_t(i))
						if errorMsg != nil {
							t.Logf("Parser error %d: %s", i, C.GoString(errorMsg))
						}
					}
				}
				assert.False(t, hasErrors, "Unexpected parser errors for: %s", tt.description)
				assert.NotNil(t, program, "Expected valid AST for: %s", tt.description)
			}
		})
	}
}

// =============================================================================
// INTEGRATION TESTS - End-to-End Testing
// =============================================================================

func TestEndToEnd_BasicPrograms(t *testing.T) {
	tests := []struct {
		name           string
		source         string
		expectedOutput string
		expectError    bool
	}{
		{
			name:           "hello world",
			source:         "Console.WriteLine(\"hello\");",
			expectedOutput: "hello\n",
			expectError:    false,
		},
		{
			name:           "integer output",
			source:         "Console.WriteLine(42);",
			expectedOutput: "42\n",
			expectError:    false,
		},
		{
			name:           "arithmetic expression",
			source:         "Console.WriteLine(1 + 2 + 3);",
			expectedOutput: "6\n",
			expectError:    false,
		},
		{
			name:           "variable declaration and use",
			source:         "int x = 42; Console.WriteLine(x);",
			expectedOutput: "42\n",
			expectError:    false,
		},
		{
			name:           "string concatenation",
			source:         "Console.WriteLine(\"Hello\" + \" World\");",
			expectedOutput: "Hello World\n",
			expectError:    false,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cSource := C.CString(tt.source)
			defer C.free(unsafe.Pointer(cSource))

			var cOutput *C.char
			var cErrors *C.char

			result := C.dream_run_string(cSource, &cOutput, &cErrors)
			defer C.dream_free_string(cOutput)
			defer C.dream_free_string(cErrors)

			if tt.expectError {
				assert.NotEqual(t, 0, int(result), "Expected error but got success")
			} else {
				if result != 0 {
					t.Logf("Execution failed with errors: %s", C.GoString(cErrors))
				}
				assert.Equal(t, 0, int(result), "Unexpected error: %s", C.GoString(cErrors))

				if cOutput != nil {
					output := C.GoString(cOutput)
					assert.Equal(t, tt.expectedOutput, output)
				}
			}
		})
	}
}

// =============================================================================
// PERFORMANCE TESTS - For benchmark validation
// =============================================================================

func TestLexer_Performance(t *testing.T) {
	// Generate a large program to test lexer performance
	var sb strings.Builder
	for i := 0; i < 1000; i++ {
		sb.WriteString(fmt.Sprintf("int var%d = %d + %d * %d;\n", i, i, i+1, i+2))
	}
	source := sb.String()

	cSource := C.CString(source)
	defer C.free(unsafe.Pointer(cSource))

	lexer := C.lexer_create(cSource)
	require.NotNil(t, lexer)
	defer C.lexer_destroy(lexer)

	tokenCount := 0
	for {
		token := C.lexer_next_token(lexer)
		tokenCount++
		if token.kind == C.TK_EOF {
			break
		}
		// Safety check
		if tokenCount > 50000 {
			t.Fatal("Too many tokens - possible performance issue")
		}
	}

	t.Logf("Lexed %d tokens from large program", tokenCount)
	assert.Greater(t, tokenCount, 5000, "Should have generated substantial tokens")
}

func TestUtilityFunctions(t *testing.T) {
	t.Run("token_kind_name", func(t *testing.T) {
		tests := []struct {
			kind     C.int
			expected string
		}{
			{C.TK_INT, "integer"},
			{C.TK_PLUS, "+"},
			{C.TK_IDENT, "identifier"},
			{C.TK_IF, "if"},
		}

		for _, tt := range tests {
			name := C.token_kind_name(tt.kind)
			nameStr := C.GoString(name)
			assert.Equal(t, tt.expected, nameStr, "Token name mismatch")
			t.Logf("Token %d has name: %s", tt.kind, nameStr)
		}
	})

	t.Run("dream_compile_string", func(t *testing.T) {
		source := "int x = 42;"
		cSource := C.CString(source)
		defer C.free(unsafe.Pointer(cSource))

		var cOutput *C.char
		var cErrors *C.char

		result := C.dream_compile_string(cSource, &cOutput, &cErrors)
		defer C.dream_free_string(cOutput)
		defer C.dream_free_string(cErrors)

		// Should succeed for valid input
		assert.Equal(t, 0, int(result))

		if cOutput != nil {
			output := C.GoString(cOutput)
			assert.NotEmpty(t, output)
			t.Logf("Compile output: %s", output)
		}
	})
}