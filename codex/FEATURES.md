# Dream Compiler Features

This document tracks the implementation status of Dream language features.

Use `tasks/MostRecentTestResults.md` for the dated, measured baseline and
`tasks/STATE.md` for the current handoff. Zig 0.17.1 is pinned; Linux cross-build
success is recorded separately from native runtime verification.

## ✅ Implemented Language Subset

### Core Language Features
- **Primitive types**: `int`, `float`, `bool`, `char`, and `string`
- **Variable declarations** with optional initializers
- **Type inference** using `var` keyword for local variables
- **Arithmetic operators**: `+`, `-`, `*`, `/`, `%`, unary minus and unary plus
- **Logical operators**: `&&`, `||`, and `!`
- **Comparison operators**: `<`, `<=`, `>`, `>=`, `==`, and `!=`
- **Assignment operators**: `=` and compound variants (`+=`, `-=`, `*=`, `/=`, `%=`)
- **Bitwise operators**: `&`, `|`, `^`, `~`, `<<`, `>>` with compound assignment variants
- **Increment/decrement**: `++` and `--` operators
- **String operations**: String concatenation with `+`, escape sequences
- **Expression grouping**: Parentheses for precedence control
- **Ternary operator**: `condition ? true_expr : false_expr`

### Control Flow
- **Conditional statements**: `if`/`else` with nested block support
- **Loop constructs**: `while`, `do-while`, and `for` loops
- **Switch statements**: With case/default support
- **Control flow**: `break`, `continue`, and `return` statements
- **Exception handling**: `try`/`catch` statements with `throw` support

### Functions and OOP
- **Function declarations**: With parameters and typed return values using `func` keyword
- **Parameter diagnostics and recovery**: Unsupported custom-type/array parameters and malformed names/separators fail before artifacts with source positions. Invalid signatures and balanced bodies are discarded; statement parsing advances past stray expression delimiters. Bounded ordinary/module-entry/dependency regressions cover this boundary; custom-type parameter support is not implemented
- **Primitive return checks**: Exact declared types and valid bare returns, including methods, nested statements, and imported functions; no implicit numeric conversion at the return boundary or proof that every path returns
- **Classes and structs**: Object construction and instantiation
- **Instance methods**: With `this` pointer (emitted as C functions)
- **Primitive pointer parameters**: C signatures preserve one `*` for `int`, `float`, `bool`, `char`, and `string` parameters through free-function declarations/definitions, static/instance methods and local/explicit base constructors. Ordinary/module-entry regressions check signatures and borrowed array reads/writes. Shape compatibility, bounds and general ownership are not checked
- **Member diagnostics**: Checked receivers, unknown members, and type-qualified instance access before C emission, including module entry classes; source positions preserved for member/base access
- **Primitive method arguments**: Exact scalar types checked through ordinary/inherited/static/instance and explicit `init` calls, including module entry classes; useful call positions and artifact-free rejection. Pointer/array shapes and custom types remain unqualified
- **Constructor arguments**: `new Type(...)` checks arity and exact primitive scalar types against a class's own instance `init`, including module entry classes. Classes without local `init` and structs take zero arguments; arguments are never silently ignored. No implicit inherited construction; pointer/array shapes and custom-type compatibility remain unqualified
- **Object-oriented programming**: Declaration-based single inheritance, typed inherited fields, static method dispatch, base access, and checked upcasts; no virtual dispatch or access-control enforcement

### I/O and Runtime
- **Console I/O**: `Console.Write`, `Console.WriteLine`, and `Console.ReadLine`. Direct indexed primitive arrays/pointer parameters retain their element format (`%d` for int/bool, `%f` for float, `%c` for char, string output for string elements). Scalar string indexing produces character output, including fields, literals, calls and an indexed string element. Ordinary/module-entry generated-C and exact-output regressions cover this backend boundary; general shape/bounds checking remains unqualified
- **Memory management**: Reference-counted allocation system
- **Scalar string declarations from literals/indexing**: Scalar declarations initialized directly from a literal or index copy the string into managed storage; normal block cleanup releases the copy. Ordinary/module-entry checks cover empty/escaped/const literals, free/static/instance paths, loop bodies, literal/dynamic indexed backing, null indexed elements and once-only evaluation with checked allocation lifetime. Identifier/call/for-header initializer, assignment and return ownership remain unqualified; see the [ownership checkpoint](../docs/development/string-ownership.md)
- **Comments**: Line (`//`) and block (`/* */`) comments
- **Arrays**: Arrays of primitive types with indexing

### Code Generation
- **C backend**: Generates C from the AST for the implemented subset
- **Optimization**: `-O1`/`-O2`/`-O3` fold canonical decimal literal `+`/`-`/`*` trees through isolated SCCP IR into emitted C; every intermediate must be in `0..INT_MAX-1`, with at most 256 operations and depth 64. Default emission retains arithmetic. General CFG emission and SSA remain disabled
- **Host optimization flags**: The C compilation command receives `-O2`/`-O3`; default and `-O1` supply no host optimization flag. This is separate from Dream IR transformations
- **Optimization emission contract**: [P0009 implementation and P0008 design](../docs/development/optimization-emission.md) describe whole-tree fallback, source ownership, and explicit internal errors before output. Unsupported expressions retain AST emission; the unqualified whole-program pipeline is no longer invoked
- **Optimization evidence**: Go quick/CI checks generated C before host compilation, exact output at all four settings, methods/modules, side effects, boundaries/fallback, object emission, and controlled internal failures. The loop regression still prints `112` and `8`; no general loop transformation is claimed
- **Object emission**: Uses the common AST emitter with runtime include directories; failures return nonzero. Run from the project directory, as with C/native emission
- **Runtime library**: Memory management, console I/O, and exception handling
- **Build integration**: Zig build system with cross-platform support

## 🚧 Partially Implemented Features

### Multi-File System
- **Status**: Function modules work through `--multi-file entry.dr`
- **Components**: Relative imports, shared dependencies, private functions, exports, recursion, signatures, and source diagnostics
- **Limits**: Imported types/globals, package paths, and separate object caching are not implemented
- **Location**: Working frontend in `src/driver/modules.c`; combined-program C generation uses the main backend

## ❌ Not Implemented / Experimental Features

### Concurrency and Async Programming
- **Status**: Not implemented - moved to `not_implemented_tests/`
- **Features**: `async`/`await` keywords, task scheduling, thread management
- **Issues**: Missing platform abstraction layer (`platform.h` not found)
- **Tests**: All concurrency tests moved to `not_implemented_tests/advanced/concurrency/`

### Graphics and Vulkan Integration  
- **Status**: Experimental - moved to `not_implemented_tests/`
- **Features**: Vulkan API bindings, surface creation, pipeline management
- **Issues**: Integration causes test timeouts, needs complete implementation
- **Tests**: All graphics tests moved to `not_implemented_tests/graphics/`
- **Components**: 
  - Vulkan handle and struct definitions
  - Helper functions for device enumeration
  - Pipeline building utilities
  - Cross-platform surface creation

### Advanced Language Features (Future)
- **Generics/Templates**: Type parameterization
- **Package management**: External dependency system  
- **Advanced OOP**: Virtual methods, abstract classes, interfaces
- **Lambda expressions**: Anonymous functions and closures
- **Pattern matching**: Match statements with destructuring
- **Advanced type system**: Union types, optional types

## 📊 Current Implementation Status

### Test Suite Results (Latest)
- **Windows Dream Suite**: See the dated counts in `tasks/MostRecentTestResults.md`
- **Basic Operations**: Fully functional
- **Control Flow**: Fully functional  
- **Functions**: Primitive explicit returns checked; missing-return proofs and non-primitive contracts remain incomplete
- **Exception Handling**: Existing annotated tests pass; additional fixtures remain skipped
- **Multi-file**: Function imports compile and run; broader module exports remain future work
- **Concurrency**: Not implemented (tests excluded)
- **Graphics**: Not implemented (tests excluded)

### Platform Support
- **Linux**: [P0007 CI run 42219194531](https://github.com/Ddemon26/DreamCompiler/actions/runs/42219194531) passed build, compiler stability regressions, and the Dream suite; local cross-build evidence remains separate
- **Windows**: Annotated Dream suite passes with Zig 0.17.1; skipped fixtures remain unverified
- **Cross-platform**: Build system supports both platforms

### Development Priority
1. **High**: Complete multi-file compilation
2. **High**: Connect optimized IR to code emission and close skipped-test coverage gaps
3. **Deferred**: Concurrency/async and graphics/Vulkan integration

## 📁 Test Organization

- **`tests/`**: Currently implemented and working features
- **`not_implemented_tests/`**: Features not yet implemented or experimental
  - `not_implemented_tests/advanced/concurrency/`: Async/await tests
  - `not_implemented_tests/graphics/`: Vulkan integration tests
