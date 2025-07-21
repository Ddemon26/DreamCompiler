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

## Parameters

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
Functions can accept and return arrays:

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