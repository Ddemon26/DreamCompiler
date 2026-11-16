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

func TestCompiler_StaticMemberReturn(t *testing.T) {
	// Independent allocation count: nested true 4*3 + false 4, readThis 2+1+2,
	// inherited 1, paths 2+2+3+2+1, collisions 1+1, ops/free/hidden 1 each = 36.
	// Caller backing/replacement, receiver and snapshot add four: 40 releases.
	checkStorageOwnershipWithLiveCount(t, "static_member_return.dr", 40, []string{
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(discard);dr_release(snapshot);return__dream_return_0;}`,
		`{constchar*__dream_return_2=StaticReturnBase___dream_return_0;dr_release(__dream_return_1);return__dream_return_2;}`,
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(inheritedText);return__dream_return_0;}`,
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(inner);dr_release(empty);dr_release(absent);dr_release(first);return__dream_return_0;}`,
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(empty);dr_release(absent);dr_release(first);return__dream_return_0;}`,
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(whileText);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(doText);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(body);dr_release(header);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(caseText);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_1=StaticReturnBase_value;dr_release(__dream_return_0);return__dream_return_1;}`,
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(opsText);return__dream_return_0;}`,
		`{constchar*__dream_return_0=StaticReturnBase_value;dr_release(freeText);return__dream_return_0;}`,
		`{constchar*__dream_return_0=StaticReturnChild_hidden;dr_release(hiddenText);return__dream_return_0;}`,
		`returnStaticReturnBase_value;`,
		`(StaticReturnBase_value=absent);`,
	},
		"backing\nbacking\nbacking\nbacking\nbacking\nbacking\nbacking\nbacking\nbacking\nbacking\ncollision\nbacking\nbacking\nbacking\nbacking\nhidden\nnull\nnull\n\n\nliteral\nbacking\nchanged\n", 0,
		`dr_release(this);`, `dr_release(source);`, `dr_release(StaticReturnBase_value);`,
		`dr_retain(`, `dream_string_copy(__dream_return_`)
	checkMemberReturnFallbacks(t, false, true)
	checkStaticMemberExpressions(t)
	checkStaticMemberRejections(t)
}

// These are emission-only: unsupported shapes and aliases need not produce valid
// host C, and writes may deliberately install dangling backing. Never run them.
func checkStaticMemberExpressions(t *testing.T) {
	t.Helper()
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	for _, modules := range []bool{false, true} {
		for _, tc := range []struct{ name, decl, body, result, emitted string }{
			{"pointer", `static string *pointer;`, ``, `Store.pointer`, `Store_pointer`},
			{"array", `static string values[1];`, ``, `Store.values`, `Store_values`},
			{"instance_field", `string plain;`, ``, `Store.plain`, ``},
			{"struct", ``, ``, `Record.shared`, `Record_shared`},
			{"variable", ``, `Parent alias = new Parent();`, `alias.shared`, `Parent_shared`},
			{"this", ``, ``, `this.shared`, `Store_shared`},
			{"base", ``, ``, `base.shared`, `Parent_shared`},
			{"nested", `Parent next;`, ``, `this.next.shared`, `Parent_shared`},
			{"call", ``, ``, `new Parent().shared`, `Parent_shared`},
			{"conditional_static", ``, ``, `stop ? Store.shared : Store.shared`, `(stop?Store_shared:Store_shared)`},
			{"conditional_local", ``, ``, `stop ? Store.shared : owned`, `(stop?Store_shared:owned)`},
			{"conditional_literal", ``, ``, `stop ? Store.shared : "literal"`, `(stop?Store_shared:"literal")`},
			{"conditional_instance", `string plain;`, ``, `stop ? Store.shared : this.plain`, `(stop?Store_shared:((structStore*)this)->plain)`},
			{"unselected_call", ``, `true ? owned : other();`, `Store.shared`, `Store_shared`},
			{"active_exception", ``, `TRY`, `Store.shared`, `Store_shared`},
		} {
			// Type-qualified nonstatic fields are rejected semantically, so
			// cover their valid instance form instead of changing semantics.
			if tc.name == "instance_field" {
				tc.result = `this.plain`
				tc.emitted = `((structStore*)this)->plain`
			}
			t.Run(fmt.Sprintf("static_%s/modules=%v", tc.name, modules), func(t *testing.T) {
				body := `string owned = "owned"; ` + tc.body + `return ` + tc.result + `;`
				if tc.body == "TRY" {
					body = `string owned = "owned"; try { return Store.shared; } catch { return Store.shared; } finally { }`
				}
				c := memberReturnEmission(t, tr, `class Parent { static string shared; }
struct Record { static string shared; }
func string other() { return "other"; }
class Store : Parent { static string shared; `+tc.decl+`
func string probe(bool stop) { `+body+` } }`, modules)
				if tc.name == "instance_field" {
					if !strings.Contains(c, `{constchar*__dream_return_0=((structStore*)this)->plain;dr_release(owned);return__dream_return_0;}`) {
						t.Fatalf("preserve prior instance support:\n%s", c)
					}
				} else if strings.Contains(c, "__dream_return_") || !strings.Contains(c, `return`+tc.emitted+`;`) {
					t.Fatalf("unsupported static expression must preserve direct return %s:\n%s", tc.emitted, c)
				}
			})
		}
	}
}

// Semantics rejects local type-name shadowing and primitive parameter/index
// receivers. Keep these diagnostic boundaries explicit; emitter guards remain.
func checkStaticMemberRejections(t *testing.T) {
	t.Helper()
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	for _, modules := range []bool{false, true} {
		for _, tc := range []struct{ name, params, before, after string }{
			{"active", ``, `int Store = 0;`, ``},
			{"exited", ``, `{ int Store = 0; }`, ``},
			{"later", ``, ``, `int Store = 0;`},
			{"header", ``, `for (int Store = 0; false;) { }`, ``},
			{"parameter", `int Store`, ``, ``},
			{"index", ``, ``, ``},
		} {
			t.Run(fmt.Sprintf("rejection_%s/modules=%v", tc.name, modules), func(t *testing.T) {
				work := t.TempDir()
				text := `class Store { static string shared; }
func string probe(` + tc.params + `) { string owned = "owned"; ` + tc.before + `return Store.shared; ` + tc.after + ` }`
				if tc.name == "index" {
					text = `class Store { static string shared; }
func string probe() { string owned = "owned"; Store values[1]; return values[0].shared; }`
				}
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
				cmd.Dir = work
				out, err := cmd.CombinedOutput()
				diagnostic := "redefinition of 'Store'"
				if tc.name == "parameter" || tc.name == "index" {
					diagnostic = "field receiver must be a class or struct"
				}
				if exit, ok := err.(*exec.ExitError); ctx.Err() != nil || !ok || exit.ExitCode() != 1 || !strings.Contains(string(out), diagnostic) {
					t.Fatalf("expected bounded semantic rejection: %v\n%s", err, out)
				}
				if _, err := os.Stat(filepath.Join(work, "build")); !os.IsNotExist(err) {
					t.Fatalf("rejected source created artifacts: %v", err)
				}
			})
		}
	}
}
