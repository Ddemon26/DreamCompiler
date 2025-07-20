# Control Flow

Control flow statements determine the order in which code executes in your Dream programs. This section covers all mechanisms for controlling program execution, from simple conditionals to complex error handling patterns.

## Overview

Control flow in Dream provides:
- **Sequential Execution** - Code runs line by line by default
- **Conditional Execution** - Code runs based on conditions
- **Repetitive Execution** - Code runs multiple times in loops
- **Exception Handling** - Code handles errors gracefully
- **Function Calls** - Transfer control to reusable code blocks

## Program Flow Concepts

### Execution Models

```dream
// Sequential execution (default)
int a = 10;        // Line 1
int b = 20;        // Line 2
int c = a + b;     // Line 3
Console.WriteLine(c); // Line 4

// Conditional execution
if (a > 5) {       // Condition determines execution
    Console.WriteLine("a is greater than 5");
}

// Repetitive execution
for (int i = 0; i < 3; i++) {  // Loop repeats code
    Console.WriteLine("Iteration: " + i);
}
```

## Control Flow Categories

### 🔀 Conditional Statements
Execute code based on boolean conditions.

#### If Statements
The fundamental conditional construct for decision making.

```dream
// Basic if statement
if (temperature > 30) {
    Console.WriteLine("It's hot today!");
}

// If-else statement
if (age >= 18) {
    Console.WriteLine("You can vote");
} else {
    Console.WriteLine("You cannot vote yet");
}

// Multi-way if-else chain
if (score >= 90) {
    Console.WriteLine("Grade: A");
} else if (score >= 80) {
    Console.WriteLine("Grade: B");
} else if (score >= 70) {
    Console.WriteLine("Grade: C");
} else {
    Console.WriteLine("Grade: F");
}

// Nested if statements
if (isLoggedIn) {
    if (hasPermission) {
        Console.WriteLine("Access granted");
    } else {
        Console.WriteLine("Access denied: insufficient permissions");
    }
} else {
    Console.WriteLine("Please log in first");
}
```

**📖 [Complete If Statements Guide](v1.1/if.md)**

#### Switch Statements
Multi-way branching for discrete values.

```dream
// Basic switch statement
switch (dayOfWeek) {
    case 1:
        Console.WriteLine("Monday");
        break;
    case 2:
        Console.WriteLine("Tuesday");
        break;
    case 3:
        Console.WriteLine("Wednesday");
        break;
    default:
        Console.WriteLine("Other day");
        break;
}

// Switch with fall-through
switch (grade) {
    case 'A':
    case 'B':
        Console.WriteLine("Good grade");
        break;
    case 'C':
        Console.WriteLine("Average grade");
        break;
    case 'D':
    case 'F':
        Console.WriteLine("Poor grade");
        break;
    default:
        Console.WriteLine("Invalid grade");
        break;
}
```

**📖 [Complete Switch Statements Guide](v1.1/switch.md)**

### 🔄 Loop Statements
Repeat code execution multiple times.

#### For Loops
Ideal for counting and iteration with known bounds.

```dream
// Basic for loop
for (int i = 0; i < 10; i++) {
    Console.WriteLine("Count: " + i);
}

// Countdown loop
for (int i = 10; i > 0; i--) {
    Console.WriteLine("Countdown: " + i);
}

// Step by different values
for (int i = 0; i < 20; i += 2) {
    Console.WriteLine("Even number: " + i);
}

// Nested loops
for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
        Console.WriteLine("Position: (" + row + ", " + col + ")");
    }
}

// Array iteration
int numbers[5];
numbers[0] = 10; numbers[1] = 20; numbers[2] = 30;
numbers[3] = 40; numbers[4] = 50;

for (int i = 0; i < 5; i++) {
    Console.WriteLine("Array[" + i + "] = " + numbers[i]);
}
```

#### While Loops
Continue while a condition remains true.

```dream
// Basic while loop
int count = 0;
while (count < 5) {
    Console.WriteLine("Count: " + count);
    count++;
}

// Input validation loop
string input;
bool validInput = false;
while (!validInput) {
    Console.Write("Enter a positive number: ");
    input = Console.ReadLine();
    // Assume parseInteger() function exists
    int number = parseInteger(input);
    if (number > 0) {
        validInput = true;
        Console.WriteLine("Valid input: " + number);
    } else {
        Console.WriteLine("Invalid input. Please try again.");
    }
}

// Processing with sentinel value
int value;
Console.WriteLine("Enter numbers (0 to stop):");
while (true) {
    value = getUserInput();
    if (value == 0) {
        break;  // Exit loop
    }
    Console.WriteLine("Processing: " + value);
}
```

#### Do-While Loops
Execute at least once, then continue while condition is true.

```dream
// Menu system with do-while
int choice;
do {
    Console.WriteLine("=== Main Menu ===");
    Console.WriteLine("1. New Game");
    Console.WriteLine("2. Load Game");
    Console.WriteLine("3. Settings");
    Console.WriteLine("4. Exit");
    Console.Write("Choose an option: ");
    
    choice = getUserChoice();
    
    switch (choice) {
        case 1:
            Console.WriteLine("Starting new game...");
            break;
        case 2:
            Console.WriteLine("Loading game...");
            break;
        case 3:
            Console.WriteLine("Opening settings...");
            break;
        case 4:
            Console.WriteLine("Goodbye!");
            break;
        default:
            Console.WriteLine("Invalid choice. Please try again.");
            break;
    }
} while (choice != 4);
```

**📖 [Complete Loops Guide](v1.1/loops.md)**

### 🚦 Flow Control Keywords
Control execution within loops and functions.

#### Break and Continue

```dream
// Break - exit loop immediately
for (int i = 0; i < 10; i++) {
    if (i == 5) {
        break;  // Exit when i equals 5
    }
    Console.WriteLine(i);  // Prints 0, 1, 2, 3, 4
}

// Continue - skip to next iteration
for (int i = 0; i < 10; i++) {
    if (i % 2 == 0) {
        continue;  // Skip even numbers
    }
    Console.WriteLine(i);  // Prints 1, 3, 5, 7, 9
}

// Break in nested loops (only breaks inner loop)
for (int outer = 0; outer < 3; outer++) {
    Console.WriteLine("Outer: " + outer);
    for (int inner = 0; inner < 5; inner++) {
        if (inner == 2) {
            break;  // Only breaks inner loop
        }
        Console.WriteLine("  Inner: " + inner);
    }
}

// Finding elements with break
int numbers[10];
// ... fill array with values ...
int target = 42;
bool found = false;

for (int i = 0; i < 10; i++) {
    if (numbers[i] == target) {
        Console.WriteLine("Found " + target + " at index " + i);
        found = true;
        break;  // Stop searching once found
    }
}

if (!found) {
    Console.WriteLine(target + " not found in array");
}
```

#### Return Statements

```dream
// Return from function
func int findMax(int array[], int length) {
    if (length == 0) {
        return -1;  // Early return for edge case
    }
    
    int max = array[0];
    for (int i = 1; i < length; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;  // Return the result
}

// Multiple return points
func string validateUser(string username, string password) {
    if (username == "") {
        return "Username cannot be empty";
    }
    if (password == "") {
        return "Password cannot be empty";
    }
    if (username.length < 3) {
        return "Username must be at least 3 characters";
    }
    if (password.length < 8) {
        return "Password must be at least 8 characters";
    }
    return "Valid user credentials";
}
```

**📖 [Complete Return Guide](v1.1/return.md)**

### ⚠️ Exception Handling
Handle errors and exceptional conditions gracefully.

#### Try-Catch-Throw

```dream
// Basic exception handling
func void safeOperation() {
    try {
        int result = riskyCalculation();
        Console.WriteLine("Result: " + result);
    } catch {
        Console.WriteLine("An error occurred during calculation");
    }
}

func int riskyCalculation() {
    int value = getUserInput();
    if (value < 0) {
        throw "Negative values not allowed";
    }
    return value * 2;
}

// Resource management with exceptions
func void processFile(string filename) {
    FileHandle file = null;
    
    try {
        file = openFile(filename);
        string content = readFile(file);
        processContent(content);
        Console.WriteLine("File processed successfully");
    } catch {
        Console.WriteLine("Error processing file: " + filename);
    }
    
    // Cleanup always happens
    if (file != null) {
        closeFile(file);
    }
}

// Validation with exceptions
func void createUser(string name, string email, int age) {
    try {
        validateName(name);
        validateEmail(email);
        validateAge(age);
        
        // Create user if all validations pass
        User newUser = new User();
        newUser.name = name;
        newUser.email = email;
        newUser.age = age;
        
        Console.WriteLine("User created successfully");
    } catch {
        Console.WriteLine("Failed to create user due to validation error");
    }
}

func void validateName(string name) {
    if (name == "") {
        throw "Name cannot be empty";
    }
    if (name.length < 2) {
        throw "Name must be at least 2 characters";
    }
}

func void validateAge(int age) {
    if (age < 0 || age > 150) {
        throw "Age must be between 0 and 150";
    }
}
```

**📖 [Complete Exception Handling Guide](exceptions.md)**

## Advanced Control Flow Patterns

### 1. State Machines

```dream
// Simple state machine using switch
enum GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
}

func void updateGame() {
    switch (currentState) {
        case MENU:
            handleMenuInput();
            if (startPressed) {
                currentState = PLAYING;
            }
            break;
            
        case PLAYING:
            updateGameLogic();
            if (pausePressed) {
                currentState = PAUSED;
            } else if (gameOver) {
                currentState = GAME_OVER;
            }
            break;
            
        case PAUSED:
            if (resumePressed) {
                currentState = PLAYING;
            }
            break;
            
        case GAME_OVER:
            if (restartPressed) {
                resetGame();
                currentState = PLAYING;
            } else if (menuPressed) {
                currentState = MENU;
            }
            break;
    }
}
```

### 2. Event Processing Loops

```dream
// Main game loop pattern
func void runGame() {
    bool running = true;
    
    while (running) {
        // Process input
        handleInput();
        
        // Update game state
        updateGame();
        
        // Render frame
        renderFrame();
        
        // Check for exit condition
        if (shouldExit()) {
            running = false;
        }
        
        // Frame rate control
        sleep(16);  // ~60 FPS
    }
    
    cleanup();
}

// Event processing with switch
func void handleInput() {
    Event event = getNextEvent();
    
    switch (event.type) {
        case KEY_PRESSED:
            handleKeyPress(event.key);
            break;
        case KEY_RELEASED:
            handleKeyRelease(event.key);
            break;
        case MOUSE_CLICK:
            handleMouseClick(event.x, event.y);
            break;
        case QUIT:
            requestExit();
            break;
    }
}
```

### 3. Search and Traversal Patterns

```dream
// Linear search with early termination
func int findElement(int array[], int length, int target) {
    for (int i = 0; i < length; i++) {
        if (array[i] == target) {
            return i;  // Found - return index
        }
    }
    return -1;  // Not found
}

// Binary search (assumes sorted array)
func int binarySearch(int array[], int length, int target) {
    int left = 0;
    int right = length - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (array[mid] == target) {
            return mid;  // Found
        } else if (array[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return -1;  // Not found
}

// Tree-like structure traversal
func void processDirectory(string path) {
    string[] files = getFiles(path);
    string[] subdirs = getSubdirectories(path);
    
    // Process files in current directory
    for (int i = 0; i < files.length; i++) {
        processFile(files[i]);
    }
    
    // Recursively process subdirectories
    for (int i = 0; i < subdirs.length; i++) {
        processDirectory(subdirs[i]);
    }
}
```

## Best Practices

### 1. Use Appropriate Control Structures

```dream
// Good - use for loops for counting
for (int i = 0; i < count; i++) {
    processItem(i);
}

// Poor - using while for simple counting
int i = 0;
while (i < count) {
    processItem(i);
    i++;
}

// Good - use while for condition-based loops
while (!connectionEstablished && retryCount < maxRetries) {
    attemptConnection();
    retryCount++;
}

// Good - use switch for discrete values
switch (userChoice) {
    case 1: startNewGame(); break;
    case 2: loadGame(); break;
    case 3: showSettings(); break;
    default: showError(); break;
}

// Poor - using if-else chain for many discrete values
if (userChoice == 1) {
    startNewGame();
} else if (userChoice == 2) {
    loadGame();
} else if (userChoice == 3) {
    showSettings();
} else {
    showError();
}
```

### 2. Avoid Deep Nesting

```dream
// Poor - deeply nested conditions
func void processUser(User user) {
    if (user != null) {
        if (user.isActive) {
            if (user.hasPermissions) {
                if (user.isVerified) {
                    // Process user
                } else {
                    Console.WriteLine("User not verified");
                }
            } else {
                Console.WriteLine("User lacks permissions");
            }
        } else {
            Console.WriteLine("User is inactive");
        }
    } else {
        Console.WriteLine("User is null");
    }
}

// Good - early returns to reduce nesting
func void processUser(User user) {
    if (user == null) {
        Console.WriteLine("User is null");
        return;
    }
    
    if (!user.isActive) {
        Console.WriteLine("User is inactive");
        return;
    }
    
    if (!user.hasPermissions) {
        Console.WriteLine("User lacks permissions");
        return;
    }
    
    if (!user.isVerified) {
        Console.WriteLine("User not verified");
        return;
    }
    
    // Process user - main logic at lowest nesting level
    performUserOperation(user);
}
```

### 3. Use Clear Loop Patterns

```dream
// Good - clear iterator patterns
for (int i = 0; i < array.length; i++) {
    processElement(array[i]);
}

// Good - clear condition patterns
while (hasMoreData()) {
    Data data = getNextData();
    processData(data);
}

// Good - clear sentinel patterns
string line;
while ((line = readLine()) != null) {
    processLine(line);
}
```

### 4. Handle Edge Cases

```dream
// Good - handle empty collections
func int findMaximum(int array[], int length) {
    if (length <= 0) {
        throw "Array cannot be empty";
    }
    
    int max = array[0];
    for (int i = 1; i < length; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

// Good - handle boundary conditions
func void printRange(int start, int end) {
    if (start > end) {
        Console.WriteLine("Invalid range: start > end");
        return;
    }
    
    for (int i = start; i <= end; i++) {
        Console.WriteLine(i);
    }
}
```

## Performance Considerations

### 1. Loop Optimization

```dream
// Good - cache array length outside loop
int length = array.length;
for (int i = 0; i < length; i++) {
    processElement(array[i]);
}

// Good - avoid repeated calculations
for (int i = 0; i < count; i++) {
    for (int j = 0; j < count; j++) {
        int index = i * count + j;  // Calculate once
        processElement(data[index]);
    }
}
```

### 2. Early Termination

```dream
// Good - use break to exit early
for (int i = 0; i < largeArray.length; i++) {
    if (largeArray[i] == target) {
        Console.WriteLine("Found at index: " + i);
        break;  // Stop searching once found
    }
}

// Good - use continue to skip unnecessary work
for (int i = 0; i < array.length; i++) {
    if (!isValid(array[i])) {
        continue;  // Skip invalid elements
    }
    
    expensiveProcess(array[i]);
}
```

## Common Patterns

### 1. Input Validation

```dream
func int getValidInteger(string prompt, int min, int max) {
    int value;
    bool valid = false;
    
    do {
        Console.Write(prompt);
        string input = Console.ReadLine();
        
        try {
            value = parseInteger(input);
            if (value >= min && value <= max) {
                valid = true;
            } else {
                Console.WriteLine("Value must be between " + min + " and " + max);
            }
        } catch {
            Console.WriteLine("Please enter a valid integer");
        }
    } while (!valid);
    
    return value;
}
```

### 2. Menu Systems

```dream
func void showMainMenu() {
    int choice;
    bool exit = false;
    
    while (!exit) {
        Console.WriteLine("\n=== Main Menu ===");
        Console.WriteLine("1. Start Game");
        Console.WriteLine("2. Load Game");
        Console.WriteLine("3. Settings");
        Console.WriteLine("4. Exit");
        
        choice = getValidInteger("Enter choice (1-4): ", 1, 4);
        
        switch (choice) {
            case 1:
                startNewGame();
                break;
            case 2:
                loadExistingGame();
                break;
            case 3:
                showSettingsMenu();
                break;
            case 4:
                exit = true;
                Console.WriteLine("Thanks for playing!");
                break;
        }
    }
}
```

### 3. Data Processing Pipelines

```dream
func void processDataFile(string filename) {
    string[] lines = readAllLines(filename);
    
    for (int i = 0; i < lines.length; i++) {
        string line = lines[i];
        
        // Skip empty lines and comments
        if (line == "" || line[0] == '#') {
            continue;
        }
        
        try {
            DataRecord record = parseLine(line);
            
            if (validateRecord(record)) {
                processRecord(record);
            } else {
                Console.WriteLine("Invalid record on line " + (i + 1));
            }
        } catch {
            Console.WriteLine("Error parsing line " + (i + 1) + ": " + line);
        }
    }
}
```

## Next Steps

After mastering control flow, explore these related topics:

1. **[Functions](functions-overview.md)** - Organize control flow into reusable functions
2. **[Exception Handling](exceptions.md)** - Advanced error handling patterns
3. **[Operators](operators.md)** - Use operators effectively in control conditions
4. **[Arrays](v1.1/arrays.md)** - Control iteration over data collections
5. **[Classes](v1.1/examples/classes.md)** - Object-oriented control flow patterns

Control flow is essential for creating dynamic, responsive programs. Understanding these concepts allows you to build complex logic while maintaining clear, readable code structure.