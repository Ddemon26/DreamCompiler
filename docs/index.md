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

### Getting Started
- **[Quick Start](intro.md)** - Install and compile your first program
- **[Installation & Usage](usage.md)** - Detailed setup instructions

### Compiler Documentation  
- **[Architecture](compiler/index.md)** - Compiler internals and design
- **[Build System](compiler/build.md)** - Zig build configuration
- **[Testing Framework](compiler/testing.md)** - Test organization and execution

### Language Reference
- **[Type System](variables.md)** - Variables, types, and declarations
- **[Operators](arithmetic.md)** - Arithmetic, logical, and comparison operators
- **[Control Flow](if.md)** - Conditionals, loops, and exception handling
- **[Functions](functions.md)** - Function definitions and calls
- **[Classes & Objects](classes.md)** - Object-oriented programming

### Advanced Features
- **[Multi-file Compilation](modules.md)** - Module system and project organization
- **[Memory Management](memory.md)** - Reference counting and resource management
- **[C Interoperability](interop.md)** - Calling C code from Dream

### Reference
- **[Grammar Specification](grammar/Grammar.md)** - Formal language specification
- **[Runtime Library](runtime.md)** - Built-in functions and APIs
- **[Changelog](changelog.md)** - Version history and updates

## Community & Support

Dream is designed for professional development teams building high-performance applications. The compiler and language prioritize reliability, maintainability, and developer productivity.

For technical support and contributions, please refer to the project repository and documentation.
