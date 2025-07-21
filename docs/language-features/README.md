# Dream Language Features

This section covers the core language constructs and features that make Dream powerful and expressive. Master these concepts to write effective Dream programs.

## Contents

### Core Language Constructs

#### [Operators](operators.md)
Complete guide to Dream's operator system:
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`
- **Comparison**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Logical**: `&&`, `||`, `!`
- **Bitwise**: `&`, `|`, `^`, `~`, `<<`, `>>`
- **Assignment**: `=`, `+=`, `-=`, etc.

#### [Control Flow](language-guide/control-flow.md)
Master program flow control:
- **Conditionals**: `if`/`else` statements
- **Loops**: `for`, `while`, `do-while`
- **Branching**: `switch` statements
- **Jump statements**: `break`, `continue`, `return`

#### [Functions](functions.md)
Learn function definition and usage:
- Function declaration and implementation
- Parameters and return values
- Function overloading
- Local vs global scope

#### [Function Overview](functions-overview.md)
Comprehensive function guide covering:
- Advanced parameter techniques
- Function organization and design
- Best practices and patterns

### Advanced Language Features

#### [Exception Handling](exceptions.md)
Robust error handling with:
- `try`/`catch`/`finally` blocks
- `throw` statements
- Custom exception types
- Error handling patterns

#### [Input/Output](io/)
Console and file operations:
- Console input/output
- File reading and writing
- Formatting and parsing
- Stream operations

## Learning Path

### Beginner Path
1. **[Operators](operators.md)** - Learn to work with data
2. **[Control Flow](language-guide/control-flow.md)** - Control program execution
3. **[Functions](functions.md)** - Organize code into reusable blocks
4. **[Input/Output](io/)** - Interact with users and files

### Intermediate Path
5. **[Function Overview](functions-overview.md)** - Advanced function techniques
6. **[Exception Handling](exceptions.md)** - Handle errors gracefully

## Practical Examples

### Using Operators and Control Flow
```dream
func calculateGrade(int score) -> string {
    if (score >= 90) {
        return "A";
    } else if (score >= 80) {
        return "B";
    } else if (score >= 70) {
        return "C";
    } else if (score >= 60) {
        return "D";
    } else {
        return "F";
    }
}

func main() {
    for (int i = 1; i <= 5; i++) {
        int score = 70 + (i * 5); // 75, 80, 85, 90, 95
        string grade = calculateGrade(score);
        Console.WriteLine($"Score {score}: Grade {grade}");
    }
}
```

### Exception Handling Example
```dream
func safeDivide(float a, float b) -> float {
    try {
        if (b == 0.0) {
            throw new DivideByZeroException("Cannot divide by zero");
        }
        return a / b;
    } catch (DivideByZeroException ex) {
        Console.WriteLine($"Error: {ex.Message}");
        return 0.0;
    }
}
```

### File I/O Example
```dream
func processFile(string filename) {
    try {
        string content = File.ReadAllText(filename);
        string[] lines = content.Split('\n');
        
        Console.WriteLine($"File has {lines.Length} lines");
        
        foreach (string line in lines) {
            if (!String.IsNullOrEmpty(line)) {
                Console.WriteLine($"Line: {line.Trim()}");
            }
        }
    } catch (FileNotFoundException) {
        Console.WriteLine($"File not found: {filename}");
    }
}
```

## What's Next?

After mastering these language features:
- **[Advanced Topics](../advanced-topics/)** - Memory management, modules, and C interop
- **[API Documentation](../api/)** - Built-in classes and runtime library
- **[Development](../development/)** - Contributing to the compiler

These language features form the core of Dream programming. Understanding them well will enable you to write clean, efficient, and maintainable Dream applications.