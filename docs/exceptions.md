# Exception Handling

Dream provides robust exception handling through `try`/`catch` blocks and `throw` statements. This allows for graceful error handling and recovery in applications.

## Basic Exception Handling

### Try-Catch Syntax
```dream
try {
    // Code that might throw an exception
    risky_operation();
} catch {
    // Code to handle any exception
    Console.WriteLine("An error occurred");
}
```

### Throwing Exceptions
Use the `throw` statement to raise exceptions:

```dream
func void validateAge(int age) {
    if (age < 0) {
        throw "Age cannot be negative";
    }
    if (age > 150) {
        throw "Age seems unrealistic";
    }
}

func void example() {
    try {
        validateAge(-5);
    } catch {
        Console.WriteLine("Invalid age provided");
    }
}
```

## Exception Examples

### Division by Zero Protection
```dream
func float safeDivide(float dividend, float divisor) {
    if (divisor == 0.0) {
        throw "Division by zero error";
    }
    return dividend / divisor;
}

func void divisionExample() {
    try {
        float result = safeDivide(10.0, 0.0);
        Console.WriteLine("Result: " + result);
    } catch {
        Console.WriteLine("Cannot divide by zero");
    }
}
```

### Array Bounds Checking
```dream
func int getArrayElement(int array[], int index, int length) {
    if (index < 0 || index >= length) {
        throw "Array index out of bounds";
    }
    return array[index];
}

func void arrayExample() {
    int numbers[5];
    numbers[0] = 10; numbers[1] = 20; numbers[2] = 30;
    numbers[3] = 40; numbers[4] = 50;
    
    try {
        int value = getArrayElement(numbers, 10, 5);  // Index 10 is out of bounds
        Console.WriteLine("Value: " + value);
    } catch {
        Console.WriteLine("Invalid array access");
    }
}
```

### Input Validation
```dream
func int parsePositiveInteger(string input) {
    if (input == "") {
        throw "Input cannot be empty";
    }
    
    int value = parseInt(input);  // Conceptual parsing function
    
    if (value <= 0) {
        throw "Value must be positive";
    }
    
    return value;
}

func void inputExample() {
    Console.Write("Enter a positive number: ");
    string userInput = Console.ReadLine();
    
    try {
        int number = parsePositiveInteger(userInput);
        Console.WriteLine("Valid number: " + number);
    } catch {
        Console.WriteLine("Please enter a valid positive number");
    }
}
```

## Multiple Exception Sources

### Sequential Operations
```dream
func void processData() {
    try {
        // Multiple operations that could fail
        string filename = getConfigFile();    // Could throw file not found
        string content = readFile(filename);  // Could throw read error  
        int value = parseConfig(content);     // Could throw parse error
        validateValue(value);                 // Could throw validation error
        
        Console.WriteLine("Processing successful: " + value);
    } catch {
        Console.WriteLine("Data processing failed");
    }
}
```

### Nested Try-Catch
```dream
func void nestedExceptionHandling() {
    try {
        // Outer operation
        Console.WriteLine("Starting main operation");
        
        try {
            // Inner operation that might fail
            int result = riskyCalculation();
            Console.WriteLine("Calculation result: " + result);
        } catch {
            Console.WriteLine("Calculation failed, using default");
            result = 0;
        }
        
        // Continue with outer operation
        Console.WriteLine("Main operation completed");
        
    } catch {
        Console.WriteLine("Main operation failed completely");
    }
}
```

## Resource Management

### Cleanup in Exception Scenarios
```dream
func void fileProcessingExample() {
    FileHandle file = null;
    
    try {
        file = openFile("data.txt");
        string content = readAll(file);
        processContent(content);
        Console.WriteLine("File processed successfully");
    } catch {
        Console.WriteLine("File processing failed");
    }
    
    // Cleanup - always runs
    if (file != null) {
        closeFile(file);
    }
}
```

### Database Connection Example
```dream
func void databaseExample() {
    Connection conn = null;
    
    try {
        conn = connectToDatabase();
        Result data = executeQuery(conn, "SELECT * FROM users");
        processResults(data);
    } catch {
        Console.WriteLine("Database operation failed");
    }
    
    // Ensure connection is closed
    if (conn != null) {
        closeConnection(conn);
    }
}
```

## Exception Patterns

### Retry Pattern
```dream
func bool attemptConnection(string server) {
    int maxAttempts = 3;
    int attempt = 0;
    
    while (attempt < maxAttempts) {
        try {
            connectToServer(server);
            Console.WriteLine("Connected successfully");
            return true;
        } catch {
            attempt++;
            Console.WriteLine("Connection attempt " + attempt + " failed");
            
            if (attempt < maxAttempts) {
                Console.WriteLine("Retrying...");
                sleep(1000);  // Wait 1 second before retry
            }
        }
    }
    
    Console.WriteLine("Failed to connect after " + maxAttempts + " attempts");
    return false;
}
```

### Validation Chain
```dream
func bool validateUser(string username, string email, int age) {
    try {
        validateUsername(username);
        validateEmail(email);
        validateAge(age);
        return true;
    } catch {
        return false;
    }
}

func void validateUsername(string username) {
    if (username == "") {
        throw "Username cannot be empty";
    }
    if (username.length < 3) {
        throw "Username must be at least 3 characters";
    }
}

func void validateEmail(string email) {
    if (email == "") {
        throw "Email cannot be empty";
    }
    if (!email.contains("@")) {
        throw "Email must contain @ symbol";
    }
}

func void validateAge(int age) {
    if (age < 0 || age > 150) {
        throw "Age must be between 0 and 150";
    }
}
```

## Error Recovery Strategies

### Fallback Values
```dream
func string getConfigValue(string key) {
    try {
        return readConfigFile(key);
    } catch {
        // Return sensible defaults
        if (key == "timeout") {
            return "30";
        } else if (key == "retries") {
            return "3";
        } else {
            return "";
        }
    }
}
```

### Alternative Methods
```dream
func string loadUserData(int userId) {
    try {
        // Try primary data source
        return loadFromDatabase(userId);
    } catch {
        try {
            // Try backup data source
            return loadFromCache(userId);
        } catch {
            // Final fallback
            return createDefaultUserData(userId);
        }
    }
}
```

### Graceful Degradation
```dream
func void displayUserProfile(int userId) {
    string name = "Unknown User";
    string email = "No email";
    int age = 0;
    
    try {
        name = getUserName(userId);
    } catch {
        Console.WriteLine("Could not load user name");
    }
    
    try {
        email = getUserEmail(userId);
    } catch {
        Console.WriteLine("Could not load user email");
    }
    
    try {
        age = getUserAge(userId);
    } catch {
        Console.WriteLine("Could not load user age");
    }
    
    // Display what we could load
    Console.WriteLine("Name: " + name);
    Console.WriteLine("Email: " + email);
    Console.WriteLine("Age: " + age);
}
```

## Mathematical Exception Handling

### Safe Mathematical Operations
```dream
func float safeSquareRoot(float number) {
    if (number < 0) {
        throw "Cannot calculate square root of negative number";
    }
    return sqrt(number);  // Built-in sqrt function
}

func int safeMod(int dividend, int divisor) {
    if (divisor == 0) {
        throw "Modulo by zero";
    }
    return dividend % divisor;
}

func void mathExample() {
    try {
        float result1 = safeSquareRoot(-4.0);
        Console.WriteLine("Square root: " + result1);
    } catch {
        Console.WriteLine("Invalid input for square root");
    }
    
    try {
        int result2 = safeMod(10, 0);
        Console.WriteLine("Modulo result: " + result2);
    } catch {
        Console.WriteLine("Invalid modulo operation");
    }
}
```

## Best Practices

### Specific Error Messages
```dream
// Good - specific error messages
func void validatePassword(string password) {
    if (password == "") {
        throw "Password cannot be empty";
    }
    if (password.length < 8) {
        throw "Password must be at least 8 characters long";
    }
    if (!containsDigit(password)) {
        throw "Password must contain at least one digit";
    }
}

// Poor - generic error message
func void validatePasswordPoor(string password) {
    if (password == "" || password.length < 8 || !containsDigit(password)) {
        throw "Invalid password";
    }
}
```

### Exception vs Return Codes
```dream
// Use exceptions for exceptional circumstances
func int divide(int a, int b) {
    if (b == 0) {
        throw "Division by zero";  // Exceptional case
    }
    return a / b;
}

// Use return values for expected conditions
func bool findUser(string username, User* user) {
    // User not found is expected, not exceptional
    if (userExists(username)) {
        *user = getUser(username);
        return true;
    }
    return false;  // Normal case, not an exception
}
```

### Early Validation
```dream
func float calculateInterest(float principal, float rate, int years) {
    // Validate all parameters early
    if (principal <= 0) {
        throw "Principal must be positive";
    }
    if (rate < 0) {
        throw "Interest rate cannot be negative";
    }
    if (years <= 0) {
        throw "Years must be positive";
    }
    
    // Perform calculation
    return principal * rate * years / 100.0;
}
```

Exception handling in Dream provides a clean way to deal with error conditions while maintaining code readability and reliability. Use exceptions for truly exceptional circumstances and provide meaningful error messages to aid in debugging and user experience.