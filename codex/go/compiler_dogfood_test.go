package dream_test

import (
	"context"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"testing"
	"time"
)

func TestCompiler_Dogfood(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	work := t.TempDir()
	run := func(t *testing.T, program, dir string, args ...string) ([]byte, error) {
		t.Helper()
		ctx, cancel := context.WithTimeout(context.Background(), 60*time.Second)
		defer cancel()
		cmd := exec.CommandContext(ctx, program, args...)
		cmd.Dir = dir
		// Emission must not invoke a host compiler or require runtime initialization.
		cmd.Env = append(os.Environ(), "CC=dream-library-mode-must-not-invoke-cc")
		out, err := cmd.CombinedOutput()
		if ctx.Err() != nil {
			t.Fatalf("command timed out: %s %v\n%s", program, args, out)
		}
		return out, err
	}
	mustRun := func(t *testing.T, program, dir string, args ...string) []byte {
		t.Helper()
		out, err := run(t, program, dir, args...)
		if err != nil {
			t.Fatalf("%s %v: %v\n%s", program, args, err, out)
		}
		return out
	}
	helper := filepath.Join(tr.projectRoot, "src", "driver", "source_text.dr")
	generated := filepath.Join(work, "source text.c")
	mustRun(t, tr.compilerPath, work, "--emit-library-c", generated, helper)
	data, err := os.ReadFile(generated)
	if err != nil {
		t.Fatal(err)
	}
	c := strings.Join(strings.Fields(string(data)), " ")
	if !strings.Contains(c, "void dream_normalize_newlines(char* buffer)") ||
		strings.Contains(c, "static void dream_normalize_newlines") ||
		strings.Contains(c, "int main(") {
		t.Fatalf("library lost external pointer signature or emitted an entry point:\n%s", c)
	}
	bootstrap := filepath.Join(tr.projectRoot, "zig-out", "bin", "dreamc-bootstrap")
	if runtime.GOOS == "windows" {
		bootstrap += ".exe"
	}
	seedC := filepath.Join(work, "seed emission.c")
	mustRun(t, bootstrap, work, "--emit-library-c", seedC, helper)
	seedData, err := os.ReadFile(seedC)
	if err != nil || string(seedData) != string(data) {
		t.Fatalf("bootstrap and production helper emission differ: %v", err)
	}
	forwardSource := filepath.Join(work, "forward.dr")
	forwardC := filepath.Join(work, "forward.c")
	if err := os.WriteFile(forwardSource, []byte("func int dogfood_caller(int n) { return dogfood_twice(n); }\nfunc int dogfood_twice(int n) { return n * 2; }"), 0644); err != nil {
		t.Fatal(err)
	}
	mustRun(t, tr.compilerPath, work, "--emit-library-c", forwardC, forwardSource)

	// Link as separate translation units. The seed symbol cannot satisfy the
	// production call: it is renamed before compilation and checked independently.
	for _, target := range []string{"native", "linux"} {
		if target == "linux" && runtime.GOOS == "linux" {
			continue // Native CI executes this target rather than merely cross-linking it.
		}
		t.Run(target, func(t *testing.T) {
			options := []string{"cc", "-std=c11", "-D_GNU_SOURCE", "-fsanitize=undefined"}
			if target == "linux" {
				options = append(options, "-target", "x86_64-linux-gnu")
			}
			object := filepath.Join(work, target+"-seed.o")
			args := append(append([]string{}, options...), "-Ddream_normalize_newlines=dream_normalize_newlines_seed", "-c",
				filepath.Join(tr.projectRoot, "src", "driver", "source_text_bootstrap.c"), "-o", object)
			mustRun(t, "zig", tr.projectRoot, args...)
			binary := filepath.Join(work, target+"-normalizer")
			if runtime.GOOS == "windows" && target == "native" {
				binary += ".exe"
			}
			args = append(append([]string{}, options...), "-I"+filepath.Join(tr.projectRoot, "src", "driver"))
			for _, part := range []string{"io", "memory", "extensions", "exceptions", "system"} {
				args = append(args, "-I"+filepath.Join(tr.runtimeLibPath, part))
			}
			args = append(args, generated, forwardC, object, filepath.Join(tr.projectRoot, "codex", "c", "source_text_harness.c"))
			for _, source := range []string{"io/console.c", "memory/memory.c", "extensions/custom.c", "exceptions/exception.c"} {
				args = append(args, filepath.Join(tr.runtimeLibPath, source))
			}
			args = append(args, "-o", binary)
			mustRun(t, "zig", tr.projectRoot, args...)
			if target == "native" {
				if got := strings.ReplaceAll(string(mustRun(t, binary, work)), "\r\n", "\n"); got != "97912 normalization cases passed\n" {
					t.Fatalf("unexpected harness output: %q", got)
				}
			}
		})
	}

	t.Run("rejections", func(t *testing.T) {
		for _, tc := range []struct{ name, source, diagnostic string }{
			{"statement", "Console.WriteLine(1);", "requires synchronous function declarations without main"},
			{"global", "int global = 1;", "requires synchronous function declarations without main"},
			{"class", "class Reader {}", "requires synchronous function declarations without main"},
			{"main", "func int main() { return 0; }", "requires synchronous function declarations without main"},
			{"async", "async func int value() { return 1; }", "requires synchronous function declarations without main"},
			{"semantic", "func int value() { return missing; }", "undefined variable 'missing'"},
			{"syntax", "func int value() { return ; }", "return requires a value"},
		} {
			t.Run(tc.name, func(t *testing.T) {
				dir := t.TempDir()
				source := filepath.Join(dir, "input.dr")
				if err := os.WriteFile(source, []byte(tc.source), 0644); err != nil {
					t.Fatal(err)
				}
				output := filepath.Join(dir, "output.c")
				out, err := run(t, tr.compilerPath, dir, "--emit-library-c", output, source)
				if exit, ok := err.(*exec.ExitError); !ok || exit.ExitCode() != 1 || !strings.Contains(string(out), tc.diagnostic) {
					t.Fatalf("wrong rejection: %v\n%s", err, out)
				}
				if _, err := os.Stat(output); !os.IsNotExist(err) {
					t.Fatalf("rejected source produced C output: %v", err)
				}
				if _, err := os.Stat(filepath.Join(dir, "build")); !os.IsNotExist(err) {
					t.Fatalf("library emission created shared build artifacts: %v", err)
				}
				if err := os.WriteFile(output, []byte("preserve existing output"), 0644); err != nil {
					t.Fatal(err)
				}
				if _, err := run(t, tr.compilerPath, dir, "--emit-library-c", output, source); err == nil {
					t.Fatal("invalid source succeeded on repeat")
				}
				if data, err := os.ReadFile(output); err != nil || string(data) != "preserve existing output" {
					t.Fatalf("rejection changed existing output: %v %q", err, data)
				}
			})
		}
	})
	t.Run("entry_newlines", func(t *testing.T) {
		// Exercise the installed compiler's normalizer, not only regenerated C.
		for _, ending := range []string{"\n", "\r\n", "\r\r\n"} {
			source := filepath.Join(work, "entry.dr")
			program := "func int answer() {" + ending + "return 42;" + ending + "}" + ending
			if err := os.WriteFile(source, []byte(program), 0644); err != nil {
				t.Fatal(err)
			}
			mustRun(t, tr.compilerPath, work, "--emit-library-c", filepath.Join(work, "entry.c"), source)
		}
	})
	t.Run("output_errors", func(t *testing.T) {
		for _, args := range [][]string{
			{"--emit-library-c"},
			{"--emit-library-c", "--dev", helper},
			{"--emit-library-c", generated, "--emit-library-c", generated, helper},
			{"--emit-library-c", generated, "--multi-file", helper},
			{"--emit-library-c", generated, "--emit-obj", helper},
			{"--emit-library-c", filepath.Join(work, "missing", "output.c"), helper},
		} {
			if out, err := run(t, tr.compilerPath, work, args...); err == nil {
				t.Fatalf("invalid output request succeeded: %v\n%s", args, out)
			}
		}
	})
	if _, err := os.Stat(filepath.Join(work, "build")); !os.IsNotExist(err) {
		t.Fatalf("library mode created shared build artifacts: %v", err)
	}
}
