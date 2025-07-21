# Dream API Documentation

This section provides comprehensive documentation for Dream's built-in classes, functions, and runtime library. All APIs are designed for high performance and developer productivity.

## Contents

### [API Reference](api-reference.md)
Complete documentation for all built-in Dream APIs, including:

#### Core Console Operations
- **Console Class** - Input/output operations, formatting, colors
- **File I/O Classes** - File, Directory, Path operations
- **String Classes** - String manipulation, StringBuilder

#### Mathematical Operations  
- **Math Class** - Mathematical functions, constants, utilities
- **DateTime/TimeSpan** - Date and time manipulation

#### Collections Framework
- **List<T>** - Dynamic arrays with comprehensive operations
- **Dictionary<K,V>** - Hash-based key-value collections
- **Set<T>** - Unique value collections with set operations

### [Runtime Library](runtime.md)
Low-level runtime functions and system interfaces:

#### Memory Management System
- **Memory Allocation** - `dr_alloc_*`, `dr_retain_*`, `dr_release_*`
- **Memory Statistics** - Debugging and profiling functions
- **Memory Pools** - High-performance allocation strategies

#### Console I/O System  
- **Output Functions** - Text, formatted, and binary output
- **Input Functions** - Line, character, and typed input
- **Console Configuration** - Colors, cursor, formatting

#### Custom Runtime Functions
- **String Utilities** - Advanced string manipulation
- **Array Operations** - Array utilities and algorithms
- **File Operations** - Low-level file system access

## Quick Reference

### Most Common APIs

#### Console Operations
```dream
// Output
Console.WriteLine("Hello World");
Console.WriteFormat("Value: {0}", 42);

// Input
string name = Console.ReadLine();
int age = Console.ReadInt();
```

#### String Manipulation
```dream
string text = "Hello World";
string upper = text.ToUpper();           // "HELLO WORLD"
string[] words = text.Split(' ');        // ["Hello", "World"]
bool contains = text.Contains("World");  // true
```

#### Collections
```dream
// Lists
List<int> numbers = new List<int>();
numbers.Add(1);
numbers.Add(2);
numbers.Add(3);

// Dictionaries
Dictionary<string, int> ages = new Dictionary<string, int>();
ages["Alice"] = 30;
ages["Bob"] = 25;
```

#### File Operations
```dream
// Reading files
string content = File.ReadAllText("data.txt");
string[] lines = File.ReadAllLines("config.txt");

// Writing files
File.WriteAllText("output.txt", "Hello World");
File.AppendAllText("log.txt", "New entry\n");
```

#### Mathematical Functions
```dream
double result = Math.Sqrt(16.0);    // 4.0
double angle = Math.PI / 4;         // 45 degrees in radians
int max = Math.Max(10, 20);         // 20
```

## API Categories by Use Case

### Text Processing
- **String Class** - Basic string operations
- **StringBuilder** - Efficient string building
- **Console.WriteFormat** - Formatted output
- **String.Split/Join** - Text parsing and assembly

### Data Management
- **List<T>** - Dynamic arrays for ordered data
- **Dictionary<K,V>** - Key-value mappings
- **Set<T>** - Unique collections
- **Array utilities** - Sorting, searching, manipulation

### File and I/O
- **File Class** - File reading/writing
- **Directory Class** - Directory operations
- **Path Class** - Path manipulation
- **Console Class** - Terminal interaction

### Mathematics and Logic
- **Math Class** - Mathematical functions
- **DateTime** - Date/time calculations
- **Comparison operators** - Sorting and ordering
- **Type conversion** - Between numeric types

### System Integration
- **Memory management** - Manual memory control when needed
- **Error handling** - Exception types and handling
- **Platform detection** - OS-specific functionality
- **Performance monitoring** - Timing and profiling

## API Design Principles

### Consistency
All Dream APIs follow consistent naming and parameter patterns:
- **PascalCase** for classes and methods
- **camelCase** for parameters and local variables  
- **Descriptive names** that clearly indicate purpose
- **Consistent parameter ordering** across similar functions

### Performance
APIs are designed for optimal performance:
- **Zero-copy operations** where possible
- **Memory pool utilization** for frequent allocations
- **Lazy evaluation** for expensive operations
- **Batch operations** for multiple items

### Safety
All APIs prioritize safety and correctness:
- **Null checks** and validation in all functions
- **Bounds checking** for array and collection access
- **Type safety** with compile-time checking
- **Exception handling** for error conditions

## Error Handling in APIs

### Exception Types
Common exception types thrown by APIs:
- **ArgumentException** - Invalid parameters
- **InvalidOperationException** - Invalid state for operation
- **IndexOutOfRangeException** - Array/collection bounds errors
- **FileNotFoundException** - File system errors
- **OutOfMemoryException** - Memory allocation failures

### Error Handling Patterns
```dream
// Safe file reading with error handling
func string readFileContent(string filename) {
    try {
        return File.ReadAllText(filename);
    } catch (FileNotFoundException) {
        Console.WriteLine($"File not found: {filename}");
        return "";
    } catch (IOException ex) {
        Console.WriteLine($"I/O error reading {filename}: {ex.Message}");
        return "";
    }
}

// Safe collection access
func T getOrDefault<T>(List<T> list, int index, T defaultValue) {
    if (index >= 0 && index < list.Count) {
        return list[index];
    }
    return defaultValue;
}
```

## Performance Tips

### String Operations
- Use `StringBuilder` for multiple concatenations
- Prefer `String.Format` over repeated concatenation
- Cache frequently used strings
- Use string interning for constants

### Collections
- Pre-size collections when final size is known
- Use appropriate collection types for use case
- Avoid unnecessary boxing/unboxing
- Leverage LINQ-style operations efficiently

### Memory Management
- Minimize object creation in loops
- Dispose of resources explicitly when possible
- Use memory pools for high-frequency allocations
- Monitor memory usage with debug builds

## What's Next?

After familiarizing yourself with the APIs:
- **[Advanced Topics](../advanced-topics/)** - Memory management and C interop
- **[Language Features](../language-features/)** - Core language constructs
- **[Development](../development/)** - Contributing to the runtime library

The Dream API provides everything needed for professional application development, from basic I/O to advanced system integration.