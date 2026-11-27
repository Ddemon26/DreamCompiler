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

func TestCompiler_OwnedCallAssignment(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	for _, modules := range []bool{false, true} {
		t.Run(fmt.Sprintf("lifetime/modules=%v", modules), func(t *testing.T) {
			work := t.TempDir()
			data, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests/basics/memory/owned_call_assignment.dr"))
			if err != nil {
				t.Fatal(err)
			}
			text := string(data)
			options := []string{}
			entry, factory := "", "factory"
			joined, nested, extended, fourth := "joined", "nested", "extended", "fourth"
			if modules {
				entry, factory = "__dream_module_0_", "__dream_module_1_factory"
				joined, nested = "__dream_module_1_joined", "__dream_module_1_nested"
				extended = "__dream_module_1_extended"
				fourth = "__dream_module_1_fourth"
				options = append(options, "--multi-file")
				text = "import labels;\n" + strings.ReplaceAll(text, "= factory();", "= labels.factory();")
				text = strings.ReplaceAll(text, "= joined(", "= labels.joined(")
				text = strings.ReplaceAll(text, "= nested(", "= labels.nested(")
				text = strings.ReplaceAll(text, "= extended(", "= labels.extended(")
				text = strings.ReplaceAll(text, "= fourth(", "= labels.fourth(")
				if err := os.WriteFile(filepath.Join(work, "labels.dr"), []byte(`module labels;
export func string factory() { string result = "owned"; return result; }
export func string joined(string source) { string result = source; return (result += factory()); }
export func string nested(string source) { string result = source; return (result += joined("-inner-")); }
export func string extended(string source) { string result = source; return (result += nested("-outer")); }
export func string fourth(string source, int callNumber) { string result = source; return (result += extended("-third")); }`), 0644); err != nil {
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
				cmd.Dir, cmd.Env = tr.projectRoot, append(os.Environ(), "CC=zig cc")
				out, err := cmd.CombinedOutput()
				if err != nil {
					t.Fatalf("%s %v: %v\n%s", binary, args, err, out)
				}
				return out
			}
			command(tr.compilerPath, append(options, "--dev", source)...)
			data, err = os.ReadFile(filepath.Join(tr.projectRoot, "build/bin/dream.c"))
			if err != nil {
				t.Fatal(err)
			}
			c := strings.Join(strings.Fields(string(data)), "")
			for _, want := range []string{
				"char*__dream_owned_0=(char*)" + factory + "();dr_release(identity);identity=__dream_owned_0;",
				"(char*)" + entry + "clone(text);dr_release(text);text=__dream_owned_0;",
				"(char*)" + entry + "clone(__dream_string_assign(&text,\"changed\"));dr_release(text);",
				"(char*)" + entry + "effect((count++));dr_release(text);",
				"(char*)" + entry + "clone(\"global\");dr_release(global);",
				"(char*)" + joined + "(target);dr_release(target);target=__dream_owned_0;",
				"(char*)" + joined + `(__dream_string_assign(&target,"new"));dr_release(target);target=__dream_owned_0;`,
				"(char*)" + nested + "(target);dr_release(target);target=__dream_owned_0;",
				"(char*)" + nested + `(__dream_string_assign(&target,"new"));dr_release(target);target=__dream_owned_0;`,
				"(char*)" + extended + "(target);dr_release(target);target=__dream_owned_0;",
				"(char*)" + extended + `(__dream_string_assign(&target,"new"));dr_release(target);target=__dream_owned_0;`,
				"(char*)" + fourth + "(target,(count++));dr_release(target);target=__dream_owned_0;",
				"(char*)" + fourth + `(__dream_string_assign(&target,"new"),(count++));dr_release(target);target=__dream_owned_0;`,
			} {
				if !strings.Contains(c, want) {
					t.Errorf("missing assignment %q", want)
				}
			}
			if strings.Contains(c, "dr_retain(") {
				t.Error("adoption must not retain")
			}
			if t.Failed() {
				return
			}
			generated := filepath.Join(work, "program.c")
			checked := strings.ReplaceAll(string(data), "../libs/", "")
			checked = strings.ReplaceAll(checked, "#include \"memory.h\"", "")
			checked = strings.ReplaceAll(checked, "#include \"custom.h\"", "")
			if err := os.WriteFile(generated, []byte(checked), 0644); err != nil {
				t.Fatal(err)
			}

			args := []string{"cc", "-std=c11", "-D_GNU_SOURCE", "-fsanitize=undefined",
				"-DDREAM_IDENTITY_PROBE=" + entry + "identityProbe",
				"-DDREAM_JOIN_ASSIGNMENT_PROBE=" + entry + "joinAssignmentProbe",
				"-DDREAM_NESTED_JOIN_ASSIGNMENT_PROBE=" + entry + "nestedJoinAssignmentProbe",
				"-DDREAM_TRIPLE_JOIN_ASSIGNMENT_PROBE=" + entry + "tripleJoinAssignmentProbe",
				"-DDREAM_FOURTH_JOIN_ASSIGNMENT_PROBE=" + entry + "fourthJoinAssignmentProbe",
				"-DDREAM_TRANSFER_PROBE=" + entry + "transfer",
				"-DDREAM_STRING_PROGRAM=\"" + filepath.ToSlash(generated) + "\"",
				"-I" + filepath.Join(tr.projectRoot, "src/runtime/memory"), "-I" + filepath.Join(tr.projectRoot, "build/libs"),
				filepath.Join(tr.projectRoot, "codex/c/owned_call_assignment_harness.c"),
				filepath.Join(tr.projectRoot, "src/runtime/extensions/custom.c"),
				filepath.Join(tr.projectRoot, "src/runtime/io/console.c"),
				filepath.Join(tr.projectRoot, "src/runtime/exceptions/exception.c")}
			if runtime.GOOS == "windows" {
				command("zig", append(append([]string{}, args...), "-target", "x86_64-linux-gnu", "-o", filepath.Join(work, "checked-linux"))...)
			}
			binary := filepath.Join(work, "checked")
			if runtime.GOOS == "windows" {
				binary += ".exe"
			}
			command("zig", append(args, "-o", binary)...)
			want := "old\nchanged\n2\nstart-outer-inner-owned\nstart-third-outer-inner-owned\n1\nglobal\n"
			if got := strings.ReplaceAll(string(command(binary)), "\r\n", "\n"); got != want {
				t.Fatalf("checked output: %q, want %q", got, want)
			}
			command(tr.compilerPath, append(options, source)...)
			binary = filepath.Join(tr.projectRoot, "dream")
			if runtime.GOOS == "windows" {
				binary += ".exe"
			}
			if got := strings.ReplaceAll(string(command(binary)), "\r\n", "\n"); got != want {
				t.Fatalf("linked output: %q, want %q", got, want)
			}
		})
	}
	checkOwnedAssignmentContexts(t, tr)
}

func checkOwnedAssignmentContexts(t *testing.T, tr *TestRunner) {
	for _, modules := range []bool{false, true} {
		prefix := ""
		if modules {
			prefix = "__dream_module_0_"
		}
		for _, tc := range []struct{ name, body, want string }{
			{"header", `string slot; for(int i=0;i<1;slot=factory()) { i++; }`, `__dream_string_assign(&slot,` + prefix + `factory())`},
			{"initializer", `string slot; string other = (slot=factory());`, `dream_string_copy(__dream_string_assign(&slot,` + prefix + `factory()))`},
			{"chained", `string slot; string other; other = (slot=factory());`, `__dream_string_assign(&other,__dream_string_assign(&slot,` + prefix + `factory()))`},
			{"borrowed", `string slot; slot=borrow();`, `__dream_string_assign(&slot,` + prefix + `borrow())`},
			{"parameter", `func void view(string slot) { slot=factory(); }`, `slot=` + prefix + `factory()`},
			{"join_header", `string slot; for(int i=0;i<1;slot=joined()) { i++; }`, `__dream_string_assign(&slot,` + prefix + `joined())`},
			{"join_initializer", `string slot; string other = (slot=joined());`, `dream_string_copy(__dream_string_assign(&slot,` + prefix + `joined()))`},
			{"join_chained", `string slot; string other; other = (slot=joined());`, `__dream_string_assign(&other,__dream_string_assign(&slot,` + prefix + `joined()))`},
			{"join_parameter", `func void view(string slot) { slot=joined(); }`, `slot=` + prefix + `joined()`},
			{"nested_join_initializer", `string slot; string other = (slot=nested());`, `dream_string_copy(__dream_string_assign(&slot,` + prefix + `nested()))`},
			{"third_join_initializer", `string slot; string other = (slot=third());`, `dream_string_copy(__dream_string_assign(&slot,` + prefix + `third()))`},
			{"fourth_join_initializer", `string slot; string other = (slot=fourth());`, `dream_string_copy(__dream_string_assign(&slot,` + prefix + `fourth()))`},
			{"fourth_join_header", `string slot; for(int i=0;i<1;slot=fourth()) { i++; }`, `__dream_string_assign(&slot,` + prefix + `fourth())`},
			{"fifth_join", `string slot; slot=fifth();`, `__dream_string_assign(&slot,` + prefix + `fifth())`},
		} {
			t.Run(fmt.Sprintf("%s/modules=%v", tc.name, modules), func(t *testing.T) {
				c := memberReturnEmission(t, tr, `func string factory(){string result="owned";return result;}
func string borrow(){return "borrowed";}
func string joined(){string result="start";return (result += factory());}
func string nested(){string result="second";return (result += joined());}
func string third(){string result="third";return (result += nested());}
func string fourth(){string result="fourth";return (result += third());}
func string fifth(){string result="fifth";return (result += fourth());}
`+tc.body, modules)
				if !strings.Contains(c, tc.want) {
					t.Fatalf("missing fallback %q: %s", tc.want, c)
				}
			})
		}
	}
}
