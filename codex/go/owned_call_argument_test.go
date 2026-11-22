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

func ownedArgumentBlock(factory, wrapper, temporary string) string {
	return "{constchar*" + temporary + "=" + factory + ";" +
		wrapper + "(" + temporary + ");dr_release((void*)" + temporary + ");}"
}

func TestCompiler_OwnedCallArgument(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	fixture, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests/basics/memory/owned_call_argument.dr"))
	if err != nil {
		t.Fatal(err)
	}
	for _, modules := range []bool{false, true} {
		entry := ""
		if modules {
			entry = "__dream_module_0_"
		}
		t.Run(fmt.Sprintf("lifetime/modules=%v", modules), func(t *testing.T) {
			work := t.TempDir()
			source := filepath.Join(work, "input.dr")
			program := string(fixture)
			var options []string
			factory, wrapper, joined, nested := "factory()", "view", "joined", "nested"
			nestedLine := "line"
			if modules {
				factory, wrapper = "__dream_module_1_factory()", "__dream_module_1_view"
				joined = "__dream_module_1_joined"
				nested = "__dream_module_1_nested"
				nestedLine = "__dream_module_1_viewLine"
				options = append(options, "--multi-file")
				program = "import labels;\n" + strings.Replace(program, "view(factory());", "labels.view(labels.factory());", 1)
				program = strings.Replace(program, "Console.Write(factory());", "Console.Write(labels.factory());", 1)
				program = strings.ReplaceAll(program, "joined(count++)", "labels.joined(count++)")
				program = strings.ReplaceAll(program, "nested(count++)", "labels.nested(count++)")
				program = strings.Replace(program, "view(labels.joined(count++));", "labels.view(labels.joined(count++));", 1)
				program = strings.Replace(program, "view(labels.nested(count++));", "labels.view(labels.nested(count++));", 1)
				program = strings.Replace(program, "line(labels.nested(count++));", "labels.viewLine(labels.nested(count++));", 1)
				program = strings.Replace(program, "Console.Write(source);", "Console.Write(source); Console.Write(source);", 1)
				program += "\nfunc void importControls() { view(factory()); labels.line(factory()); }\n"
				imported := `module labels;
export func string factory() { string result = "owned"; return result; }
export func string joined(int value) { string result = "start"; return (result += factory()); }
export func string nested(int value) { string result = "outer-"; return (result += joined(value)); }
export func void view(string source) { Console.Write(source); }
export func void viewLine(string source) { Console.WriteLine(source); }
export func void line(string source) { Console.WriteLine(source); Console.WriteLine(source); }`
				if err := os.WriteFile(filepath.Join(work, "labels.dr"), []byte(imported), 0644); err != nil {
					t.Fatal(err)
				}
			}
			if err := os.WriteFile(source, []byte(program), 0644); err != nil {
				t.Fatal(err)
			}
			command := func(binary string, args ...string) string {
				t.Helper()
				ctx, cancel := context.WithTimeout(context.Background(), 180*time.Second)
				defer cancel()
				cmd := exec.CommandContext(ctx, binary, args...)
				cmd.Dir, cmd.Env = tr.projectRoot, append(os.Environ(), "CC=zig cc")
				out, err := cmd.CombinedOutput()
				if err != nil {
					t.Fatalf("%s %v: %v\n%s", binary, args, err, out)
				}
				return strings.ReplaceAll(string(out), "\r\n", "\n")
			}
			command(tr.compilerPath, append(append([]string{}, options...), "--dev", source)...)
			data, err := os.ReadFile(filepath.Join(tr.projectRoot, "build/bin/dream.c"))
			if err != nil {
				t.Fatal(err)
			}
			var emitted strings.Builder
			for _, line := range strings.Split(string(data), "\n") {
				line = strings.TrimSpace(line)
				if !strings.HasPrefix(line, "#line ") && !strings.HasPrefix(line, "/* Debug:") {
					emitted.WriteString(strings.Join(strings.Fields(line), ""))
				}
			}
			c := emitted.String()
			temp0, temp1 := "dreamOwnedArgument0", "dreamOwnedArgument1"
			for _, want := range []string{
				ownedArgumentBlock(factory, wrapper, temp0),
				ownedArgumentBlock(factory, "dr_console_write", temp0),
				ownedArgumentBlock(entry+"factory()", "dr_console_writeln", temp0),
				"if(1)" + ownedArgumentBlock(entry+"effect((count++))", "dr_console_write", temp1),
				ownedArgumentBlock(entry+"factory()", "dr_console_writeln", temp1),
				"if(1)" + ownedArgumentBlock(entry+"clone("+temp0+")", entry+"line", temp1),
				ownedArgumentBlock(entry+"effect((count++))", entry+"line", temp1),
				"if(1)" + ownedArgumentBlock(joined+"((count++))", "dr_console_write", temp0),
				ownedArgumentBlock(joined+"((count++))", "dr_console_writeln", temp0),
				"if(1)" + ownedArgumentBlock(joined+"((count++))", wrapper, temp0),
				ownedArgumentBlock(joined+"((count++))", entry+"line", temp0),
				"if(1)" + ownedArgumentBlock(nested+"((count++))", "dr_console_write", temp0),
				ownedArgumentBlock(nested+"((count++))", "dr_console_writeln", temp0),
				"if(1)" + ownedArgumentBlock(nested+"((count++))", wrapper, temp0),
				ownedArgumentBlock(nested+"((count++))", nestedLine, temp0),
			} {
				if got := strings.Count(c, want); got != 1 {
					t.Errorf("expected one %q, got %d", want, got)
				}
			}
			if strings.Count(c, "dr_release((void*)dreamOwnedArgument") != 18 ||
				strings.Contains(c, "dr_retain(") {
				t.Error("expected ten wrapper and eight direct Console release sites without any retains")
			}
			if modules && (!strings.Contains(c, entry+"view("+entry+"factory());") ||
				!strings.Contains(c, "__dream_module_1_line("+entry+"factory());")) {
				t.Error("same-spelled consumers must use their own definitions")
			}
			if t.Failed() {
				return
			}

			generated := filepath.Join(work, "program.c")
			checked := strings.NewReplacer("../libs/", "", `#include "memory.h"`, "", `#include "custom.h"`, "").Replace(string(data))
			if err := os.WriteFile(generated, []byte(checked), 0644); err != nil {
				t.Fatal(err)
			}
			args := []string{"cc", "-std=c11", "-D_GNU_SOURCE", "-fsanitize=undefined",
				"-DDREAM_IDENTITY_PROBE=" + entry + "identityProbe",
				"-DDREAM_JOIN_OUTPUT_PROBE=" + entry + "joinOutputProbe",
				"-DDREAM_NESTED_OUTPUT_PROBE=" + entry + "nestedOutputProbe",
				"-DDREAM_STRING_PROGRAM=\"" + filepath.ToSlash(generated) + "\"",
				"-I" + filepath.Join(tr.projectRoot, "src/runtime/memory"),
				"-I" + filepath.Join(tr.projectRoot, "build/libs"),
				filepath.Join(tr.projectRoot, "codex/c/owned_call_argument_harness.c"),
				filepath.Join(tr.projectRoot, "src/runtime/extensions/custom.c"),
				filepath.Join(tr.projectRoot, "src/runtime/io/console.c"),
				filepath.Join(tr.projectRoot, "src/runtime/exceptions/exception.c")}
			suffix := ""
			if runtime.GOOS == "windows" {
				suffix = ".exe"
				command("zig", append(append([]string{}, args...), "-target", "x86_64-linux-gnu", "-o", filepath.Join(work, "checked-linux"))...)
			}
			binary := filepath.Join(work, "checked"+suffix)
			command("zig", append(args, "-o", binary)...)
			const want = "snapshot\neffect\n1\nowned\nowned\neffectowned\n2\nstartownedstartowned\nstartownedstartowned\n4\nouter-startownedouter-startowned\nouter-startownedouter-startowned\n4\n"
			if got := command(binary); got != want {
				t.Fatalf("checked output: %q, want %q", got, want)
			}
			command(tr.compilerPath, append(options, source)...)
			if got := command(filepath.Join(tr.projectRoot, "dream"+suffix)); got != want {
				t.Fatalf("linked output: %q, want %q", got, want)
			}
		})
		t.Run(fmt.Sprintf("contexts/modules=%v", modules), func(t *testing.T) {
			c := memberReturnEmission(t, tr, `
func string factory() { string result = "owned"; return result; }
func string joined() { string result = "start"; return (result += factory()); }
func string nested() { string result = "outer-"; return (result += joined()); }
func string deeper() { string result = "third-"; return (result += nested()); }
func string unproven(string source) { string result = "start"; return (result += source); }
func string absent() { string result; return result; }
func string borrowed(string value) { return value; }
func void line(string value) { Console.WriteLine(value); }
func void forward(string value) { line(value); }
func void read(string value) { Console.ReadLine(); }
func void computed(string value) { Console.WriteLine(true ? value : "other"); }
func string returnValue(string value) { return value; }
func string pointerFactory(string* value) { string result = "owned"; return result; }
func void pointerView(string* value) { Console.WriteLine(value); }
async func void later(string value) { Console.WriteLine(value); }
func void probe(string* values) {
    line(absent());
    for (line(factory()); false; line(factory())) {}
    true ? line(factory()) : line(factory());
    line(borrowed("source"));
    forward(factory());
    read(factory());
    computed(factory());
    returnValue(factory());
    later(factory());
    Console.WriteLine(factory());
    Console.Write(absent());
    Console.Write(borrowed("source"));
    Console.WriteLine("literal");
    Console.Write(factory() + "!");
    Console.Write(joined() + "!");
    Console.Write(nested() + "!");
    Console.WriteLine(deeper());
    Console.WriteLine(unproven("source"));
    line(joined());
    line(nested());
    line(deeper());
    forward(nested());
    line(true ? nested() : "other");
    forward(joined());
    line(unproven("source"));
    line(true ? joined() : "other");
    Console.Write(pointerFactory(values));
    line(pointerFactory(values));
    pointerView(factory());
}`, modules)
			factory, wrapper := entry+"factory()", entry+"line"
			for _, want := range []string{
				ownedArgumentBlock(entry+"absent()", wrapper, "dreamOwnedArgument0"),
				"for(" + wrapper + "(" + factory + ");0;" + wrapper + "(" + factory + "))",
				"(1?" + wrapper + "(" + factory + "):" + wrapper + "(" + factory + "));",
				wrapper + "(" + entry + "borrowed(\"source\"));",
				entry + "forward(" + factory + ");",
				entry + "read(" + factory + ");",
				entry + "computed(" + factory + ");",
				entry + "returnValue(" + factory + ");",
				entry + "later(" + factory + ");",
				ownedArgumentBlock(factory, "dr_console_writeln", "dreamOwnedArgument0"),
				ownedArgumentBlock(entry+"absent()", "dr_console_write", "dreamOwnedArgument0"),
				"dr_console_write(" + entry + "borrowed(\"source\"));",
				"dr_console_writeln(\"literal\");",
				"dr_console_write((dream_concat(" + factory + ",\"!\")));",
				"dr_console_write((dream_concat(" + entry + "joined(),\"!\")));",
				"dr_console_write((dream_concat(" + entry + "nested(),\"!\")));",
				"dr_console_writeln(" + entry + "deeper());",
				"dr_console_writeln(" + entry + "unproven(\"source\"));",
				ownedArgumentBlock(entry+"joined()", wrapper, "dreamOwnedArgument0"),
				ownedArgumentBlock(entry+"nested()", wrapper, "dreamOwnedArgument0"),
				wrapper + "(" + entry + "deeper());",
				entry + "forward(" + entry + "nested());",
				wrapper + "((1?" + entry + "nested():\"other\"));",
				entry + "forward(" + entry + "joined());",
				wrapper + "(" + entry + "unproven(\"source\"));",
				wrapper + "((1?" + entry + "joined():\"other\"));",
				"dr_console_write(" + entry + "pointerFactory(values));",
				wrapper + "(" + entry + "pointerFactory(values));",
				entry + "pointerView(" + factory + ");",
			} {
				if !strings.Contains(c, want) {
					t.Errorf("missing context %q", want)
				}
			}
			if strings.Count(c, "dr_release((void*)dreamOwnedArgument") != 5 {
				t.Error("only the five eligible complete statements should release")
			}
		})
	}
	t.Run("lowered_names", func(t *testing.T) {
		c := memberReturnEmission(t, tr, `
enum Number { dreamOwnedArgument0 = 1 }
func int dreamOwnedArgument1() { return 2; }
class __dream_owned { static int arg_0; }
func string factory(int value) { string result = "owned"; return result; }
func void line(string value) { Console.WriteLine(value); }
line(factory(Number.dreamOwnedArgument0 + dreamOwnedArgument1() + __dream_owned.arg_0));
Console.WriteLine(factory(Number.dreamOwnedArgument0 + dreamOwnedArgument1() + __dream_owned.arg_0));`, false)
		factory := "factory(((dreamOwnedArgument0+dreamOwnedArgument1())+__dream_owned_arg_0))"
		for _, consumer := range []string{"line", "dr_console_writeln"} {
			want := ownedArgumentBlock(factory, consumer, "dreamOwnedArgument2")
			if !strings.Contains(c, want) {
				t.Errorf("missing hygienic release block %q", want)
			}
		}
	})
}
