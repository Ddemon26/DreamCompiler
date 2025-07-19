# Variables & Type System

Dream is a statically-typed language with type inference support. Variables must be declared before use and have a specific type that is checked at compile time.

## Basic Types

### Primitive Types

| Type | Description | Size | Range/Notes |
|------|-------------|------|-------------|
| `int` | 32-bit signed integer | 4 bytes | -2,147,483,648 to 2,147,483,647 |
| `float` | IEEE 754 single precision | 4 bytes | ±3.4e±38 (~7 decimal digits) |
| `bool` | Boolean value | 1 byte | `true` or `false` |
| `char` | Single character | 1 byte | ASCII/UTF-8 character |
| `string` | String of characters | Variable | Reference-counted string |

### Variable Declaration

#### Explicit Type Declaration
```dream
// Basic variable declarations
int age = 25;
float pi = 3.14159;
bool isActive = true;
char grade = 'A';
string name = "John Doe";
```

#### Type Inference with `var`
The compiler can automatically infer types:

```dream
var count = 42;        // Inferred as int
var temperature = 98.6; // Inferred as float
var enabled = true;     // Inferred as bool
var initial = 'J';     // Inferred as char
var message = "Hello";  // Inferred as string
```

#### Uninitialized Variables
Variables can be declared without initial values:

```dream
int x;              // Uninitialized int (undefined behavior to read)
float y;            // Uninitialized float
bool flag;          // Uninitialized bool
string text;        // Uninitialized string (null)

// Must assign before use
x = 100;
y = 2.5;
flag = false;
text = "Now initialized";
```

## Literals

### Integer Literals
```dream
int decimal = 42;           // Decimal
int hex = 0xFF;             // Hexadecimal (255)
int binary = 0b1010;        // Binary (10)
int withSeparators = 1_000_000; // Underscore separators
```

### Float Literals
```dream
float simple = 3.14;        // Basic decimal
float scientific = 1.5e10;  // Scientific notation
float smallNumber = 2.5e-3; // Small scientific notation
```

### Character Literals
```dream
char letter = 'A';          // Simple character
char newline = '\n';        // Escape sequence
char tab = '\t';            // Tab character
char quote = '\'';          // Escaped single quote
char unicode = '\u0041';    // Unicode escape (A)
```

### String Literals
```dream
string simple = "Hello World";
string escaped = "Line 1\nLine 2\tTabbed";
string quotes = "He said \"Hello\"";
string path = "C:\\Users\\Name"; // Escaped backslashes
```

### Boolean Literals
```dream
bool isTrue = true;
bool isFalse = false;
```

## Variable Scope

### Global Scope
Variables declared outside functions are global:

```dream
int globalCounter = 0;  // Global variable

func increment() {
    globalCounter++;    // Can access global
}
```

### Function Scope
Variables declared inside functions are local:

```dream
func calculate() {
    int localVar = 10;  // Local to this function
    return localVar * 2;
}

func other() {
    // localVar is not accessible here
    int localVar = 20;  // Different variable with same name
}
```

### Block Scope
Variables declared in blocks are scoped to that block:

```dream
func example() {
    int outer = 1;
    
    if (true) {
        int inner = 2;      // Only accessible in this block
        outer = inner + 1;  // Can access outer scope
    }
    
    // inner is not accessible here
    Console.WriteLine(outer); // OK - outer is still in scope
}
```

## Type Conversions

### Implicit Conversions
Some conversions happen automatically:

```dream
int intValue = 42;
float floatValue = intValue;    // int to float (automatic)

char ch = 'A';
int ascii = ch;                 // char to int (automatic)
```

### String Concatenation
String concatenation with automatic conversion:

```dream
int age = 25;
string name = "John";
string message = name + " is " + age + " years old";
// Result: "John is 25 years old"

float price = 19.99;
string priceTag = "Price: $" + price;
// Result: "Price: $19.99"
```

## Constants

While `const` is reserved for future use, you can create effectively constant variables:

```dream
// Convention: Use UPPER_CASE for constants
int MAX_USERS = 1000;
float PI = 3.14159;
string APP_NAME = "DreamApp";
```

## Arrays

### Array Declaration
```dream
// Fixed-size arrays
int numbers[5];              // Array of 5 integers
float temperatures[7];       // Array of 7 floats
string names[3];            // Array of 3 strings
```

### Array Initialization and Access
```dream
int scores[3];
scores[0] = 85;             // Set first element
scores[1] = 92;             // Set second element  
scores[2] = 78;             // Set third element

int firstScore = scores[0]; // Read first element
Console.WriteLine(firstScore); // Prints: 85
```

### Multi-dimensional Arrays
```dream
// 2D array (3x3 matrix)
int matrix[3][3];
matrix[0][0] = 1;
matrix[0][1] = 2;
matrix[1][0] = 3;

int value = matrix[0][1];   // Gets 2
```

## Advanced Variable Features

### Assignment Operators
```dream
int x = 10;

x += 5;     // x = x + 5  (now 15)
x -= 3;     // x = x - 3  (now 12)
x *= 2;     // x = x * 2  (now 24)
x /= 4;     // x = x / 4  (now 6)
x %= 5;     // x = x % 5  (now 1)
```

### Increment and Decrement
```dream
int count = 5;

count++;    // Post-increment: count becomes 6
++count;    // Pre-increment: count becomes 7
count--;    // Post-decrement: count becomes 6
--count;    // Pre-decrement: count becomes 5

// In expressions
int a = count++;    // a gets 5, count becomes 6
int b = ++count;    // count becomes 7, b gets 7
```

### Multiple Assignment
```dream
// Declare multiple variables of same type
int a, b, c;
a = b = c = 10;     // All variables get value 10

// Different types
int x = 1;
float y = 2.5;
bool z = true;
```

## Memory Management

Dream uses automatic reference counting for memory management:

```dream
// String memory is automatically managed
string text1 = "Hello";
string text2 = text1;      // Reference count increases
text1 = "World";            // Original "Hello" reference count decreases

// Arrays are also automatically managed
int numbers[100];           // Automatically allocated
// Automatically freed when out of scope
```

## Best Practices

### Naming Conventions
```dream
// Use camelCase for variables
int userAge = 25;
string firstName = "John";
bool isLoggedIn = true;

// Use UPPER_CASE for constants
int MAX_CONNECTIONS = 100;
string DEFAULT_PATH = "/home/user";

// Use descriptive names
int temperatureInCelsius = 25;    // Good
int t = 25;                       // Poor
```

### Initialization
```dream
// Always initialize variables before use
int count = 0;          // Good - explicit initialization
string name = "";       // Good - empty string
bool flag = false;      // Good - explicit false

// Avoid uninitialized variables
int x;                  // Dangerous - uninitialized
// int result = x + 5;  // Undefined behavior
```

### Type Safety
```dream
// Be explicit about types when clarity is important
float calculateAverage(int values[], int count) {
    int sum = 0;                    // Explicit int for accumulator
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    return (float)sum / count;      // Explicit cast to float
}
```

## Error Handling

### Common Type Errors
```dream
// These will cause compile-time errors:
// int x = "string";        // Type mismatch
// float y = true;          // Type mismatch  
// string z = 42;           // Type mismatch (no auto-conversion)

// Array bounds are not checked at compile time:
int arr[3];
arr[5] = 10;                // Runtime error (undefined behavior)
```

### Type Checking
```dream
func processValue(var value) {
    // Type of 'value' is inferred from usage
    if (value > 0) {            // Must be numeric
        Console.WriteLine("Positive: " + value);
    }
}

processValue(42);           // OK - int
processValue(3.14);         // OK - float  
// processValue("text");    // Error - string not numeric
```

This comprehensive type system provides type safety while maintaining developer productivity through features like type inference and automatic memory management.