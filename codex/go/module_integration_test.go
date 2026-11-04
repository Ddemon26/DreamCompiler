package dream_test

import (
	"context"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"runtime"
	"strings"
	"testing"
	"time"
)

// Each case builds a real source tree outside the repository's working directory.
func TestCompiler_Modules(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	cases := []struct {
		name, output, diagnostic string
		files                    map[string]string
	}{
		{"private_names", "103", "", map[string]string{
			"main.dr":  `import alpha; import beta; func int helper() { return 100; } Console.WriteLine(alpha.value() + beta.value() + helper());`,
			"alpha.dr": `module alpha; func int helper() { return 1; } export func int value() { return helper(); }`,
			"beta.dr":  `module beta; func int helper() { return 2; } export func int value() { return helper(); }`,
		}},
		{"diamond", "7", "", map[string]string{
			"main.dr":   `import left; import right; Console.WriteLine(left.value() + right.value());`,
			"left.dr":   `module left; import shared; export func int value() { return shared.value() + 1; }`,
			"right.dr":  `module right; import shared; export func int value() { return shared.value() + 2; }`,
			"shared.dr": `module shared; export func int value() { return 2; }`,
		}},
		{"forward_recursion", "1", "", map[string]string{
			"main.dr":   `import parity; Console.WriteLine(parity.even(4));`,
			"parity.dr": `module parity; export func int even(int n) { if (n == 0) return 1; return odd(n - 1); } func int odd(int n) { if (n == 0) return 0; return even(n - 1); }`,
		}},
		{"return_types", "Hello\n1.500000\nZ\n1", "", map[string]string{
			"main.dr":   `import values; Console.WriteLine(values.caption()); Console.WriteLine(values.weight()); Console.WriteLine(values.marker()); Console.WriteLine(values.ready());`,
			"values.dr": `module values; export func string caption() { return "Hello"; } export func float weight() { return 1.5; } export func char marker() { return 'Z'; } export func bool ready() { return true; }`,
		}},
		{"entry_class", "42", "", map[string]string{
			"main.dr":   `import values; class Reader { func int read() { return values.number(); } } Reader reader = new Reader(); Console.WriteLine(reader.read());`,
			"values.dr": `module values; export func int number() { return 42; }`,
		}},
		{"dependency_return_type", "", "return expression has incompatible type", map[string]string{
			"main.dr": `import values;`, "values.dr": "module values;\nexport func int value() {\nreturn \"bad\";\n}\n",
		}},
		{"dependency_return_bare", "", "return requires a value", map[string]string{
			"main.dr": `import values;`, "values.dr": "module values;\nexport func int value() {\nreturn;\n}\n",
		}},
		{"dependency_return_void", "", "void function cannot return an expression", map[string]string{
			"main.dr": `import values;`, "values.dr": "module values;\nexport func void value() {\nreturn 1;\n}\n",
		}},
		{"dependency_return_conditional", "", "conditional branches have incompatible types", map[string]string{
			"main.dr": `import values;`, "values.dr": "module values;\nexport func int value(bool flag) {\nreturn flag ? 1 : \"bad\";\n}\n",
		}},
		{"missing", "", "cannot load module 'missing'", map[string]string{"main.dr": `import missing;`}},
		{"mismatched_name", "", "module declaration must match import 'values'", map[string]string{
			"main.dr": `import values;`, "values.dr": `module different; export func int value() { return 1; }`,
		}},
		{"cycle", "", "circular import", map[string]string{
			"main.dr": `import first;`, "first.dr": `module first; import second;`, "second.dr": `module second; import first;`,
		}},
		{"private_access", "", "has no exported function 'hidden'", map[string]string{
			"main.dr": `import values; int result = values.hidden();`, "values.dr": `module values; func int hidden() { return 1; }`,
		}},
		{"argument_count", "", "expected 1 arguments but got 2", map[string]string{
			"main.dr": `import values; int result = values.echo(1, 2);`, "values.dr": `module values; export func int echo(int n) { return n; }`,
		}},
		{"argument_type", "", "argument 1 has incompatible type", map[string]string{
			"main.dr": `import values; int result = values.echo("bad");`, "values.dr": `module values; export func int echo(int n) { return n; }`,
		}},
		{"dependency_error", "", "undefined variable 'missing'", map[string]string{
			"main.dr": `import values; int result = values.value();`, "values.dr": "module values;\nexport func int value() {\nreturn missing;\n}\n",
		}},
		{"dependency_parse_error", "", "expected expression", map[string]string{
			"main.dr": `import values;`, "values.dr": `module values; export func int value() { int n = ; return n; }`,
		}},
		{"unsupported_export", "", "only functions can be exported", map[string]string{
			"main.dr": `import values;`, "values.dr": `module values; export int value = 1;`,
		}},
		{"duplicate_module", "", "multiple module declarations", map[string]string{
			"main.dr": `import values;`, "values.dr": `module values; module values;`,
		}},
		{"alias_shadow", "", "conflicts with a module function or import", map[string]string{
			"main.dr": `import values; int values = 1;`, "values.dr": `module values; export func int value() { return 1; }`,
		}},
	}
	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			work := t.TempDir()
			for name, source := range tc.files {
				if err := os.WriteFile(filepath.Join(work, name), []byte(source), 0644); err != nil {
					t.Fatal(err)
				}
			}
			ctx, cancel := context.WithTimeout(context.Background(), 180*time.Second)
			defer cancel()
			args := []string{"--multi-file"}
			if tc.diagnostic != "" {
				args = append(args, "--dev")
			}
			args = append(args, filepath.Join(work, "main.dr"))
			cmd := exec.CommandContext(ctx, tr.compilerPath, args...)
			cmd.Dir = tr.projectRoot
			if tc.diagnostic != "" {
				cmd.Dir = work
			}
			cmd.Env = append(os.Environ(), "CC=zig cc")
			output, err := cmd.CombinedOutput()
			if ctx.Err() != nil {
				t.Fatalf("Compiler timed out: %s", output)
			}
			if tc.diagnostic != "" {
				if exit, ok := err.(*exec.ExitError); !ok || exit.ExitCode() != 1 {
					t.Fatalf("Expected exit 1, got %v: %s", err, output)
				}
				if !strings.Contains(string(output), tc.diagnostic) {
					t.Fatalf("Missing %q: %s", tc.diagnostic, output)
				}
				plainOutput := regexp.MustCompile(`\x1b+\[[0-9;]*m`).ReplaceAllString(string(output), "")
				if (tc.name == "dependency_error" || strings.HasPrefix(tc.name, "dependency_return_")) && !strings.Contains(plainOutput, "values.dr:3:") {
					t.Fatalf("Missing source file: %s", output)
				}
				if _, err := os.Stat(filepath.Join(work, "build")); !os.IsNotExist(err) {
					t.Fatalf("Rejected program produced artifacts: %v", err)
				}
				return
			}
			if err != nil {
				t.Fatalf("Compilation failed: %v\n%s", err, output)
			}
			executable := "dream"
			if runtime.GOOS == "windows" {
				executable += ".exe"
			}
			run := exec.CommandContext(ctx, filepath.Join(tr.projectRoot, executable))
			run.Dir = tr.projectRoot
			actual, err := run.CombinedOutput()
			if err != nil {
				t.Fatalf("Program failed: %v\n%s", err, actual)
			}
			if got := strings.TrimSpace(strings.ReplaceAll(string(actual), "\r\n", "\n")); got != tc.output {
				t.Fatalf("Expected %q, got %q", tc.output, got)
			}
		})
	}
}
