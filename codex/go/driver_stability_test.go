package dream_test

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"strings"
	"testing"
	"time"
)

// Invalid programs must fail before creating any generated build artifacts.
func TestCompiler_DiagnosticExitStatus(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	cases := []struct {
		name, source, diagnostic string
		options                  []string
	}{
		{"member_missing_receiver", "\n\nmissing.read();", "undefined variable 'missing'", nil},
		{"member_primitive_receiver", "int value = 1;\n\nvalue.read();", "method receiver must be a class or struct", nil},
		{"member_expression_receiver", "\n\n(1 + 2).read();", "method receiver must be a class or struct", nil},
		{"member_unknown_method", "class Reader {} Reader reader = new Reader();\n\nreader.read();", "unknown method 'read'", nil},
		{"member_field_called", "class Reader { int value; } Reader reader = new Reader();\n\nreader.value();", "unknown method 'value'", nil},
		{"member_type_method", "class Reader { func int read() { return 1; } }\n\nReader.read();", "instance method requires an object", nil},
		{"member_type_field_read", "class Reader { int value; }\n\nConsole.WriteLine(Reader.value);", "instance field requires an object", nil},
		{"member_type_field_write", "class Reader { int value; }\n\nReader.value = 2;", "instance field requires an object", nil},
		{"member_inherited_type_field", "class Parent { int value; } class Child : Parent {}\n\nConsole.WriteLine(Child.value);", "instance field requires an object", nil},
		{"member_nested_receiver", "class Reader { int value; } Reader reader = new Reader();\n\nreader.value.read();", "method receiver must be a class or struct", nil},
		{"member_argument_analysis", "class Reader { static func int read(int n) { return n; } }\n\nReader.read(missing);", "undefined variable 'missing'", nil},
		{"member_static_this", "class Reader { static func int read() {\n\nreturn this.read(); } }", "undefined variable 'this'", nil},
		{"member_static_base", "class Parent { func int read() { return 1; } } class Child : Parent { static func int read() {\n\nreturn base.read(); } }", "base access requires a base class and an instance method", nil},
		{"member_unknown_field", "class Reader {} Reader reader = new Reader();\n\nConsole.WriteLine(reader.missing);", "unknown field 'missing'", nil},
		{"member_primitive_field", "int value = 1;\n\nConsole.WriteLine(value.missing);", "field receiver must be a class or struct", nil},
		{"syntax", "int x = ;", "error:", nil},
		{"undefined_variable", "int x = missing + 1;", "undefined variable 'missing'", nil},
		{"const_assignment", "const int x = 42; x = 50;", "cannot assign to const variable 'x'", nil},
		{"return_int", "func int value() { return \"bad\"; }", "return expression has incompatible type: expected 'int', got 'string'", nil},
		{"return_float", "func float value() { return 1; }", "return expression has incompatible type: expected 'float', got 'int'", nil},
		{"return_char", "func char value() { return 65; }", "return expression has incompatible type: expected 'char', got 'int'", nil},
		{"return_bool", "func bool value() { return 1; }", "return expression has incompatible type: expected 'bool', got 'int'", nil},
		{"return_string", "func string value() { return 1; }", "return expression has incompatible type: expected 'string', got 'int'", nil},
		{"return_narrowing", "func int value() { return 1.5; }", "return expression has incompatible type: expected 'int', got 'float'", nil},
		{"return_bare", "func int value() { return; }", "return requires a value", nil},
		{"return_void_value", "func void value() { return 1; }", "void function cannot return an expression", nil},
		{"return_void_expression", "func void nothing() {} func void value() { return nothing(); }", "void function cannot return an expression", nil},
		{"return_void_result", "func void nothing() {} func int value() { return nothing(); }", "return expression has incompatible type: expected 'int', got 'void'", nil},
		{"return_method", "class Reader { func int value() { return \"bad\"; } }", "return expression has incompatible type", nil},
		{"return_static_bare", "class Reader { static func int value() { return; } }", "return requires a value", nil},
		{"return_switch", "func int value(int n) { switch(n) { case 1: return \"bad\"; default: return 2; } }", "return expression has incompatible type", nil},
		{"return_try", "func int value() { try { return \"bad\"; } catch { return 1; } }", "return expression has incompatible type", nil},
		{"return_catch", "func int value() { try { return 1; } catch { return \"bad\"; } }", "return expression has incompatible type", nil},
		{"return_finally", "func int value() { try { return 1; } finally { return; } }", "return requires a value", nil},
		{"return_conditional", "func int value(bool flag) { return flag ? 1 : \"bad\"; }", "conditional branches have incompatible types", nil},
		{"return_conditional_missing", "func int value(bool flag) { return flag ? 1 : missing; }", "undefined variable 'missing'", nil},
		{"return_unary_missing", "func int value() { return -missing; }", "undefined variable 'missing'", nil},
		{"promoted_warning", "int unusedValue = 42;", "variable might be unused", []string{"-Werror"}},
	}
	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			work := t.TempDir()
			source := filepath.Join(work, "input.dr")
			if err := os.WriteFile(source, []byte(tc.source), 0644); err != nil {
				t.Fatal(err)
			}
			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()
			args := append([]string{"--dev"}, tc.options...)
			cmd := exec.CommandContext(ctx, tr.compilerPath, append(args, source)...)
			cmd.Dir = work
			output, err := cmd.CombinedOutput()
			if ctx.Err() != nil {
				t.Fatalf("Compiler timed out: %s", output)
			}
			if exit, ok := err.(*exec.ExitError); !ok || exit.ExitCode() != 1 {
				t.Fatalf("Expected exit 1, got %v: %s", err, output)
			}
			if !strings.Contains(string(output), tc.diagnostic) {
				t.Fatalf("Missing diagnostic %q: %s", tc.diagnostic, output)
			}
			if strings.HasPrefix(tc.name, "member_") {
				plain := regexp.MustCompile(`\x1b+\[[0-9;]*m`).ReplaceAllString(string(output), "")
				if !regexp.MustCompile(`(?m)(^|:)3:[1-9][0-9]*: error:`).MatchString(plain) {
					t.Fatalf("Missing useful source position on line 3: %s", output)
				}
			}
			if _, err := os.Stat(filepath.Join(work, "build")); !os.IsNotExist(err) {
				t.Fatalf("Invalid program created build artifacts: %v", err)
			}
		})
	}
}

func TestCompiler_ArenaGrowth(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	var source strings.Builder
	source.WriteString("int first = 41;\n")
	for i := 0; i < 512; i++ {
		fmt.Fprintf(&source, "int value%d = %d;\n", i, i)
	}
	source.WriteString("Console.WriteLine(first + value511);\n")
	tr.RunTest(t, CompilerTestCase{
		Name: "arena_growth", Source: source.String(), ExpectSuccess: true, ExpectedOutput: "552",
		Timeout: 180 * time.Second,
	})
}

func TestCompiler_WarningsRemainNonfatal(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	tr.RunTest(t, CompilerTestCase{
		Name: "warning", Source: "int unusedValue = 42; Console.WriteLine(42);",
		ExpectSuccess: true, ExpectedOutput: "42",
	})
}
