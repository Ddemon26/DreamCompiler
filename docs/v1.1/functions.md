# Functions (v1.1.12)

Functions in Dream are first-class constructs that encapsulate reusable code. They support type-safe parameters, return values, and local scope with comprehensive features for professional development.

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

func showMenu() {
    Console.WriteLine("1. New Game");
    Console.WriteLine("2. Load Game");
    Console.WriteLine("3. Exit");
}

// Call the functions
greet();
printSeparator();
showMenu();
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

func bool isEven(int number) {
    return number % 2 == 0;
}

// Using functions with return values
int sum = add(5, 3);                    // sum = 8
float area = calculateArea(2.5);        // area ≈ 19.63
string message = getGreeting("Alice");  // message = "Hello, Alice!"
bool even = isEven(10);                 // even = true
```

## Parameters

### Single Parameters
```dream
func void printNumber(int num) {
    Console.WriteLine("Number: " + num);
}

func string formatCurrency(float amount) {
    return "$" + amount;
}

func char toUpperCase(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;  // Convert to uppercase
    }
    return c;
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

func float calculateDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);  // Conceptual sqrt function
}

// Function calls
int product = multiply(7, 6);                    // 42
string formatted = formatName("John", "Doe");    // "Doe, John"
bool inRange = isInRange(15, 10, 20);           // true
float distance = calculateDistance(0.0, 0.0, 3.0, 4.0); // 5.0
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

func void logMessage(string level, string message, bool includeTimestamp) {
    if (includeTimestamp) {
        Console.WriteLine("[" + getCurrentTime() + "] " + level + ": " + message);
    } else {
        Console.WriteLine(level + ": " + message);
    }
}

// Usage
string userInfo = formatUser("Alice", 30, true);
// Result: "Alice (age 30) - active"

float bmi = calculateBMI(70.0, 1.75);  // BMI calculation
logMessage("INFO", "Application started", true);
```

## Advanced Function Features

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

func int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

// Usage
int fact5 = factorial(5);    // 120
int fib8 = fibonacci(8);     // 21
int greatestCommon = gcd(48, 18); // 6
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

func int findMax(int numbers[], int length) {
    if (length == 0) return 0;
    
    int max = numbers[0];
    for (int i = 1; i < length; i++) {
        if (numbers[i] > max) {
            max = numbers[i];
        }
    }
    return max;
}

func void sortArray(int numbers[], int length) {
    // Simple bubble sort
    for (int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (numbers[j] > numbers[j + 1]) {
                int temp = numbers[j];
                numbers[j] = numbers[j + 1];
                numbers[j + 1] = temp;
            }
        }
    }
}

// Usage
int scores[5];
scores[0] = 85; scores[1] = 92; scores[2] = 78; scores[3] = 95; scores[4] = 88;

int totalScore = sum(scores, 5);        // 438
float avgScore = average(scores, 5);    // 87.6
int maxScore = findMax(scores, 5);      // 95
sortArray(scores, 5);                   // Sorts the array in-place
```

## Variable Scope in Functions

### Local Variables
Variables declared inside functions are local:

```dream
func int calculate() {
    int localVar = 10;      // Local to this function
    int result = localVar * 2;
    int temp = result + 5;  // Also local
    return result;
}

func void example() {
    int value = calculate(); // value = 20
    // localVar, result, and temp are not accessible here
}
```

### Parameter Scope
Parameters are local to the function:

```dream
func int processValue(int input) {
    input = input * 2;      // Modifies local copy
    int processed = input + 10;
    return processed;
}

func void testScope() {
    int original = 5;
    int doubled = processValue(original);
    // original is still 5 (pass by value)
    // doubled is 20 (5 * 2 + 10)
    Console.WriteLine("Original: " + original); // 5
    Console.WriteLine("Processed: " + doubled); // 20
}
```

### Global Variable Access
Functions can access global variables:

```dream
int globalCounter = 0;
string appName = "DreamApp";

func void incrementCounter() {
    globalCounter++;        // Modifies global variable
}

func int getCounter() {
    return globalCounter;   // Reads global variable
}

func string getAppInfo() {
    return appName + " v1.0 - Counter: " + globalCounter;
}
```

## Mathematical Functions

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

func bool isPrime(int num) {
    if (num < 2) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    
    for (int i = 3; i * i <= num; i += 2) {
        if (num % i == 0) return false;
    }
    return true;
}

func int abs(int value) {
    return value < 0 ? -value : value;
}

func float min(float a, float b) {
    return a < b ? a : b;
}

func float max(float a, float b) {
    return a > b ? a : b;
}

func float clamp(float value, float minVal, float maxVal) {
    return min(max(value, minVal), maxVal);
}
```

## String Processing Functions

```dream
func int stringLength(string str) {
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

func bool startsWith(string text, string prefix) {
    // Simplified implementation
    return true; // Would check if text starts with prefix
}

func string trim(string text) {
    // Would remove leading/trailing whitespace
    return text;
}

// Usage
string repeated = repeat("Hi! ", 3);  // "Hi! Hi! Hi! "
string combined = concatenateThree("Hello", " ", "World");  // "Hello World"
bool hasPrefix = startsWith("Hello World", "Hello"); // true
```

## Input/Output Functions

```dream
func string getUserName() {
    Console.Write("Enter your name: ");
    return Console.ReadLine();
}

func int getUserAge() {
    Console.Write("Enter your age: ");
    string input = Console.ReadLine();
    return parseInt(input);  // Conceptual parsing function
}

func void displayUserInfo(string name, int age) {
    Console.WriteLine("=== User Information ===");
    Console.WriteLine("Name: " + name);
    Console.WriteLine("Age: " + age);
    
    if (age >= 18) {
        Console.WriteLine("Status: Adult");
    } else {
        Console.WriteLine("Status: Minor");
    }
    Console.WriteLine("========================");
}

func bool confirmAction(string action) {
    Console.Write("Are you sure you want to " + action + "? (y/n): ");
    string response = Console.ReadLine();
    return response == "y" || response == "Y" || response == "yes";
}

// Interactive program
func void runUserProgram() {
    string name = getUserName();
    int age = getUserAge();
    displayUserInfo(name, age);
    
    if (confirmAction("continue")) {
        Console.WriteLine("Continuing...");
    } else {
        Console.WriteLine("Cancelled.");
    }
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

func int safeParseInt(string input) {
    if (input == "") {
        throw "Input cannot be empty";
    }
    // Would parse string to integer
    return 0; // Placeholder
}
```

### Validation Functions
```dream
func bool isValidAge(int age) {
    return age >= 0 && age <= 150;
}

func bool isValidEmail(string email) {
    // Simplified email validation
    if (email == "") return false;
    // Would check for @ symbol and proper format
    return true; // Placeholder
}

func void validateUserInput(string name, int age, string email) {
    if (name == "") {
        throw "Name cannot be empty";
    }
    if (!isValidAge(age)) {
        throw "Age must be between 0 and 150";
    }
    if (!isValidEmail(email)) {
        throw "Invalid email format";
    }
}
```

## Best Practices

### Function Naming
```dream
// Use verb-noun or descriptive names
func int calculateSum(int a, int b) { return a + b; }      // Good
func bool isPositive(int num) { return num > 0; }         // Good
func string formatCurrency(float amount) { return "$" + amount; } // Good

func int calc(int a, int b) { return a + b; }             // Poor - unclear
func bool check(int num) { return num > 0; }              // Poor - vague
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

func int arraySum(int numbers[], int length) {
    if (length < 0) {
        throw "Array length cannot be negative";
    }
    if (length == 0) {
        return 0; // Valid case
    }
    
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += numbers[i];
    }
    return sum;
}
```

### Return Early Pattern
```dream
func string processText(string input) {
    if (input == "") {
        return "";  // Early return for edge case
    }
    
    if (stringLength(input) == 1) {
        return toUpperCase(input);  // Early return for simple case
    }
    
    // Main processing logic
    string result = trim(input);
    // Additional processing...
    return result;
}

func int findElement(int array[], int length, int target) {
    if (length <= 0) {
        return -1;  // Early return for invalid input
    }
    
    for (int i = 0; i < length; i++) {
        if (array[i] == target) {
            return i;  // Early return when found
        }
    }
    
    return -1;  // Not found
}
```

Functions are essential building blocks that promote code reuse, maintainability, and testing. They provide clear interfaces and encapsulation while supporting Dream's type safety features for professional software development.
