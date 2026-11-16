package dream_test

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"testing"
	"time"
)

func TestCompiler_BorrowedMemberReturn(t *testing.T) {
	// 34 callee string allocations plus caller backing/replacement, receiver and
	// independent snapshot: all 38 must be released before runtime shutdown.
	checkStorageOwnership(t, "borrowed_member_return.dr", 38, []string{
		`{constchar*__dream_return_0=((structMemberReturnBase*)this)->value;dr_release(discard);dr_release(snapshot);return__dream_return_0;}`,
		`{constchar*__dream_return_2=((structMemberReturnBase*)this)->__dream_return_0;dr_release(__dream_return_1);return__dream_return_2;}`,
		`{constchar*__dream_return_0=((structMemberReturnBase*)this)->value;dr_release(inheritedText);return__dream_return_0;}`,
		`{constchar*__dream_return_0=((structMemberReturnBase*)this)->value;dr_release(inner);dr_release(empty);dr_release(absent);dr_release(first);return__dream_return_0;}`,
		`{constchar*__dream_return_0=((structMemberReturnBase*)this)->value;dr_release(empty);dr_release(absent);dr_release(first);return__dream_return_0;}`,
		`{constchar*__dream_return_0=((structMemberReturnBase*)this)->value;dr_release(whileText);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=((structMemberReturnBase*)this)->value;dr_release(doText);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=((structMemberReturnBase*)this)->value;dr_release(body);dr_release(header);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=((structMemberReturnBase*)this)->value;dr_release(caseText);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_1=((structMemberReturnBase*)this)->value;dr_release(__dream_return_0);return__dream_return_1;}`,
		`{constchar*__dream_return_0=((structMemberReturnBase*)this)->value;dr_release(opsText);return__dream_return_0;}`,
		`return((structMemberReturnBase*)this)->value;`,
	},
		"backing\nbacking\nbacking\nbacking\nbacking\nbacking\nbacking\nbacking\nbacking\nbacking\ncollision\nbacking\nbacking\nbacking\nnull\nnull\n\n\nliteral\nbacking\nchanged\n", true,
		`dr_release(this);`, `dr_release(source);`,
		`dr_retain(`, `dream_string_copy(__dream_return_`)
	checkMemberReturnFallbacks(t, false, false)
}

func checkMemberReturnFallbacks(t *testing.T, conditional, static bool) {
	t.Helper()
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	result := `this.value`
	emitted := `return((structProbe*)this)->value;`
	if conditional {
		result = `stop ? (stop ? this.value : owned) : source`
		emitted = `return(stop?(stop?((structProbe*)this)->value:owned):source);`
	}
	if static {
		result = `Probe.shared`
		emitted = `returnProbe_shared;`
	}
	// Never execute these programs: aliases and writes can make their field or
	// receiver dangling. Inspect both early/later returns before host compilation.
	barriers := []struct{ name, statement string }{
		{"field_write", `this.value = owned;`},
		{"receiver_write", `this = this;`},
		{"other_field_write", `Probe.shared = owned;`},
		{"scalar_write", `int x = 0; x = 1;`},
		{"compound", `source += owned;`},
		{"prefix", `int x = 0; ++x;`},
		{"postfix", `int x = 0; x--;`},
		{"conditional", `stop ? (this.value = owned) : source;`},
		{"initializer", `string copy = (this.value = owned);`},
		{"nested_block", `{ this.value = owned; }`},
		{"if_condition", `if ((this.value = owned) == source) { }`},
		{"if_then", `if (stop) this.value = owned;`},
		{"if_else", `if (stop) { } else this.value = owned;`},
		{"while_condition", `while ((this.value = owned) == source) { break; }`},
		{"while_body", `while (false) { this.value = owned; }`},
		{"do_condition", `do { } while ((this.value = owned) == source);`},
		{"do_body", `do { this.value = owned; } while (false);`},
		{"for_init", `for (this.value = owned; false;) { }`},
		{"for_condition", `for (; (this.value = owned) == source;) { break; }`},
		{"for_update", `for (; false; this.value = owned) { }`},
		{"for_body", `for (; false;) { this.value = owned; }`},
		{"switch_selector", `switch (indices[0] = 1) { default: break; }`},
		{"switch_body", `switch (1) { case 1: this.value = owned; break; }`},
		{"index_write", `indices[0] = 1;`},
		{"index_expression", `indices[(this.value = owned) == source ? 0 : 1];`},
		{"free_call", `store(owned);`},
		{"method_call", `this.store(owned);`},
		{"static_call", `Probe.storeStatic(owned);`},
		{"call_initializer", `string copy = other();`},
		{"call_condition", `if (other() == source) { }`},
		{"call_argument", `store(other());`},
		{"call_header", `for (; false; other()) { }`},
		{"console_write", `Console.WriteLine(owned);`},
		{"console_read", `string copy = Console.ReadLine();`},
		{"constructor", `new Parent();`},
		{"class_local", `Parent fresh = new Parent();`},
		{"exited_alias", `{ Parent alias = this; }`},
		{"aliased_write", `{ Parent alias = this; alias.value = owned; }`},
		{"pointer_local", `string *pointer;`},
		{"array_local", `string values[1];`},
		{"struct_local", `Record record;`},
		{"completed_exception", `try { } catch { } finally { }`},
		{"throw", `throw owned;`},
		{"await", `await source;`},
	}
	if static {
		barriers = append(barriers, []struct{ name, statement string }{
			{"derived_static_write", `Derived.shared = owned;`},
			{"instance_static_write", `this.shared = owned;`},
			{"global_local_backing", `Probe.shared = owned;`},
		}...)
	}
	for _, modules := range []bool{false, true} {
		for _, tc := range barriers {
			for _, before := range []bool{false, true} {
				t.Run(fmt.Sprintf("fallback_%s/before=%v/modules=%v", tc.name, before, modules), func(t *testing.T) {
					statement := tc.statement
					early := `if (stop) return ` + result + `;`
					if before {
						statement, early = early, statement
					}
					text := memberFallbackPrelude + `class Probe : Parent {
string value; static string shared; string *pointer; string values[1];
func init() { }
func void store(string input) { this.value = input; }
static func void storeStatic(string input) { }
func string probe(bool stop, string source, int *indices) {
string owned = "owned";
` + early + statement + `return ` + result + `; } } class Derived : Probe { }`
					c := memberReturnEmission(t, tr, text, modules)
					if strings.Count(c, emitted) != 2 || strings.Contains(c, "__dream_return_") {
						t.Fatalf("barrier must preserve both original returns:\n%s", c)
					}
				})
			}
		}
		// Same-body shape/result controls; these are valid --dev sources even
		// where the existing C backend/shape typing cannot produce a runnable C file.
		for _, tc := range []struct{ name, decl, result, emitted string }{
			{"static", ``, `Probe.shared`, `returnProbe_shared;`},
			{"pointer", ``, `this.pointer`, `return((structProbe*)this)->pointer;`},
			{"array", ``, `this.values`, `return((structProbe*)this)->values;`},
			{"conditional_static", ``, `stop ? this.value : Probe.shared`, `return(stop?((structProbe*)this)->value:Probe_shared);`},
			{"base", ``, `base.value`, `return((structParent*)this)->value;`},
			{"local_receiver", `Parent local = new Parent();`, `local.value`, `return((structParent*)local)->value;`},
			{"struct_receiver", `Record record;`, `record.value`, `returnrecord.value;`},
			{"nested_receiver", ``, `this.next.value`, `return((structParent*)((structProbe*)this)->next)->value;`},
		} {
			t.Run(fmt.Sprintf("shape_%s/modules=%v", tc.name, modules), func(t *testing.T) {
				if conditional {
					tc.result = `stop ? this.value : (` + tc.result + `)`
					tc.emitted = `return(stop?((structProbe*)this)->value:` + strings.TrimSuffix(strings.TrimPrefix(tc.emitted, `return`), `;`) + `);`
				}
				text := memberFallbackPrelude + `class Probe : Parent {
string value; static string shared; string *pointer; string values[1]; Parent next;
func string probe(bool stop, string source) { string owned = "owned"; ` + tc.decl + `return ` + tc.result + `; } }`
				c := memberReturnEmission(t, tr, text, modules)
				if tc.name == "static" && !conditional {
					// P0047 migrates this formerly unsupported direct result to
					// positive exact-C coverage; lifetime is checked separately.
					if !strings.Contains(c, `{constchar*__dream_return_0=Probe_shared;dr_release(owned);return__dream_return_0;}`) {
						t.Fatalf("direct static result must clean locals:\n%s", c)
					}
					return
				}
				if !strings.Contains(c, tc.emitted+`dr_release(`) || strings.Contains(c, "__dream_return_") {
					t.Fatalf("unsupported result must keep direct return before cleanup, wanted %s:\n%s", tc.emitted, c)
				}
			})
		}
		for _, tc := range []struct{ name, method, emitted string }{
			{"active_exception", `func string probe() { string owned = "owned";
try { return this.value; } catch { return this.value; } finally { return this.value; } }`,
				`return((structProbe*)this)->value;`},
		} {
			t.Run(fmt.Sprintf("guard_%s/modules=%v", tc.name, modules), func(t *testing.T) {
				if conditional {
					tc.method = strings.ReplaceAll(tc.method, `return this.value;`, `return true ? this.value : "unused";`)
					tc.emitted = `return(1?((structProbe*)this)->value:"unused");`
				}
				c := memberReturnEmission(t, tr, `class Probe { string value; `+tc.method+` }`, modules)
				if !strings.Contains(c, tc.emitted) || strings.Contains(c, "__dream_return_") {
					t.Fatalf("guard must preserve previous return emission:\n%s", c)
				}
			})
		}
		t.Run(fmt.Sprintf("guard_async/modules=%v", modules), func(t *testing.T) {
			// Async instance methods are rejected by today's class grammar. Use
			// an accepted free async function/static field as an emission control.
			c := memberReturnEmission(t, tr, `class Probe { static string shared; }
async func string probe() { string owned = "owned"; return Probe.shared; }`, modules)
			if !strings.Contains(c, `_current_task,Probe_shared);returnNULL;`) || strings.Contains(c, "__dream_return_") {
				t.Fatalf("async worker must preserve task-result emission:\n%s", c)
			}
		})
	}
}

const memberFallbackPrelude = `class Parent { string value; }
struct Record { string value; }
func void store(string input) { }
func string other() { return "other"; }
`

func memberReturnEmission(t *testing.T, tr *TestRunner, text string, modules bool) string {
	t.Helper()
	work := t.TempDir()
	args := []string{"--dev"}
	if modules {
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
	cmd.Dir = tr.projectRoot
	if out, err := cmd.CombinedOutput(); err != nil {
		t.Fatalf("member emission failed: %v\n%s", err, out)
	}
	data, err := os.ReadFile(filepath.Join(tr.projectRoot, "build", "bin", "dream.c"))
	if err != nil {
		t.Fatal(err)
	}
	return strings.Join(strings.Fields(string(data)), "")
}
