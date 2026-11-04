package dream_test

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
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
		{"syntax", "int x = ;", "error:", nil},
		{"undefined_variable", "int x = missing + 1;", "undefined variable 'missing'", nil},
		{"const_assignment", "const int x = 42; x = 50;", "cannot assign to const variable 'x'", nil},
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
