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

// Exercise both driver parsing and the dependency loader, with bounded failure
// and a fresh working directory so rejected input cannot leave artifacts.
func TestCompiler_ParserRecovery(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	type rejection struct {
		name, source, diagnostic string
		line                     int
		later                    []string
	}
	cases := []rejection{
		{"custom_constructor", "class Child {}\nclass Reader {\nfunc init(Child child) {}\n}", "expected parameter type", 3, nil},
		{"custom_function", "class Child {}\n\nfunc read(Child child) {}", "expected parameter type", 3, nil},
		{"unknown_custom_type", "\n\nfunc read(Unknown child) {}", "expected parameter type", 3, nil},
		{"custom_pointer", "class Child {}\n\nfunc read(Child* child) {}", "expected parameter type", 3, nil},
		{"second_custom_parameter", "class Child {}\n\nfunc read(int first, Child child) {}", "expected parameter type", 3, nil},
		{"missing_name", "\n\nfunc read(int) {}", "expected parameter name", 3, nil},
		{"duplicate_comma", "\n\nfunc read(int first,, int second) {}", "expected parameter type", 3, nil},
		{"missing_comma", "\n\nfunc read(int first int second) {}", "expected ')'", 3, nil},
		{"array_parameter", "\n\nfunc read(int items[]) {}", "expected ')'", 3, nil},
		{"nested_parentheses", "class Child {}\n\nfunc read(Child (child)) {}", "expected parameter type", 3, nil},
		{"type_at_eof", "\n\nfunc read(Child", "expected parameter type", 3, nil},
		{"name_at_eof", "\n\nfunc read(int\n", "expected parameter name", 4, nil},
		{"missing_close", "\n\nfunc read(int child {}", "expected ')'", 3, nil},
		{"enclosing_brace", "class Reader {\n\nfunc read(int }", "expected parameter name", 3, nil},
		{"following_declarations", "class Child {}\nclass Reader {\nfunc init(Child child) { if (true) { Console.WriteLine(1); } }\nfunc next(int) {}\n}\nfunc after(int) {}", "expected parameter type", 3, []string{"4:14: error: expected parameter name", "6:15: error: expected parameter name"}},
	}
	// These delimiters are synchronization boundaries for expressions. A failed
	// statement must consume one rather than retry it forever in its enclosing loop.
	for _, delimiter := range []string{",", ")", "]", "}"} {
		cases = append(cases, rejection{"top_level_" + fmt.Sprintf("%x", delimiter), "\n\n" + delimiter, "expected expression", 3, nil})
		if delimiter != "}" { // A closing block brace belongs to the caller.
			cases = append(cases,
				rejection{"block_" + fmt.Sprintf("%x", delimiter), "{\n\n" + delimiter + " }", "expected expression", 3, nil},
				rejection{"switch_" + fmt.Sprintf("%x", delimiter), "switch (1) { case 1:\n\n" + delimiter + " }", "expected expression", 3, nil})
		}
	}
	plainText := regexp.MustCompile(`\x1b+\[[0-9;]*m`)
	for _, mode := range []string{"ordinary", "module_entry", "module_dependency"} {
		t.Run(mode, func(t *testing.T) {
			for _, tc := range cases {
				t.Run(tc.name, func(t *testing.T) {
					work := t.TempDir()
					text := tc.source
					args := []string{"--dev"}
					input := filepath.Join(work, "input.dr")
					if mode != "ordinary" {
						args = append(args, "--multi-file")
						if mode == "module_entry" {
							text = "import values; " + text // Keep diagnostic line numbers.
							if err := os.WriteFile(filepath.Join(work, "values.dr"), []byte("module values;"), 0644); err != nil {
								t.Fatal(err)
							}
						} else {
							if err := os.WriteFile(filepath.Join(work, "values.dr"), []byte("module values; "+text), 0644); err != nil {
								t.Fatal(err)
							}
							text = "import values;"
						}
					}
					if err := os.WriteFile(input, []byte(text), 0644); err != nil {
						t.Fatal(err)
					}
					ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
					defer cancel()
					cmd := exec.CommandContext(ctx, tr.compilerPath, append(args, input)...)
					cmd.Dir = work
					output, err := cmd.CombinedOutput()
					if ctx.Err() != nil {
						t.Fatalf("Compiler timed out: %s", output)
					}
					if exit, ok := err.(*exec.ExitError); !ok || exit.ExitCode() != 1 {
						t.Fatalf("Expected exit 1, got %v: %s", err, output)
					}
					plain := plainText.ReplaceAllString(string(output), "")
					position := regexp.MustCompile(fmt.Sprintf(`(?m)(^|:)%d:[1-9][0-9]*: error:`, tc.line))
					if !strings.Contains(plain, tc.diagnostic) || !position.MatchString(plain) {
						t.Fatalf("Missing diagnostic %q at line %d: %s", tc.diagnostic, tc.line, output)
					}
					if mode == "module_dependency" && !strings.Contains(plain, "values.dr:") {
						t.Fatalf("Missing dependency source path: %s", output)
					}
					if strings.Contains(tc.source, "Child") || strings.Contains(tc.source, "Unknown") {
						if !strings.Contains(plain, "Custom-type parameters are not supported") || strings.Count(plain, "error: expected parameter type") != 1 {
							t.Fatalf("Expected one custom-parameter diagnostic with a useful hint: %s", output)
						}
					}
					for _, later := range tc.later {
						if !strings.Contains(plain, later) {
							t.Fatalf("Recovery lost following declaration diagnostic %q: %s", later, output)
						}
					}
					entries, err := os.ReadDir(work)
					if err != nil {
						t.Fatal(err)
					}
					for _, entry := range entries {
						if entry.IsDir() || (entry.Name() != "input.dr" && entry.Name() != "values.dr") {
							t.Fatalf("Rejected program created artifact %q", entry.Name())
						}
					}
				})
			}
		})
	}
}
