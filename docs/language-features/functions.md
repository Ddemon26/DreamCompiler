# Functions

Functions in Dream are first-class constructs that encapsulate reusable code. They support type-safe parameters, return values, and local scope.

## Function Declaration Syntax

Functions are declared using the `func` keyword:

```dream
func functionName(parameters) {
    // function body
}

// With return type
func returnType functionName(parameters) {
    // function body
    return value;
}
```

## Basic Functions

### Functions with No Return Value
Functions without a return type specification return nothing (void):

```dream
func greet() {
    Console.WriteLine("Hello, World!");
}

func printSeparator() {
    Console.WriteLine("------------------------");
}

// Call the functions
greet();
printSeparator();
```

### Functions with Return Values
Specify the return type before the function name:

```dream
func int add(int a, int b) {
    return a + b;
}

func float calculateArea(float radius) {
    float pi = 3.14159;
    return pi * radius * radius;
}

func string getGreeting(string name) {
    return "Hello, " + name + "!";
}

// Using functions with return values
int sum = add(5, 3);                    // sum = 8
float area = calculateArea(2.5);        // area ≈ 19.63
string message = getGreeting("Alice");  // message = "Hello, Alice!"
```

## Checked primitive returns

Functions and instance/static methods returning `int`, `float`, `bool`, `char`, or
`string` require each explicit `return` to provide an expression of that exact type.
There is no implicit conversion at the return boundary: `func float weight() { return
1; }` is rejected; use `return 1.0;`. Arithmetic expressions keep their existing
promotion rules, and an `int`/`float` conditional expression has type `float`.
Other conditional branches must have matching types; both branches are checked.

A `void` function (including one with no written return type) accepts `return;` and
rejects `return expression;`, even when the expression calls another void function.
These checks run before C emission, including inside loops, switches, and
try/catch/finally blocks and in imported function modules. Dependency diagnostics
identify the dependency source and return line. Top-level entry-program returns
retain their existing behavior.

This checks explicit primitive returns, not whether every control-flow path returns.
Pointer/array shape checking and broader operator validation remain separate work;
async and non-primitive return contracts are outside this supported subset.

## Parameters

### Declaration syntax and recovery

The supported scalar parameter types are `int`, `float`, `bool`, `char`, and
`string`. Custom-type parameters such as `func init(Child child) {}` are not
supported. They produce an `expected parameter type` error and an explanatory
hint before generated artifacts, in ordinary sources, module entries and imported
modules. Malformed parameter names, separators and closing parentheses also fail
with source positions. Recovery discards the invalid signature and its balanced
body so parsing can continue at the following declaration.

One primitive pointer marker is preserved in emitted parameter signatures; see
the bounded pointer contract below. Pointer argument compatibility remains
unqualified. Array parameter declarations (`int items[]`) are unsupported.
The parser recovery checks do not qualify arbitrary malformed input or implement
these additional parameter types.

### Checked primitive method arguments

Calls such as `reader.take(value)`, `Reader.take(value)`, `this.take(value)`, and
`base.take(value)` check each primitive scalar parameter (`int`, `float`, `bool`,
`char`, `string`) against the analyzed expression type before generating C. As with
free-function arguments and primitive returns, types must match exactly: an `int`
argument does not implicitly widen to a `float` parameter, and `bool`/`char` remain
distinct from `int`. Existing expression promotions still apply; `2 + 0.5` has type
`float`. A void result or `null` does not match a scalar parameter.

Resolution follows the receiver's declared type and inherited members, including
static methods reached through a simple object variable. Rejections identify the
method, one-based argument index, expected/actual types, and call position. These
checks also apply to classes in multi-file entry sources; rejected input produces
no new output artifacts. All supplied arguments are analyzed even on arity errors.

Pointer parameters are outside this rule. Array/pointer expression shapes are not
represented by the scalar type token and remain unqualified, even when their base
token matches a scalar parameter. Custom types and broader operator validation
remain separate work.
Explicit `object.init(...)` calls use the checked method path.

### Checked constructor arguments

`new Reader(value)` checks arity against the class's own instance `init` method
and checks its primitive scalar parameters with the same exact-type policy as
method calls. Diagnostics name the constructed type, argument index and expected/
actual types, at the `new` expression before output artifacts are created. Every
supplied argument is analyzed once, including excess arguments and nested calls.
These checks also apply to classes in multi-file entry sources.

Construction invokes only the class's own `init`. A derived class without one
takes zero arguments; it does not invoke an inherited `init`. Declare a local
constructor and call `base.init(...)` explicitly when needed. A class without
`init` and a zero-initialized struct also take zero arguments. Struct `init`
methods are not invoked by `new`. Arguments in these cases are errors rather
than silently ignored. A local static `init` cannot serve as a constructor;
call that static method explicitly instead.

Pointer/array parameter shapes and custom-type compatibility remain unqualified,
as with method arguments. Class field declaration initializers remain unresolved.

### Primitive pointer parameter emission

`int*`, `float*`, `bool*`, `char*`, and `string*` parameters retain their pointer
marker in free-function prototypes and definitions, static/instance methods, and
instance `init` methods. `string*` maps to C `const char **`, while `bool*` maps to
`int*`. This works in ordinary and multi-file entry sources, including inherited
methods, `this`/`base` forwarding, local construction and explicit `init` calls.

The regression borrows live primitive arrays and an `int*` alias, reads/writes
their elements and checks exact output. Callers must supply compatible storage
and keep it alive for the entire call. This adds no shape, lifetime, ownership,
bounds or null-dereference checking. Address-of and unary dereference, custom-type
parameters, multiple pointer levels, pointer returns and async paths are not
qualified by these checks.

Direct `Console.Write(p[i])` and `Console.WriteLine(p[i])` select the primitive
element format, including `%f` for float and `%c` for char parameters. String
array/pointer elements print as strings; indexing a scalar string prints a
character. This also applies to local arrays and inherited member storage in
ordinary and module-entry programs. It adds no semantic shape or bounds checks.
Scalar `string text = values[i]` declarations copy indexed storage into
their own managed allocation before normal block cleanup. See the bounded
[string ownership checkpoint](../development/string-ownership.md); other string
ownership paths and general pointer safety remain unqualified.

### Single Parameters
```dream
func void printNumber(int num) {
    Console.WriteLine("Number: " + num);
}

func string makeUppercase(string text) {
    // Note: String manipulation functions would be built-in
    return text.toUpperCase(); // Conceptual - not implemented yet
}
```

### Multiple Parameters
```dream
func int multiply(int x, int y) {
    return x * y;
}

func string formatName(string first, string last) {
    return last + ", " + first;
}

func bool isInRange(int value, int min, int max) {
    return value >= min && value <= max;
}

// Function calls
int product = multiply(7, 6);                    // 42
string formatted = formatName("John", "Doe");    // "Doe, John"
bool inRange = isInRange(15, 10, 20);           // true
```

### Different Parameter Types
```dream
func string formatUser(string name, int age, bool isActive) {
    string status = isActive ? "active" : "inactive";
    return name + " (age " + age + ") - " + status;
}

func float calculateBMI(float weight, float height) {
    return weight / (height * height);
}

// Usage
string userInfo = formatUser("Alice", 30, true);
// Result: "Alice (age 30) - active"

float bmi = calculateBMI(70.0, 1.75);  // BMI calculation
```

## Advanced Function Features

### Function Overloading (Not Yet Implemented)
Currently, function names must be unique. Future versions may support overloading:

```dream
// This is planned for future versions:
// func int add(int a, int b) { return a + b; }
// func float add(float a, float b) { return a + b; }

// Current workaround - use descriptive names:
func int addIntegers(int a, int b) { return a + b; }
func float addFloats(float a, float b) { return a + b; }
```

### Recursive Functions
Functions can call themselves:

```dream
func int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

func int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// Usage
int fact5 = factorial(5);    // 120
int fib8 = fibonacci(8);     // 21
```

### Functions with Arrays
Array parameters and returns are future syntax. The following examples are
illustrative and do not compile with the current parameter grammar:

```dream
func int sum(int numbers[], int length) {
    int total = 0;
    for (int i = 0; i < length; i++) {
        total += numbers[i];
    }
    return total;
}

func float average(int numbers[], int length) {
    int total = sum(numbers, length);
    return (float)total / length;
}

// Usage
int scores[5];
scores[0] = 85; scores[1] = 92; scores[2] = 78; scores[3] = 95; scores[4] = 88;

int totalScore = sum(scores, 5);        // 438
float avgScore = average(scores, 5);    // 87.6
```

## Variable Scope in Functions

### Local Variables
Variables declared inside functions are local:

```dream
func int calculate() {
    int localVar = 10;      // Local to this function
    int result = localVar * 2;
    return result;
}

func void example() {
    int value = calculate(); // value = 20
    // localVar is not accessible here
}
```

### Parameter Scope
Parameters are local to the function:

```dream
func int processValue(int input) {
    input = input * 2;      // Modifies local copy
    return input;
}

func void testScope() {
    int original = 5;
    int doubled = processValue(original);
    // original is still 5 (pass by value)
    // doubled is 10
}
```

### Global Variable Access
Functions can access global variables:

```dream
int globalCounter = 0;

func void incrementCounter() {
    globalCounter++;        // Modifies global variable
}

func int getCounter() {
    return globalCounter;   // Reads global variable
}
```

## Function Examples

### Mathematical Functions
```dream
func float power(float base, int exponent) {
    if (exponent == 0) return 1.0;
    if (exponent == 1) return base;
    
    float result = 1.0;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}

func int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

func bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}
```

### String Processing Functions
```dream
func int stringLength(string str) {
    // This would use built-in string length functionality
    // Simplified example
    int length = 0;
    // Implementation would count characters
    return length;
}

func string concatenateThree(string a, string b, string c) {
    return a + b + c;
}

func string repeat(string str, int times) {
    string result = "";
    for (int i = 0; i < times; i++) {
        result = result + str;
    }
    return result;
}

// Usage
string repeated = repeat("Hi! ", 3);  // "Hi! Hi! Hi! "
string combined = concatenateThree("Hello", " ", "World");  // "Hello World"
```

### Utility Functions
```dream
func int min(int a, int b) {
    return a < b ? a : b;
}

func int max(int a, int b) {
    return a > b ? a : b;
}

func int clamp(int value, int minVal, int maxVal) {
    return min(max(value, minVal), maxVal);
}

func bool isEven(int num) {
    return num % 2 == 0;
}

func bool isOdd(int num) {
    return !isEven(num);
}

// Usage examples
int smaller = min(10, 5);           // 5
int clamped = clamp(15, 0, 10);     // 10
bool even = isEven(8);              // true
```

## Input/Output Functions

### Console Interaction
```dream
func string getUserName() {
    Console.Write("Enter your name: ");
    return Console.ReadLine();
}

func int getUserAge() {
    Console.Write("Enter your age: ");
    string input = Console.ReadLine();
    // Note: String to int conversion would need built-in support
    return parseInteger(input);  // Conceptual function
}

func void displayUserInfo(string name, int age) {
    Console.WriteLine("User Information:");
    Console.WriteLine("Name: " + name);
    Console.WriteLine("Age: " + age);
    
    if (age >= 18) {
        Console.WriteLine("Status: Adult");
    } else {
        Console.WriteLine("Status: Minor");
    }
}

// Interactive program
func void runUserProgram() {
    string name = getUserName();
    int age = getUserAge();
    displayUserInfo(name, age);
}
```

## Error Handling in Functions

### Return Value Validation
```dream
func bool divide(int dividend, int divisor, int* result) {
    if (divisor == 0) {
        return false;  // Indicate error
    }
    *result = dividend / divisor;  // Note: Pointer syntax conceptual
    return true;      // Indicate success
}

// Alternative approach using exceptions
func float safeDivide(float a, float b) {
    if (b == 0.0) {
        throw "Division by zero error";
    }
    return a / b;
}
```

### Validation Functions
```dream
func bool isValidAge(int age) {
    return age >= 0 && age <= 150;
}

func bool isValidEmail(string email) {
    // Simplified email validation
    return email.length > 5 && email.contains("@");  // Conceptual
}

func string validateInput(string input) {
    if (input == "") {
        throw "Input cannot be empty";
    }
    return input;
}
```

## Best Practices

### Function Naming
```dream
// Use verb-noun or descriptive names
func int calculateSum(int a, int b) { /* ... */ }      // Good
func bool isPositive(int num) { /* ... */ }            // Good
func string formatCurrency(float amount) { /* ... */ }  // Good

func int calc(int a, int b) { /* ... */ }              // Poor - unclear
func bool check(int num) { /* ... */ }                 // Poor - vague
```

### Single Responsibility
```dream
// Good - single responsibility
func float calculateCircleArea(float radius) {
    return 3.14159 * radius * radius;
}

func void printCircleArea(float area) {
    Console.WriteLine("Area: " + area);
}

// Poor - multiple responsibilities  
func void calculateAndPrintArea(float radius) {
    float area = 3.14159 * radius * radius;
    Console.WriteLine("Area: " + area);
    // Harder to reuse and test
}
```

### Parameter Validation
```dream
func float calculatePercentage(int part, int whole) {
    if (whole == 0) {
        throw "Cannot calculate percentage: whole cannot be zero";
    }
    if (part < 0 || whole < 0) {
        throw "Cannot calculate percentage: values must be non-negative";
    }
    return ((float)part / whole) * 100.0;
}
```

### Return Early Pattern
```dream
func string processText(string input) {
    if (input == "") {
        return "";  // Early return for edge case
    }
    
    if (input.length == 1) {
        return input.toUpperCase();  // Early return for simple case
    }
    
    // Main processing logic
    string result = input.trim();
    result = result.toLowerCase();
    return result;
}
```

Functions are essential building blocks that promote code reuse, maintainability, and testing. They provide clear interfaces and encapsulation while supporting Dream's type safety features.
