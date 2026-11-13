package dream_test

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"testing"
	"time"
)

func TestCompiler_IndexedStrings(t *testing.T) {
	checkStringOwnership(t, "indexed_string_copy.dr", 4, []string{
		"text=dream_string_copy((values[(index++)]));",
		"nested=dream_string_copy((values[0]));",
		"snapshot=dream_string_copy((values[0]));",
		"absent=dream_string_copy((nullable[0]));",
		"dr_release(nested);", "dr_release(text);",
	}, "literal\nliteral\nliteral\n1\nliteral\ndynamic value\ndynamic value\ndynamic value\n1\ndynamic value\ndynamic value\ndynamic value\ndynamic value\n")
}

func TestCompiler_LiteralStrings(t *testing.T) {
	checkStringOwnership(t, "literal_string_copy.dr", 8, []string{
		`text=dream_string_copy("literal");`,
		`empty=dream_string_copy("");`,
		`escaped=dream_string_copy("quote:\"ok\"");`,
		`staticText=dream_string_copy("static");`,
		`instanceText=dream_string_copy("instance");`,
		`repeated=dream_string_copy("loop");`,
		`globalText=dream_string_copy("global");`,
		`managed=(dream_concat("dynamic","value"));`,
		"dr_release(text);", "dr_release(empty);", "dr_release(escaped);",
		"dr_release(managed);", "dr_release(staticText);", "dr_release(instanceText);",
		"dr_release(repeated);",
	}, "literal\nempty\nquote: \"ok\"\nliteral\ndynamic value\nstatic\ninstance\nloop\nloop\nglobal\n")
}

func checkStringOwnership(t *testing.T, fixture string, releases int, emittedChecks []string, want string) {
	t.Helper()
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	program, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests", "basics", "memory", fixture))
	if err != nil {
		t.Fatal(err)
	}
	for _, mode := range []string{"ordinary", "module_entry"} {
		t.Run(mode, func(t *testing.T) {
			work := t.TempDir()
			text := string(program)
			options := []string{}
			if mode == "module_entry" {
				if fixture == "indexed_string_copy.dr" {
					text = strings.ReplaceAll(text, "\"literal\";", "labels.literal();")
				} else {
					text += "\nlabels.literal();\n"
				}
				text = "import labels;\n" + text
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
			for _, emitted := range emittedChecks {
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
			// Match build.zig's runtime feature macros, including the POSIX strdup
			// declaration used by exception.c when compiling with Linux C11 headers.
			compileArgs := []string{"cc", "-std=c11", "-D_GNU_SOURCE", "-fsanitize=undefined",
				fmt.Sprintf("-DDREAM_STRING_RELEASES=%d", releases),
				"-DDREAM_STRING_PROGRAM=\"" + filepath.ToSlash(generated) + "\"",
				"-I" + filepath.Join(tr.projectRoot, "src", "runtime", "memory"),
				"-I" + filepath.Join(tr.projectRoot, "build", "libs"),
				filepath.Join(tr.projectRoot, "codex", "c", "indexed_string_harness.c"),
				filepath.Join(tr.projectRoot, "src", "runtime", "extensions", "custom.c"),
				filepath.Join(tr.projectRoot, "src", "runtime", "io", "console.c"),
				filepath.Join(tr.projectRoot, "src", "runtime", "exceptions", "exception.c")}
			if runtime.GOOS == "windows" {
				// Exercise Linux's headers and linker here as well: a native Windows
				// build alone does not detect missing POSIX feature-test macros.
				linuxArgs := append([]string{}, compileArgs...)
				linuxArgs = append(linuxArgs, "-target", "x86_64-linux-gnu", "-o", filepath.Join(work, "checked-strings-linux"))
				command("zig", linuxArgs...)
			}
			command("zig", append(compileArgs, "-o", binary)...)
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
