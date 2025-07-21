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
- Introduced developer-only `Console.Write`, `WriteLine`, `ReadLine` and `Read` macros for compiler debugging.

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

Version 1.1.03 (2025-07-18)
- Added basic try/catch statements and the `throw` keyword.
- Code generation now uses setjmp/longjmp for simple exception handling.
- Updated grammar and added tests for exception handling.

Version 1.1.04 (2025-07-19)
- Improved cross-platform support for Linux and Windows.
- Added `src/util/platform.h` and updated code to use it.
- CI now builds and tests on Windows.
- Updated Windows setup scripts to rely on MinGW's bundled gdb.

Version 1.1.05 (2025-07-20)
- Exception handling moved to a shared runtime library.
- `catch` is now a fully supported keyword and removed from the
  future-reserved list in the grammar.
- Added tests covering nested try/catch blocks and propagation
  across function calls.

Version 1.1.06 (2025-07-21)
- Diagnostics now report line and column numbers.
- Added `--verbose` flag for caret-highlighted messages.

Version 1.1.07 (2025-07-22)
- Generated C now includes `#line` directives to improve debugging back to Dream source.

Version 1.1.08 (2025-07-23)
- Implemented a cross-platform Language Server Protocol (LSP) backend.
- VS Code and JetBrains extensions now use `dream-language-server` for
  completions, hover docs, go-to-definition and live diagnostics.

Version 1.1.09 (2025-07-24)
- `dream-language-server` now queries the compiler for symbol information.
- Added `--symbols` flag to `parse` driver for JSON symbol output.
- Editor completions and definitions are derived from the parsed AST.

Version 1.1.10 (2025-07-25)
- Optimisation pipeline upgraded with copy propagation, common subexpression
  elimination and peephole passes.
- SCCP now folds conditional branches and prunes unreachable blocks.
- `DreamCompiler` accepts `-O1`/`-O2`/`-O3` and forwards optimisation flags to
  the C compiler.
Version 1.1.11 (2025-08-01)
- Introduced rudimentary reference counting for strings and class instances.
- All allocations are tracked and freed on program exit.

Version 1.1.12 (2025-08-05)
- `#line` directives now appear before every statement for improved debugger
  mapping.
- Compiled binaries include debug symbols by passing `-g` to `zig cc`.

Version 1.1.13 (2025-08-10)
- Introduced Vulkan handle and struct definitions in the standard library.

Version 1.1.14 (2025-08-15)
- Added `dr_vk_enumerate_physical_devices` runtime helper returning a list of
  available devices.
Version 1.1.15 (2025-08-22)
- Added `dr_vkCreateInstance` and `dr_vkDestroyInstance` runtime stubs for simple Vulkan setup.

Version 1.1.16 (2025-08-30)
- Published `Vulkan` standard module exposing instance creation, device enumeration and basic device/buffer management functions.

Version 1.1.17 (2025-09-05)
- Added cross-platform surface creation wrappers `dr_vkCreateSurface` and `dr_vkDestroySurface`.
- Runtime library now links against `user32` on Windows or `xcb` on Linux for WSI.

Version 1.1.18 (2025-09-12)
- Added `dr_vkCreateSimpleSwapchain` runtime helper and Dream wrappers `createDefaultSwapchain` and `destroySwapchain`.
- New `pickFirstPhysicalDevice` helper simplifies choosing a GPU.

Version 1.1.19 (2025-09-28)
- Expanded Vulkan module with shader module, descriptor set layout and pipeline
  layout bindings.
- Documentation updated with examples for building pipelines.
