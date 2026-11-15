package dream_test

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
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

func TestCompiler_IdentifierStrings(t *testing.T) {
	checkStringOwnership(t, "identifier_string_copy.dr", 12, []string{
		"first=dream_string_copy(borrowed);",
		"nested=dream_string_copy(first);",
		"chained=dream_string_copy(nested);",
		"staticText=dream_string_copy(staticInput);",
		"instanceText=dream_string_copy(instanceInput);",
		"snapshot=dream_string_copy(backing);",
		"absent=dream_string_copy(nullable);",
		"globalSnapshot=dream_string_copy(globalBacking);",
		"repeated=dream_string_copy(globalSnapshot);",
		`backing=(dream_concat("owned","value"));`,
		`globalBacking=(dream_concat("dynamic","value"));`,
		"dr_release(first);", "dr_release(nested);", "dr_release(chained);",
		"dr_release(staticText);", "dr_release(instanceText);",
		"dr_release(snapshot);", "dr_release(backing);", "dr_release(repeated);",
		"dr_release(nullable);", "dr_release(absent);",
	},
		"literal\nliteral\nliteral\nliteral\nliteral\ndynamic value\ndynamic value\ndynamic value\ndynamic value\ndynamic value\nstatic\ninstance\nowned value\nowned value\ndynamic value\ndynamic value\nnull\ndynamic value\ndynamic value\n")
}

func TestCompiler_MemberStrings(t *testing.T) {
	checkStringOwnership(t, "member_string_copy.dr", 16, []string{
		"thisCopy=dream_string_copy(((structBase*)this)->value);",
		"nested=dream_string_copy(((structBase*)this)->value);",
		"staticCopy=dream_string_copy(Base_shared);",
		"baseCopy=dream_string_copy(((structBase*)this)->value);",
		"fieldCopy=dream_string_copy(((structBase*)literalOwner)->value);",
		"dynamicCopy=dream_string_copy(((structBase*)managedOwner)->value);",
		"absent=dream_string_copy(((structBase*)nullOwner)->value);",
		"temporary=dream_string_copy(((structFresh*)({structFresh*tmp=dr_alloc(sizeof(structFresh));Fresh_init(tmp);tmp;}))->value);",
		"globalCopy=dream_string_copy(((structBase*)globalOwner)->value);",
		"repeated=dream_string_copy(Base_shared);",
		`backing=(dream_concat("managed","value"));`,
		`globalBacking=(dream_concat("static","value"));`,
		"dr_release(thisCopy);", "dr_release(nested);", "dr_release(staticCopy);",
		"dr_release(baseCopy);", "dr_release(fieldCopy);", "dr_release(literalOwner);",
		"dr_release(dynamicCopy);", "dr_release(managedOwner);", "dr_release(backing);",
		"dr_release(absent);", "dr_release(nullOwner);", "dr_release(temporary);",
		"dr_release(repeated);",
	}, "literal\nliteral\nliteral\nliteral\nliteral\nliteral\nliteral\nliteral\nmanaged value\nmanaged value\nmanaged value\nmanaged value\nmanaged value\nmanaged value\nmanaged value\nmanaged value\nmanaged value\nnull\ncreated\ntemporary\nstatic value\nstatic value\nstatic value\nglobal\nglobal\nglobal\nchanged\n")
}

func TestCompiler_ForStrings(t *testing.T) {
	checkStringOwnership(t, "for_string_copy.dr", 9, []string{
		`zero=dream_string_copy("zero");`,
		`repeated=dream_string_copy(source);`,
		`indexed=dream_string_copy((values[(index++)]));`,
		`member=dream_string_copy(HeaderSource_text);`,
		`managed=(dream_concat("owned","value"));`,
		`absent=dream_string_copy((values[1]));`,
		`stopped=dream_string_copy("break");`,
		`continued=dream_string_copy("continue");`,
		`repeated=dream_string_copy("reused");`,
		`top=dream_string_copy("top");`,
		"}dr_release(zero);}", "}dr_release(repeated);}",
		"}dr_release(indexed);}", "}dr_release(member);}",
		"}dr_release(managed);}", "}dr_release(absent);}",
		"}dr_release(stopped);}", "}dr_release(continued);}",
		"}dr_release(top);}",
	},
		"source value\nsource value\nsource value\nsource value\n1\nfield\nowned value\nnull\nbreak\ncontinue\nsource value\nreused\ntop\n")
}

func TestCompiler_CallStrings(t *testing.T) {
	// P0022 additionally releases globalBacking's old allocation on replacement.
	checkStringOwnership(t, "call_string_copy.dr", 15, []string{
		`literalText=dream_string_copy(@entry@literal());`,
		`emptyText=dream_string_copy(@entry@empty());`,
		`alias=dream_string_copy(@entry@borrow(backing));`,
		`nested=dream_string_copy(@entry@borrow(alias));`,
		`freshText=dream_string_copy(@entry@fresh());`,
		`localText=dream_string_copy(@entry@fromLocal());`,
		`nullText=dream_string_copy(@entry@borrow(absent));`,
		`staticText=dream_string_copy(Reader_getStatic());`,
		`instanceText=dream_string_copy(Reader_getInstance(((structReader*)reader)));`,
		`baseText=dream_string_copy(Base_get(((structBase*)this)));`,
		`zero=dream_string_copy(@entry@literal());`,
		`header=dream_string_copy(@entry@borrow(backing));`,
		`repeated=dream_string_copy(@entry@borrow("tick"));`,
		`globalText=dream_string_copy(@entry@once());`,
		`globalSnapshot=dream_string_copy(@entry@borrow(globalBacking));`,
		`__dream_string_assign(&globalBacking,"changed");`,
		`dr_release(literalText);`, `dr_release(emptyText);`, `dr_release(alias);`,
		`dr_release(nested);`, `dr_release(freshText);`, `dr_release(localText);`,
		`dr_release(nullText);`, `dr_release(staticText);`, `dr_release(instanceText);`,
		`dr_release(reader);`, `dr_release(baseText);`, `dr_release(repeated);`,
		`}dr_release(zero);}`, `}dr_release(header);}`,
	},
		"literal\nempty\nbacking value\nbacking value\nbacking value\nfresh value\nlocal value\nnull\nstatic\ninstance\nbase\ntick\ntick\nbacking value\nbacking value\nonce\nbacking value\nbacking value\nglobal\n")
}

func TestCompiler_AssignedStrings(t *testing.T) {
	checkStringOwnership(t, "assigned_string_copy.dr", 39, []string{
		`char*copy=dream_string_copy(text);dr_release(*slot);*slot=copy;returncopy;`,
		`replaced=dream_string_copy("old");`,
		`__dream_string_assign(&replaced,"literal");`,
		`__dream_string_assign(&text,backing);`,
		`__dream_string_assign(&text,text);`,
		`__dream_string_assign(&text,@entry@borrow(text));`,
		`__dream_string_assign(&text,source);`,
		`__dream_string_assign(&text,(values[(index++)]));`,
		`__dream_string_assign(&text,absent);`,
		`__dream_string_assign(&text,((void*)0));`,
		`__dream_string_assign(&text,"");`,
		`__dream_string_assign(&text,@entry@once(backing));`,
		`__dream_string_assign(&text,(dream_concat(text,"appended")));`,
		`__dream_string_assign(&text,@entry@fresh());`,
		`@entry@borrow(__dream_string_assign(&text,"expression"))`,
		`__dream_string_assign(&text,__dream_string_assign(&other,"chain"));`,
		`__dream_string_assign(&text,__dream_string_assign(&text,"nestedassignment"));`,
		`pending=NULL;`, `untouched=NULL;`,
		`__dream_string_assign(&pending,"pending");`,
		`managed=(dream_concat("managed","old"));`,
		`__dream_string_assign(&managed,backing);`,
		`__dream_string_assign(&nullStart,backing);`,
		`__dream_string_assign(&nullStart,((void*)0));`,
		`__dream_string_assign(&header,@entry@borrow(header)))`,
		`}dr_release(header);}`, `__dream_string_assign(&repeated,backing);`,
		`__dream_string_assign(&staticText,"static");`,
		`__dream_string_assign(&instanceText,"instance");`,
		`__dream_string_assign(&baseText,((structBase*)this)->field);`,
		`(input="parameter");`, `(((structBase*)this)->field="base");`,
		`((nullable[0])=globalBacking);`,
		`__dream_string_assign(&globalText,"topliteral");`,
		`__dream_string_assign(&globalText,globalBacking);`,
		`dr_release(text);`, `dr_release(other);`, `dr_release(pending);`,
		`dr_release(untouched);`, `dr_release(managed);`, `dr_release(nullStart);`,
	}, "literal\nbacking value\nbacking value\nbacking value\nnested value\nbacking value\n1\nnull\nstill null\nempty\nonce\nbacking value\nbacking value appended\nfresh value\nexpression\nchain\nchain\nnested assignment\npending\nbacking value\ncleared\nheader\nheader\nbacking value\nbacking value\nstatic\ninstance\nbase\nparameter\nbacking value\ntop literal\nbacking value\nbacking value\n")
}

func TestCompiler_StringContainers(t *testing.T) {
	t.Run("array", func(t *testing.T) {
		checkStringOwnership(t, "array_string_storage.dr", 8, []string{
			`char*localValues[4];`, `char*nestedValues[1];`,
			`nestedCopy=dream_string_copy((nestedValues[0]));`,
			`__dream_string_assign(&owner,"changed");`,
			`dr_release(nestedCopy);`, `dr_release(repeatedCopy);`,
			`dr_release(staticCopy);`, `dr_release(instanceCopy);`,
			`dr_release(owner);`, `dr_release(reader);`,
		}, "owned array\nliteral\nbacking value\nowned array\nnull element\nbacking value\nowned array\nchanged\nchanged\nchanged\nstatic\ninstance\nbacking value\n",
			`dr_release(localValues);`, `dr_release(nestedValues);`,
			`dr_release(repeatedValues);`, `dr_release(unusedValues);`,
			`dr_release(staticValues);`, `dr_release(instanceValues);`)
	})
	t.Run("pointer", func(t *testing.T) {
		checkStringOwnership(t, "pointer_string_storage.dr", 7, []string{
			`char**alias=input;`, `char**nestedAlias=alias;`,
			`(nestedAlias=other);`, `(nullAlias=((void*)0));`,
			`nestedCopy=dream_string_copy((nestedAlias[0]));`,
			`reboundCopy=dream_string_copy((nestedAlias[0]));`,
			`dr_release(nestedCopy);`, `dr_release(reboundCopy);`,
			`dr_release(absentCopy);`, `dr_release(repeatedCopy);`,
			`dr_release(staticCopy);`, `dr_release(instanceCopy);`,
			`dr_release(reader);`,
		}, "backing value\nalternate\nnull element\nbacking value\nbacking value\nnull pointer\nbacking value\nbacking value\nbacking value\nbacking value\nbacking value\n",
			`dr_release(alias);`, `dr_release(nestedAlias);`,
			`dr_release(nullAlias);`, `dr_release(repeatedAlias);`,
			`dr_release(unusedAlias);`, `dr_release(staticAlias);`,
			`dr_release(instanceAlias);`, `dr_release(input);`, `dr_release(other);`)
	})
}

func TestCompiler_LoopExitStrings(t *testing.T) {
	checkStringOwnership(t, "loop_exit_strings.dr", 37, []string{
		// The exact sequences end at the jump: later declarations, surviving
		// outer bindings and for headers must not be included in cleanup.
		`dr_release(deep);dr_release(absent);dr_release(empty);dr_release(whileBody);continue;}`,
		`dr_release(deep);dr_release(absent);dr_release(empty);dr_release(whileBody);break;}`,
		`dr_release(doBody);continue;}`, `dr_release(doBody);break;}`,
		`dr_release(switchContinue);dr_release(forBody);continue;}`,
		`dr_release(switchBreak);break;}`, `{dr_release(forBody);break;}`,
		`{dr_release(innerBody);continue;}`, `{dr_release(innerBody);break;}`,
		`}dr_release(innerHeader);}`, `{dr_release(outerBody);break;}`,
		`{dr_release(loopInSwitch);break;}`, `{dr_release(caseOwner);break;}`,
		`{dr_release(staticText);continue;}`, `{dr_release(instanceText);break;}`,
		`{dr_release(topBody);continue;}`, `{dr_release(topBody);break;}`,
		`}dr_release(header);}`, `}dr_release(topHeader);}`,
	},
		"backing value\nnormal\ndo value\nbacking value\nbacking value\ndefault\nbacking value\nbacking value\nouter loop\ncase\nouter\nbacking value\nstatic\ninstance\ntop\nbacking value\n",
		`dr_release(localValues);`, `dr_release(alias);`, `dr_release(source);`,
		`dr_release(input);`)
}

func TestCompiler_ConditionalStrings(t *testing.T) {
	checkStringOwnership(t, "conditional_string_copy.dr", 22, []string{
		// 19 exercise releases + two method locals + replaced top-level backing.
		// Wrapping the complete conditional preserves C's selected-branch evaluation.
		`literal=dream_string_copy((1?"literal":@entry@forbidden()));`,
		`empty=dream_string_copy((0?@entry@forbidden():""));`,
		`borrowed=dream_string_copy((0?@entry@forbidden():backing));`,
		`reverse=dream_string_copy((1?backing:@entry@forbidden()));`,
		`nullThen=dream_string_copy((1?absent:@entry@forbidden()));`,
		`nullElse=dream_string_copy((0?@entry@forbidden():absent));`,
		`nested=dream_string_copy((1?(0?@entry@forbidden():backing):@entry@forbidden()));`,
		`effects=dream_string_copy((@entry@condition()?@entry@selected():@entry@forbidden()));`,
		`indexed=dream_string_copy((1?(values[(index++)]):(values[(index++)])));`,
		`selectedCopy=dream_string_copy((1?source:absent));`,
		`managedThen=dream_string_copy((1?(dream_concat("owned","then")):@entry@forbidden()));`,
		`managedElse=dream_string_copy((0?@entry@forbidden():(dream_concat("owned","else"))));`,
		`replaced=dream_string_copy((1?backing:absent));`,
		`__dream_string_assign(&replaced,"changed");`,
		`header=dream_string_copy((1?backing:absent));`,
		`repeated=dream_string_copy(((i==0)?backing:header));`,
		`stopped=dream_string_copy((0?absent:backing));`,
		`dr_release(repeated);continue;`, `dr_release(stopped);break;`,
		`}dr_release(header);}`, `dr_release(selectedCopy);`,
		`staticText=dream_string_copy((0?@entry@forbidden():"static"));`,
		`instanceText=dream_string_copy((1?"instance":@entry@forbidden()));`,
		`globalSnapshot=dream_string_copy((0?@entry@forbidden():globalBacking));`,
		`dr_release(literal);`, `dr_release(borrowed);`, `dr_release(reverse);`,
		`dr_release(nullThen);`, `dr_release(nullElse);`, `dr_release(nested);`,
		`dr_release(effects);`, `dr_release(indexed);`, `dr_release(snapshot);`,
	}, "literal\nempty\nbacking value\nbacking value\nnull then\nnull else\nbacking value\ncondition\nselected\ncall\nbacking value\n1\ninner value\ninner value\nowned then\nowned else\nchanged\nbacking value\nbacking value\nbacking value\nbacking value\nstatic\ninstance\nbacking value\n")
}

func TestCompiler_AssignmentInitializers(t *testing.T) {
	// 26 exercise releases, two method copies and two top-level replacements.
	checkStringOwnership(t, "assignment_initializer_copy.dr", 30, []string{
		`copy=dream_string_copy(__dream_string_assign(&source,"next"));`,
		`__dream_string_assign(&copy,"copychanged");`,
		`self=dream_string_copy(__dream_string_assign(&source,source));`,
		`chain=dream_string_copy(__dream_string_assign(&source,__dream_string_assign(&source,"chain")));`,
		`indexed=dream_string_copy(((values[(index++)])=@entry@once(backing)));`,
		`parameter=dream_string_copy((backing="parameter"));`,
		`field=dream_string_copy((Reader_shared=absent));`,
		`empty=dream_string_copy(((values[1])=""));`,
		`nullCopy=dream_string_copy(__dream_string_assign(&source,absent));`,
		`pending=dream_string_copy(((values[0])=backing));`,
		`__dream_string_assign(&pending,"replaced");`,
		`header=dream_string_copy(__dream_string_assign(&source,"header"));`,
		`repeated=dream_string_copy(__dream_string_assign(&source,header));`,
		`dr_release(repeated);continue;`, `}dr_release(header);}`,
		`zero=dream_string_copy(__dream_string_assign(&source,"zero"));`,
		`}dr_release(zero);}`,
		`stopped=dream_string_copy(__dream_string_assign(&source,"break"));`,
		`dr_release(stopped);break;`,
		`freshCopy=dream_string_copy(__dream_string_assign(&source,(dream_concat("fresh","value"))));`,
		`staticText=dream_string_copy((input="static"));`,
		`instanceText=dream_string_copy((((structReader*)this)->field="instance"));`,
		`globalCopy=dream_string_copy(__dream_string_assign(&globalSource,"top"));`,
		`__dream_string_assign(&globalSource,"changed");`,
		`dr_release(copy);`, `dr_release(source);`, `dr_release(self);`,
		`dr_release(chain);`, `dr_release(indexed);`, `dr_release(parameter);`,
		`dr_release(field);`, `dr_release(empty);`, `dr_release(nullCopy);`,
		`dr_release(pending);`, `dr_release(freshCopy);`,
		`dr_release(staticText);`, `dr_release(instanceText);`,
	},
		"next\nnext\ncopy changed\nnext\nchain\nonce\n1\nbacking value\nbacking value\nparameter\nparameter\nnull field\nempty\nnull copy\nnext\nreplaced\nheader\nheader\nbreak\nfresh value\nstatic\ninstance\ntop\n")
}

func TestCompiler_AppendedStrings(t *testing.T) {
	// 43 exercise releases, four method releases, two top-level replacements.
	checkStringOwnership(t, "appended_string_copy.dr", 49, []string{
		`staticchar*__dream_string_append(constchar**slot,constchar*text)`,
		`char*joined=dream_concat(*slot?*slot:"",text?text:"");dr_release(*slot);*slot=joined;returnjoined;`,
		`__dream_string_append(&text,"literal");`,
		`snapshot=dream_string_copy(__dream_string_append(&text,text));`,
		`dr_console_writeln(__dream_string_append(&selected,@entry@once((values[(index++)]))));`,
		`__dream_string_append(&selected,Reader_shared);`,
		`__dream_string_append(&survivor,source);`,
		`__dream_string_append(&empty,absent);`,
		`__dream_string_append(&nullable,((void*)0));`,
		`__dream_string_append(&nullable,(values[1]));`,
		`nestedCopy=dream_string_copy(__dream_string_append(&nested,__dream_string_append(&nested,"B")));`,
		`__dream_string_append(&text,__dream_string_assign(&text,"plainexpression"))`,
		`__dream_string_append(&fresh,(dream_concat("fresh","value")));`,
		`body=dream_string_copy(__dream_string_append(&header,""));`,
		`__dream_string_append(&header,"x"))`, `}dr_release(header);}`,
		`dr_release(body);continue;`, `dr_release(body);dr_release(stopped);break;`,
		`zero=dream_string_copy(__dream_string_append(&text,""));`, `}dr_release(zero);}`,
		`topCopy=dream_string_copy(__dream_string_append(&top,"value"));`,
		`__dream_string_append(&top,"changed");`,
		`(number+=2);`, `(real+=2.0);`,
	}, "start literal\nstart literalstart literal\nstart literalstart literal\nchanged\nonce\nleft borrowed\n1\nleft borrowed field\nborrowed\nalive source\nempty\nnull left\nnull left\nABAB\nABAB\nplain expressionplain expression\nfresh value\nloopx\nloopxx\nstopx\n7\n3.500000\nstatic value\ninstance value\ntop value\nborrowed\n")

	tr := NewTestRunner(t)
	defer tr.Cleanup()
	for _, modules := range []bool{false, true} {
		for _, rejected := range []struct{ declaration, operand, diagnostic string }{
			{`const string text = "fixed";`, `" suffix"`, "cannot assign to const variable 'text'"},
			{`string text = "fixed";`, `1`, "string '+=' requires a string or null operand"},
			{`string text = "fixed";`, `1.5`, "string '+=' requires a string or null operand"},
			{`string text = "fixed";`, `true`, "string '+=' requires a string or null operand"},
			{`string text = "fixed";`, `'x'`, "string '+=' requires a string or null operand"},
		} {
			work := t.TempDir()
			text := rejected.declaration + "\ntext += " + rejected.operand + ";\n"
			args := []string{"--dev"}
			if modules {
				text = "import labels; " + text
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
			cmd := exec.CommandContext(ctx, tr.compilerPath, append(args, source)...)
			cmd.Dir = work
			out, err := cmd.CombinedOutput()
			cancel()
			if exit, ok := err.(*exec.ExitError); !ok || exit.ExitCode() != 1 || ctx.Err() == context.DeadlineExceeded {
				t.Fatalf("append rejection %q (modules=%v): expected exit 1, got %v: %s", rejected.operand, modules, err, out)
			}
			plain := regexp.MustCompile(`\x1b\[[0-9;]*m`).ReplaceAllString(string(out), "")
			if !strings.Contains(plain, rejected.diagnostic) || !regexp.MustCompile(`\b2:\d+:`).MatchString(plain) {
				t.Fatalf("append rejection %q (modules=%v): missing positioned diagnostic: %s", rejected.operand, modules, out)
			}
			if _, err := os.Stat(filepath.Join(work, "build")); !os.IsNotExist(err) {
				t.Fatalf("append rejection created artifacts: %v", err)
			}
		}
	}
}

func TestCompiler_ScalarReturnCleanup(t *testing.T) {
	checkStorageOwnership(t, "scalar_return_cleanup.dr", 47, []string{
		`{int__dream_return_0=((structScalarToken*)inner)->value;dr_release(inner);dr_release(empty);dr_release(absent);dr_release(text);dr_release(outer);return__dream_return_0;}`,
		`{int__dream_return_0=((structScalarToken*)outer)->value;dr_release(empty);dr_release(absent);dr_release(text);dr_release(outer);return__dream_return_0;}`,
		`{int__dream_return_0=((structScalarToken*)whileBody)->value;dr_release(whileBody);dr_release(whileOuter);return__dream_return_0;}`,
		`{int__dream_return_0=((structScalarToken*)doOuter)->value;dr_release(doBody);dr_release(doOuter);return__dream_return_0;}`,
		`{int__dream_return_0=((structScalarToken*)forBody)->value;dr_release(deep);dr_release(forBody);dr_release(header);dr_release(forOuter);return__dream_return_0;}`,
		`{int__dream_return_0=((structScalarToken*)caseOwner)->value;dr_release(caseOwner);dr_release(switchOuter);return__dream_return_0;}`,
		`{int__dream_return_0=((structScalarToken*)never)->value;dr_release(never);dr_release(zeroHeader);dr_release(zeroOuter);return__dream_return_0;}`,
		`{int__dream_return_0=((structScalarToken*)zeroOuter)->value;dr_release(zeroOuter);return__dream_return_0;}`,
		`{int__dream_return_0=(@entry@recurse((depth-1))+1);dr_release(recursive);return__dream_return_0;}`,
		`{float__dream_return_0=1.5;dr_release(staticText);return__dream_return_0;}`,
		`{int__dream_return_0=(((structScalarReader*)this)->value+=1);dr_release(methodText);dr_release(instanceOwner);return__dream_return_0;}`,
		`{char__dream_return_0=(characters[(((structScalarReader*)this)->hits++)]);dr_release(charText);return__dream_return_0;}`,
		`{int__dream_return_0=(stop?(((structScalarToken*)boolOwner)->value==1):0);dr_release(boolText);dr_release(boolOwner);return__dream_return_0;}`,
		`{int__dream_return_2=((structScalarToken*)__dream_return_0)->value;dr_release(__dream_return_0);return__dream_return_2;}`,
		`{int__dream_return_3=@entry@__dream_return_2();dr_release(callText);return__dream_return_3;}`,
		`{int__dream_return_0=((structScalarToken*)completedOuter)->value;dr_release(completedOuter);return__dream_return_0;}`,
		`return1;dr_release(pendingTry);`, `return2;dr_release(pendingCatch);`,
		`return3;dr_release(pendingFinally);`, `returnresult;dr_release(result);`,
		`returnsource;`, `returnletter;`, `return15;`,
	}, "2\n1\n1\n4\n5\n6\n7\n8\n9\n2\n1.500000\n11\n11\nA\nZ\n2\n1\n0\n12\n13\n14\n15\nbacking\n", true,
		`dr_release(this);`, `dr_release(parameter);`, `dr_release(pointer);`,
		`dr_release(source);`, `dr_release(values);`, `dr_release(characters);`,
		`constchar*__dream_return_`)
}

func TestCompiler_VoidReturnStrings(t *testing.T) {
	checkStorageOwnership(t, "void_return_strings.dr", 40, []string{
		`{dr_release(inner);dr_release(absent);dr_release(empty);dr_release(outer);return;}`,
		`{dr_release(whileBody);dr_release(whileOuter);return;}`,
		`{dr_release(doBody);dr_release(doOuter);return;}`,
		`{dr_release(deep);dr_release(forBody);dr_release(header);dr_release(forOuter);return;}`,
		`{dr_release(caseText);dr_release(switchOuter);return;}`,
		`{dr_release(never);dr_release(zeroHeader);dr_release(zeroOuter);return;}`,
		`{dr_release(zeroOuter);return;}`, `{dr_release(recursive);return;}`,
		`{dr_release(staticText);return;}`,
		`{dr_release(methodDeep);dr_release(instanceText);return;}`,
		`{dr_release(completedOuter);return;}`,
	},
		"fall\nbacking value\ndo\nearly\nbacking value\nbacking valuex\nnormal\nafter\nstatic\ninstance\ntry\nfinally\nbacking value\n", true,
		`dr_release(parameter);`, `dr_release(pointer);`, `dr_release(source);`,
		`dr_release(values);`, `dr_release(borrowed);`, `dr_release(alias);`,
		`dr_release(this);`,
		`dr_release(pendingTry);dr_release(pendingOuter);return;`,
		`dr_release(pendingCatch);dr_release(pendingOuter);return;`,
		`dr_release(pendingFinally);dr_release(pendingOuter);return;`)
}

func TestCompiler_VoidReturnClasses(t *testing.T) {
	checkStorageOwnership(t, "void_return_classes.dr", 40, []string{
		`{dr_release(inner);dr_release(text);dr_release(outer);return;}`,
		`{dr_release(whileBody);dr_release(whileOuter);return;}`,
		`{dr_release(doBody);dr_release(doOuter);return;}`,
		`{dr_release(deep);dr_release(forBody);dr_release(bodyText);dr_release(header);dr_release(forOuter);return;}`,
		`{dr_release(caseOwner);dr_release(switchOuter);return;}`,
		`{dr_release(switchOuter);return;}`,
		`{dr_release(never);dr_release(zeroHeader);dr_release(zeroOuter);return;}`,
		`{dr_release(zeroOuter);return;}`, `{dr_release(recursive);return;}`,
		`{dr_release(staticOwner);return;}`,
		`{dr_release(methodText);dr_release(instanceOwner);return;}`,
		`{dr_release(completedOuter);return;}`,
		`{dr_release(reader);dr_release(backing);return;}`,
	},
		"2\n1\n4\n5\n6\n7\n8\n9\n9\nbacking\n", true,
		`dr_release(this);`,
		`dr_release(pendingTry);dr_release(pendingOuter);return;`,
		`dr_release(pendingCatch);dr_release(pendingOuter);return;`,
		`dr_release(pendingFinally);dr_release(pendingOuter);return;`)
}

func TestCompiler_LoopExitClasses(t *testing.T) {
	checkStorageOwnership(t, "loop_exit_classes.dr", 33, []string{
		`{dr_release(deep);dr_release(text);dr_release(whileBody);continue;}`,
		`{dr_release(deep);dr_release(text);dr_release(whileBody);break;}`,
		`{dr_release(doBody);continue;}`, `{dr_release(doBody);break;}`,
		`{dr_release(switchContinue);dr_release(forBody);continue;}`,
		`{dr_release(switchBreak);break;}`, `{dr_release(forBody);break;}`,
		`{dr_release(innerBody);continue;}`, `{dr_release(innerBody);break;}`,
		`}dr_release(innerHeader);}`, `{dr_release(outerBody);break;}`,
		`{dr_release(loopInSwitch);break;}`, `{dr_release(caseOwner);break;}`,
		`{dr_release(staticToken);continue;}`, `{dr_release(instanceToken);break;}`,
		`{dr_release(topBody);continue;}`, `{dr_release(topBody);break;}`,
		`}dr_release(header);}`, `}dr_release(topHeader);}`,
		`dr_release(late);`, `dr_release(switchNormal);`,
		`dr_release(survivor);`, `dr_release(reader);`,
	},
		"2\n9\n3\n1\nheader\n2\nheader\n7\n8\n10\n11\n12\ntop\n", true)
}

func checkStringOwnership(t *testing.T, fixture string, releases int, emittedChecks []string, want string, forbiddenChecks ...string) {
	t.Helper()
	checkStorageOwnership(t, fixture, releases, emittedChecks, want, false, forbiddenChecks...)
}

func checkStorageOwnership(t *testing.T, fixture string, releases int, emittedChecks []string, want string, emptyBeforeShutdown bool, forbiddenChecks ...string) {
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
				// Entry functions are lowered in module mode; methods keep their names.
				prefix := ""
				if mode == "module_entry" {
					prefix = "__dream_module_0_"
				}
				emitted = strings.ReplaceAll(emitted, "@entry@", prefix)
				if !strings.Contains(c, emitted) {
					t.Fatalf("missing ownership emission %q", emitted)
				}
			}
			for _, forbidden := range forbiddenChecks {
				if strings.Contains(c, forbidden) {
					t.Fatalf("unexpected ownership emission %q", forbidden)
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
			if emptyBeforeShutdown {
				compileArgs = append(compileArgs, "-DDREAM_EMPTY_BEFORE_SHUTDOWN")
			}
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
