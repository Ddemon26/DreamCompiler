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
- **Scalar string declarations from literals/indexing/identifiers/members/calls/conditionals/assignments**: Scalar declarations initialized directly from a literal, index, identifier, member, call, conditional or plain assignment expression normally copy the string into managed storage; normal block cleanup releases the copy. Ordinary/module-entry checks cover empty/escaped/const literals, free/static/instance paths, loop bodies, literal/dynamic backing, nested identifier chains, borrowed parameters, null, inherited `this`/`base` and static fields, and once-only index/receiver evaluation with checked allocation lifetime. Copies leave source ownership unchanged. Supported primitive for-header declarations use the same initializer path in a loop scope, with one header release after zero/multiple iterations or loop-targeting break; continue preserves the header through update. Header names are unavailable afterward and may be reused by later loops. Calls evaluate once and normally copy readable literal/borrowed/managed/null results; P0050 adopts direct proven factory initializers as described below. Unsupported managed results can remain registered until shutdown. Plain `=` on qualified owned scalar declarations copies the replacement before releasing the old value, including self/chained assignment and for-header updates; declarations without an initializer start as null. Break/continue release qualified owned scalar strings in exited scopes, preserving outer locals and for headers through continue/update. Switch breaks preserve enclosing loop locals; switch continues clean through the enclosing loop body. Ordinary/module-entry regressions require exact jump cleanup sequences, 37 checked releases and 16 output lines. Conditional initializers copy only the selected value, evaluating the condition/selected branch once; nineteen copy sites, 22 checked releases and 24 output lines per mode cover source lifetime and existing replacement/jump cleanup. Typed null-valued string branches are covered; literal null/string branch typing remains rejected. Fresh selected managed results can remain registered until shutdown. Plain assignment initializers evaluate the assignment once and copy its readable result; seventeen copy sites, thirty checked releases and twenty-three output lines per mode cover independent source/copy replacement, borrowed destinations and existing replacement/jump/header cleanup. Destination ownership remains unchanged. Parameters, field/index destinations, other compound operators, unsupported string initializer forms, class alias/container ownership, destructors/field ownership and string/pointer/class result transfer and exception/finally unwinding remain unqualified; see the [ownership checkpoint](../docs/development/string-ownership.md)
- **Owned scalar string `+=`**: Qualified mutable scalar identifiers append a string or null operand once, concatenate before releasing their old storage, and yield the new value. Null contributes empty text, and null plus null creates a managed empty string. Self/nested updates, copied declaration results, borrowed/member/index/managed operands, header/jump cleanup, methods and top-level snapshots are checked in both modes. Numeric compound operators keep their existing C emission. Const updates and non-string operands are rejected before artifacts. Complete append statements (P0058), direct scalar string initializers (P0059), appends printed directly by Console (P0060), appends passed to proven one-parameter Console wrappers (P0061), and direct synchronous returns of appends to owned locals (P0063) release the original operand when it passes the factory proof. Other fresh managed operands can remain registered until shutdown. Borrowed destinations, conditional operands and other compound operators remain unqualified. See the [append return ownership rule](../docs/development/string-ownership.md#direct-append-returns-with-proven-factory-operands-p0063).
- **Owned strings on bare void return**: Synchronous void free/static/instance functions release active qualified scalar locals in reverse declaration order, including nested scopes and for headers. Unbraced returns keep cleanup conditional; parameters, `this`, borrowed arrays/pointers and later declarations receive no release. Both modes check 40 nonnull releases (39 strings and one normal class release), 13 output lines, recursion, caller survival and an empty registry before shutdown. Returns inside active try/catch/finally, async exits and value transfer remain unqualified; see the [ownership checkpoint](../docs/development/string-ownership.md).
- **Class locals on bare void return**: Synchronous void free/static/instance functions release active scalar class locals together with owned strings in reverse declaration order. Parameters and implicit `this` survive; nested scopes, string headers, unbraced paths, recursion, constructor/method returns and normal exception completion are checked in ordinary/module-entry modes (30 class and ten string releases, ten output lines, empty registry before shutdown). Fresh independent instances are qualified; class aliases/containers, destructors/field destruction, value-return transfer, async and active exception returns remain separate; see the [ownership checkpoint](../docs/development/string-ownership.md).
- **Primitive value-return cleanup**: Synchronous `int`/`float`/`bool`/`char` returns evaluate once into a typed temporary before reverse-order qualified string/class cleanup. Both modes check 47 nonnull releases (21 classes, 26 strings), 23 output lines, exact evaluation/cleanup order, temporary-name collisions, field/index reads, effects, headers, recursion, methods and empty pre-shutdown storage. Borrowed parameters/receivers survive; no temporary is needed without qualified locals. String/pointer/class result transfer, shape checking, aliases/containers/destructors/fields, async and active exception unwinding remain separate; see the [ownership checkpoint](../docs/development/string-ownership.md).
- **Direct owned-local string returns**: P0037 saves a known-owned scalar local result, releases other qualified string/class locals in reverse order, and preserves the returned allocation with its original reference until shutdown. P0037 itself adds no result copy or consumption; P0050 additionally adopts the narrow proven factory initializers below. Both modes require 45 nonnull releases, 21 output lines and exactly 17 returned allocations before shutdown (44 total allocations before the fix). Nested scopes, loops/switches, headers, null/empty/const locals, unbraced paths, recursion, methods and caller survival are checked. Borrowed/compound string expressions, alias analysis, async and active exception exits remain unqualified; see the [ownership checkpoint](../docs/development/string-ownership.md).
- **Direct literal string returns**: P0038 releases all qualified string/class locals before returning a literal's static pointer, including parenthesized, empty and escaped literals. Both modes require 51 nonnull releases (18 classes, 33 strings), 19 output lines and an empty registry before shutdown. Nested/header scopes, conditional paths, loops/switches, recursion, methods, caller survival and normal exception completion are checked. No result allocation or retain is added. See the [contract](../docs/development/string-ownership.md#direct-literal-string-returns-p0038).
- **Conditional literal string returns**: P0039 extends static-result cleanup to nested `?:` expressions with all-literal value leaves. Reached conditions execute once before reverse cleanup; unselected branches do not execute. Both modes require 55 nonnull releases (18 classes, 37 strings), 34 output lines and an empty pre-shutdown registry. Exact C checks preserve unsupported-leaf and active-exception emission; the unused owned-local control checks P0040 cleanup. No result copy or retain is added. General result ownership, async and active exception exits remain separate; see the [contract](../docs/development/string-ownership.md#conditional-literal-string-returns-p0039).
- **Conditional owned-local string returns**: P0040 qualifies `?:` trees with literal or known-owned scalar local value leaves. Capture once, then compare candidate pointers during reverse cleanup to preserve the selected allocation and release unselected owners. Both modes require 64 nonnull releases (18 classes, 46 strings), 36 output lines and exactly 14 returned allocations with original references before shutdown. Nested/repeated/null/empty leaves, equal-content slots, condition replacement, headers, unbraced paths, methods, collisions and caller survival are checked. No result copy, retain or consumption is added; rebound/compound leaves and async/active exception exits remain separate. P0042 additionally qualifies unchanged borrowed scalar parameter leaves. See the [contract](../docs/development/string-ownership.md#conditional-owned-local-string-returns-p0040).
- **Unchanged borrowed string-parameter returns**: P0041 captures a direct scalar parameter result before reverse cleanup when a whole-body scan proves that parameter is never rebound. Both modes check 58 releases (20 classes, 38 strings), 17 output lines, empty pre/post-shutdown registries and caller backing/snapshot survival; 24 additional cases preserve fallback for nested/later writes. No result copy or retain is added. Rebound results, aliases and async/active exception exits remain separate. P0042 extends the proof to conditional value leaves. See the [contract](../docs/development/string-ownership.md#unchanged-borrowed-string-parameter-returns-p0041).
- **Conditional borrowed string results**: P0042 qualifies conditional trees mixing literals, known-owned locals and unchanged borrowed scalar parameters. Capture once, preserve only selected owned storage and release other qualified locals. Both modes require 75 checked releases, 39 output lines and five retained owned results; 48 additional rebinding cases preserve the entire old return in either nested branch. Caller backing/snapshots survive, including null/empty storage; conditions run only when selected and once. No result copy, retain or consumption is added. Unsupported leaves and async/active exception exits remain separate. See the [contract](../docs/development/string-ownership.md#conditional-borrowed-string-parameter-returns-p0042).
- **Read-only direct instance string-field returns**: P0044 captures scalar nonstatic `this.field` results once before reverse local cleanup, including inherited owner casts. A separate whole-body read-only proof rejects writes, calls/new/console, custom-type/pointer/array locals, exceptions/await and unknown nodes/operators. Both modes require 38 releases, 21 output lines and empty pre/post-shutdown registries; 196 emission-only controls preserve fallback for unsafe bodies/shapes and exception/async guards. Caller backing and independent snapshots survive without result copy/retain or receiver release. Class parameters and async instance methods remain rejected by grammar; P0045 extends the proof to conditional member leaves while local/struct/nested/base instance receivers remain separate. P0047-P0048 cover type-qualified static fields below. See the [contract](../docs/development/string-ownership.md#read-only-direct-instance-string-field-returns-p0044).
- **Conditional read-only instance string-field returns**: P0045 permits proven member reads as conditional value leaves alongside literals, known-owned scalar locals and unchanged scalar parameters. Every leaf qualifies; any member leaf requires the unchanged whole-body read-only proof, including conditions and unselected branches. Capture once, preserve only selected owned storage and reverse-clean other locals without member result copy/retain or receiver release. Both modes check 59 releases, 27 output lines and four original-reference owned survivors until shutdown; 196 shared fallback controls plus fourteen expression rejections and two positive nested-else controls pass. Unsupported shapes/effects and async/exception exits preserve prior emission. Caller consumption and retained owned-result accumulation remain separate. See the [contract](../docs/development/string-ownership.md#conditional-read-only-instance-string-field-returns-p0045).
- **Direct read-only static string-field returns**: P0047 captures direct `Type.field` scalar static string
results once before reverse local cleanup. An unshadowed class type and actual owning declaration are required;
the existing whole-body read-only barriers reject writes/calls/aliases/shadowing and unsupported nodes/shapes.
Both modes check 40 releases, 23 output lines, caller backing/snapshot survival and empty pre/post-shutdown storage;
252 subtests cover lifetime, exact C, effects/receiver/shape fallbacks and artifact-free semantic rejection.
Inherited/hidden owners, free/static/instance readers and collisions are checked without result copy/retain or
global release. P0048 adds conditional static leaves below; other qualifiers, field ownership and
async/exception exits remain separate. See the [contract](../docs/development/static-member-return-cleanup.md).
- **Conditional read-only static string-field returns**: P0048 permits proven type-qualified scalar static fields as conditional value leaves mixed with instance fields, unchanged parameters, known-owned locals and literals. Every leaf must qualify under the unchanged whole-body read-only barriers. Capture once before reverse cleanup and preserve only selected owned storage, without result copy/retain or global release. Both modes check 67 releases, 35 output lines and four original-reference owned survivors until shutdown, with inherited/hidden owners, free/static/instance readers, caller/snapshot survival and unsafe-leaf/effect/shape/async/exception controls. Caller consumption and retained owned-result accumulation remain separate. See the [contract](../docs/development/string-ownership.md#conditional-read-only-static-string-field-returns-p0048).
- **Proven owned call initializers**: P0050 adopts a direct free-function result into a scalar string declaration only when the actual synchronous lowered definition has exactly one scalar string local declaration and its terminal direct local return. Only literal, absent/null AST or scalar string-parameter sources qualify; parameters must have scalar primitive shapes. Caller bindings cannot shadow the callee. No caller copy/retain/release is added; existing destination cleanup owns the original reference. Both modes check 10,000 original-pointer releases with peak live count one, imported/forward definitions, snapshots, headers/jumps/returns and program-lifetime top-level storage. Borrowed/mixed/method/effect/shape/recursive definitions and other consumers keep their existing behavior. Direct literal-null declarations and async-result string declarations remain semantic errors; typed-null snapshots execute. See the [contract and evidence](../docs/development/owned-call-result-consumption.md).
- **Scalar initializers that adopt returned joins**: P0064 extends direct declaration adoption to a synchronous free function that declares one qualifying mutable string local and directly returns an append of a proven factory result to that local. The declaration releases the original joined allocation. Ordinary and imported module calls each pass 10,000 original-pointer checks with zero survivors per call. See the [returned join rules](../docs/development/string-ownership.md#scalar-initializers-that-adopt-returned-joins-p0064).
- **Complete assignments that adopt returned joins** — P0065 extends the same proof to complete assignments to owned scalar strings. The destination adopts the exact pointer after the call and releases its current value, including one installed by a nested assignment in an argument. See the [assignment rules](../docs/development/string-ownership.md#complete-assignments-that-adopt-returned-joins-p0065).
- **Proven owned call assignments**: P0052 adopts the original proven factory result in complete assignment statements to owned scalar string identifier slots. A fresh temporary evaluates RHS once before releasing the current slot and storing the original allocation. Self-snapshot, nested argument replacement, original-pointer release order, peak live storage two and top-level lifetime are checked in both modes. Header/nested/chained assignment expressions and borrowed/mixed/method/shape/body fallbacks retain previous behavior. See the [assignment contract](../docs/development/owned-call-assignment.md).
- **Discarded proven owned calls** — P0054 releases the original result of a standalone factory call after normal completion. P0066 extends that statement to proven returned joins. Both modes verify 10,000 releases of each kind of original pointer at reference count one, with zero survivors after each call. See the [discard contract](../docs/development/owned-call-discard.md).
- **Proven owned arguments to Console wrappers**: P0056 releases the original pointer after a complete call statement to a synchronous void free function whose sole scalar string parameter is used only by one `Console.Write` or `Console.WriteLine` statement. The check uses actual lowered definitions, including imports. Both modes verify 10,000 releases with peak live storage one and zero survivors. See the [argument contract](../docs/development/owned-call-argument.md).
- **Direct Console output of proven factory results**: P0057 releases the original pointer immediately after a complete `Console.Write` or `Console.WriteLine` statement whose entire operand passes the existing factory proof. Each writer is checked for 10,000 original-pointer releases per mode at peak live count one. See the [ownership contract](../docs/development/string-ownership.md#direct-console-output-of-proven-factory-results-p0057).
- **Direct Console output of returned joins** — P0067 extends that statement cleanup to calls that satisfy the returned-join proof. Each writer reads the original join before the caller releases it, including with imported factories. See the [returned join rules](../docs/development/string-ownership.md#direct-console-output-of-returned-joins-p0067).
- **Returned joins passed to Console wrappers** — P0068 releases the original join after a complete call statement to a proven Console wrapper. The caller evaluates the factory arguments once and passes the join through the wrapper to the writer before release. See the [returned-join argument rule](../docs/development/string-ownership.md#returned-joins-passed-to-console-wrappers-p0068).
- **Complete appends with returned joins** — P0069 releases the original operand after a complete `text += joined();` statement concatenates it into a new string. The compiler requires a known-owned scalar destination and a proven returned join. See the [append rule](../docs/development/string-ownership.md#complete-appends-with-proven-returned-joins-p0069).
- **Append initializers with returned joins** — P0070 releases a proven returned join in `string snapshot = (text += joined());` after updating the owned scalar destination. The snapshot receives an independent managed copy, including in const and `for`-header declarations. See the [initializer rule](../docs/development/string-ownership.md#append-initializers-with-proven-returned-joins-p0070).
- **Console output of appends with returned joins** — P0071 releases a proven returned join in `Console.Write(text += joined());` or `Console.WriteLine(text += joined());` after updating the owned scalar destination. The destination keeps the value sent to the writer. See the [Console append rule](../docs/development/string-ownership.md#console-output-of-appends-with-proven-returned-joins-p0071).
- **Console wrapper arguments with returned join appends** — P0072 extends that release to a complete `view(text += joined());` call when `view` is a proven single-parameter Console wrapper. The owned destination keeps the value after the wrapper returns. See the [wrapper append rule](../docs/development/string-ownership.md#append-arguments-with-proven-returned-joins-p0072).
- **Class locals on break/continue**: Jump cleanup matches normal class-local releases in exited scopes. Both ordinary/module-entry modes check 27 class and six string releases, thirteen output lines and an empty live registry before shutdown; outer instances, string headers and nested loop/switch boundaries survive correctly. Fresh independent allocations are qualified; alias retention, class pointers/arrays, destructors/field destruction and value-return/exception exits remain unqualified
- **Borrowed string containers**: Local string arrays and explicit `string*` bindings receive no scalar release at normal block exit; their elements/targets remain borrowed without automatic retain, copy or element destruction. Scalar copies from elements and class-instance cleanup remain intact. Ordinary/module-entry checked-allocator regressions cover nested/unused storage, literal/managed/null elements, alias rebinding, loops and static/instance methods. Backing must outlive alias use; owning containers, element destruction and dangling-alias checks remain unqualified
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
- **First production compiler dogfooding**: Entry-file CRLF normalization is
  implemented in `src/driver/source_text.dr`. A C-only host bootstrap emits its C
  library translation unit; the production compiler links that implementation
  instead of the C seed, including cross-builds. `--emit-library-c OUTPUT SOURCE`
  accepts synchronous function-only sources without `main`, exposes external C
  functions and omits the entry point. Modules, global storage, top-level statements,
  type declarations and async are rejected. This does not establish full self-hosting
  or general C interop; see the [checkpoint](../docs/development/compiler-dogfooding.md).

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
