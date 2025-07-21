# Functions Overview

Functions are the fundamental building blocks of Dream programs. They encapsulate reusable code, enable modular design, and provide the foundation for organized, maintainable software development.

## Overview

Functions in Dream provide:
- **Code Reusability** - Write once, use many times
- **Modularity** - Break complex problems into smaller pieces
- **Abstraction** - Hide implementation details behind clear interfaces
- **Type Safety** - Compile-time verification of parameter and return types
- **Scope Management** - Isolated variable namespaces

## Function Concepts

### What is a Function?

A function is a named block of code that:
1. **Takes input** through parameters (optional)
2. **Performs operations** on that input
3. **Returns output** as a result (optional)
4. **Can be called** from other parts of your program

```dream
// Function anatomy
func returnType functionName(parameterType parameterName) {
    // Function body - the code that runs
    return value;  // Optional return statement
}

// Example function
func int add(int a, int b) {
    int result = a + b;
    return result;
}

// Function call
int sum = add(5, 3);  // sum = 8
```

## Function Categories

### 🔧 By Return Type

#### Void Functions
Functions that perform actions but don't return values.

```dream
// Void function (no return type specified)
func greetUser(string name) {
    Console.WriteLine("Hello, " + name + "!");
    Console.WriteLine("Welcome to Dream!");
}

// Usage - call for side effects
greetUser("Alice");  // Prints greeting messages
```

#### Value-Returning Functions
Functions that compute and return results.

```dream
// Functions with return values
func int multiply(int x, int y) {
    return x * y;
}

func float calculateArea(float radius) {
    return 3.14159 * radius * radius;
}

func bool isEven(int number) {
    return number % 2 == 0;
}

func string formatName(string first, string last) {
    return last + ", " + first;
}

// Usage - use returned values
int product = multiply(6, 7);           // 42
float area = calculateArea(5.0);        // 78.54
bool even = isEven(10);                 // true
string formatted = formatName("John", "Doe"); // "Doe, John"
```

### 🎯 By Purpose

#### Utility Functions
Perform common operations and calculations.

```dream
// Mathematical utilities
func int abs(int value) {
    return value < 0 ? -value : value;
}

func float max(float a, float b) {
    return a > b ? a : b;
}

func int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// String utilities
func int stringLength(string text) {
    // Implementation would count characters
    return 0; // Placeholder
}

func string toUpperCase(string text) {
    // Implementation would convert to uppercase
    return text; // Placeholder
}
```

#### Business Logic Functions
Implement specific application requirements.

```dream
// E-commerce functions
func float calculateTax(float price, float taxRate) {
    return price * (taxRate / 100.0);
}

func float applyDiscount(float price, float discountPercent) {
    float discount = price * (discountPercent / 100.0);
    return price - discount;
}

func bool isEligibleForFreeShipping(float orderTotal) {
    return orderTotal >= 50.0;
}

// User management functions
func bool isValidEmail(string email) {
    // Simplified validation
    return email.length > 5 && email.contains("@");
}

func string generateUserId(string firstName, string lastName) {
    return firstName.toLowerCase() + "." + lastName.toLowerCase();
}
```

#### Data Processing Functions
Transform and analyze data.

```dream
// Array processing
func int findMax(int array[], int length) {
    if (length <= 0) return 0;
    
    int max = array[0];
    for (int i = 1; i < length; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

func float calculateAverage(int numbers[], int count) {
    if (count <= 0) return 0.0;
    
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += numbers[i];
    }
    return (float)sum / count;
}

func bool contains(int array[], int length, int target) {
    for (int i = 0; i < length; i++) {
        if (array[i] == target) {
            return true;
        }
    }
    return false;
}
```

## Function Design Principles

### 1. Single Responsibility

Each function should do one thing well:

```dream
// Good - single responsibility
func float calculateCircleArea(float radius) {
    return 3.14159 * radius * radius;
}

func void printCircleArea(float area) {
    Console.WriteLine("Circle area: " + area);
}

// Poor - multiple responsibilities
func void calculateAndPrintCircleArea(float radius) {
    float area = 3.14159 * radius * radius;  // Calculation
    Console.WriteLine("Circle area: " + area); // Output
    // Harder to reuse calculation separately
}
```

### 2. Clear Interfaces

Use descriptive names and appropriate parameter types:

```dream
// Good - clear function interfaces
func bool isValidAge(int age) {
    return age >= 0 && age <= 150;
}

func string formatCurrency(float amount, string currency) {
    return currency + amount;
}

func int countOccurrences(string text, char target) {
    int count = 0;
    for (int i = 0; i < text.length; i++) {
        if (text[i] == target) {
            count++;
        }
    }
    return count;
}

// Poor - unclear interfaces
func bool check(int x) {           // What does it check?
    return x >= 0 && x <= 150;
}

func string format(float x, string y) { // What format?
    return y + x;
}
```

### 3. Input Validation

Validate parameters to ensure function correctness:

```dream
// Good - parameter validation
func float calculatePercentage(int part, int whole) {
    if (whole == 0) {
        throw "Cannot calculate percentage: whole cannot be zero";
    }
    if (part < 0 || whole < 0) {
        throw "Cannot calculate percentage: values must be non-negative";
    }
    return ((float)part / whole) * 100.0;
}

func int getCharacterAt(string text, int index) {
    if (text == "") {
        throw "String cannot be empty";
    }
    if (index < 0 || index >= text.length) {
        throw "Index out of bounds: " + index;
    }
    return text[index];
}
```

## Advanced Function Concepts

### 🔄 Recursive Functions

Functions that call themselves to solve problems:

```dream
// Classic recursion examples
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

// Tree-like structure processing
func int calculateDepth(TreeNode node) {
    if (node == null) {
        return 0;
    }
    
    int leftDepth = calculateDepth(node.left);
    int rightDepth = calculateDepth(node.right);
    
    return 1 + max(leftDepth, rightDepth);
}
```

### 🔗 Function Composition

Building complex operations from simpler functions:

```dream
// Basic functions
func float celsiusToFahrenheit(float celsius) {
    return celsius * 9.0 / 5.0 + 32.0;
}

func float kelvinToCelsius(float kelvin) {
    return kelvin - 273.15;
}

// Composed function
func float kelvinToFahrenheit(float kelvin) {
    float celsius = kelvinToCelsius(kelvin);
    return celsiusToFahrenheit(celsius);
}

// Data processing pipeline
func void processUserData(string rawData) {
    string cleanedData = cleanInput(rawData);
    UserRecord record = parseUserRecord(cleanedData);
    bool isValid = validateUserRecord(record);
    
    if (isValid) {
        saveUserRecord(record);
        notifyUserCreated(record.email);
    } else {
        logValidationError(record);
    }
}
```

### 🎛️ Higher-Order Patterns

Functions that work with other functions conceptually:

```dream
// Function that takes behavior as parameter concept
func void processArray(int array[], int length, string operation) {
    for (int i = 0; i < length; i++) {
        if (operation == "print") {
            Console.WriteLine("Element: " + array[i]);
        } else if (operation == "double") {
            array[i] *= 2;
        } else if (operation == "square") {
            array[i] *= array[i];
        }
    }
}

// Strategy pattern with functions
func float calculateTotal(float price, string discountType) {
    if (discountType == "percentage") {
        return applyPercentageDiscount(price, 10.0);
    } else if (discountType == "fixed") {
        return applyFixedDiscount(price, 5.0);
    } else {
        return price;
    }
}
```

## Practical Function Examples

### 📊 Data Analysis Functions

```dream
// Statistical functions
func float calculateStandardDeviation(float values[], int count) {
    if (count <= 1) return 0.0;
    
    float mean = calculateMean(values, count);
    float sumSquaredDiffs = 0.0;
    
    for (int i = 0; i < count; i++) {
        float diff = values[i] - mean;
        sumSquaredDiffs += diff * diff;
    }
    
    return sqrt(sumSquaredDiffs / (count - 1));
}

func float calculateMean(float values[], int count) {
    if (count <= 0) return 0.0;
    
    float sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }
    return sum / count;
}

// Data filtering
func int filterPositiveNumbers(int input[], int inputLength, int output[]) {
    int outputCount = 0;
    
    for (int i = 0; i < inputLength; i++) {
        if (input[i] > 0) {
            output[outputCount] = input[i];
            outputCount++;
        }
    }
    
    return outputCount;
}
```

### 🎮 Game Development Functions

```dream
// Game logic functions
func bool isColliding(Rectangle rect1, Rectangle rect2) {
    return rect1.x < rect2.x + rect2.width &&
           rect1.x + rect1.width > rect2.x &&
           rect1.y < rect2.y + rect2.height &&
           rect1.y + rect1.height > rect2.y;
}

func float calculateDistance(Point p1, Point p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

func void updatePlayerPosition(Player player, float deltaTime) {
    player.position.x += player.velocity.x * deltaTime;
    player.position.y += player.velocity.y * deltaTime;
    
    // Boundary checking
    if (player.position.x < 0) player.position.x = 0;
    if (player.position.y < 0) player.position.y = 0;
    if (player.position.x > SCREEN_WIDTH) player.position.x = SCREEN_WIDTH;
    if (player.position.y > SCREEN_HEIGHT) player.position.y = SCREEN_HEIGHT;
}
```

### 💼 Business Application Functions

```dream
// Financial calculations
func float calculateCompoundInterest(float principal, float rate, int periods) {
    float amount = principal;
    for (int i = 0; i < periods; i++) {
        amount *= (1.0 + rate / 100.0);
    }
    return amount - principal;
}

func float calculateMonthlyPayment(float loanAmount, float annualRate, int months) {
    if (annualRate == 0.0) {
        return loanAmount / months;
    }
    
    float monthlyRate = annualRate / 100.0 / 12.0;
    float factor = power(1.0 + monthlyRate, months);
    
    return loanAmount * (monthlyRate * factor) / (factor - 1.0);
}

// Inventory management
func bool isInStock(Product product, int requestedQuantity) {
    return product.currentStock >= requestedQuantity;
}

func void updateInventory(Product product, int soldQuantity) {
    product.currentStock -= soldQuantity;
    
    if (product.currentStock < product.reorderPoint) {
        triggerReorder(product);
    }
}
```

## Best Practices

### 1. Function Naming

```dream
// Good - verb-noun pattern for actions
func void saveUserData(User user) { /* ... */ }
func bool validateEmail(string email) { /* ... */ }
func int calculateTotalPrice(Item items[], int count) { /* ... */ }

// Good - is/has/can pattern for boolean functions
func bool isValidPassword(string password) { /* ... */ }
func bool hasPermission(User user, string action) { /* ... */ }
func bool canProcessOrder(Order order) { /* ... */ }

// Poor - unclear names
func void process(User user) { /* ... */ }      // Process what?
func bool check(string data) { /* ... */ }       // Check what?
func int calc(int a, int b) { /* ... */ }        // Calculate what?
```

### 2. Parameter Design

```dream
// Good - appropriate parameter counts
func float calculateArea(float width, float height) {
    return width * height;
}

func void logMessage(string level, string message) {
    Console.WriteLine("[" + level + "] " + message);
}

// Consider grouping related parameters
struct Rectangle {
    float x, y, width, height;
}

func bool isPointInside(Point point, Rectangle rect) {
    return point.x >= rect.x && point.x <= rect.x + rect.width &&
           point.y >= rect.y && point.y <= rect.y + rect.height;
}
```

### 3. Error Handling

```dream
// Good - explicit error handling
func float safeDivide(float numerator, float denominator) {
    if (denominator == 0.0) {
        throw "Division by zero";
    }
    return numerator / denominator;
}

func User findUserById(int userId) {
    User user = searchUserDatabase(userId);
    if (user == null) {
        throw "User not found: " + userId;
    }
    return user;
}

// Good - validate all inputs
func void transferMoney(Account from, Account to, float amount) {
    if (from == null || to == null) {
        throw "Invalid account reference";
    }
    if (amount <= 0) {
        throw "Transfer amount must be positive";
    }
    if (from.balance < amount) {
        throw "Insufficient funds";
    }
    
    from.balance -= amount;
    to.balance += amount;
}
```

### 4. Documentation Through Code

```dream
// Self-documenting function design
func bool isEligibleForDiscount(Customer customer, Order order) {
    bool isLoyalCustomer = customer.membershipYears >= 2;
    bool isLargeOrder = order.totalAmount >= 100.0;
    bool hasCoupon = customer.hasCoupon;
    
    return isLoyalCustomer || isLargeOrder || hasCoupon;
}

// Clear step-by-step processing
func void processPayment(Order order, PaymentInfo payment) {
    validatePaymentInfo(payment);
    
    float tax = calculateTax(order);
    float total = order.subtotal + tax;
    
    bool paymentSuccessful = chargePayment(payment, total);
    
    if (paymentSuccessful) {
        updateOrderStatus(order, "PAID");
        sendConfirmationEmail(order.customer.email, order);
    } else {
        handlePaymentFailure(order, payment);
    }
}
```

## Organization Strategies

### 1. Grouping Related Functions

```dream
// Math utility functions
func float abs(float value) { return value < 0 ? -value : value; }
func float min(float a, float b) { return a < b ? a : b; }
func float max(float a, float b) { return a > b ? a : b; }
func float clamp(float value, float min, float max) { 
    return value < min ? min : (value > max ? max : value);
}

// String utility functions
func bool isEmpty(string text) { return text == "" || text == null; }
func string trim(string text) { /* remove whitespace */ return text; }
func bool startsWith(string text, string prefix) { /* check prefix */ return true; }
func bool endsWith(string text, string suffix) { /* check suffix */ return true; }
```

### 2. Layered Function Design

```dream
// Low-level functions
func void writeToDatabase(string query) { /* ... */ }
func string readFromDatabase(string query) { /* ... */ }

// Mid-level functions
func void saveUser(User user) {
    string query = buildInsertQuery(user);
    writeToDatabase(query);
}

func User loadUser(int userId) {
    string query = buildSelectQuery(userId);
    string result = readFromDatabase(query);
    return parseUserFromResult(result);
}

// High-level functions
func void registerNewUser(string name, string email, string password) {
    validateRegistrationData(name, email, password);
    
    User newUser = createUser(name, email, password);
    saveUser(newUser);
    
    sendWelcomeEmail(email);
    logUserRegistration(newUser.id);
}
```

## Function Reference Guide

### 📖 Documentation Structure

Each function section provides:

1. **[Function Definitions](v1.1/examples/functions.md)** - Complete syntax and examples
2. **[Return Values](v1.1/return.md)** - Return statement usage and patterns
3. **[Parameters](parameters.md)** - Parameter passing and validation
4. **[Scope and Lifetime](scope.md)** - Variable scope in functions
5. **[Error Handling](exceptions.md)** - Exception handling in functions

### 🚀 Quick Reference

| Topic | Description | Example |
|-------|-------------|---------|
| Declaration | `func returnType name(params)` | `func int add(int a, int b)` |
| Void Function | No return value | `func greet() { Console.WriteLine("Hi"); }` |
| Return Value | `return expression;` | `return a + b;` |
| Parameters | Typed input values | `func process(string text, int count)` |
| Local Variables | Variables inside function | `int local = 10;` |
| Recursion | Function calls itself | `return factorial(n-1);` |

## Next Steps

After understanding function concepts, explore these topics:

1. **[Function Definitions](v1.1/examples/functions.md)** - Detailed syntax and examples
2. **[Return Values](v1.1/return.md)** - Master return statements
3. **[Parameters](parameters.md)** - Advanced parameter techniques
4. **[Control Flow](control-flow.md)** - Use functions with control structures
5. **[Classes](v1.1/examples/classes.md)** - Object-oriented function organization
6. **[Modules](modules.md)** - Organize functions across files

Functions are the cornerstone of well-structured Dream programs. Understanding how to design, implement, and organize functions effectively is essential for building maintainable, scalable applications.