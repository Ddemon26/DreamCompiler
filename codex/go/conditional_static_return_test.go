package dream_test

import (
	"fmt"
	"strings"
	"testing"
)

func TestCompiler_ConditionalStaticReturn(t *testing.T) {
	// Three mixed/nullable selected string locals and one selected for-header
	// survive with their original references; every other allocation is reclaimed.
	// Callee tally: mixed 4*4+5*2, nullable 6*2, repeated/collision 2 each,
	// loop 3*2+2*2, alternatePath 2*2, members 2, four new readers 2 each = 66.
	// Minus four transferred locals, plus five caller allocations (receiver,
	// backing/replacement, external source and snapshot) gives 67 releases.
	checkStorageOwnershipWithLiveCount(t, "conditional_static_return.dr", 67, []string{
		`{constchar*__dream_return_0=(choice?ConditionalStaticBase_value:ConditionalStaticBase_alternate);dr_release(discard);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(a?(b?ConditionalStaticBase_value:first):(b?source:"literal"));dr_release(inner);dr_release(empty);dr_release(absent);dr_release(right);if(first!=__dream_return_0)dr_release(first);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(a?ConditionalStaticBase_alternate:right);dr_release(empty);dr_release(absent);if(right!=__dream_return_0)dr_release(right);dr_release(first);return__dream_return_0;}`,
		`{constchar*__dream_return_0=((path==0)?ConditionalStaticBase_value:((path==1)?absent:empty));dr_release(discard);if(empty!=__dream_return_0)dr_release(empty);if(absent!=__dream_return_0)dr_release(absent);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?ConditionalStaticBase_value:ConditionalStaticBase_value);dr_release(discard);return__dream_return_0;}`,
		`{constchar*__dream_return_2=(choice?ConditionalStaticBase___dream_return_0:"unused");dr_release(__dream_return_1);return__dream_return_2;}`,
		`{constchar*__dream_return_0=(choice?ConditionalStaticBase_value:header);dr_release(body);if(header!=__dream_return_0)dr_release(header);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?ConditionalStaticBase_value:"after");dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?ConditionalStaticBase_value:"while");dr_release(inner);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?ConditionalStaticBase_value:"case");dr_release(caseText);dr_release(outer);return__dream_return_0;}`,
		`return(choice?ConditionalStaticBase_value:"unused");`,
		`{constchar*__dream_return_0=(choice?ConditionalStaticBase_value:((structConditionalStaticBase*)this)->instanceValue);dr_release(discard);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?ConditionalStaticBase_value:"static");dr_release(discard);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?ConditionalStaticBase_value:"free");dr_release(freeText);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?ConditionalStaticHidden_value:ConditionalStaticBase_value);dr_release(hiddenText);return__dream_return_0;}`,
		`(ConditionalStaticBase_value=absent);`,
	},
		"backing\nbacking\nsource\nliteral\nalternate\nright\nbacking\nnull\n\nnull\n\nliteral\nbacking\nbacking\ncollision\nunused\nbacking\nheader\nbacking\nafter\nbacking\nbacking\nbacking\nalternate\nbacking\nbacking\nbacking\nbacking\nstatic\nbacking\nfree\nhidden\nbacking\nbacking\nchanged\n", 4,
		`dr_release(this);`, `dr_release(source);`, `dr_retain(`,
		`dream_string_copy(__dream_return_`, `dr_release(ConditionalStaticBase_value);`)
	checkMemberReturnFallbacks(t, true, true)
	checkConditionalStaticExpressions(t)
}

func checkConditionalStaticExpressions(t *testing.T) {
	t.Helper()
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	// A single unsupported leaf or effect, even in an unselected branch,
	// preserves the entire return. These deliberately unsafe sources never run.
	for _, modules := range []bool{false, true} {
		for _, tc := range []struct{ name, decl, body, result, emitted string }{
			{"field_write", ``, ``, `(Store.shared = owned) == source ? Store.shared : owned`, `(Store_shared=owned)==source`},
			{"parameter_write", ``, ``, `(source = owned) == owned ? Store.shared : source`, `(source=owned)==owned`},
			{"condition_call", ``, ``, `other() == source ? Store.shared : "literal"`, `other()==source`},
			{"unselected_call", ``, ``, `true ? Store.shared : other()`, `1?Store_shared:other()`},
			{"unselected_concat", ``, ``, `true ? Store.shared : source + "x"`, `1?Store_shared:(dream_concat(source,"x"))`},
			{"unselected_assignment", ``, ``, `true ? Store.shared : (source = owned)`, `1?Store_shared:(source=owned)`},
			{"unselected_index", ``, ``, `true ? Store.shared : values[0]`, `1?Store_shared:(values[0])`},
			{"pointer", `static string *pointer;`, ``, `stop ? Store.shared : Store.pointer`, `stop?Store_shared:Store_pointer`},
			{"array", `static string array[1];`, ``, `stop ? Store.shared : Store.array`, `stop?Store_shared:Store_array`},
			{"struct", ``, ``, `stop ? Store.shared : Record.shared`, `stop?Store_shared:Record_shared`},
			{"this_static", ``, ``, `stop ? Store.shared : this.shared`, `stop?Store_shared:Store_shared`},
			{"base_static", ``, ``, `stop ? Store.shared : base.shared`, `stop?Store_shared:Parent_shared`},
			{"nested_static", `Parent next;`, ``, `stop ? Store.shared : this.next.shared`, `stop?Store_shared:Parent_shared`},
			{"variable_static", ``, `Parent alias = new Parent();`, `stop ? Store.shared : alias.shared`, `stop?Store_shared:Parent_shared`},
			{"nested_else", ``, ``, `stop ? "literal" : (stop ? source : Store.shared)`, ``},
			{"two_qualifiers", ``, ``, `stop ? Store.shared : Parent.shared`, ``},
		} {
			t.Run(fmt.Sprintf("conditional_static_%s/modules=%v", tc.name, modules), func(t *testing.T) {
				if tc.name == "unselected_call" && modules {
					tc.emitted = `1?Store_shared:__dream_module_0_other()`
				}
				c := memberReturnEmission(t, tr, `class Parent { static string shared; }
struct Record { static string shared; }
func string other() { return "other"; }
class Store : Parent { static string shared; `+tc.decl+`
func string probe(bool stop, string source, string *values) {
string owned = "owned"; `+tc.body+`return `+tc.result+`; } }`, modules)
				if tc.emitted == "" {
					expr := `(stop?"literal":(stop?source:Store_shared))`
					if tc.name == "two_qualifiers" {
						expr = `(stop?Store_shared:Parent_shared)`
					}
					if !strings.Contains(c, `{constchar*__dream_return_0=`+expr+`;dr_release(owned);return__dream_return_0;}`) {
						t.Fatalf("safe static leaves must qualify:\n%s", c)
					}
				} else if strings.Contains(c, "__dream_return_") || !strings.Contains(c, tc.emitted) || !strings.Contains(c, `;dr_release(owned);`) {
					t.Fatalf("unsupported leaf must preserve entire old return:\n%s", c)
				}
			})
		}
		for _, guard := range []string{"exception", "async"} {
			t.Run(fmt.Sprintf("conditional_static_%s/modules=%v", guard, modules), func(t *testing.T) {
				text := `class Store { static string shared; }
async func string probe(bool stop) { string owned = "owned"; return stop ? Store.shared : "literal"; }`
				want := `_current_task,(stop?Store_shared:"literal"));returnNULL;`
				if guard == "exception" {
					text = `class Store { static string shared; }
func string probe(bool stop) { string owned = "owned";
try { return stop ? Store.shared : "literal"; }
catch { return stop ? Store.shared : "literal"; }
finally { return stop ? Store.shared : "literal"; } }`
					want = `return(stop?Store_shared:"literal");`
				}
				c := memberReturnEmission(t, tr, text, modules)
				if strings.Contains(c, "__dream_return_") || !strings.Contains(c, want) {
					t.Fatalf("guard must preserve previous result emission:\n%s", c)
				}
			})
		}
	}
}
