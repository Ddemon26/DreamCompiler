package dream_test

import (
	"fmt"
	"strings"
	"testing"
)

func TestCompiler_ConditionalMemberReturn(t *testing.T) {
	// Three mixed/nullable selected string locals and one selected for-header
	// survive with their original references; every other allocation is reclaimed.
	// 58 callee allocations minus four transferred locals, plus five caller
	// allocations (receiver, backing/replacement, external source and snapshot).
	checkStorageOwnershipWithLiveCount(t, "conditional_member_return.dr", 59, []string{
		`{constchar*__dream_return_0=(choice?((structConditionalMemberBase*)this)->value:((structConditionalMemberBase*)this)->alternate);dr_release(discard);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(a?(b?((structConditionalMemberBase*)this)->value:first):(b?source:"literal"));dr_release(inner);dr_release(empty);dr_release(absent);dr_release(right);if(first!=__dream_return_0)dr_release(first);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(a?((structConditionalMemberBase*)this)->alternate:right);dr_release(empty);dr_release(absent);if(right!=__dream_return_0)dr_release(right);dr_release(first);return__dream_return_0;}`,
		`{constchar*__dream_return_0=((path==0)?((structConditionalMemberBase*)this)->value:((path==1)?absent:empty));dr_release(discard);if(empty!=__dream_return_0)dr_release(empty);if(absent!=__dream_return_0)dr_release(absent);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?((structConditionalMemberBase*)this)->value:((structConditionalMemberBase*)this)->value);dr_release(discard);return__dream_return_0;}`,
		`{constchar*__dream_return_2=(choice?((structConditionalMemberBase*)this)->__dream_return_0:"unused");dr_release(__dream_return_1);return__dream_return_2;}`,
		`{constchar*__dream_return_0=(choice?((structConditionalMemberBase*)this)->value:header);dr_release(body);if(header!=__dream_return_0)dr_release(header);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?((structConditionalMemberBase*)this)->value:"after");dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?((structConditionalMemberBase*)this)->value:"while");dr_release(inner);dr_release(outer);return__dream_return_0;}`,
		`{constchar*__dream_return_0=(choice?((structConditionalMemberBase*)this)->value:"case");dr_release(caseText);dr_release(outer);return__dream_return_0;}`,
		`return(choice?((structConditionalMemberBase*)this)->value:"unused");`,
	},
		"backing\nbacking\nsource\nliteral\nalternate\nright\nbacking\nnull\n\nnull\n\nliteral\nbacking\nbacking\ncollision\nunused\nbacking\nheader\nbacking\nafter\nbacking\nbacking\nbacking\nalternate\nbacking\nbacking\nchanged\n", 4,
		`dr_release(this);`, `dr_release(source);`, `dr_retain(`,
		`dream_string_copy(__dream_return_`)
	checkMemberReturnFallbacks(t, true, false)
	checkConditionalMemberExpressions(t)
}

func checkConditionalMemberExpressions(t *testing.T) {
	t.Helper()
	tr := NewTestRunner(t)
	defer tr.Cleanup()
	// A bad condition or a single unsupported leaf must preserve the entire
	// old return, including when that leaf is never selected. Do not execute.
	for _, modules := range []bool{false, true} {
		for _, tc := range []struct{ name, result, emitted string }{
			{"condition_field_write", `(this.value = owned) == source ? this.value : owned`, `(((structProbe*)this)->value=owned)==source`},
			{"condition_parameter_write", `(source = owned) == owned ? this.value : source`, `(source=owned)==owned`},
			{"condition_call", `other() == source ? this.value : "literal"`, `other()==source`},
			{"unselected_call", `true ? this.value : other()`, `1?((structProbe*)this)->value:`},
			{"unselected_concat", `true ? this.value : source + "x"`, `1?((structProbe*)this)->value:(dream_concat(source,"x"))`},
			{"unselected_assignment", `true ? this.value : (source = owned)`, `1?((structProbe*)this)->value:(source=owned)`},
			{"unselected_index", `true ? this.value : values[0]`, `1?((structProbe*)this)->value:(values[0])`},
			{"nested_else_member", `stop ? "literal" : (stop ? source : this.value)`, ``},
		} {
			t.Run(fmt.Sprintf("expression_%s/modules=%v", tc.name, modules), func(t *testing.T) {
				text := memberFallbackPrelude + `class Probe { string value;
func string probe(bool stop, string source, string *values) { string owned = "owned"; return ` + tc.result + `; } }`
				c := memberReturnEmission(t, tr, text, modules)
				if tc.emitted == "" {
					if !strings.Contains(c, `{constchar*__dream_return_0=(stop?"literal":(stop?source:((structProbe*)this)->value));dr_release(owned);return__dream_return_0;}`) {
						t.Fatalf("nested else member must qualify:\n%s", c)
					}
				} else if strings.Contains(c, "__dream_return_") || !strings.Contains(c, tc.emitted) || !strings.Contains(c, `;dr_release(owned);`) {
					t.Fatalf("unsupported expression must preserve original return:\n%s", c)
				}
			})
		}
	}
}
