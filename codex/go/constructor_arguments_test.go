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

func TestCompiler_ConstructorArguments(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	type rejection struct{ name, source, diagnostic string }
	var cases []rejection
	primitives := []struct{ name, value string }{
		{"int", "1"}, {"float", "1.5"}, {"bool", "true"},
		{"char", "'Z'"}, {"string", `"label"`},
	}
	diagnostic := func(index int, expected, actual string) string {
		return fmt.Sprintf("argument %d has incompatible type for constructor 'Reader': expected '%s', got '%s'", index, expected, actual)
	}
	for _, expected := range primitives {
		for _, actual := range primitives {
			if expected.name != actual.name {
				cases = append(cases, rejection{
					expected.name + "_from_" + actual.name,
					fmt.Sprintf("class Reader { func init(%s n) {} }\n\nReader reader = new Reader(%s);", expected.name, actual.value),
					diagnostic(1, expected.name, actual.name),
				})
			}
		}
	}
	cases = append(cases,
		rejection{"too_few", "class Reader { func init(int n) {} }\n\nReader reader = new Reader();", "expected 1 arguments but got 0 for constructor 'Reader'"},
		rejection{"too_many", "class Reader { func init(int n) {} }\n\nReader reader = new Reader(1, 2);", "expected 1 arguments but got 2 for constructor 'Reader'"},
		rejection{"default_class", "class Reader {}\n\nReader reader = new Reader(1);", "expected 0 arguments but got 1 for constructor 'Reader'"},
		rejection{"default_struct", "struct Reader {}\n\nReader reader = new Reader(1);", "expected 0 arguments but got 1 for constructor 'Reader'"},
		rejection{"struct_init_not_called", "struct Reader { func init(int n) {} }\n\nReader reader = new Reader(1);", "expected 0 arguments but got 1 for constructor 'Reader'"},
		rejection{"inherited_init_not_called", "class Parent { func init(int n) {} } class Reader : Parent {}\n\nReader reader = new Reader(1);", "expected 0 arguments but got 1 for constructor 'Reader'"},
		rejection{"local_init_selected", "class Parent { func init(string n) {} } class Reader : Parent { func init(int n) {} }\n\nReader reader = new Reader(\"bad\");", diagnostic(1, "int", "string")},
		rejection{"static_init", "class Reader { static func init(int n) {} }\n\nReader reader = new Reader(1);", "constructor 'Reader' requires an instance init method"},
		rejection{"init_field", "class Reader { int init; }\n\nReader reader = new Reader(1);", "expected 0 arguments but got 1 for constructor 'Reader'"},
		rejection{"second_argument", "class Reader { func init(int n, bool flag) {} }\n\nReader reader = new Reader(1, 2);", diagnostic(2, "bool", "int")},
		rejection{"nested_function", "func float value() { return 1.5; } class Reader { func init(int n) {} }\n\nReader reader = new Reader(value());", diagnostic(1, "int", "float")},
		rejection{"nested_method", "class Reader { func init(int n) {} static func float value() { return 1.5; } }\n\nReader reader = new Reader(Reader.value());", diagnostic(1, "int", "float")},
		rejection{"nested_invalid_method", "class Reader { func init(int n) {} static func int value(int n) { return n; } }\n\nReader reader = new Reader(Reader.value(1.5));", "argument 1 has incompatible type for method 'value': expected 'int', got 'float'"},
		rejection{"nested_invalid_constructor", "class Child { int value; func init(int n) {} } class Reader { func init(int n) {} }\n\nReader reader = new Reader(new Child(1.5).value);", "argument 1 has incompatible type for constructor 'Child': expected 'int', got 'float'"},
		rejection{"conditional", "class Reader { func init(int n) {} }\n\nReader reader = new Reader(true ? 1 : 1.5);", diagnostic(1, "int", "float")},
		rejection{"void_result", "func void nothing() {} class Reader { func init(int n) {} }\n\nReader reader = new Reader(nothing());", diagnostic(1, "int", "void")},
		rejection{"null_scalar", "class Reader { func init(string n) {} }\n\nReader reader = new Reader(null);", diagnostic(1, "string", "void")},
		rejection{"pointer_parameter_deferred", "class Reader { func init(int* p, int n) {} }\n\nReader reader = new Reader(null, 1.5);", diagnostic(2, "int", "float")},
		rejection{"extra_argument_analyzed", "class Reader { func init(int n) {} }\n\nReader reader = new Reader(1, missing);", "undefined variable 'missing'"},
		rejection{"default_argument_analyzed", "class Reader {}\n\nReader reader = new Reader(missing);", "undefined variable 'missing'"},
	)
	plainText := regexp.MustCompile(`\x1b+\[[0-9;]*m`)
	position := regexp.MustCompile(`(?m)(^|:)3:[1-9][0-9]*: error:`)
	program, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests", "advanced", "oop", "constructor_arguments.dr"))
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
						text = "import values; " + text // Preserve line 3.
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
					if strings.HasPrefix(tc.diagnostic, "argument ") && strings.Count(plain, "has incompatible type for") != 1 {
						t.Fatalf("Argument analyzed more than once: %s", output)
					}
					if strings.Contains(tc.diagnostic, "undefined variable") && strings.Count(plain, tc.diagnostic) != 1 {
						t.Fatalf("Argument analyzed more than once: %s", output)
					}
					if _, err := os.Stat(filepath.Join(work, "build")); !os.IsNotExist(err) {
						t.Fatalf("Rejected program created artifacts: %v", err)
					}
				})
			}
			t.Run("valid", func(t *testing.T) {
				work := t.TempDir()
				text := string(program)
				want := "42\n2.500000\n1\nZ\nlabel\n8\n9\n10\n11\n12\n13\n2"
				args := []string{}
				if modules {
					text = "import values;\n" + text + "\nCounter imported = new Counter(values.number()); Console.WriteLine(imported.value);\n"
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
