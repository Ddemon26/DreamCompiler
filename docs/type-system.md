# Type System

Dream features a comprehensive static type system with type inference capabilities. This section covers all aspects of working with types, from basic primitives to complex data structures.

## Overview

Dream's type system is designed for:
- **Type Safety** - Catch errors at compile time
- **Performance** - Efficient memory usage and execution
- **Clarity** - Clear and predictable behavior
- **Productivity** - Type inference reduces boilerplate

## Type Categories

### Primitive Types

Dream provides five core primitive types that form the foundation of the type system:

| Type | Size | Range | Description |
|------|------|-------|-------------|
| `int` | 4 bytes | -2,147,483,648 to 2,147,483,647 | 32-bit signed integer |
| `float` | 4 bytes | ±3.4e±38 | IEEE 754 single precision |
| `bool` | 1 byte | `true` or `false` | Boolean value |
| `char` | 1 byte | 0-255 | Single character (ASCII/UTF-8) |
| `string` | Variable | N/A | Reference-counted string |

### Composite Types

Build complex data structures using composite types:

- **[Arrays](v1.1/arrays.md)** - Fixed-size collections of elements
- **[Structs](v1.1/examples/classes.md#structs)** - Value types for grouping related data
- **[Classes](v1.1/examples/classes.md#classes)** - Reference types for object-oriented programming

## Core Concepts

### 1. Variable Declaration

Learn the fundamentals of declaring and initializing variables:

```dream
// Explicit type declaration
int age = 25;
float temperature = 98.6;
bool isActive = true;
char grade = 'A';
string name = "John Doe";

// Type inference with var
var count = 42;        // Inferred as int
var price = 19.99;     // Inferred as float
var enabled = false;   // Inferred as bool
```

**📖 [Complete Variables Guide](v1.1/variables.md)**

### 2. Array Types

Work with fixed-size collections of elements:

```dream
// Array declarations
int numbers[5];
float temperatures[7];
string names[3];

// Array initialization
numbers[0] = 10;
numbers[1] = 20;
numbers[2] = 30;

// Multi-dimensional arrays
int matrix[3][3];
matrix[0][0] = 1;
matrix[0][1] = 2;
```

**📖 [Complete Arrays Guide](v1.1/arrays.md)**

### 3. String Type

Handle text and character data efficiently:

```dream
// String literals
string simple = "Hello World";
string escaped = "Line 1\nLine 2\tTabbed";
string quotes = "He said \"Hello\"";

// String concatenation
string first = "Hello";
string second = "World";
string combined = first + " " + second;

// Automatic conversion in concatenation
int age = 25;
string message = "Age: " + age;  // "Age: 25"
```

**📖 [Complete Strings Guide](v1.1/strings.md)**

### 4. Object Types

Create complex data structures with classes and structs:

```dream
// Struct definition (value type)
struct Point {
    int x;
    int y;
}

// Class definition (reference type)
class Person {
    string name;
    int age;
    bool isEmployed;
}

// Usage
Point origin;           // Value type - stack allocated
origin.x = 0;
origin.y = 0;

Person employee = new Person();  // Reference type - heap allocated
employee.name = "Alice";
employee.age = 30;
```

**📖 [Complete Classes & Structs Guide](v1.1/examples/classes.md)**

## Type Operations

### Type Conversions

Dream supports both implicit and explicit type conversions:

```dream
// Implicit conversions (safe)
int intValue = 42;
float floatValue = intValue;    // int to float (automatic)

char ch = 'A';
int ascii = ch;                 // char to int (automatic)

// String conversion (automatic in concatenation)
int number = 123;
string text = "Value: " + number;  // Automatic conversion
```

### Type Checking

Ensure type safety at compile time:

```dream
// Compile-time type checking
int x = 10;
// x = "hello";  // Error: Cannot assign string to int

// Function parameter type checking
func int add(int a, int b) {
    return a + b;
}

int result = add(5, 10);        // OK
// int result = add(5, "10");   // Error: Wrong parameter type
```

## Memory Management

### Value vs Reference Types

Understanding the difference is crucial for effective programming:

```dream
// Value types (structs) - copied when assigned
struct Point {
    int x;
    int y;
}

Point p1;
p1.x = 10;
p1.y = 20;

Point p2 = p1;  // p2 is a copy of p1
p2.x = 30;      // Only modifies p2

Console.WriteLine("p1.x: " + p1.x);  // Prints: p1.x: 10
Console.WriteLine("p2.x: " + p2.x);  // Prints: p2.x: 30
```

```dream
// Reference types (classes) - share the same object
class Rectangle {
    float width;
    float height;
}

Rectangle rect1 = new Rectangle();
rect1.width = 10.0;
rect1.height = 5.0;

Rectangle rect2 = rect1;  // rect2 references the same object
rect2.width = 20.0;       // Modifies the shared object

Console.WriteLine("rect1.width: " + rect1.width);  // Prints: rect1.width: 20.0
```

### Automatic Memory Management

Dream uses reference counting for automatic memory management:

```dream
// Memory is automatically managed
string text1 = "Hello";
string text2 = text1;      // Reference count increases
text1 = "World";            // Original reference count decreases

// Arrays are also automatically managed
int numbers[100];           // Automatically allocated
// Automatically freed when out of scope
```

## Best Practices

### 1. Type Selection

Choose the right type for your data:

```dream
// Use appropriate types for your data
int userAge = 25;           // Good - age is always an integer
float price = 19.99;        // Good - price needs decimal precision
bool isActive = true;       // Good - clear true/false state
char grade = 'A';           // Good - single character
string message = "Hello";   // Good - text data

// Avoid unclear or inappropriate types
float userAge = 25.0;       // Poor - age doesn't need decimal precision
int isActive = 1;           // Poor - unclear true/false representation
```

### 2. Naming Conventions

Use clear, descriptive names:

```dream
// Good naming conventions
int userAge = 25;
string firstName = "John";
bool isLoggedIn = true;
float temperatureInCelsius = 23.5;

// Poor naming conventions
int a = 25;                 // Unclear what 'a' represents
string n = "John";          // Unclear abbreviation
bool flag = true;           // Unclear what the flag indicates
float t = 23.5;             // Unclear what 't' represents
```

### 3. Initialization

Always initialize variables before use:

```dream
// Good - explicit initialization
int count = 0;
string message = "";
bool isValid = false;

// Dangerous - uninitialized variables
int count;                  // Undefined value
// int result = count + 5;  // Undefined behavior
```

### 4. Type Safety

Leverage the type system for safety:

```dream
// Use the type system to prevent errors
func float calculatePercentage(int part, int total) {
    if (total == 0) {
        throw "Cannot divide by zero";
    }
    return (float)part / total * 100.0;
}

// Type checking prevents runtime errors
float percentage = calculatePercentage(25, 100);  // OK
// float percentage = calculatePercentage("25", 100);  // Compile error
```

## Advanced Topics

### 1. Type Inference

Let the compiler deduce types when appropriate:

```dream
// Use var for obvious types
var numbers = new int[10];      // Clearly an int array
var message = "Hello World";    // Clearly a string
var isActive = true;            // Clearly a bool

// Use explicit types for clarity
func float calculateAverage(int values[], int count) {
    float sum = 0.0;            // Explicit float for clarity
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    return sum / count;
}
```

### 2. Complex Type Compositions

Build sophisticated data structures:

```dream
// Nested structures
struct Address {
    string street;
    string city;
    string zipCode;
}

class Customer {
    string name;
    Address address;
    int customerID;
    bool isActive;
}

// Arrays of complex types
Customer customers[100];
Address officeLocations[5];

// Initialization
Customer newCustomer = new Customer();
newCustomer.name = "Jane Smith";
newCustomer.address.street = "123 Main St";
newCustomer.address.city = "Springfield";
newCustomer.customerID = 12345;
```

### 3. Performance Considerations

Choose types based on performance needs:

```dream
// Use structs for small, frequently copied data
struct Vector2 {
    float x;
    float y;
}

// Use classes for larger, shared objects
class GameWorld {
    int width;
    int height;
    string name;
    GameObject objects[1000];
    bool isActive;
}

// Consider memory usage
int smallArray[10];     // Stack allocated
int largeArray[10000];  // Consider heap allocation for very large arrays
```

## Type System Reference

### Quick Type Reference

| Type | Default Value | Null/Zero Check | Conversion |
|------|---------------|-----------------|------------|
| `int` | 0 | `value == 0` | Automatic to `float` |
| `float` | 0.0 | `value == 0.0` | From `int` (automatic) |
| `bool` | `false` | `value == false` | Not automatic |
| `char` | '\0' | `value == '\0'` | Automatic to `int` |
| `string` | `""` | `value == ""` | Automatic in concatenation |

### Common Type Patterns

```dream
// Configuration data
struct Config {
    string serverUrl;
    int port;
    bool useSSL;
    float timeoutSeconds;
}

// State management
class GameState {
    int score;
    int lives;
    float timeRemaining;
    bool isPaused;
    string playerName;
}

// Data processing
func void processUserData(string name, int age, bool isActive) {
    if (name == "") {
        throw "Name cannot be empty";
    }
    if (age < 0 || age > 150) {
        throw "Invalid age range";
    }
    
    // Process valid data
    Console.WriteLine("Processing user: " + name);
}
```

## Next Steps

After mastering the type system, explore these related topics:

1. **[Operators](operators.md)** - Learn how to manipulate and work with your data
2. **[Functions](functions-overview.md)** - Organize your code with type-safe functions
3. **[Control Flow](control-flow.md)** - Control program execution based on data values
4. **[Arrays](v1.1/arrays.md)** - Work with collections of typed data
5. **[Classes & Structs](v1.1/examples/classes.md)** - Build complex object-oriented systems

The type system is the foundation of Dream programming. Understanding these concepts will make you more productive and help you write safer, more maintainable code.