package dream_test

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"runtime"
	"strings"
	"testing"
	"time"
)

// Exercise the real receiver lookup in ordinary and module-entry sources.
func TestCompiler_MethodArguments(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	type rejection struct {
		name, source, diagnostic string
	}
	var cases []rejection
	primitives := []struct{ name, value string }{
		{"int", "1"}, {"float", "1.5"}, {"bool", "true"},
		{"char", "'Z'"}, {"string", `"label"`},
	}
	diagnostic := func(index int, method, expected, actual string) string {
		return fmt.Sprintf("argument %d has incompatible type for method '%s': expected '%s', got '%s'", index, method, expected, actual)
	}
	for _, expected := range primitives {
		for _, actual := range primitives {
			if expected.name == actual.name {
				continue
			}
			cases = append(cases, rejection{
				expected.name + "_from_" + actual.name,
				fmt.Sprintf("class Reader { func void take(%s value) {} } Reader reader = new Reader();\n\nreader.take(%s);", expected.name, actual.value),
				diagnostic(1, "take", expected.name, actual.name),
			})
		}
	}
	cases = append(cases,
		rejection{"static", "class Reader { static func void take(int n) {} }\n\nReader.take(1.5);", diagnostic(1, "take", "int", "float")},
		rejection{"inherited_instance", "class Parent { func void take(int n) {} } class Child : Parent {} Child child = new Child();\n\nchild.take(1.5);", diagnostic(1, "take", "int", "float")},
		rejection{"inherited_static", "class Parent { static func void take(float n) {} } class Child : Parent {}\n\nChild.take(1);", diagnostic(1, "take", "float", "int")},
		rejection{"static_through_object", "class Parent { static func void take(bool n) {} } class Child : Parent {} Child child = new Child();\n\nchild.take(1);", diagnostic(1, "take", "bool", "int")},
		rejection{"base", "class Parent { func void take(char n) {} } class Child : Parent { func void check() {\n\nbase.take(1); } }", diagnostic(1, "take", "char", "int")},
		rejection{"this", "class Reader { func void take(string n) {} func void check() {\n\nthis.take(1); } }", diagnostic(1, "take", "string", "int")},
		rejection{"second_argument", "class Reader { func void take(int n, bool flag) {} } Reader reader = new Reader();\n\nreader.take(1, 2);", diagnostic(2, "take", "bool", "int")},
		rejection{"pointer_parameter_deferred", "class Reader { func void take(int* p, int n) {} } Reader reader = new Reader();\n\nreader.take(null, 1.5);", diagnostic(2, "take", "int", "float")},
		rejection{"nested_call", "func float value() { return 1.5; } class Reader { func void take(int n) {} } Reader reader = new Reader();\n\nreader.take(value());", diagnostic(1, "take", "int", "float")},
		rejection{"nested_method", "class Reader { func float value() { return 1.5; } func void take(int n) {} } Reader reader = new Reader();\n\nreader.take(reader.value());", diagnostic(1, "take", "int", "float")},
		rejection{"nested_invalid_argument", "class Reader { func int value(int n) { return n; } func void take(int n) {} } Reader reader = new Reader();\n\nreader.take(reader.value(1.5));", diagnostic(1, "value", "int", "float")},
		rejection{"conditional", "class Reader { func void take(int n) {} } Reader reader = new Reader();\n\nreader.take(true ? 1 : 1.5);", diagnostic(1, "take", "int", "float")},
		rejection{"void_result", "func void nothing() {} class Reader { func void take(int n) {} } Reader reader = new Reader();\n\nreader.take(nothing());", diagnostic(1, "take", "int", "void")},
		rejection{"null_scalar", "class Reader { func void take(string n) {} } Reader reader = new Reader();\n\nreader.take(null);", diagnostic(1, "take", "string", "void")},
		rejection{"explicit_init", "class Reader { func void init(int n) {} } Reader reader = new Reader(1);\n\nreader.init(1.5);", diagnostic(1, "init", "int", "float")},
		rejection{"override", "class Parent { func void take(int n) {} } class Child : Parent { func void take(string n) {} } Child child = new Child();\n\nchild.take(1);", diagnostic(1, "take", "string", "int")},
		rejection{"declared_receiver", "class Parent { func void take(int n) {} } class Child : Parent { func void take(string n) {} } Parent parent = new Child();\n\nparent.take(\"bad\");", diagnostic(1, "take", "int", "string")},
		rejection{"struct", "struct Reader { static func void take(int n) {} }\n\nReader.take(1.5);", diagnostic(1, "take", "int", "float")},
		rejection{"too_few", "class Reader { func void take(int n) {} } Reader reader = new Reader();\n\nreader.take();", "incorrect number of method arguments"},
		rejection{"too_many", "class Reader { func void take(int n) {} } Reader reader = new Reader();\n\nreader.take(1, 2);", "incorrect number of method arguments"},
		rejection{"extra_argument_analyzed", "class Reader { func void take(int n) {} } Reader reader = new Reader();\n\nreader.take(1, missing);", "undefined variable 'missing'"},
	)
	plainText := regexp.MustCompile(`\x1b+\[[0-9;]*m`)
	position := regexp.MustCompile(`(?m)(^|:)3:[1-9][0-9]*: error:`)
	program, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests", "advanced", "oop", "method_arguments.dr"))
	if err != nil {
		t.Fatal(err)
	}
	for _, modules := range []bool{false, true} {
		mode := "ordinary"
		if modules {
			mode = "module_entry"
		}
		t.Run(mode, func(t *testing.T) {
			for _, tc := range cases {
				t.Run(tc.name, func(t *testing.T) {
					work := t.TempDir()
					text := tc.source
					args := []string{"--dev"}
					if modules {
						text = "import values; " + text // Preserve the checked line 3.
						args = append(args, "--multi-file")
						if err := os.WriteFile(filepath.Join(work, "values.dr"), []byte("module values;"), 0644); err != nil {
							t.Fatal(err)
						}
					}
					source := filepath.Join(work, "input.dr")
					if err := os.WriteFile(source, []byte(text), 0644); err != nil {
						t.Fatal(err)
					}
					ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
					defer cancel()
					cmd := exec.CommandContext(ctx, tr.compilerPath, append(args, source)...)
					cmd.Dir = work
					output, err := cmd.CombinedOutput()
					if ctx.Err() != nil {
						t.Fatalf("Compiler timed out: %s", output)
					}
					if exit, ok := err.(*exec.ExitError); !ok || exit.ExitCode() != 1 {
						t.Fatalf("Expected exit 1, got %v: %s", err, output)
					}
					plain := plainText.ReplaceAllString(string(output), "")
					if !strings.Contains(plain, tc.diagnostic) || !position.MatchString(plain) {
						t.Fatalf("Missing diagnostic %q at line 3: %s", tc.diagnostic, output)
					}
					if modules && !strings.Contains(plain, "input.dr:3:") {
						t.Fatalf("Missing entry source file: %s", output)
					}
					if strings.HasPrefix(tc.diagnostic, "argument ") && strings.Count(plain, "has incompatible type for method") != 1 {
						t.Fatalf("Unexpected additional argument diagnostics: %s", output)
					}
					if _, err := os.Stat(filepath.Join(work, "build")); !os.IsNotExist(err) {
						t.Fatalf("Rejected program created artifacts: %v", err)
					}
				})
			}
			t.Run("valid", func(t *testing.T) {
				work := t.TempDir()
				text := string(program)
				want := "42\n1.500000\n1\nZ\nlabel\n7\n8\npair\n9\n10\n2.500000"
				args := []string{}
				if modules {
					text = "import values;\n" + text + "\nConsole.WriteLine(child.integer(values.number()));\n"
					want += "\n43"
					args = append(args, "--multi-file")
					if err := os.WriteFile(filepath.Join(work, "values.dr"), []byte("module values; export func int number() { return 43; }"), 0644); err != nil {
						t.Fatal(err)
					}
				}
				source := filepath.Join(work, "input.dr")
				if err := os.WriteFile(source, []byte(text), 0644); err != nil {
					t.Fatal(err)
				}
				ctx, cancel := context.WithTimeout(context.Background(), 180*time.Second)
				defer cancel()
				cmd := exec.CommandContext(ctx, tr.compilerPath, append(args, source)...)
				cmd.Dir = tr.projectRoot
				cmd.Env = append(os.Environ(), "CC=zig cc")
				output, err := cmd.CombinedOutput()
				if err != nil {
					t.Fatalf("Valid compilation failed: %v\n%s", err, output)
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
				if got := strings.TrimSpace(strings.ReplaceAll(string(actual), "\r\n", "\n")); got != want {
					t.Fatalf("Expected %q, got %q", want, got)
				}
			})
		})
	}
}
