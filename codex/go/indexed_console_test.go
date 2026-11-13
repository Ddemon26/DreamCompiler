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

// Check format selection before linking; runtime output also checks once-only indexing.
func TestCompiler_IndexedConsole(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	program, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests", "basics", "io", "indexed_console.dr"))
	if err != nil {
		t.Fatal(err)
	}
	for _, mode := range []string{"ordinary", "module_entry"} {
		t.Run(mode, func(t *testing.T) {
			work := t.TempDir()
			text := string(program)
			options := []string{}
			if mode == "module_entry" {
				text = "import values;\n" + strings.ReplaceAll(text, "reals[0] = 2.5;", "reals[0] = values.real();")
				options = append(options, "--multi-file")
				if err := os.WriteFile(filepath.Join(work, "values.dr"), []byte("module values; export func float real() { return 2.5; }"), 0644); err != nil {
					t.Fatal(err)
				}
			}
			source := filepath.Join(work, "input.dr")
			if err := os.WriteFile(source, []byte(text), 0644); err != nil {
				t.Fatal(err)
			}
			command := func(args ...string) {
				t.Helper()
				ctx, cancel := context.WithTimeout(context.Background(), 180*time.Second)
				defer cancel()
				cmd := exec.CommandContext(ctx, tr.compilerPath, args...)
				cmd.Dir = tr.projectRoot
				cmd.Env = append(os.Environ(), "CC=zig cc")
				if out, err := cmd.CombinedOutput(); err != nil {
					t.Fatalf("compiler %v failed: %v\n%s", args, err, out)
				}
			}
			command(append(append([]string{}, options...), "--dev", source)...)
			data, err := os.ReadFile(filepath.Join(tr.projectRoot, "build", "bin", "dream.c"))
			if err != nil {
				t.Fatal(err)
			}
			c := strings.Join(strings.Fields(string(data)), "")
			for _, emitted := range []string{
				`printf("%f\n",(((structBase*)this)->reals[0]));`,
				`printf("%c\n",(((structBase*)this)->marks[0]));`,
				`dr_console_writeln((((structBase*)this)->words[0]));`,
				`printf("%c\n",(((structBase*)this)->label[0]));`,
				`printf("%c\n",(((structBase*)this)->label[1]));`,
				`printf("%f\n",(r[0]));`,
				`printf("%c\n",(c[0]));`,
				`printf("%d\n",(n[0]));`,
				`printf("%d\n",(b[0]));`,
				`dr_console_writeln((s[0]));`,
				`printf("%f",(r[0]));`,
				`printf("%f\n",(reals[(index++)]));`,
				`printf("%c\n",((words[0])[1]));`,
				`printf("%c\n",(scalar[0]));`,
				`printf("%c\n",(text[1]));`,
				`printf("%c\n",("literal"[2]));`,
				`printf("%c\n",(innerText[0]));`,
				`printf("%f\n",(alias[0]));`,
			} {
				if !strings.Contains(c, emitted) {
					t.Errorf("missing indexed format %q", emitted)
				}
			}
			if t.Failed() {
				t.FailNow()
			}
			command(append(append([]string{}, options...), source)...)
			executable := "dream"
			if runtime.GOOS == "windows" {
				executable += ".exe"
			}
			ctx, cancel := context.WithTimeout(context.Background(), 20*time.Second)
			defer cancel()
			run := exec.CommandContext(ctx, filepath.Join(tr.projectRoot, executable))
			run.Dir = tr.projectRoot
			out, err := run.CombinedOutput()
			want := "42\n2.500000\n1\nZ\narray\n2.500000\n1\nr\nt\na\nt\ni\n4.500000\nL\n4.500000\narray\n2.500000\n42\n2.500000\n1\nZ\narray\n2.500000Z\n2.500000\nZ\narray\n3.500000\nM\nmember\ns\n9\n1\na\n3.500000\nM\nmember\nc\n3.500000\nM\nmember\na\n"
			if got := strings.ReplaceAll(string(out), "\r\n", "\n"); err != nil || got != want {
				t.Fatalf("runtime: %v\ngot %q\nwant %q", err, got, want)
			}
		})
	}
}
