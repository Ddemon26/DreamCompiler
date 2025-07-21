# Dream Runtime Library Reference

The Dream Runtime Library provides the foundational functions and APIs that generated Dream code depends on. This comprehensive reference covers all built-in functions, data structures, and system interfaces.

## Runtime Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Dream Runtime Stack                      │
├─────────────────────────────────────────────────────────────┤
│  Dream User Code (.dr files)                               │
│    ↓                                                        │
│  Generated C Code (dream.c)                                │
│    ↓                                                        │
│  Dream Runtime Library (memory.c, console.c, custom.c)     │
│    ↓                                                        │
│  Platform C Standard Library (libc)                        │
│    ↓                                                        │
│  Operating System (Windows, Linux, macOS)                  │
└─────────────────────────────────────────────────────────────┘
```

## Core Runtime Components

### 1. Memory Management (`memory.h/.c`)

The memory subsystem provides automatic memory management through reference counting.

#### Memory Allocation Functions

```c
// String memory management
char* dr_alloc_string(const char* content);
char* dr_retain_string(char* str);
void dr_release_string(char* str);

// String concatenation functions (NEW - 2025-07-21)
char* dream_concat(const char* str1, const char* str2);
char* dream_concat_string_int(const char* str, int value);
char* dream_concat_int_string(int value, const char* str);
char* dream_concat_string_float(const char* str, float value);
char* dream_concat_float_string(float value, const char* str);
char* dream_int_to_string(int value);
char* dream_float_to_string(float value);

// Legacy functions
char* dr_string_concat(const char* a, const char* b);  // Deprecated - use dream_concat
int dr_string_length(const char* str);
bool dr_string_equals(const char* a, const char* b);

// Array memory management
void* dr_alloc_array(size_t element_size, size_t count);
void* dr_retain_array(void* array);
void dr_release_array(void* array);
size_t dr_array_length(void* array);

// Generic object memory management
void* dr_alloc_object(size_t size);
void* dr_retain_object(void* obj);
void dr_release_object(void* obj);

// Memory pool management
dr_pool_t* dr_create_pool(size_t block_size);
void* dr_pool_alloc(dr_pool_t* pool);
void dr_pool_free(dr_pool_t* pool, void* ptr);
void dr_destroy_pool(dr_pool_t* pool);

// Bulk cleanup
void dr_release_all(void);
```

#### Memory Statistics and Debugging

```c
// Memory usage statistics
typedef struct {
    size_t total_allocated;
    size_t total_freed;
    size_t current_usage;
    size_t peak_usage;
    size_t allocation_count;
    size_t free_count;
} dr_memory_stats_t;

dr_memory_stats_t dr_get_memory_stats(void);
void dr_print_memory_stats(void);
void dr_dump_allocations(void);
```

**Usage in Dream Code:**
```dream
// Dream code - memory management is automatic
string message = "Hello, World!";
int[] numbers = [1, 2, 3, 4, 5];
// Memory automatically managed by runtime
```

**Generated C Code:**
```c
// Generated C code with runtime calls
char* message = dr_alloc_string("Hello, World!");
int* numbers = dr_alloc_array(sizeof(int), 5);
// ... initialization of array elements ...
dr_release_string(message);
dr_release_array(numbers);
```

### 2. String Concatenation API (`custom.h/.c`) - *New in 2025-07-21*

The string concatenation API provides efficient string operations with automatic memory management and type conversion.

#### Core Concatenation Functions

```c
/**
 * Concatenate two strings
 * @param str1 First string (null-terminated)
 * @param str2 Second string (null-terminated) 
 * @return New string containing str1 + str2 (caller owns memory)
 */
char* dream_concat(const char* str1, const char* str2);

/**
 * Concatenate string with integer
 * @param str String part
 * @param value Integer value to append
 * @return New string containing str + string representation of value
 */
char* dream_concat_string_int(const char* str, int value);

/**
 * Concatenate integer with string
 * @param value Integer value to prepend
 * @param str String part
 * @return New string containing string representation of value + str
 */
char* dream_concat_int_string(int value, const char* str);

/**
 * Concatenate string with float
 * @param str String part
 * @param value Float value to append  
 * @return New string containing str + string representation of value
 */
char* dream_concat_string_float(const char* str, float value);

/**
 * Concatenate float with string
 * @param value Float value to prepend
 * @param str String part
 * @return New string containing string representation of value + str
 */
char* dream_concat_float_string(float value, const char* str);
```

#### Helper Functions

```c
/**
 * Convert integer to string representation
 * @param value Integer value
 * @return String representation (caller owns memory)
 */
char* dream_int_to_string(int value);

/**
 * Convert float to string representation  
 * @param value Float value
 * @return String representation (caller owns memory)
 */
char* dream_float_to_string(float value);
```

**Usage Examples in Dream Code:**

```dream
// Basic string concatenation
string greeting = "Hello" + " " + "World";  // Uses dream_concat()

// Mixed-type concatenation  
string message = "Count: " + 42;            // Uses dream_concat_string_int()
string price = 19.99 + " dollars";          // Uses dream_concat_float_string()

// Complex expressions
int items = 5;
float total = 99.50;
string summary = "Items: " + items + ", Total: $" + total;
// Generates multiple concatenation calls with proper precedence
```

**Generated C Code:**

```c
// Dream: "Hello" + " World"
char* greeting = dream_concat("Hello", " World");

// Dream: "Count: " + 42
char* message = dream_concat_string_int("Count: ", 42);

// Dream: "Items: " + items + ", Total: $" + total  
char* temp1 = dream_concat_string_int("Items: ", items);
char* temp2 = dream_concat(temp1, ", Total: $");
char* summary = dream_concat_float_string(temp2, total);
```

#### Memory Management

- All concatenation functions return newly allocated strings using `dr_alloc()`
- Memory is automatically freed by `dr_release_all()` at program termination
- Intermediate temporary strings in complex expressions are properly managed
- No manual memory management required in Dream code

### 3. Console I/O (`console.h/.c`)

The console subsystem provides all standard input/output operations.

#### Output Functions

```c
// Basic output
void dr_console_write(const char* text);
void dr_console_writeline(const char* text);
void dr_console_write_char(char c);
void dr_console_write_int(int value);
void dr_console_write_float(double value);
void dr_console_write_bool(bool value);

// Formatted output
void dr_console_printf(const char* format, ...);
void dr_console_write_formatted(const char* format, ...);

// Error output
void dr_console_error(const char* text);
void dr_console_error_line(const char* text);
```

#### Input Functions

```c
// Basic input
char* dr_console_readline(void);
char dr_console_readchar(void);
int dr_console_readint(void);
double dr_console_readfloat(void);
bool dr_console_readbool(void);

// Input validation
bool dr_console_has_input(void);
void dr_console_clear_input(void);
```

#### Console Configuration

```c
// Console properties
void dr_console_set_title(const char* title);
void dr_console_clear(void);
void dr_console_set_cursor_position(int x, int y);
void dr_console_get_cursor_position(int* x, int* y);
void dr_console_set_color(int foreground, int background);
void dr_console_reset_color(void);
```

**Usage in Dream Code:**
```dream
// Dream Console API
Console.WriteLine("Enter your name:");
string name = Console.ReadLine();
Console.WriteLine("Hello, " + name + "!");

Console.Write("Enter a number: ");
int number = Console.ReadInt();
Console.WriteLine("You entered: " + number);

// Advanced console features
Console.SetTitle("My Dream Application");
Console.Clear();
Console.SetColor(ConsoleColor.Green, ConsoleColor.Black);
```

### 3. Custom Runtime (`custom.h/.c`)

The custom runtime provides specialized functions for advanced operations.

#### String Manipulation

```c
// Advanced string operations
char* dr_string_substring(const char* str, int start, int length);
char* dr_string_replace(const char* str, const char* old_val, const char* new_val);
char* dr_string_tolower(const char* str);
char* dr_string_toupper(const char* str);
char* dr_string_trim(const char* str);
bool dr_string_startswith(const char* str, const char* prefix);
bool dr_string_endswith(const char* str, const char* suffix);
int dr_string_indexof(const char* str, const char* search);
char** dr_string_split(const char* str, const char* delimiter, int* count);

// String builder
typedef struct dr_stringbuilder_t dr_stringbuilder_t;
dr_stringbuilder_t* dr_stringbuilder_create(void);
void dr_stringbuilder_append(dr_stringbuilder_t* sb, const char* text);
void dr_stringbuilder_append_char(dr_stringbuilder_t* sb, char c);
void dr_stringbuilder_append_int(dr_stringbuilder_t* sb, int value);
void dr_stringbuilder_append_float(dr_stringbuilder_t* sb, double value);
char* dr_stringbuilder_tostring(dr_stringbuilder_t* sb);
void dr_stringbuilder_destroy(dr_stringbuilder_t* sb);
```

#### Array Operations

```c
// Array utilities
void dr_array_copy(void* dest, void* src, size_t element_size, size_t count);
void dr_array_fill(void* array, size_t element_size, size_t count, void* value);
void dr_array_reverse(void* array, size_t element_size, size_t count);
void dr_array_sort(void* array, size_t element_size, size_t count, 
                   int (*compare)(const void*, const void*));
int dr_array_search(void* array, size_t element_size, size_t count, 
                    void* value, int (*compare)(const void*, const void*));
```

#### Math Functions

```c
// Mathematical operations
double dr_math_abs(double x);
double dr_math_sqrt(double x);
double dr_math_pow(double base, double exponent);
double dr_math_sin(double x);
double dr_math_cos(double x);
double dr_math_tan(double x);
double dr_math_log(double x);
double dr_math_exp(double x);
double dr_math_floor(double x);
double dr_math_ceil(double x);
double dr_math_round(double x);
int dr_math_min_int(int a, int b);
int dr_math_max_int(int a, int b);
double dr_math_min_double(double a, double b);
double dr_math_max_double(double a, double b);
```

#### File I/O Operations

```c
// File operations
typedef struct dr_file_t dr_file_t;

dr_file_t* dr_file_open(const char* filename, const char* mode);
void dr_file_close(dr_file_t* file);
char* dr_file_read_all(const char* filename);
bool dr_file_write_all(const char* filename, const char* content);
char* dr_file_read_line(dr_file_t* file);
bool dr_file_write_line(dr_file_t* file, const char* text);
bool dr_file_exists(const char* filename);
size_t dr_file_size(const char* filename);
bool dr_file_delete(const char* filename);
```

**Usage in Dream Code:**
```dream
// String operations
string text = "Hello World";
string upper = text.ToUpper();
string[] words = text.Split(" ");

// Math operations  
double result = Math.Sqrt(16.0);
int max = Math.Max(10, 20);

// File I/O
string content = File.ReadAllText("data.txt");
File.WriteAllText("output.txt", "Hello, File!");
```

## Built-in Classes and Interfaces

### Console Class

```dream
// Complete Console API available in Dream
static class Console {
    // Output methods
    static func void Write(string text);
    static func void WriteLine(string text);
    static func void Write(char c);
    static func void Write(int value);
    static func void Write(float value);
    static func void Write(bool value);
    
    // Input methods
    static func string ReadLine();
    static func char ReadChar();
    static func int ReadInt();
    static func float ReadFloat();
    static func bool ReadBool();
    
    // Console properties
    static func void SetTitle(string title);
    static func void Clear();
    static func void SetCursorPosition(int x, int y);
    static func void SetColor(ConsoleColor foreground, ConsoleColor background);
    static func void ResetColor();
}
```

### Math Class

```dream
// Mathematical constants and functions
static class Math {
    // Constants
    static const double PI = 3.14159265358979323846;
    static const double E = 2.71828182845904523536;
    
    // Basic operations
    static func double Abs(double value);
    static func double Sqrt(double value);
    static func double Pow(double base, double exponent);
    
    // Trigonometric functions
    static func double Sin(double value);
    static func double Cos(double value);
    static func double Tan(double value);
    static func double Asin(double value);
    static func double Acos(double value);
    static func double Atan(double value);
    
    // Logarithmic functions
    static func double Log(double value);
    static func double Log10(double value);
    static func double Exp(double value);
    
    // Rounding functions
    static func double Floor(double value);
    static func double Ceiling(double value);
    static func double Round(double value);
    
    // Min/Max functions
    static func int Min(int a, int b);
    static func int Max(int a, int b);
    static func double Min(double a, double b);
    static func double Max(double a, double b);
}
```

### File Class

```dream
// File system operations
static class File {
    // Read operations
    static func string ReadAllText(string filename);
    static func string[] ReadAllLines(string filename);
    static func byte[] ReadAllBytes(string filename);
    
    // Write operations
    static func void WriteAllText(string filename, string content);
    static func void WriteAllLines(string filename, string[] lines);
    static func void WriteAllBytes(string filename, byte[] data);
    
    // File properties
    static func bool Exists(string filename);
    static func long GetSize(string filename);
    static func DateTime GetLastWriteTime(string filename);
    
    // File management
    static func void Delete(string filename);
    static func void Copy(string source, string destination);
    static func void Move(string source, string destination);
}
```

### StringBuilder Class

```dream
// Efficient string building
class StringBuilder {
    // Constructor
    func StringBuilder();
    func StringBuilder(int capacity);
    
    // Append methods
    func StringBuilder Append(string text);
    func StringBuilder Append(char c);
    func StringBuilder Append(int value);
    func StringBuilder Append(float value);
    func StringBuilder Append(bool value);
    func StringBuilder AppendLine(string text);
    
    // Modification methods
    func StringBuilder Insert(int index, string text);
    func StringBuilder Remove(int start, int length);
    func StringBuilder Replace(string oldValue, string newValue);
    
    // Properties and methods
    func int Length { get; }
    func int Capacity { get; set; }
    func string ToString();
    func void Clear();
}
```

## Runtime Initialization and Cleanup

### Initialization Sequence

```c
// Runtime initialization (automatically called)
void dr_runtime_init(void) {
    dr_memory_init();
    dr_console_init();
    dr_custom_init();
}

// Runtime cleanup (automatically called)
void dr_runtime_cleanup(void) {
    dr_custom_cleanup();
    dr_console_cleanup();
    dr_memory_cleanup();
}
```

### Program Entry Point

Every Dream program requires a main function that gets wrapped by the runtime:

```c
// Generated C wrapper for Dream main function
int main(int argc, char* argv[]) {
    dr_runtime_init();
    
    // User's main function call
    int result = dream_main();
    
    dr_runtime_cleanup();
    return result;
}
```

## Error Handling and Exceptions

### Runtime Error Types

```c
typedef enum {
    DR_ERROR_NONE = 0,
    DR_ERROR_NULL_REFERENCE,
    DR_ERROR_INDEX_OUT_OF_BOUNDS,
    DR_ERROR_DIVISION_BY_ZERO,
    DR_ERROR_STACK_OVERFLOW,
    DR_ERROR_OUT_OF_MEMORY,
    DR_ERROR_INVALID_ARGUMENT,
    DR_ERROR_FILE_NOT_FOUND,
    DR_ERROR_IO_ERROR,
    DR_ERROR_INVALID_CAST,
    DR_ERROR_CUSTOM = 1000
} dr_error_type_t;

// Error handling functions
void dr_throw_error(dr_error_type_t type, const char* message);
void dr_set_error_handler(void (*handler)(dr_error_type_t, const char*));
const char* dr_get_error_message(dr_error_type_t type);
```

### Exception Handling in Dream

```dream
// Exception handling syntax
func void divide_example(int a, int b) {
    try {
        int result = a / b;
        Console.WriteLine("Result: " + result);
    } catch (DivisionByZeroException ex) {
        Console.WriteLine("Error: Cannot divide by zero");
    } catch (Exception ex) {
        Console.WriteLine("Unexpected error: " + ex.Message);
    }
}

// Custom exceptions
class CustomException : Exception {
    func CustomException(string message) : base(message) {
    }
}

func void throw_example() {
    throw new CustomException("Something went wrong!");
}
```

## Performance Optimization

### Memory Pool Configuration

```c
// Configurable memory pools for different object sizes
typedef struct {
    size_t small_object_threshold;  // Objects <= this size use pools
    size_t pool_block_size;        // Size of each pool block
    size_t max_pools;              // Maximum number of pools
    bool enable_cycle_detection;   // Enable circular reference detection
} dr_memory_config_t;

void dr_memory_configure(const dr_memory_config_t* config);
```

### String Optimization

```c
// String interning for frequently used strings
char* dr_string_intern(const char* str);
bool dr_string_is_interned(const char* str);
void dr_string_clear_intern_table(void);

// StringBuilder capacity management
void dr_stringbuilder_reserve(dr_stringbuilder_t* sb, int capacity);
void dr_stringbuilder_shrink_to_fit(dr_stringbuilder_t* sb);
```

## Platform-Specific Features

### Windows-Specific Functions

```c
#ifdef _WIN32
// Windows console functions
void dr_console_set_codepage(int codepage);
void dr_console_enable_ansi(void);
HANDLE dr_console_get_handle(void);
#endif
```

### Linux/Unix-Specific Functions

```c
#ifdef __unix__
// Unix terminal functions
void dr_console_set_raw_mode(bool enable);
void dr_console_get_terminal_size(int* width, int* height);
#endif
```

## Runtime Configuration

### Environment Variables

The Dream runtime respects several environment variables:

| Variable | Description | Default |
|----------|-------------|---------|
| `DREAM_MEMORY_DEBUG` | Enable memory debugging | `false` |
| `DREAM_MEMORY_STATS` | Print memory statistics on exit | `false` |
| `DREAM_POOL_SIZE` | Default memory pool size | `4096` |
| `DREAM_MAX_STRING_LENGTH` | Maximum string length | `1048576` |
| `DREAM_CONSOLE_ENCODING` | Console text encoding | `utf-8` |

### Runtime Configuration API

```c
// Configuration structure
typedef struct {
    bool debug_memory;
    bool print_stats_on_exit;
    size_t default_pool_size;
    size_t max_string_length;
    const char* console_encoding;
} dr_config_t;

// Configuration functions
void dr_set_config(const dr_config_t* config);
dr_config_t dr_get_config(void);
void dr_load_config_from_env(void);
```

## Debugging and Profiling Support

### Debug Builds

In debug builds, additional runtime checks are enabled:

```c
#ifdef DEBUG
    #define DR_DEBUG_MEMORY
    #define DR_DEBUG_BOUNDS_CHECK
    #define DR_DEBUG_NULL_CHECK
    #define DR_DEBUG_STACK_CHECK
#endif
```

### Memory Leak Detection

```c
// Memory leak detection (debug builds only)
void dr_memory_start_leak_detection(void);
void dr_memory_stop_leak_detection(void);
void dr_memory_report_leaks(void);
```

### Performance Profiling

```c
// Built-in profiling support
typedef struct {
    const char* name;
    double total_time;
    int call_count;
} dr_profile_entry_t;

void dr_profile_begin(const char* name);
void dr_profile_end(const char* name);
dr_profile_entry_t* dr_profile_get_results(int* count);
void dr_profile_reset(void);
void dr_profile_print_report(void);
```

## Conclusion

The Dream Runtime Library provides a comprehensive foundation for Dream programs, offering:

- **Automatic Memory Management** - Reference counting with cycle detection
- **Rich I/O Support** - Console, file, and string operations
- **Mathematical Functions** - Complete math library
- **Error Handling** - Structured exception support
- **Performance Optimization** - Memory pools and string interning
- **Cross-Platform Support** - Windows, Linux, and macOS compatibility
- **Debug Support** - Memory leak detection and profiling

All runtime functions are automatically linked when compiling Dream programs, providing a seamless development experience while maintaining high performance and reliability.