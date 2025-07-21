# Operators

Dream provides a comprehensive set of operators for manipulating data, performing calculations, and controlling program logic. This section covers all operators organized by category and usage patterns.

## Overview

Operators in Dream are symbols or keywords that perform operations on operands (variables, literals, or expressions). They are categorized by:

- **Functionality** - What they do (arithmetic, logical, etc.)
- **Precedence** - Order of evaluation in expressions
- **Associativity** - Left-to-right or right-to-left evaluation
- **Operand Count** - Unary, binary, or ternary

## Operator Categories

### 🔢 Arithmetic Operators
Perform mathematical calculations on numeric types.

| Operator | Name | Example | Result |
|----------|------|---------|--------|
| `+` | Addition | `5 + 3` | `8` |
| `-` | Subtraction | `5 - 3` | `2` |
| `*` | Multiplication | `5 * 3` | `15` |
| `/` | Division | `10 / 3` | `3` (integer), `3.333...` (float) |
| `%` | Modulo | `10 % 3` | `1` |
| `-` | Unary Minus | `-5` | `-5` |
| `+` | Unary Plus | `+5` | `5` |

```dream
int a = 10, b = 3;
int sum = a + b;        // 13
int difference = a - b; // 7
int product = a * b;    // 30
int quotient = a / b;   // 3 (integer division)
int remainder = a % b;  // 1

float x = 10.0, y = 3.0;
float result = x / y;   // 3.333...
```

**📖 [Complete Arithmetic Guide](v1.1/arithmetic.md)**

### 📊 Comparison Operators
Compare values and return boolean results.

| Operator | Name | Example | Description |
|----------|------|---------|-------------|
| `==` | Equal | `a == b` | True if values are equal |
| `!=` | Not Equal | `a != b` | True if values are different |
| `<` | Less Than | `a < b` | True if a is less than b |
| `<=` | Less Equal | `a <= b` | True if a is less than or equal to b |
| `>` | Greater Than | `a > b` | True if a is greater than b |
| `>=` | Greater Equal | `a >= b` | True if a is greater than or equal to b |

```dream
int score = 85;
bool passed = score >= 60;        // true
bool perfect = score == 100;      // false
bool needsImprovement = score < 70; // false

string name1 = "Alice";
string name2 = "Bob";
bool sameNames = name1 == name2;   // false
bool differentNames = name1 != name2; // true
```

**📖 [Complete Comparison Guide](v1.1/comparison.md)**

### 🔗 Logical Operators
Perform boolean logic operations.

| Operator | Name | Example | Description |
|----------|------|---------|-------------|
| `&&` | Logical AND | `a && b` | True if both operands are true |
| `\|\|` | Logical OR | `a \|\| b` | True if either operand is true |
| `!` | Logical NOT | `!a` | True if operand is false |

```dream
bool isAdult = age >= 18;
bool hasLicense = true;
bool canDrive = isAdult && hasLicense;  // Both must be true

bool isWeekend = day == "Saturday" || day == "Sunday";
bool isWorking = !isWeekend;  // Inverse of isWeekend

// Short-circuit evaluation
bool result = false && expensiveFunction();  // expensiveFunction() not called
```

**📖 [Complete Logical Guide](v1.1/logical.md)**

### ⚡ Bitwise Operators
Manipulate individual bits for low-level programming.

| Operator | Name | Example | Description |
|----------|------|---------|-------------|
| `&` | Bitwise AND | `a & b` | Sets bit if both bits are 1 |
| `\|` | Bitwise OR | `a \| b` | Sets bit if either bit is 1 |
| `^` | Bitwise XOR | `a ^ b` | Sets bit if bits are different |
| `~` | Bitwise NOT | `~a` | Inverts all bits |
| `<<` | Left Shift | `a << n` | Shifts bits left by n positions |
| `>>` | Right Shift | `a >> n` | Shifts bits right by n positions |

```dream
int a = 12;     // Binary: 1100
int b = 10;     // Binary: 1010

int andResult = a & b;   // Binary: 1000 = 8
int orResult = a | b;    // Binary: 1110 = 14
int xorResult = a ^ b;   // Binary: 0110 = 6
int notResult = ~a;      // Binary: 0011 = -13 (two's complement)

int shifted = a << 2;    // Binary: 110000 = 48 (multiply by 4)
int unshifted = a >> 1;  // Binary: 110 = 6 (divide by 2)
```

**📖 [Complete Bitwise Guide](v1.1/bitwise.md)**

### ❓ Conditional Operator
Provide inline conditional expressions (ternary operator).

```dream
// Syntax: condition ? valueIfTrue : valueIfFalse
int age = 20;
string status = age >= 18 ? "adult" : "minor";

float temperature = 25.5;
string weather = temperature > 30.0 ? "hot" : 
                 temperature > 20.0 ? "warm" : "cool";

int max = a > b ? a : b;  // Get maximum of two values
```

**📖 [Complete Conditional Guide](v1.1/conditional.md)**

## Assignment Operators

### Basic Assignment

```dream
int x = 10;         // Initialize
x = 20;             // Assign new value
```

### Compound Assignment
Combine operations with assignment for concise code.

| Operator | Equivalent | Example |
|----------|------------|---------|
| `+=` | `x = x + y` | `x += 5` |
| `-=` | `x = x - y` | `x -= 3` |
| `*=` | `x = x * y` | `x *= 2` |
| `/=` | `x = x / y` | `x /= 4` |
| `%=` | `x = x % y` | `x %= 3` |
| `&=` | `x = x & y` | `x &= mask` |
| `\|=` | `x = x \| y` | `x \|= flag` |
| `^=` | `x = x ^ y` | `x ^= toggle` |
| `<<=` | `x = x << y` | `x <<= 2` |
| `>>=` | `x = x >> y` | `x >>= 1` |

```dream
int score = 100;
score += 50;    // score is now 150
score -= 25;    // score is now 125
score *= 2;     // score is now 250
score /= 5;     // score is now 50

// Bitwise compound assignment
int flags = 0b00001010;
flags |= 0b00000101;   // Set additional bits
flags &= 0b11110000;   // Clear lower 4 bits
flags ^= 0b11111111;   // Toggle all bits
```

### Increment and Decrement

```dream
int counter = 5;

// Post-increment/decrement (returns old value, then modifies)
int a = counter++;  // a = 5, counter = 6
int b = counter--;  // b = 6, counter = 5

// Pre-increment/decrement (modifies first, then returns new value)
int c = ++counter;  // counter = 6, c = 6
int d = --counter;  // counter = 5, d = 5

// Common usage in loops
for (int i = 0; i < 10; i++) {    // Post-increment
    Console.WriteLine(i);
}

for (int j = 10; j > 0; --j) {    // Pre-decrement
    Console.WriteLine(j);
}
```

## Operator Precedence

Understanding precedence helps write correct expressions without excessive parentheses:

| Precedence | Operators | Associativity | Description |
|------------|-----------|---------------|-------------|
| 1 (Highest) | `()`, `[]`, `.` | Left-to-right | Grouping, indexing, member access |
| 2 | `!`, `~`, `-`, `+`, `++`, `--` | Right-to-left | Unary operators |
| 3 | `*`, `/`, `%` | Left-to-right | Multiplicative |
| 4 | `+`, `-` | Left-to-right | Additive |
| 5 | `<<`, `>>` | Left-to-right | Shift |
| 6 | `<`, `<=`, `>`, `>=` | Left-to-right | Relational |
| 7 | `==`, `!=` | Left-to-right | Equality |
| 8 | `&` | Left-to-right | Bitwise AND |
| 9 | `^` | Left-to-right | Bitwise XOR |
| 10 | `\|` | Left-to-right | Bitwise OR |
| 11 | `&&` | Left-to-right | Logical AND |
| 12 | `\|\|` | Left-to-right | Logical OR |
| 13 | `? :` | Right-to-left | Conditional |
| 14 (Lowest) | `=`, `+=`, `-=`, etc. | Right-to-left | Assignment |

### Precedence Examples

```dream
// Without parentheses - using precedence rules
int result = 2 + 3 * 4;         // 14 (not 20), multiplication first
bool test = 5 > 3 && 2 < 4;     // true, comparisons before logical AND
int value = 10 + 5 > 8 ? 1 : 0; // 1, addition before comparison before conditional

// With parentheses for clarity
int result2 = (2 + 3) * 4;      // 20, addition first
bool test2 = (5 > 3) && (2 < 4); // true, explicit grouping
int value2 = (10 + 5) > 8 ? 1 : 0; // 1, explicit grouping
```

## String Operations

Special operator behavior with strings:

```dream
// String concatenation with +
string first = "Hello";
string second = "World";
string combined = first + " " + second;  // "Hello World"

// Automatic type conversion in concatenation
int age = 25;
string message = "Age: " + age;  // "Age: 25"

float price = 19.99;
string priceTag = "Price: $" + price;  // "Price: $19.99"

// String comparison
string name1 = "Alice";
string name2 = "Alice";
bool same = name1 == name2;  // true

// String concatenation is left-associative
string result = "Value: " + 10 + 20;  // "Value: 1020" (not "Value: 30")
string result2 = "Value: " + (10 + 20); // "Value: 30" (parentheses force addition)
```

## Practical Examples

### Calculator Functions

```dream
// Using various operators in calculations
func float calculateTax(float amount, float rate) {
    return amount * (rate / 100.0);
}

func int roundToNearest(float value) {
    return (int)(value + 0.5);  // Simple rounding
}

func bool isEven(int number) {
    return (number & 1) == 0;   // Bitwise check for even numbers
}

func int clamp(int value, int min, int max) {
    return value < min ? min : (value > max ? max : value);
}
```

### Data Processing

```dream
// Working with flags using bitwise operators
int PERMISSION_READ = 0b001;    // 1
int PERMISSION_WRITE = 0b010;   // 2
int PERMISSION_EXECUTE = 0b100; // 4

func bool hasPermission(int userPerms, int checkPerm) {
    return (userPerms & checkPerm) != 0;
}

func int grantPermission(int userPerms, int newPerm) {
    return userPerms | newPerm;
}

func int revokePermission(int userPerms, int removePerm) {
    return userPerms & ~removePerm;
}

// Usage
int userPermissions = PERMISSION_READ | PERMISSION_WRITE;  // 3
bool canRead = hasPermission(userPermissions, PERMISSION_READ);    // true
bool canExecute = hasPermission(userPermissions, PERMISSION_EXECUTE); // false
userPermissions = grantPermission(userPermissions, PERMISSION_EXECUTE); // 7
```

### Conditional Logic

```dream
// Complex conditional expressions
func string determineGrade(int score) {
    return score >= 90 ? "A" :
           score >= 80 ? "B" :
           score >= 70 ? "C" :
           score >= 60 ? "D" : "F";
}

func bool isValidDate(int day, int month, int year) {
    return day >= 1 && day <= 31 &&
           month >= 1 && month <= 12 &&
           year >= 1900 && year <= 2100;
}

func int getMaxOfThree(int a, int b, int c) {
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}
```

## Best Practices

### 1. Use Parentheses for Clarity

```dream
// Good - clear intent with parentheses
bool isValidRange = (value >= min) && (value <= max);
int calculation = (a + b) * (c - d);

// Acceptable - simple expressions
int sum = a + b + c;
bool isPositive = value > 0;
```

### 2. Prefer Readable Over Clever

```dream
// Good - clear and readable
bool isWeekend = (day == "Saturday") || (day == "Sunday");

// Poor - unnecessarily complex
bool isWeekend = !!(day == "Saturday" | day == "Sunday");  // Don't do this
```

### 3. Use Appropriate Operators

```dream
// Good - use logical operators for booleans
bool result = isActive && isValid;

// Poor - using bitwise operators for booleans
bool result = isActive & isValid;  // Works but misleading

// Good - use bitwise operators for flags
int flags = FLAG_A | FLAG_B | FLAG_C;

// Poor - using logical operators for flags
bool flags = FLAG_A || FLAG_B || FLAG_C;  // Wrong type and logic
```

### 4. Be Careful with Floating-Point Comparisons

```dream
// Poor - direct floating-point comparison
float a = 0.1 + 0.2;
bool equal = (a == 0.3);  // May be false due to precision

// Good - use epsilon for floating-point comparison
func bool floatEquals(float a, float b, float epsilon) {
    float diff = a - b;
    return (diff < 0 ? -diff : diff) < epsilon;
}

bool equal = floatEquals(a, 0.3, 0.0001);
```

## Performance Tips

### 1. Use Appropriate Operations

```dream
// Fast - bitwise operations for powers of 2
int doubled = value << 1;        // Multiply by 2
int halved = value >> 1;         // Divide by 2 (positive numbers)
bool isEven = (value & 1) == 0;  // Check if even

// Fast - avoid expensive operations in loops
for (int i = 0; i < array.length; i++) {  // Cache length if not constant
    // Process array[i]
}
```

### 2. Leverage Short-Circuit Evaluation

```dream
// Efficient - cheap condition first
if (isEnabled && expensiveCheck()) {
    // expensiveCheck() only called if isEnabled is true
}

// Efficient - null check before member access
if (object != null && object.isValid()) {
    // Prevents null reference errors
}
```

## Next Steps

After mastering operators, explore these related topics:

1. **[Control Flow](control-flow.md)** - Use operators in conditional statements and loops
2. **[Functions](functions-overview.md)** - Create functions that use operators effectively
3. **[Type System](type-system.md)** - Understand how operators work with different types
4. **[Expressions](expressions.md)** - Build complex expressions using multiple operators

Operators are the building blocks for creating expressions and controlling program logic. Understanding their behavior, precedence, and appropriate usage is essential for writing effective Dream programs.