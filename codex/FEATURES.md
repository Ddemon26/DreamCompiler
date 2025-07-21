# Dream Compiler Features

This document tracks the implementation status of Dream language features.

## ✅ Fully Implemented Features

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
- **Classes and structs**: Object construction and instantiation
- **Instance methods**: With `this` pointer (emitted as C functions)
- **Object-oriented programming**: Basic class inheritance and method calls

### I/O and Runtime
- **Console I/O**: `Console.Write`, `Console.WriteLine`, and `Console.ReadLine`
- **Memory management**: Reference-counted allocation system
- **Comments**: Line (`//`) and block (`/* */`) comments
- **Arrays**: Arrays of primitive types with indexing

### Code Generation
- **C backend**: Complete C code generation pipeline
- **Multi-file compilation**: Module system with import/export (framework implemented)
- **Runtime library**: Memory management, console I/O, and exception handling
- **Build integration**: Zig build system with cross-platform support

## 🚧 Partially Implemented Features

### Multi-File System
- **Status**: Framework exists, needs debugging
- **Components**: Module declarations, import/export statements
- **Issues**: Cross-module linking and dependency resolution need work
- **Location**: Tests moved to individual test files, framework in `src/codegen/module.c`

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
- **Core Features**: ~85-95% pass rate on Linux/Windows
- **Basic Operations**: Fully functional
- **Control Flow**: Fully functional  
- **Functions**: Fully functional
- **Exception Handling**: Fully functional
- **Multi-file**: Framework exists, needs debugging
- **Concurrency**: Not implemented (tests excluded)
- **Graphics**: Not implemented (tests excluded)

### Platform Support
- **Linux**: Primary development platform, ~95% test pass rate
- **Windows**: Good support, ~85% test pass rate (some struct parsing issues)
- **Cross-platform**: Build system supports both platforms

### Development Priority
1. **High**: Complete multi-file compilation system
2. **Medium**: Concurrency/async implementation with platform abstraction
3. **Low**: Graphics/Vulkan integration (specialized use case)

## 📁 Test Organization

- **`tests/`**: Currently implemented and working features
- **`not_implemented_tests/`**: Features not yet implemented or experimental
  - `not_implemented_tests/advanced/concurrency/`: Async/await tests
  - `not_implemented_tests/graphics/`: Vulkan integration tests
