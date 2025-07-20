# Dream Language Reference

Welcome to the comprehensive Dream language reference documentation. Dream is a modern, statically-typed programming language designed for systems programming with C#-style syntax and high performance.

## Language Overview

Dream combines the performance of C with the developer experience of high-level languages. It features:

- **Static Typing** with type inference
- **Automatic Memory Management** through reference counting
- **Multi-file Compilation** with module system
- **Exception Handling** for robust error management
- **Object-Oriented Programming** with classes and structs
- **Cross-Platform Compilation** to native executables

## Quick Example

```dream
// Hello World in Dream
func main() {
    string name = Console.ReadLine();
    Console.WriteLine("Hello, " + name + "!");
}
```

## Documentation Sections

### 🔧 Type System
Learn about Dream's comprehensive type system and how to work with different data types.

- **[Variables & Types](type-system.md)** - Core type system, declarations, and type inference
- **[Arrays](v1.1/arrays.md)** - Fixed-size arrays and multi-dimensional arrays
- **[Strings](v1.1/strings.md)** - String literals, operations, and manipulation
- **[Classes & Structs](v1.1/examples/classes.md)** - Object-oriented programming and value types

### ⚡ Operators
Master all operators available in Dream for different types of operations.

- **[Operators Overview](operators.md)** - Complete operator reference by category
- **[Arithmetic](v1.1/arithmetic.md)** - Mathematical operations and expressions
- **[Comparison](v1.1/comparison.md)** - Relational and equality operators
- **[Logical](v1.1/logical.md)** - Boolean logic and short-circuit evaluation
- **[Bitwise](v1.1/bitwise.md)** - Bit manipulation and binary operations
- **[Conditional](v1.1/conditional.md)** - Ternary operator and conditional expressions

### 🌊 Control Flow
Understand how to control program execution flow with various statements.

- **[Control Flow Overview](control-flow.md)** - Program flow control concepts
- **[Conditionals](v1.1/if.md)** - If statements and branching logic
- **[Loops](v1.1/loops.md)** - For, while, and do-while iterations
- **[Switch Statements](v1.1/switch.md)** - Multi-way branching and pattern matching
- **[Exception Handling](exceptions.md)** - Try-catch-throw error management

### 🔨 Functions
Learn how to define, call, and organize code using functions.

- **[Functions Overview](functions-overview.md)** - Function concepts and organization
- **[Function Definitions](v1.1/examples/functions.md)** - Creating and using functions
- **[Return Values](v1.1/return.md)** - Returning data from functions
- **[Parameters](parameters.md)** - Function parameters and argument passing

### 💬 Input & Output
Handle user interaction and data exchange with the console and external systems.

- **[Console Operations](v1.1/console.md)** - Basic console input/output
- **[Input/Output](input-output.md)** - Advanced I/O operations and formatting

## Advanced Topics

### 📦 Modular Programming
- **[Multi-file Compilation](modules.md)** - Module system, imports, and exports
- **[Project Organization](project-organization.md)** - Best practices for large projects

### ⚙️ Memory & Performance
- **[Memory Management](memory.md)** - Automatic memory management and optimization
- **[C Interoperability](interop.md)** - Calling C code from Dream

### 📚 Reference Materials
- **[Grammar Specification](grammar/Grammar.md)** - Formal language grammar
- **[Keywords & Operators](keywords.md)** - Complete language reference
- **[Runtime Library](runtime.md)** - Built-in functions and APIs
- **[Changelog](v1.1/changelog.md)** - Version history and updates

## Learning Path

### 🚀 Getting Started
1. **[Variables & Types](type-system.md)** - Learn the basic type system
2. **[Functions](functions-overview.md)** - Understand function basics
3. **[Control Flow](control-flow.md)** - Master program flow control
4. **[Console I/O](v1.1/console.md)** - Handle user interaction

### 🏗️ Building Applications
1. **[Classes & Structs](v1.1/examples/classes.md)** - Object-oriented design
2. **[Exception Handling](exceptions.md)** - Robust error management
3. **[Multi-file Projects](modules.md)** - Organize larger codebases
4. **[Memory Management](memory.md)** - Optimize performance

### 🔧 Advanced Topics
1. **[Bitwise Operations](v1.1/bitwise.md)** - Low-level programming
2. **[C Interoperability](interop.md)** - System integration
3. **[Grammar Specification](grammar/Grammar.md)** - Language internals

## Code Examples

Throughout this documentation, you'll find practical examples demonstrating real-world usage:

### Basic Program Structure
```dream
// Import statements (optional)
import string_utils;

// Global variables
int globalCounter = 0;

// Function definitions
func int calculate(int a, int b) {
    return a * b + globalCounter;
}

// Main entry point
func main() {
    int result = calculate(5, 10);
    Console.WriteLine("Result: " + result);
}
```

### Object-Oriented Example
```dream
// Class definition
class Calculator {
    float lastResult;
    bool hasError;
}

// Function operating on class
func float divide(Calculator calc, float a, float b) {
    if (b == 0.0) {
        calc.hasError = true;
        return 0.0;
    }
    calc.lastResult = a / b;
    calc.hasError = false;
    return calc.lastResult;
}

func main() {
    Calculator calc = new Calculator();
    float result = divide(calc, 10.0, 3.0);
    
    if (!calc.hasError) {
        Console.WriteLine("10 / 3 = " + result);
    }
}
```

### Error Handling Example
```dream
func void safeOperation() {
    try {
        int result = riskyCalculation();
        Console.WriteLine("Success: " + result);
    } catch {
        Console.WriteLine("An error occurred during calculation");
    }
}

func int riskyCalculation() {
    int value = getUserInput();
    if (value < 0) {
        throw "Negative values not allowed";
    }
    return value * 2;
}
```

## Best Practices

### Code Organization
- Use meaningful variable and function names
- Keep functions focused on single responsibilities
- Organize related functionality into separate files
- Use proper indentation and consistent formatting

### Type Safety
- Prefer explicit type declarations for clarity
- Use type inference (`var`) for obvious cases
- Validate input parameters in functions
- Handle all possible error conditions

### Performance
- Use appropriate data types for your needs
- Prefer structs for simple value types
- Use classes for complex entities requiring reference semantics
- Consider memory usage patterns in your designs

### Error Handling
- Use exceptions for exceptional circumstances
- Validate inputs early and explicitly
- Provide meaningful error messages
- Clean up resources in all code paths

## Version Information

This documentation covers **Dream Language v1.1.12**, which includes all core language features and the complete standard library. For version-specific information, use the version selector to view documentation for different releases.

**Current Features:**
- Complete type system with inference
- Object-oriented programming with classes and structs
- Comprehensive operator set including bitwise operations
- Full control flow constructs with exception handling
- Multi-file compilation with module system
- Automatic memory management
- Cross-platform compilation support

Ready to start learning Dream? Begin with the **[Type System](type-system.md)** to understand the foundation of the language!