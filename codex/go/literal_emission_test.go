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

// Inspect C before invoking the host compiler: runtime output alone could hide
// a missing Dream transformation behind the host's -O2/-O3 optimizer.
func TestCompiler_LiteralEmission(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	work := t.TempDir()
	source := filepath.Join(work, "main.dr")
	program := `import values;
func int freeValue() { return (8 * 7) + 1; }
func int effect() { Console.WriteLine("effect"); return 5; }
class Reader { func int read() { return (9 * 7) + 2; } }
int result = (6 * 7) + (10 - 3);
int nested = ((2 + 3) * (4 + 5)) + ((7 - 2) * (3 + 1));
int zero = (9 - 9) * (7 + 2);
int boundary = 2147483645 + 1;
int negativeIntermediate = (1 - 2) + 3;
int unary = -(2 + 3);
int leading = 010 + 1;
int division = (6 * 7) / 2;
int remainder = (9 + 4) % 5;
int shift = (2 + 3) << 1;
int seed = 4;
int identifier = seed + (2 * 3);
int called = effect() + (2 * 3);
int incremented = seed++ + (2 * 3);
int assigned = (seed = 7) + (2 * 3);
float mixed = 1.5 + (2 * 3);
Reader reader = new Reader();
Console.WriteLine(result); Console.WriteLine(nested); Console.WriteLine(zero);
Console.WriteLine(boundary); Console.WriteLine(freeValue());
Console.WriteLine(reader.read()); Console.WriteLine(values.value());
Console.WriteLine(negativeIntermediate); Console.WriteLine(unary);
Console.WriteLine(leading); Console.WriteLine(division); Console.WriteLine(remainder);
Console.WriteLine(shift); Console.WriteLine(identifier); Console.WriteLine(called);
Console.WriteLine(incremented); Console.WriteLine(assigned); Console.WriteLine(seed);
Console.WriteLine(mixed); Console.WriteLine("a" + "b");
`
	if err := os.WriteFile(source, []byte(program), 0644); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(filepath.Join(work, "values.dr"), []byte(`module values;
export func int value() { return (10 * 7) + 3; }`), 0644); err != nil {
		t.Fatal(err)
	}
	command := func(dir string, args ...string) []byte {
		t.Helper()
		ctx, cancel := context.WithTimeout(context.Background(), 180*time.Second)
		defer cancel()
		cmd := exec.CommandContext(ctx, tr.compilerPath, args...)
		cmd.Dir = dir
		cmd.Env = append(os.Environ(), "CC=zig cc")
		out, err := cmd.CombinedOutput()
		if err != nil {
			t.Fatalf("compiler %v failed: %v\n%s", args, err, out)
		}
		return out
	}
	readC := func() string {
		t.Helper()
		data, err := os.ReadFile(filepath.Join(tr.projectRoot, "build", "bin", "dream.c"))
		if err != nil {
			t.Fatal(err)
		}
		return strings.Join(strings.Fields(string(data)), " ")
	}
	initializer := func(c, name string) string {
		t.Helper()
		match := regexp.MustCompile(`\b` + name + `\s*=\s*([^;]+);`).FindStringSubmatch(c)
		if len(match) != 2 {
			t.Fatalf("missing initializer for %s", name)
		}
		return match[1]
	}
	var original string
	for _, level := range []string{"default", "-O1", "-O2", "-O3"} {
		t.Run(level, func(t *testing.T) {
			options := []string{"--multi-file"}
			if level != "default" {
				options = append(options, level)
			}
			args := append(append([]string{}, options...), "--dev", source)
			command(tr.projectRoot, args...)
			c := readC()
			if level == "default" {
				original = c
			}
			for name, value := range map[string]string{"result": "49", "nested": "65", "zero": "0", "boundary": "2147483646"} {
				got := initializer(c, name)
				if level == "default" {
					if got == value || !strings.ContainsAny(got, "+-*") {
						t.Fatalf("default %s unexpectedly folded: %s", name, got)
					}
				} else if got != value {
					t.Fatalf("%s: expected literal %s, got %s", name, value, got)
				}
			}
			for _, name := range []string{"negativeIntermediate", "unary", "leading", "division", "remainder", "shift", "identifier", "called", "incremented", "assigned", "mixed"} {
				if got, want := initializer(c, name), initializer(original, name); got != want {
					t.Fatalf("fallback %s changed: %s => %s", name, want, got)
				}
			}
			if level != "default" {
				for _, value := range []string{"57", "65", "73"} {
					if !strings.Contains(c, "return "+value+";") {
						t.Fatalf("function/method/module return %s not folded", value)
					}
				}
				if !strings.Contains(c, "values.dr") {
					t.Fatal("lost imported source mapping")
				}
			}
			command(tr.projectRoot, append(append([]string{}, options...), source)...)
			executable := "dream"
			if runtime.GOOS == "windows" {
				executable += ".exe"
			}
			ctx, cancel := context.WithTimeout(context.Background(), 20*time.Second)
			defer cancel()
			run := exec.CommandContext(ctx, filepath.Join(tr.projectRoot, executable))
			run.Dir = tr.projectRoot
			out, err := run.CombinedOutput()
			want := "effect\n49\n65\n0\n2147483646\n57\n65\n73\n2\n-5\n9\n21\n3\n10\n10\n11\n10\n13\n7\n7.500000\nab\n"
			if got := strings.ReplaceAll(string(out), "\r\n", "\n"); err != nil || got != want {
				t.Fatalf("runtime: %v\ngot %q\nwant %q", err, got, want)
			}
		})
	}

	// Undefined source is inspected only, never executed. Large balanced/deep
	// expressions must retain the entire tree rather than fold a prefix.
	var balanced func(int) string
	balanced = func(depth int) string {
		if depth == 0 {
			return "1"
		}
		return "(" + balanced(depth-1) + "+" + balanced(depth-1) + ")"
	}
	deep := "1"
	for i := 0; i < 66; i++ {
		deep = "(" + deep + "+1)"
	}
	acceptedDepth := "1"
	for i := 0; i < 64; i++ {
		acceptedDepth = "(" + acceptedDepth + "+1)"
	}
	unsafeSource := filepath.Join(work, "fallback.dr")
	fallback := fmt.Sprintf("int maxLeaf = 2147483647 + 0; int overflow = 2147483646 + 2; int zeroDivisor = 9 / 0; int deep = %s; int large = %s; int acceptedDepth = %s; int acceptedSize = %s + 1;", deep, balanced(9), acceptedDepth, balanced(8))
	if err := os.WriteFile(unsafeSource, []byte(fallback), 0644); err != nil {
		t.Fatal(err)
	}
	command(tr.projectRoot, "--dev", unsafeSource)
	baseline := readC()
	for _, level := range []string{"-O1", "-O2", "-O3"} {
		command(tr.projectRoot, level, "--dev", unsafeSource)
		c := readC()
		for _, name := range []string{"maxLeaf", "overflow", "zeroDivisor", "deep", "large"} {
			if initializer(c, name) != initializer(baseline, name) {
				t.Fatalf("%s %s failed whole-tree fallback", level, name)
			}
		}
		for name, value := range map[string]string{"acceptedDepth": "65", "acceptedSize": "257"} {
			if got := initializer(c, name); got != value {
				t.Fatalf("%s at resource boundary: expected %s, got %s", name, value, got)
			}
		}
	}

	// The object path uses the same bridge. Require an artifact-free starting
	// output slot, and test failure separately in a fresh directory.
	objectSource := filepath.Join(work, "object.dr")
	if err := os.WriteFile(objectSource, []byte("func int value() { return (6 * 7) + (10 - 3); }"), 0644); err != nil {
		t.Fatal(err)
	}
	object := filepath.Join(tr.projectRoot, "a.o")
	if _, err := os.Stat(object); !os.IsNotExist(err) {
		t.Fatalf("object output already exists; preserve it: %v", err)
	}
	command(tr.projectRoot, "-O1", "--emit-obj", objectSource)
	if info, err := os.Stat(object); err != nil || info.Size() == 0 {
		t.Fatalf("missing object: %v", err)
	}
	if err := os.Remove(object); err != nil {
		t.Fatal(err)
	}
	// A fresh directory without the project's runtime headers forces host failure;
	// it must propagate nonzero instead of silently reporting successful emission.
	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()
	cmd := exec.CommandContext(ctx, tr.compilerPath, "-O1", "--emit-obj", objectSource)
	cmd.Dir = work
	out, err := cmd.CombinedOutput()
	if exit, ok := err.(*exec.ExitError); !ok || exit.ExitCode() != 1 || !strings.Contains(string(out), "failed to run:") {
		t.Fatalf("object failure status: %v\n%s", err, out)
	}
	if _, err := os.Stat(filepath.Join(work, "a.o")); !os.IsNotExist(err) {
		t.Fatalf("failed object compilation produced artifact: %v", err)
	}
}

func TestCompiler_LiteralFailures(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	work := t.TempDir()
	binary := filepath.Join(work, "literal-harness")
	if runtime.GOOS == "windows" {
		binary += ".exe"
	}
	ctx, cancel := context.WithTimeout(context.Background(), 180*time.Second)
	defer cancel()
	cmd := exec.CommandContext(ctx, "zig", "cc", "-std=c11", "-Wall", "-Wextra", "-fsanitize=undefined", filepath.Join(tr.projectRoot, "codex", "c", "literal_fold_harness.c"), "-o", binary)
	if out, err := cmd.CombinedOutput(); err != nil {
		t.Fatalf("harness build: %v\n%s", err, out)
	}
	for _, mode := range []string{"ok", "sccp", "arena", "mismatch", "convergence"} {
		cmd := exec.CommandContext(ctx, binary, mode)
		cmd.Dir = work
		out, err := cmd.CombinedOutput()
		if mode == "ok" {
			if err != nil {
				t.Fatalf("invariant harness: %v\n%s", err, out)
			}
		} else {
			reason := "out of memory"
			if mode == "mismatch" || mode == "convergence" {
				reason = "invalid transformed expression"
			}
			if exit, ok := err.(*exec.ExitError); !ok || exit.ExitCode() != 1 || !strings.Contains(string(out), "input.dr:12:4: error: literal optimization failed: "+reason) {
				t.Fatalf("fault %s: %v\n%s", mode, err, out)
			}
		}
	}
	for _, path := range []string{"build", "a.o", "dream", "dream.exe"} {
		if _, err := os.Stat(filepath.Join(work, path)); !os.IsNotExist(err) {
			t.Fatalf("internal failure produced %s: %v", path, err)
		}
	}
}
