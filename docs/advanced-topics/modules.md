# Multi-file Compilation & Modules

Dream supports modular programming through a comprehensive module system that enables organizing code across multiple files, managing dependencies, and creating reusable libraries.

## Module System Overview

The Dream module system provides:
- **Module Declaration** - Define module boundaries and names
- **Import/Export** - Control what functions and types are accessible between modules
- **Dependency Resolution** - Automatic topological sorting of module dependencies
- **Separate Compilation** - Compile modules independently for faster builds

## Module Declaration

### Basic Module Declaration
```dream
// math_utils.dr
module math_utils;

func int add(int a, int b) {
    return a + b;
}

func int multiply(int a, int b) {
    return a * b;
}
```

### Module with Exports
Use the `export` keyword to make functions available to other modules:

```dream
// math_utils.dr
module math_utils;

// Private function - only accessible within this module
func int square(int x) {
    return x * x;
}

// Public functions - accessible from other modules
export func int add(int a, int b) {
    return a + b;
}

export func int power(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i++) {
        result = multiply(result, base);
    }
    return result;
}

export func int multiply(int a, int b) {
    return a * b;
}
```

## Importing Modules

### Basic Import
```dream
// main.dr
import math_utils;

func main() {
    int sum = math_utils.add(5, 3);           // 8
    int product = math_utils.multiply(4, 7);   // 28
    int powered = math_utils.power(2, 8);      // 256
    
    Console.WriteLine("Sum: " + sum);
    Console.WriteLine("Product: " + product);
    Console.WriteLine("Power: " + powered);
}
```

### Multiple Imports
```dream
// main.dr
import math_utils;
import string_utils;
import console_lib;

func main() {
    // Use functions from different modules
    int result = math_utils.add(10, 20);
    string message = string_utils.concat("Result: ", result);
    console_lib.printColored(message, "green");
}
```

## Complete Module Examples

### String Utilities Module
```dream
// string_utils.dr
module string_utils;

export func string concat(string a, string b) {
    return a + b;
}

export func string concatThree(string a, string b, string c) {
    return a + b + c;
}

export func string repeat(string str, int times) {
    string result = "";
    for (int i = 0; i < times; i++) {
        result = result + str;
    }
    return result;
}

export func int length(string str) {
    // Would use built-in string length functionality
    int len = 0;
    // Implementation would count characters
    return len;
}

export func bool startsWith(string str, string prefix) {
    // Simplified implementation
    if (length(prefix) > length(str)) {
        return false;
    }
    // Would compare characters
    return true; // Placeholder
}
```

### Data Processing Module
```dream
// data_processor.dr
module data_processor;

import math_utils;

// Private helper function
func bool isValid(int value) {
    return value >= 0 && value <= 1000;
}

export func int processArray(int data[], int size) {
    int sum = 0;
    int validCount = 0;
    
    for (int i = 0; i < size; i++) {
        if (isValid(data[i])) {
            sum = math_utils.add(sum, data[i]);
            validCount++;
        }
    }
    
    return validCount > 0 ? sum / validCount : 0;
}

export func bool validateData(int data[], int size) {
    for (int i = 0; i < size; i++) {
        if (!isValid(data[i])) {
            return false;
        }
    }
    return true;
}
```

### Configuration Module
```dream
// config.dr
module config;

export struct AppConfig {
    string appName;
    int maxUsers;
    bool debugMode;
    float version;
}

// Private default configuration
AppConfig defaultConfig;

func void initializeDefaults() {
    defaultConfig.appName = "DreamApp";
    defaultConfig.maxUsers = 100;
    defaultConfig.debugMode = false;
    defaultConfig.version = 1.0;
}

export func AppConfig getDefaultConfig() {
    initializeDefaults();
    return defaultConfig;
}

export func AppConfig createConfig(string name, int users, bool debug) {
    AppConfig config;
    config.appName = name;
    config.maxUsers = users;
    config.debugMode = debug;
    config.version = 1.0;
    return config;
}

export func void printConfig(AppConfig config) {
    Console.WriteLine("App Name: " + config.appName);
    Console.WriteLine("Max Users: " + config.maxUsers);
    Console.WriteLine("Debug Mode: " + (config.debugMode ? "enabled" : "disabled"));
    Console.WriteLine("Version: " + config.version);
}
```

## Using Multiple Modules

### Main Application
```dream
// main.dr
import math_utils;
import string_utils;
import data_processor;
import config;

func void demonstrateMath() {
    Console.WriteLine("=== Math Demo ===");
    int a = 15, b = 25;
    
    int sum = math_utils.add(a, b);
    int product = math_utils.multiply(a, b);
    int powered = math_utils.power(2, 4);
    
    Console.WriteLine("Sum: " + sum);
    Console.WriteLine("Product: " + product);
    Console.WriteLine("Power: " + powered);
}

func void demonstrateStrings() {
    Console.WriteLine("=== String Demo ===");
    
    string greeting = string_utils.concat("Hello", " World");
    string repeated = string_utils.repeat("Hi! ", 3);
    string full = string_utils.concatThree("Welcome", " to ", "Dream!");
    
    Console.WriteLine(greeting);
    Console.WriteLine(repeated);
    Console.WriteLine(full);
}

func void demonstrateDataProcessing() {
    Console.WriteLine("=== Data Processing Demo ===");
    
    int numbers[5];
    numbers[0] = 10; numbers[1] = 20; numbers[2] = 30;
    numbers[3] = 40; numbers[4] = 50;
    
    bool valid = data_processor.validateData(numbers, 5);
    int average = data_processor.processArray(numbers, 5);
    
    Console.WriteLine("Data valid: " + (valid ? "yes" : "no"));
    Console.WriteLine("Average: " + average);
}

func void demonstrateConfig() {
    Console.WriteLine("=== Configuration Demo ===");
    
    config.AppConfig defaultCfg = config.getDefaultConfig();
    config.printConfig(defaultCfg);
    
    Console.WriteLine("");
    
    config.AppConfig customCfg = config.createConfig("MyApp", 500, true);
    config.printConfig(customCfg);
}

func main() {
    demonstrateMath();
    Console.WriteLine("");
    
    demonstrateStrings();
    Console.WriteLine("");
    
    demonstrateDataProcessing();
    Console.WriteLine("");
    
    demonstrateConfig();
}
```

## Module Compilation Process

### Build System Integration
The Zig build system handles multi-file compilation:

```bash
# Build all modules
zig build compile-multi

# Generated file structure
build/
├── modules/
│   ├── math_utils.c
│   ├── string_utils.c
│   ├── data_processor.c
│   └── config.c
├── headers/
│   ├── math_utils.h
│   ├── string_utils.h
│   ├── data_processor.h
│   └── config.h
└── bin/
    └── main.c
```

### Compilation Steps
1. **Dependency Analysis** - Topological sort of module dependencies
2. **Module Compilation** - Generate separate C files for each module
3. **Header Generation** - Create `.h` files with extern declarations
4. **Static Library** - Build `libdreamrt.a` with runtime components
5. **Linking** - Combine all modules into final executable

### Generated C Code
```c
// Generated math_utils.h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

extern int math_utils_add(int a, int b);
extern int math_utils_multiply(int a, int b);
extern int math_utils_power(int base, int exponent);

#endif

// Generated math_utils.c
#include "math_utils.h"
#include "memory.h"

static int square(int x) {  // Private function
    return x * x;
}

int math_utils_add(int a, int b) {  // Exported function
    return a + b;
}

int math_utils_multiply(int a, int b) {  // Exported function
    return a * b;
}

int math_utils_power(int base, int exponent) {  // Exported function
    int result = 1;
    for (int i = 0; i < exponent; i++) {
        result = math_utils_multiply(result, base);
    }
    return result;
}
```

## Advanced Module Features

### Circular Dependency Resolution
The compiler detects and reports circular dependencies:

```dream
// module_a.dr
module module_a;
import module_b;  // Error: Circular dependency

export func void funcA() {
    module_b.funcB();
}

// module_b.dr  
module module_b;
import module_a;  // Error: Circular dependency

export func void funcB() {
    module_a.funcA();
}
```

### Module Hierarchies
Organize modules in directories:

```
src/
├── utils/
│   ├── math_utils.dr
│   └── string_utils.dr
├── data/
│   ├── processor.dr
│   └── validator.dr
└── main.dr
```

```dream
// Reference modules with path prefixes
import utils.math_utils;
import utils.string_utils;
import data.processor;

func main() {
    int result = utils.math_utils.add(1, 2);
    string text = utils.string_utils.concat("Hello", " World");
    data.processor.processData(result);
}
```

### Conditional Compilation
Modules can include platform-specific code:

```dream
// platform_utils.dr
module platform_utils;

#ifdef WINDOWS
export func string getPathSeparator() {
    return "\\";
}
#else
export func string getPathSeparator() {
    return "/";
}
#endif

export func string joinPath(string dir, string file) {
    return dir + getPathSeparator() + file;
}
```

## Testing Multi-file Projects

### Test Organization
```bash
tests/multifile/
├── math_utils.dr      # Math utility module
├── string_utils.dr    # String utility module  
├── main.dr           # Main program using both modules
└── expected_output    # Expected program output
```

### Running Multi-file Tests
```bash
# Test multi-file compilation
./codex/test_cli.sh filter multifile

# Test specific multi-file project
zig build compile-multi
cd build/bin && ./main
```

## Best Practices

### Module Organization
```dream
// Good: Logical grouping
// file_utils.dr - File operations
// string_utils.dr - String manipulation  
// math_utils.dr - Mathematical functions

// Poor: Everything in one module
// utils.dr - Mixed utilities
```

### Export Guidelines
```dream
// Good: Export only public API
module user_manager;

export func bool createUser(string name, string email) { /* ... */ }
export func bool deleteUser(int userId) { /* ... */ }

// Private helper functions
func bool validateEmail(string email) { /* ... */ }
func int generateUserId() { /* ... */ }

// Poor: Export everything
module user_manager;

export func bool createUser(string name, string email) { /* ... */ }
export func bool validateEmail(string email) { /* ... */ }  // Internal detail
export func int generateUserId() { /* ... */ }              // Internal detail
```

### Dependency Management
```dream
// Good: Minimal dependencies
module calculator;
import math_utils;  // Only what's needed

// Poor: Unnecessary dependencies
module calculator;
import math_utils;
import string_utils;  // Not used
import file_utils;    // Not used
```

### Module Documentation
```dream
// user_service.dr
// User Service Module
// 
// Provides user management functionality including:
// - User creation and deletion
// - Authentication and authorization
// - User profile management
//
// Dependencies: database, crypto_utils
// Exports: createUser, authenticateUser, getUserProfile

module user_service;
import database;
import crypto_utils;

export func bool createUser(string username, string password) {
    // Implementation
}
```

The module system enables building large, maintainable Dream applications with clear separation of concerns and efficient compilation.