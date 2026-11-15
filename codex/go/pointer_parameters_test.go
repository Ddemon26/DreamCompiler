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

// Inspect signatures before host compilation, then exercise borrowed pointers
// through real methods, local/base constructors and forward free-function calls.
func TestCompiler_PointerParameters(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	program, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests", "advanced", "oop", "pointer_parameters.dr"))
	if err != nil {
		t.Fatal(err)
	}
	for _, mode := range []string{"ordinary", "module_entry"} {
		t.Run(mode, func(t *testing.T) {
			work := t.TempDir()
			text := string(program)
			options := []string{}
			if mode == "module_entry" {
				text = "import values;\n" + text + "\nConsole.WriteLine(child.read(numbers, values.index()));\n"
				options = append(options, "--multi-file")
				if err := os.WriteFile(filepath.Join(work, "values.dr"), []byte("module values; export func int index() { return 0; }"), 0644); err != nil {
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
			freePrefix := ""
			if mode == "module_entry" {
				freePrefix = "__dream_module_0_"
			}
			for _, signature := range []string{
				"staticvoidBuffer_init(structBuffer*this,int*n,float*r,int*f,char*c,constchar**s,intoffset)",
				"staticvoidChild_init(structChild*this,int*n,float*r,int*f,char*c,constchar**s,intoffset)",
				"staticintBuffer_read(structBuffer*this,int*n,intindex)",
				"staticvoidBuffer_update(int*n,float*r,int*f,char*c,constchar**s)",
				"staticintBuffer_throughThis(structBuffer*this,int*n)",
				"staticintChild_throughBase(structChild*this,int*n)",
				"staticint" + freePrefix + "forward(int*);", "staticint" + freePrefix + "forward(int*n)",
				"staticint" + freePrefix + "readFree(int*);", "staticint" + freePrefix + "readFree(int*n)",
			} {
				if !strings.Contains(c, signature) {
					t.Errorf("missing pointer signature %q", signature)
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
			want := "42\n2.500000\n1\nZ\nlabel\n42\n43\n44\n45\n46\n47\n48\n"
			if mode == "module_entry" {
				want += "48\n"
			}
			if got := strings.ReplaceAll(string(out), "\r\n", "\n"); err != nil || got != want {
				t.Fatalf("runtime: %v\ngot %q\nwant %q", err, got, want)
			}
		})
	}
}
