# Dream Compiler & Language Documentation

**DreamCompiler** is a modern, high-performance compiler that translates Dream language (`.dr`) source files into optimized C code using Zig as the build system. The compilation pipeline produces portable C code that compiles to native executables.

## What is Dream?

Dream is a C#-style programming language designed for systems programming with modern language features. It combines the performance of C with the developer experience of high-level languages.

```dream
// Hello World in Dream
func main() {
    Console.WriteLine("Hello, World!");
}
```

## Architecture Overview

```
.dr files → DreamCompiler → Portable C → zig cc → Native Executable
```

The compiler implements a full compilation pipeline:
- **Lexical Analysis** - re2c-generated tokenizer
- **Parsing** - Pratt parser with comprehensive error recovery
- **Semantic Analysis** - Type checking and symbol resolution  
- **IR Generation** - SSA-based three-address intermediate representation
- **Optimization** - Multiple passes including SCCP, DCE, LICM, CSE
- **Code Generation** - Clean, readable C code output

## Key Features

### Language Features
- **Static Typing** - `int`, `float`, `bool`, `char`, `string`, arrays, classes
- **Modern Control Flow** - if/else, loops, switch, exception handling
- **Object-Oriented** - Classes, structs, inheritance
- **Multi-file Compilation** - Module system with import/export
- **Memory Management** - Reference-counted automatic memory management

### Compiler Features
- **Cross-Platform** - Windows, Linux, macOS support
- **Fast Compilation** - Optimized compilation pipeline
- **Debug Support** - Line mapping for source-level debugging
- **IDE Integration** - IntelliSense and language server support
- **Comprehensive Testing** - 100+ automated test cases

## Current Version: v1.1.12

The latest stable release is **DreamCompiler v1.1.12**, featuring complete multi-file compilation, advanced optimization, and comprehensive language support.

## Platform Support

| Platform | Status | Test Pass Rate |
|----------|--------|----------------|
| Linux    | Full Support | ~95% |
| Windows  | Stable | ~85% |
| macOS    | In Development | TBD |

## Quick Start

### Installation
```bash
# Clone the repository
git clone https://github.com/company/DreamCompiler
cd DreamCompiler

# Build the compiler
zig build

# Run a simple program
zig build run -- examples/hello.dr
```

### Your First Program
Create a file called `hello.dr`:

```dream
func main() {
    string name = Console.ReadLine();
    Console.WriteLine("Hello, " + name + "!");
}
```

Compile and run:
```bash
zig build run -- hello.dr
cd build/bin && ./hello
```

## Documentation Sections

### 🚀 Getting Started
New to Dream? Start here to get up and running quickly.
- **[Quick Start](getting-started/intro.md)** - Install and compile your first program
- **[Installation & Usage](getting-started/usage.md)** - Detailed setup instructions
- **[Examples & Tutorials](getting-started/examples/)** - Hands-on examples and code samples

### 🔧 Fundamentals
Learn the core concepts and building blocks of the Dream language.
- **[Type System](fundamentals/type-system.md)** - Variables, types, and type inference
- **[Variables & Declarations](fundamentals/variables.md)** - Variable declarations and scoping

### ⚡ Language Features
Master the syntax and features that make Dream powerful and expressive.
- **[Operators](language-features/operators.md)** - Arithmetic, logical, and comparison operators
- **[Control Flow](language-features/language-guide/control-flow.md)** - Conditionals, loops, and branching
- **[Functions](language-features/functions.md)** - Function definitions, parameters, and returns
- **[Function Overview](language-features/functions-overview.md)** - Complete guide to functions
- **[Exception Handling](language-features/exceptions.md)** - Error handling with try/catch/throw
- **[Input/Output](language-features/io/)** - Console and file I/O operations

### 🏗️ Advanced Topics
Deep dive into sophisticated features for complex applications.
- **[Multi-file Compilation](advanced-topics/modules.md)** - Module system and project organization
- **[Memory Management](advanced-topics/memory.md)** - Reference counting and resource management
- **[C Interoperability](advanced-topics/interop.md)** - Calling C libraries from Dream code

### 📚 API Documentation
Complete reference for all built-in classes, functions, and runtime features.
- **[API Reference](api/api-reference.md)** - Comprehensive API documentation with examples
- **[Runtime Library](api/runtime.md)** - Built-in functions and system interfaces

### 🔨 Development
Resources for compiler development, contribution, and advanced usage.
- **[Developer Guide](development/developer-guide.md)** - Complete guide for contributors and advanced developers
- **[Compiler Architecture](development/compiler/index.md)** - Internal compiler design and structure
- **[Build System](development/compiler/build.md)** - Zig build configuration and customization
- **[Testing Framework](development/compiler/testing.md)** - Test organization, execution, and contribution

### 📖 Reference Materials
Specifications, version history, and complete language documentation.
- **[Language Reference](reference/language-reference.md)** - Complete language specification and guide
- **[Grammar Specification](reference/grammar/Grammar.md)** - Formal language grammar (BNF)
- **[Changelog](reference/changelog.md)** - Version history, updates, and migration guides
- **[Version Archives](reference/)** - Historical documentation for v1.0, v1.1, and earlier releases

## Community & Support

Dream is designed for professional development teams building high-performance applications. The compiler and language prioritize reliability, maintainability, and developer productivity.

For technical support and contributions, please refer to the project repository and documentation.
