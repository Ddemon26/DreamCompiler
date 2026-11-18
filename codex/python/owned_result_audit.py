"""Check ownership of factory results across caller consumption paths.

Run from the checkout with an already built compiler, with no competing writer:
    python codex/python/owned_result_audit.py

P0049's original retention measurements remain in its pass/contract. P0050
supersedes owned_initializers: callers adopt the five originals. P0052 adds an
isolated assignment probe and reduces other_consumers survivors from five to four.
P0054 adds isolated discard and reduces other_consumers survivors to three.
P0055 isolates synchronous argument consumption and checks two exclusion cases.
P0056 releases eligible wrapper arguments and leaves two other_consumers survivors.
P0057 isolates and releases eligible direct Console arguments, leaving append as
the remaining other_consumers survivor.
The real generated C, allocator and unchanged checked harness are used throughout.
"""

import json
import os
from pathlib import Path
import subprocess
import tempfile
import time


ROOT = Path(__file__).resolve().parents[2]
COMMON = '''
func string owned() { string result = "owned"; return result; }
func string borrowed(string source) { return source; }
'''


def argument_release(consumer):
    return ("{constchar*dreamOwnedArgument0=@entry@owned();"
            f"{consumer}(dreamOwnedArgument0);dr_release((void*)dreamOwnedArgument0);}}")


ARGUMENT_RELEASE = argument_release("@entry@view")
CASES = [
    {
        "name": "isolated_console_write",
        "releases": 8, "live": 0,
        "source": COMMON + 'for (int i = 0; i < 8; i++) Console.Write(owned());',
        "output": "owned" * 8,
        "c": [argument_release("dr_console_write")],
    },
    {
        "name": "isolated_console_writeln",
        "releases": 8, "live": 0,
        "source": COMMON + 'for (int i = 0; i < 8; i++) Console.WriteLine(owned());',
        "output": "owned\n" * 8,
        "c": [argument_release("dr_console_writeln")],
    },
    {
        "name": "isolated_argument",
        "releases": 8, "live": 0,
        "source": COMMON + '''
func void view(string source) { Console.WriteLine(source); }
for (int i = 0; i < 8; i++) view(owned());
''',
        "output": "owned\n" * 8,
        "c": [ARGUMENT_RELEASE, "dr_console_writeln(source);"],
    },
    {
        "name": "argument_stored_by_consumer",
        "releases": 0, "live": 1,
        "source": COMMON + '''
class Vault { static string saved; }
func void view(string source) {
    Console.WriteLine(source);
    Vault.saved = source;
}
view(owned());
Console.WriteLine(Vault.saved);
''',
        "output": "owned\nowned\n",
        "c": ["@entry@view(@entry@owned());",
              "(Vault_saved=source);", "dr_console_writeln(Vault_saved);"],
    },
    {
        "name": "argument_with_sibling",
        "releases": 0, "live": 1,
        "source": COMMON + '''
func int sibling() { Console.WriteLine("sibling"); return 7; }
func void view(string source, int number) {
    Console.WriteLine(source);
    Console.WriteLine(number);
}
view(owned(), sibling());
''',
        "output": "sibling\nowned\n7\n",
        "c": ["@entry@view(@entry@owned(),@entry@sibling());"],
    },
    {"name": "isolated_discard", "releases": 8, "live": 0,
     "source": COMMON + 'for (int i = 0; i < 8; i++) owned();',
     "output": "", "c": ["dr_release((void*)@entry@owned());"]},
    {"name": "isolated_assignment", "releases": 4, "live": 0,
     "source": COMMON + 'func void exercise() { string text = "old"; text = owned(); text = owned(); text = owned(); } exercise();',
     "output": "", "c": []},
    {
        "name": "owned_initializers", "releases": 6, "live": 0,
        "source": COMMON + '''
func string cloned(string source) { string result = source; return result; }
func string emptyResult() { const string result = ""; return result; }
func string nullResult() { string result; return result; }
func void exercise() {
    string backing = "backing";
    for (int i = 0; i < 3; i++) {
        string text = owned(); Console.WriteLine(text);
    }
    string snapshot = cloned(backing); Console.WriteLine(snapshot);
    string empty = emptyResult(); Console.WriteLine(empty);
    string absent = nullResult();
    if (absent == null) Console.WriteLine("null");
}
exercise();
''',
        "output": "owned\nowned\nowned\nbacking\n\nnull\n",
        "c": ['text=@entry@owned();',
              'snapshot=@entry@cloned(backing);',
              'empty=@entry@emptyResult();',
              'absent=@entry@nullResult();'],
    },
    {
        "name": "borrowed_initializers", "releases": 7, "live": 0,
        "source": COMMON + '''
func string literal() { return "literal"; }
class Reader {
    string value; static string shared;
    func void init(string source) { this.value = source; }
    func string read() { return this.value; }
}
func string readStatic() { return Reader.shared; }
func void exercise() {
    string backing = "backing";
    Reader reader = new Reader(backing);
    Reader.shared = backing;
    string a = literal(); string b = borrowed(backing);
    string c = reader.read(); string d = readStatic();
    Console.WriteLine(a); Console.WriteLine(b);
    Console.WriteLine(c); Console.WriteLine(d);
    string absent; Reader.shared = absent; reader.value = absent;
    backing = "changed";
    Console.WriteLine(b); Console.WriteLine(backing);
}
exercise();
''',
        "output": "literal\nbacking\nbacking\nbacking\nbacking\nchanged\n",
        "c": ['a=dream_string_copy(@entry@literal());',
              'b=dream_string_copy(@entry@borrowed(backing));',
              'c=dream_string_copy(Reader_read(((structReader*)reader)));',
              'd=dream_string_copy(@entry@readStatic());'],
    },
    {
        "name": "mixed_initializers", "releases": 4, "live": 1,
        "source": COMMON + '''
func string mixed(bool selectOwned, string source) {
    string result = "owned"; return selectOwned ? result : source;
}
func void exercise() {
    string backing = "backing";
    string a = mixed(true, backing); string b = mixed(false, backing);
    Console.WriteLine(a); Console.WriteLine(b); Console.WriteLine(backing);
}
exercise();
''',
        "output": "owned\nbacking\nbacking\n",
        "c": ['a=dream_string_copy(@entry@mixed(1,backing));',
              'b=dream_string_copy(@entry@mixed(0,backing));',
              'if(result!=__dream_return_0)dr_release(result);'],
    },
    {
        "name": "other_consumers", "releases": 6, "live": 1,
        "source": COMMON + '''
func void view(string source) { Console.WriteLine(source); }
func void exercise() {
    string text = "start";
    text = owned(); text += owned();
    Console.WriteLine(owned()); owned(); view(owned());
    Console.WriteLine(text);
}
exercise();
''',
        "output": "owned\nowned\nownedowned\n",
        "c": ['(char*)@entry@owned();dr_release(text);text=__dream_owned_0;',
              '__dream_string_append(&text,@entry@owned());',
              argument_release("dr_console_writeln"),
              ARGUMENT_RELEASE,
              'dr_release((void*)@entry@owned());'],
    },
]


def command(args):
    result = subprocess.run(
        [str(arg) for arg in args], cwd=ROOT,
        env={**os.environ, "CC": "zig cc"}, capture_output=True,
        text=True, timeout=180, check=False,
    )
    if result.returncode:
        raise RuntimeError(
            f"Command failed ({result.returncode}): {args}\n"
            f"{result.stdout}{result.stderr}"
        )
    return result.stdout.replace("\r\n", "\n")


def main():
    started = time.monotonic()
    compiler = ROOT / "zig-out/bin" / ("DreamCompiler.exe" if os.name == "nt" else "DreamCompiler")
    results = []
    for case in CASES:
        for modules in (False, True):
            with tempfile.TemporaryDirectory(prefix="P0049-audit-", dir=ROOT / "build") as work:
                work = Path(work)
                source = work / "input.dr"
                text = case["source"]
                options = ["--multi-file"] if modules else []
                if modules:
                    text = "import labels;\n" + text + "\nlabels.literal();\n"
                    (work / "labels.dr").write_text(
                        'module labels; export func string literal() { return "literal"; }',
                        encoding="utf-8",
                    )
                source.write_text(text, encoding="utf-8")
                command([compiler, *options, "--dev", source])
                generated = (ROOT / "build/bin/dream.c").read_text(encoding="utf-8")
                compact = "".join(generated.split())
                for expected in case["c"]:
                    expected = expected.replace("@entry@", "__dream_module_0_" if modules else "")
                    if expected not in compact:
                        raise AssertionError(f'{case["name"]}: missing generated C {expected}')
                program = work / "program.c"
                program.write_text(
                    generated.replace("../libs/", "")
                    .replace('#include "memory.h"', "")
                    .replace('#include "custom.h"', ""), encoding="utf-8",
                )
                args = [
                    "zig", "cc", "-std=c11", "-D_GNU_SOURCE", "-fsanitize=undefined",
                    f'-DDREAM_STRING_RELEASES={case["releases"]}',
                    f'-DDREAM_LIVE_BEFORE_SHUTDOWN={case["live"]}',
                    f'-DDREAM_STRING_PROGRAM="{program.as_posix()}"',
                    "-I" + str(ROOT / "src/runtime/memory"),
                    "-I" + str(ROOT / "build/libs"),
                    ROOT / "codex/c/indexed_string_harness.c",
                    ROOT / "src/runtime/extensions/custom.c",
                    ROOT / "src/runtime/io/console.c",
                    ROOT / "src/runtime/exceptions/exception.c",
                ]
                if os.name == "nt":
                    command([*args, "-target", "x86_64-linux-gnu", "-o", work / "checked-linux"])
                binary = work / ("checked.exe" if os.name == "nt" else "checked")
                command([*args, "-o", binary])
                if command([binary]) != case["output"]:
                    raise AssertionError(f'{case["name"]}: checked output differs')
                command([compiler, *options, source])
                if command([ROOT / ("dream.exe" if os.name == "nt" else "dream")]) != case["output"]:
                    raise AssertionError(f'{case["name"]}: linked output differs')
                item = {key: case[key] for key in ("name", "releases", "live")}
                item["mode"] = "module_entry" if modules else "ordinary"
                results.append(item)
                print(json.dumps(item), flush=True)
    print(json.dumps({"probes": len(results), "seconds": round(time.monotonic() - started, 3)}))


if __name__ == "__main__":
    main()
