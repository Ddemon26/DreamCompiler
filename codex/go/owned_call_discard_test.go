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

func TestCompiler_OwnedCallDiscard(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	for _, modules := range []bool{false, true} {
		t.Run(fmt.Sprintf("lifetime/modules=%v", modules), func(t *testing.T) {
			work := t.TempDir()
			data, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests/basics/memory/owned_call_discard.dr"))
			if err != nil {
				t.Fatal(err)
			}
			source := filepath.Join(work, "input.dr")
			program := string(data)
			var options []string
			entry, identity := "", "factory"
			joined, nested, deeper := "joined", "nested", "deeper"
			entryDiscards := 10
			if modules {
				entry, identity = "__dream_module_0_", "__dream_module_1_factory"
				joined = "__dream_module_1_joined"
				nested = "__dream_module_1_nested"
				deeper = "__dream_module_1_deeper"
				entryDiscards-- // The identity probe calls the imported definition.
				options = append(options, "--multi-file")
				program = "import labels;\n" + strings.Replace(program, "{ factory(); }", "{ labels.factory(); }", 1)
				program = strings.Replace(program, "(joined(count++))", "(labels.joined(count++))", 1)
				program = strings.Replace(program, "(nested(count++))", "(labels.nested(count++))", 1)
				program = strings.Replace(program, "(deeper(count++))", "(labels.deeper(count++))", 1)
				program += "\nlabels.empty();\n"
				if err := os.WriteFile(filepath.Join(work, "labels.dr"), []byte(`module labels;
export func string factory() { string result = "owned"; return result; }
export func string joined(int value) { string result = "start"; return (result += factory()); }
export func string nested(int value) { string result = "outer-"; return (result += joined(value)); }
export func string deeper(int value) { string result = "third-"; return (result += nested(value)); }
export func string empty() { return ""; }`), 0644); err != nil {
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
			data, err = os.ReadFile(filepath.Join(tr.projectRoot, "build/bin/dream.c"))
			if err != nil {
				t.Fatal(err)
			}
			c := strings.Join(strings.Fields(string(data)), "")
			for _, call := range []string{
				entry + "clone(backing)",
				entry + `clone(__dream_string_assign(&backing,"changed"))`,
				entry + "clone(missing)", entry + "empty()", entry + "escaped()",
				entry + "absent()", entry + "effect((count++))",
			} {
				want := "dr_release((void*)" + call + ");"
				if !strings.Contains(c, want) {
					t.Errorf("missing exact discard release %q", want)
				}
			}
			if got := strings.Count(c, "dr_release((void*)"+entry+"factory());"); got != entryDiscards {
				t.Errorf("entry factory release sites: %d, want %d", got, entryDiscards)
			}
			if modules && strings.Count(c, "dr_release((void*)"+identity+"());") != 1 {
				t.Error("expected one release of the imported factory result")
			}
			if strings.Contains(c, "dr_retain(") ||
				strings.Contains(c, "dr_release((void*)"+entry+"borrow(") ||
				strings.Contains(c, "dr_release((void*)"+entry+"literal(") ||
				(modules && (!strings.Contains(c, "__dream_module_1_empty();") ||
					strings.Contains(c, "dr_release((void*)__dream_module_1_empty("))) {
				t.Error("discard must preserve borrowed results and add no retain")
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
				"-DDREAM_JOIN_DISCARD_PROBE=" + entry + "joinDiscardProbe",
				"-DDREAM_NESTED_JOIN_DISCARD_PROBE=" + entry + "nestedJoinDiscardProbe",
				"-DDREAM_TRIPLE_JOIN_DISCARD_PROBE=" + entry + "tripleJoinDiscardProbe",
				"-DDREAM_STRING_PROGRAM=\"" + filepath.ToSlash(generated) + "\"",
				"-I" + filepath.Join(tr.projectRoot, "src/runtime/memory"),
				"-I" + filepath.Join(tr.projectRoot, "build/libs"),
				filepath.Join(tr.projectRoot, "codex/c/owned_call_discard_harness.c"),
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
			const want = "backing\nchanged\n1\n1\n1\n1\n"
			if got := command(binary); got != want {
				t.Fatalf("checked output: %q, want %q", got, want)
			}
			for _, name := range []string{joined, nested, deeper} {
				call := name + "((count++))"
				if strings.Count(c, call) != 1 || strings.Count(c, "dr_release((void*)"+call+");") != 1 {
					t.Errorf("expected one call and direct release of %q", call)
				}
			}
			command(tr.compilerPath, append(options, source)...)
			if got := command(filepath.Join(tr.projectRoot, "dream"+suffix)); got != want {
				t.Fatalf("linked output: %q, want %q", got, want)
			}
		})
		t.Run(fmt.Sprintf("contexts/modules=%v", modules), func(t *testing.T) {
			entry := ""
			if modules {
				entry = "__dream_module_0_"
			}
			c := memberReturnEmission(t, tr, `func string factory() { string result = "owned"; return result; }
func string clone(string source) { string result = source; return result; }
func string joined() { string result = "start"; return (result += factory()); }
func string nested() { string result = "outer-"; return (result += joined()); }
func string deeper() { string result = "third-"; return (result += nested()); }
func string fourth() { string result = "fourth-"; return (result += deeper()); }
func string unproven(string source) { string result = "start"; return (result += source); }
async func string later() { string result = "async"; return result; }
func void run() {
    for (factory(); false; factory()) { }
    true ? factory() : "borrowed";
    clone(factory());
    for (joined(); false; joined()) { }
    true ? joined() : "borrowed";
    clone(joined());
    for (nested(); false; nested()) { }
    true ? nested() : "borrowed";
    clone(nested());
    for (deeper(); false; deeper()) { }
    true ? deeper() : "borrowed";
    clone(deeper());
    fourth();
    unproven("borrowed");
    later();
}`, modules)
			factory := entry + "factory()"
			joined := entry + "joined()"
			nested := entry + "nested()"
			deeper := entry + "deeper()"
			for _, want := range []string{
				"for(" + factory + ";0;" + factory + ")",
				"(1?" + factory + ":\"borrowed\");",
				"dr_release((void*)" + entry + "clone(" + factory + "));",
				"for(" + joined + ";0;" + joined + ")",
				"(1?" + joined + ":\"borrowed\");",
				"dr_release((void*)" + entry + "clone(" + joined + "));",
				"for(" + nested + ";0;" + nested + ")",
				"(1?" + nested + ":\"borrowed\");",
				"dr_release((void*)" + entry + "clone(" + nested + "));",
				"for(" + deeper + ";0;" + deeper + ")",
				"(1?" + deeper + ":\"borrowed\");",
				"dr_release((void*)" + entry + "clone(" + deeper + "));",
				entry + "fourth();",
				entry + "unproven(\"borrowed\");",
				entry + "later();",
			} {
				if !strings.Contains(c, want) {
					t.Errorf("missing context %q", want)
				}
			}
			if strings.Contains(c, "dr_release((void*)"+factory) ||
				strings.Contains(c, "dr_release((void*)"+joined) ||
				strings.Contains(c, "dr_release((void*)"+nested) ||
				strings.Contains(c, "dr_release((void*)"+entry+"deeper(") ||
				strings.Contains(c, "dr_release((void*)"+entry+"fourth(") ||
				strings.Contains(c, "dr_release((void*)"+entry+"unproven(") ||
				strings.Contains(c, "dr_release((void*)"+entry+"later(") {
				t.Error("discard release must exclude inner calls, headers, compound expressions and async results")
			}
		})
	}
}
