# Multi-file Compilation and Modules

Compile an entry file and its sibling modules with:

```sh
zig build run -- --multi-file tests/multifile/main.dr
# Or use the dedicated build step:
zig build compile-multi -- tests/multifile/main.dr
```

The compiler writes `build/bin/dream.c` and links `dream` (`dream.exe` on Windows)
with the runtime. `--dev --multi-file entry.dr` emits C without invoking the C compiler.
Pass one entry file; dependencies are discovered from its imports.

## Declare and import functions

```dream
// arithmetic.dr
module arithmetic;

func int twice(int n) { return n * 2; }
export func int answer(int n) { return twice(n) + 2; }
```

```dream
// main.dr, beside arithmetic.dr
import arithmetic;
Console.WriteLine(arithmetic.answer(20));
```

Each import is resolved relative to the file containing that import, independently
of the shell's working directory. `import arithmetic;` loads `arithmetic.dr`, which
must declare `module arithmetic;`. Dependencies may import other sibling modules.
Shared dependencies are loaded once; circular imports are rejected.

Only exported functions may be called from another module. Private helpers remain
available within their own file. Identical helper or export names in separate modules
do not collide. Calls must use the imported module name, such as `arithmetic.answer(20)`.
Local variables and parameters must not shadow module function or import names.

Forward calls and recursion within modules are supported. Function argument counts
and primitive argument types are checked before C generation. String, float, char,
bool, and int return values use their declared output formatting. Dependency parser
and semantic errors retain the dependency file's source context.

## Current boundaries

Imported modules currently contain function declarations, imports, and one module
declaration. Exported variables, classes, structs, and other imported top-level
statements are rejected. The entry file can use the regular implemented language
subset, including classes. Imports are simple identifiers; package search paths,
import aliases, and dotted package imports are not implemented.

The working driver resolves module symbols and sends the combined program through
the existing C backend. Separate per-module object generation, incremental module
caching, and the older backend abstraction in `src/codegen/module.c` remain future
work. Module directives require `--multi-file`; they are not silently ignored in
single-file mode.

## Verification

`tests/multifile/main.dr` is an executable regression. Its two helper modules are
listed as fixtures in `codex/test_config.json` and excluded from standalone test
discovery. `codex/go/module_integration_test.go` creates independent source trees
and checks dependencies, private names, exports, recursion, signature errors,
source diagnostics, and failure before generating build artifacts.
