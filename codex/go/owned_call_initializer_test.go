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

func TestCompiler_OwnedCallInitializer(t *testing.T) {
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	for _, modules := range []bool{false, true} {
		t.Run(fmt.Sprintf("lifetime/modules=%v", modules), func(t *testing.T) {
			work := t.TempDir()
			data, err := os.ReadFile(filepath.Join(tr.projectRoot, "tests/basics/memory/owned_call_initializer.dr"))
			if err != nil {
				t.Fatal(err)
			}
			text := string(data)
			options := []string{}
			entry, identity, clone := "", "owned", "clone"
			copies := 18
			if modules {
				copies++ // Independent copy of the imported borrowed result.
				entry, identity, clone = "__dream_module_0_", "__dream_module_1_owned", "__dream_module_1_clone"
				options = append(options, "--multi-file")
				text = "import labels;\n" + strings.ReplaceAll(text, "string identity = owned();", "string identity = labels.owned();")
				text = strings.ReplaceAll(text, "snapshot = clone(backing)", "snapshot = labels.clone(backing)")
				text = strings.ReplaceAll(text, " = joined()", " = labels.joined()")
				text = strings.ReplaceAll(text, " = nestedJoin()", " = labels.nestedJoin()")
				text = strings.ReplaceAll(text, "string first =", "string importedBorrow = labels.emptyFactory(); string first =")
				if err := os.WriteFile(filepath.Join(work, "labels.dr"), []byte(`module labels;
export func string owned() { string result = "owned"; return result; }
export func string clone(string source) { const string result = source; return result; }
export func string joined() { string result = "start"; return (result += owned()); }
export func string nestedJoin() { string result = "outer-"; return (result += joined()); }
export func string emptyFactory() { return ""; }`), 0644); err != nil {
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
				"identity=" + identity + "();", "first=" + entry + "owned();",
				"snapshot=" + clone + "(backing);", "escaped=" + entry + "escapedFactory();",
				"empty=" + entry + "emptyFactory();", "absent=" + entry + "absentFactory();",
				"explicitNull=" + entry + "nullFactory(absent);", "zero=" + entry + "owned();",
				"header=" + entry + "owned();", "body=" + entry + "owned();",
				"forwarded=" + entry + "owned();", "__dream_return_0=forwarded;return__dream_return_0;",
				"global=" + entry + "globalFactory();", "dr_release(identity);",
			} {
				if !strings.Contains(c, want) {
					t.Errorf("missing adopted initializer/cleanup %q", want)
				}
			}
			if strings.Contains(c, "dream_string_copy("+entry+"owned(") || strings.Contains(c, "dream_string_copy("+clone+"(") || strings.Contains(c, "dr_retain(") {
				t.Error("proven factories must transfer once without a caller copy/retain")
			}
			if modules && !strings.Contains(c, "importedBorrow=dream_string_copy(__dream_module_1_emptyFactory());") {
				t.Error("same-spelled imported borrowed factory must keep its copy")
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
			// Independent tally: 17 nonnull factory/backing copies in exercise,
			// including the append's old slot and operand,
			// plus one global (and one imported borrowed copy in module mode).
			// Null factories allocate/release nothing. The identity
			// and transfer probes run separately with explicit original-pointer checks.
			args := []string{"cc", "-std=c11", "-D_GNU_SOURCE", "-fsanitize=undefined",
				fmt.Sprintf("-DDREAM_EXPECTED_COPIES=%d", copies), "-DDREAM_IDENTITY_PROBE=" + entry + "identityProbe",
				"-DDREAM_APPEND_PROBE=" + entry + "appendProbe",
				"-DDREAM_NESTED_APPEND_PROBE=" + entry + "nestedAppendProbe",
				"-DDREAM_TRANSFER_PROBE=" + entry + "transfer",
				"-DDREAM_STRING_PROGRAM=\"" + filepath.ToSlash(generated) + "\"",
				"-I" + filepath.Join(tr.projectRoot, "src/runtime/memory"), "-I" + filepath.Join(tr.projectRoot, "build/libs"),
				filepath.Join(tr.projectRoot, "codex/c/owned_call_initializer_harness.c"),
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
			join := entry + "joined"
			nestedJoin := entry + "nestedJoin"
			if modules {
				join = "__dream_module_1_joined"
				nestedJoin = "__dream_module_1_nestedJoin"
			}
			if !strings.Contains(c, "snapshot="+join+"();dr_release(snapshot);") ||
				!strings.Contains(c, "appended="+join+"();") {
				t.Fatalf("direct append-return initializers must adopt %s", join)
			}
			want := "owned\nescaped \"value\"\nempty\nnull\nbacking\nchanged\nowned\n7\nstartowned\nglobal\n"
			if got := strings.ReplaceAll(string(command(binary)), "\r\n", "\n"); got != want {
				t.Fatalf("checked output: %q, want %q", got, want)
			}
			if !strings.Contains(c, "snapshot="+nestedJoin+"();dr_release(snapshot);") {
				t.Fatalf("nested append-return initializer must adopt %s", nestedJoin)
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
	checkOwnedCallFallbacks(t, tr)
}

func checkOwnedCallFallbacks(t *testing.T, tr *TestRunner) {
	t.Helper()
	for _, modules := range []bool{false, true} {
		prefix := ""
		if modules {
			prefix = "__dream_module_0_"
		}
		for _, tc := range []struct{ name, definition, call string }{
			{"literal_borrow", `func string factory() { return "literal"; }`, `factory()`},
			{"parameter_borrow", `func string factory(string source) { return source; }`, `factory("source")`},
			{"static_borrow", `class Store { static string value; } func string factory() { return Store.value; }`, `factory()`},
			{"instance_borrow", `class Store { string value; func string factory() { return this.value; } } Store item = new Store();`, `item.factory()`},
			{"owned_method", `class Store { func string factory() { string result = "owned"; return result; } } Store item = new Store();`, `item.factory()`},
			{"owned_static_method", `class Store { static func string factory() { string result = "owned"; return result; } }`, `Store.factory()`},
			{"extra_local", `func string factory() { string result = "owned"; int extra = 1; return result; }`, `factory()`},
			{"unreachable_statement", `func string factory() { string result = "owned"; return result; Console.WriteLine("later"); }`, `factory()`},
			{"nested_block", `func string factory() { { string result = "owned"; return result; } }`, `factory()`},
			{"mixed_parameter", `func string factory(bool choice,string source) { string result = "owned"; return choice ? result : source; }`, `factory(true,"source")`},
			{"mixed_literal", `func string factory(bool choice) { string result = "owned"; return choice ? result : "literal"; }`, `factory(true)`},
			{"alternative_return", `func string factory(bool choice) { string result = "owned"; if (choice) return result; return "literal"; }`, `factory(true)`},
			{"different_binding", `func string factory(string source) { string result = "owned"; return source; }`, `factory("source")`},
			{"concat_init", `func string factory() { string result = "owned" + "value"; return result; }`, `factory()`},
			{"call_init", `func string other() { return "other"; } func string factory() { string result = other(); return result; }`, `factory()`},
			{"recursive_init", `func string factory() { string result = factory(); return result; }`, `factory()`},
			{"read_init", `func string factory() { string result = Console.ReadLine(); return result; }`, `factory()`},
			{"assignment_init", `func string factory(string source) { string result = (source = "changed"); return result; }`, `factory("source")`},
			{"conditional_init", `func string factory(bool choice) { string result = choice ? "owned" : "other"; return result; }`, `factory(true)`},
			{"global_init", `string backing = "backing"; func string factory() { string result = backing; return result; }`, `factory()`},
			{"field_init", `class Store { static string value; } func string factory() { string result = Store.value; return result; }`, `factory()`},
			{"array_local", `func string factory() { string result[1]; return result; }`, `factory()`},
			{"pointer_local", `func string factory(string *source) { string *result = source; return result; } string values[1];`, `factory(values)`},
			{"unused_pointer_param", `func string factory(string *source) { string result = "owned"; return result; } string values[1];`, `factory(values)`},
			{"loop_body", `func string factory() { string result = "owned"; while (false) { } return result; }`, `factory()`},
			{"write_body", `func string factory() { string result = "owned"; result = "changed"; return result; }`, `factory()`},
			{"alias_body", `func string factory() { string result = "owned"; string alias = result; return result; }`, `factory()`},
			{"exception_body", `func string factory() { string result = "owned"; try { return result; } catch { return result; } }`, `factory()`},
			{"append_borrowed_operand", `func string borrowed(string source) { return source; } func string factory() { string result = "start"; return (result += borrowed(result)); }`, `factory()`},
			{"append_other_destination", `func string owned() { string result = "owned"; return result; } string outer = "outer"; func string factory() { string result = "start"; return (outer += owned()); }`, `factory()`},
			{"append_extra_statement", `func string owned() { string result = "owned"; return result; } func string factory() { string result = "start"; Console.WriteLine(result); return (result += owned()); }`, `factory()`},
			{"append_beyond_initializer_proof", `func string owned() { string result = "owned"; return result; } func string joined() { string result = "start"; return (result += owned()); } func string nested() { string result = "outer"; return (result += joined()); } func string factory() { string result = "third"; return (result += nested()); }`, `factory()`},
		} {
			t.Run(fmt.Sprintf("fallback_%s/modules=%v", tc.name, modules), func(t *testing.T) {
				c := memberReturnEmission(t, tr, tc.definition+` string probe = `+tc.call+`;`, modules)
				assigned := memberReturnEmission(t, tr, tc.definition+` string probe; probe = `+tc.call+`; `+tc.call+`;`, modules)
				call := strings.ReplaceAll(tc.call, "factory(", prefix+"factory(")
				call = strings.ReplaceAll(call, "true", "1")
				if strings.Contains(tc.call, ".") {
					call = "Store_factory()"
					if strings.HasPrefix(tc.call, "item.") {
						call = "Store_factory(((structStore*)item))"
					}
				}
				if !strings.Contains(assigned, "__dream_string_assign(&probe,"+call+");") {
					t.Fatalf("unsupported assignment changed: %s", assigned)
				}
				if !strings.Contains(assigned, "*/"+call+";") || strings.Contains(assigned, "dr_release((void*)"+call) {
					t.Fatalf("unsupported discard changed: %s", assigned)
				}
				if !strings.Contains(c, "probe=dream_string_copy("+call+");") {
					t.Fatalf("unsupported definition/consumer must keep its copy %q:\n%s", call, c)
				}
			})
		}
		t.Run(fmt.Sprintf("other_consumers/modules=%v", modules), func(t *testing.T) {
			c := memberReturnEmission(t, tr, `func string factory() { string result = "owned"; return result; }
func void view(string source) { }
func string forward() { return factory(); }
string target = "start"; target = factory(); target += factory();
string appended = (target += factory());
string nestedAppend = true ? (target += factory()) : "literal";
string compound = true ? factory() : "literal";
Console.WriteLine(factory()); factory(); view(factory());
func string joined() { string result = "start"; return (result += factory()); }
string direct = joined(); target = joined(); joined(); Console.WriteLine(joined());
string nestedJoin = true ? joined() : "literal";
func string nested() { string result = "outer"; return (result += joined()); }
string snapshot = nested(); target = nested(); nested(); Console.WriteLine(nested());
string conditional = true ? nested() : "literal";`, modules)
			for _, want := range []string{
				"(char*)" + prefix + "factory();dr_release(target);target=__dream_owned_0;",
				"{constchar*dreamOwnedArgument0=" + prefix + "factory();__dream_string_append(&target,dreamOwnedArgument0);dr_release((void*)dreamOwnedArgument0);}",
				"appended=dream_string_copy(__dream_string_append_owned(&target," + prefix + "factory()));",
				"nestedAppend=dream_string_copy((1?__dream_string_append(&target," + prefix + "factory()):\"literal\"));",
				"compound=dream_string_copy((1?" + prefix + "factory():\"literal\"));",
				"return" + prefix + "factory();",
				ownedArgumentBlock(prefix+"factory()", "dr_console_writeln", "dreamOwnedArgument0"),
				prefix + "view(" + prefix + "factory());",
				"dr_release((void*)" + prefix + "factory());",
				"direct=" + prefix + "joined();",
				"(char*)" + prefix + "joined();dr_release(target);target=__dream_owned_0;",
				"dr_release((void*)" + prefix + "joined());",
				ownedArgumentBlock(prefix+"joined()", "dr_console_writeln", "dreamOwnedArgument0"),
				"nestedJoin=dream_string_copy((1?" + prefix + `joined():"literal"));`,
				"snapshot=" + prefix + "nested();",
				"(char*)" + prefix + "nested();dr_release(target);target=__dream_owned_0;",
				"dr_release((void*)" + prefix + "nested());",
				"dr_console_writeln(" + prefix + "nested());",
				"conditional=dream_string_copy((1?" + prefix + `nested():"literal"));`,
			} {
				if !strings.Contains(c, want) {
					t.Fatalf("other consumer changed: missing %q", want)
				}
			}
		})
		t.Run(fmt.Sprintf("primitive_params/modules=%v", modules), func(t *testing.T) {
			c := memberReturnEmission(t, tr, `func string factory(int a,float b,bool c,char d,string source) { const string result = source; return result; }
string probe = factory(1,2.0,true,'x',"source");`, modules)
			if !strings.Contains(c, "probe="+prefix+`factory(1,2.0,1,'x',"source");`) {
				t.Fatalf("scalar primitive parameters should qualify:\n%s", c)
			}
		})
		for _, tc := range []struct{ name, source, diagnostic string }{
			{"unresolved", `string probe = missing();`, "undefined function"},
			{"shadow_parameter", `func string factory() { string result = "owned"; return result; } func void use(string factory) { string probe = factory(); }`, "undefined function"},
			{"shadow_local", `func string factory() { string result = "owned"; return result; } func void use() { string factory = "shadow"; string probe = factory(); }`, "redefinition"},
			{"binding_collision", `func string factory(string source) { string source = "owned"; return source; } string probe = factory("source");`, "redefinition"},
			{"direct_null", `func string factory() { string result = null; return result; } string probe = factory();`, "cannot assign expression of different type"},
			{"async", `async func string factory() { string result = "owned"; return result; } string probe = factory();`, "cannot assign expression of different type"},
		} {
			t.Run(fmt.Sprintf("rejected_%s/modules=%v", tc.name, modules), func(t *testing.T) {
				work := t.TempDir()
				text, args := tc.source, []string{"--dev"}
				diagnostic := tc.diagnostic
				if modules {
					if strings.HasPrefix(tc.name, "shadow_") {
						diagnostic = "conflicts with a module function or import"
					}
					text = "import labels;\n" + text
					args = append(args, "--multi-file")
					if err := os.WriteFile(filepath.Join(work, "labels.dr"), []byte("module labels;"), 0644); err != nil {
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
				out, err := cmd.CombinedOutput()
				if code, ok := err.(*exec.ExitError); !ok || code.ExitCode() != 1 || !strings.Contains(string(out), diagnostic) {
					t.Fatalf("expected exit 1 with %q: %v\n%s", diagnostic, err, out)
				}
				if _, err := os.Stat(filepath.Join(work, "build")); !os.IsNotExist(err) {
					t.Fatalf("rejected source created artifacts: %v", err)
				}
			})
		}
	}
}
