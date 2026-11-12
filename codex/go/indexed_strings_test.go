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

func TestCompiler_IndexedStrings(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	program, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests", "basics", "memory", "indexed_string_copy.dr"))
	if err != nil {
		t.Fatal(err)
	}
	for _, mode := range []string{"ordinary", "module_entry"} {
		t.Run(mode, func(t *testing.T) {
			work := t.TempDir()
			text := string(program)
			options := []string{}
			if mode == "module_entry" {
				text = "import labels;\n" + strings.ReplaceAll(text, "\"literal\";", "labels.literal();")
				options = append(options, "--multi-file")
				if err := os.WriteFile(filepath.Join(work, "labels.dr"), []byte("module labels; export func string literal() { return \"literal\"; }"), 0644); err != nil {
					t.Fatal(err)
				}
			}
			source := filepath.Join(work, "input.dr")
			if err := os.WriteFile(source, []byte(text), 0644); err != nil {
				t.Fatal(err)
			}
			command := func(binary string, args ...string) []byte {
				t.Helper()
				ctx, cancel := context.WithTimeout(context.Background(), 180*time.Second)
				defer cancel()
				cmd := exec.CommandContext(ctx, binary, args...)
				cmd.Dir = tr.projectRoot
				cmd.Env = append(os.Environ(), "CC=zig cc")
				out, err := cmd.CombinedOutput()
				if err != nil {
					t.Fatalf("%s %v failed: %v\n%s", binary, args, err, out)
				}
				return out
			}
			command(tr.compilerPath, append(options, "--dev", source)...)
			data, err := os.ReadFile(filepath.Join(tr.projectRoot, "build", "bin", "dream.c"))
			if err != nil {
				t.Fatal(err)
			}
			c := strings.Join(strings.Fields(string(data)), "")
			for _, emitted := range []string{
				"text=dream_string_copy((values[(index++)]));",
				"nested=dream_string_copy((values[0]));",
				"snapshot=dream_string_copy((values[0]));",
				"absent=dream_string_copy((nullable[0]));",
				"dr_release(nested);", "dr_release(text);",
			} {
				if !strings.Contains(c, emitted) {
					t.Fatalf("missing ownership emission %q", emitted)
				}
			}
			// Preserve the generated program while allowing the harness to use the
			// real allocator with checked cleanup operands, rather than guessing
			// lifetime correctness from output that freed memory may still contain.
			generated := filepath.Join(work, "program.c")
			checked := strings.ReplaceAll(string(data), "../libs/", "")
			// The harness already includes the source allocator and custom headers;
			// installed copies use pragma once and must not redeclare DrRef.
			checked = strings.ReplaceAll(checked, "#include \"memory.h\"", "")
			checked = strings.ReplaceAll(checked, "#include \"custom.h\"", "")
			if err := os.WriteFile(generated, []byte(checked), 0644); err != nil {
				t.Fatal(err)
			}
			binary := filepath.Join(work, "checked-strings")
			if runtime.GOOS == "windows" {
				binary += ".exe"
			}
			command("zig", "cc", "-std=c11", "-fsanitize=undefined",
				"-DDREAM_STRING_PROGRAM=\""+filepath.ToSlash(generated)+"\"",
				"-I"+filepath.Join(tr.projectRoot, "src", "runtime", "memory"),
				"-I"+filepath.Join(tr.projectRoot, "build", "libs"),
				filepath.Join(tr.projectRoot, "codex", "c", "indexed_string_harness.c"),
				filepath.Join(tr.projectRoot, "src", "runtime", "extensions", "custom.c"),
				filepath.Join(tr.projectRoot, "src", "runtime", "io", "console.c"),
				filepath.Join(tr.projectRoot, "src", "runtime", "exceptions", "exception.c"), "-o", binary)
			want := "literal\nliteral\nliteral\n1\nliteral\ndynamic value\ndynamic value\ndynamic value\n1\ndynamic value\ndynamic value\ndynamic value\ndynamic value\n"
			if got := strings.ReplaceAll(string(command(binary)), "\r\n", "\n"); got != want {
				t.Fatalf("checked runtime: got %q, want %q", got, want)
			}
			command(tr.compilerPath, append(options, source)...)
			executable := "dream"
			if runtime.GOOS == "windows" {
				executable += ".exe"
			}
			if got := strings.ReplaceAll(string(command(filepath.Join(tr.projectRoot, executable))), "\r\n", "\n"); got != want {
				t.Fatalf("linked runtime: got %q, want %q", got, want)
			}
		})
	}
}
