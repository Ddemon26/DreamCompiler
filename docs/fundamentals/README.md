# Dream Language Fundamentals

This section covers the core concepts and building blocks of the Dream programming language. These are the essential topics every Dream developer should understand.

## Contents

### [Type System](type-system.md)
Learn about Dream's comprehensive type system, including:
- Primitive types (`int`, `float`, `bool`, `char`, `string`)
- Type inference and explicit typing
- Type safety and conversion rules
- Advanced type concepts

### [Variables & Declarations](variables.md)
Master variable declaration and management:
- Variable declaration syntax
- Initialization and assignment
- Scope and lifetime rules
- Naming conventions and best practices

## Learning Path

If you're new to Dream, we recommend following this order:

1. **Start with [Variables & Declarations](variables.md)** - Learn the basics of working with data
2. **Move to [Type System](type-system.md)** - Understand how Dream's type system works

## What's Next?

After mastering the fundamentals, move on to:
- **[Language Features](../language-features/)** - Operators, functions, and control flow
- **[API Documentation](../api/)** - Built-in classes and functions

## Code Examples

### Basic Variable Declaration
```dream
// Explicit typing
int age = 25;
string name = "Alice";
bool isActive = true;

// Type inference
var count = 42;        // Inferred as int
var message = "Hello"; // Inferred as string
```

### Working with Different Types
```dream
func main() {
    // Numbers
    int whole = 42;
    float decimal = 3.14159;
    
    // Text
    char letter = 'A';
    string text = "Hello World";
    
    // Logic
    bool isTrue = true;
    bool isFalse = !isTrue;
    
    Console.WriteLine($"Age: {whole}, Pi: {decimal}, Letter: {letter}");
}
```

These fundamentals form the foundation for all Dream programming. Take your time to understand them well before moving on to more advanced topics.