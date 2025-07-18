# Dream Compiler Changelog

Version 1.1.0 (2025-07-16)
- Documentation site rebuilt. `index.html` now loads `index.md` and navigation focuses on compiler and language guides.
- Restored language documentation pages and detailed table of contents.
- Added automated test runner to `zig build test`.
- Implemented bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`).
- Bitwise NOT `~` operator is now documented as implemented.
- Implemented parsing of `if`/`else` statements.
- Lexer updated to recognise additional tokens (`??`, `??=`, `=>`, `::`, `->`,
  `&=`, `|=`, `^=`, `<<=`, `>>=`) per Grammar v0.3.
- Added basic C code generation for programs.
- Implemented `--emit-obj` to compile generated C to an object file.
- Added support for `for` loops in the parser and code generator.
- Recognised `void` as reserved keyword per Grammar v0.3.
- Fixed Windows build by replacing `mkstemps` with portable `tmpnam_s` fallback.
- Restored generation of `build/bin/dream.c` when compiling `.dr` files.
- Added support for primitive types (`int`, `float`, `char`, `bool`, `string`) in the parser and C code generation.
- Introduced developer-only `Console.Write`/`WriteLine` macros for compiler debugging.

Version 1.1.01 (2025-07-17)
- Documented current feature gaps in codex/FEATURES.md and added tasks/TODO.md.
- Added parsing and code generation for `while` loops.
- Implemented `do` loops.
- Implemented `break` statements.
- Implemented `continue` and `return` statements.
- Added prefix/postfix increment and decrement operators.
- Added compound assignment operators for arithmetic and bitwise ops.
- Added string concatenation for `string` values.
- Implemented ternary conditional operator `?:`.
- Added unary plus operator and enabled comparison operators.
- Implemented basic `switch` statements in parser and code generator.
- Variable declarations no longer require an initialiser.
- Added array declarations and indexing for primitive types.
- Implemented function declarations with parameters and typed return values.
- Added support for declaring multiple variables in a single statement.
- Added `var` keyword for local type inference.
- Parser now recognises `class` and `struct` declarations (no object creation yet).

Version 1.1.02 (2025-07-18)
- Implemented object construction for classes and structs using the `new` keyword.
- Updated documentation and added regression tests.
