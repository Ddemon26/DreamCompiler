# C Interoperability in Dream

Dream provides comprehensive interoperability with C libraries, allowing seamless integration with existing C codebases and system libraries. This enables Dream programs to leverage the vast ecosystem of C libraries while maintaining type safety and memory management.

## Overview

Dream's C interop system allows you to:

- **Call C functions** from Dream code
- **Use C structs and types** in Dream programs  
- **Pass Dream objects** to C functions safely
- **Link with C libraries** at compile time
- **Handle C callbacks** and function pointers
- **Manage mixed memory** between Dream and C

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Dream ↔ C Interop                       │
├─────────────────────────────────────────────────────────────┤
│  Dream Code (.dr files)                                    │
│    ↓                                                        │
│  Dream Interop Layer (type marshaling, safety checks)      │
│    ↓                                                        │
│  Generated C Code with extern declarations                  │
│    ↓                                                        │
│  C Libraries (static .a or dynamic .so/.dll)               │
│    ↓                                                        │
│  System APIs (OS, hardware, drivers)                       │
└─────────────────────────────────────────────────────────────┘
```

## Basic C Function Calls

### Simple Function Declarations

```dream
// Declare C functions using 'extern' keyword
extern func int add(int a, int b);
extern func double sqrt(double x);
extern func void printf(string format, ...);

func main() {
    int result = add(5, 3);        // Calls C function
    double root = sqrt(16.0);      // Calls C math function
    printf("Result: %d\n", result); // Calls C printf
}
```

**Generated C Code:**
```c
// External declarations generated automatically
extern int add(int a, int b);
extern double sqrt(double x);
extern int printf(const char* format, ...);

int dream_main(void) {
    int result = add(5, 3);
    double root = sqrt(16.0);
    printf("Result: %d\n", result);
    return 0;
}
```

### Standard Library Functions

Dream provides convenient wrappers for common C standard library functions:

```dream
// Standard C library functions
extern func void* malloc(int size);
extern func void free(void* ptr);
extern func void* memcpy(void* dest, void* src, int n);
extern func int strlen(string str);
extern func int strcmp(string a, string b);

// File operations
extern func FILE* fopen(string filename, string mode);
extern func int fclose(FILE* file);
extern func int fprintf(FILE* file, string format, ...);
extern func string fgets(string buffer, int size, FILE* file);

func file_example() {
    FILE* file = fopen("data.txt", "r");
    if (file != null) {
        char[256] buffer;
        string line = fgets(buffer, 256, file);
        printf("Read: %s", line);
        fclose(file);
    }
}
```

## Type Marshaling

### Primitive Type Mapping

Dream automatically handles type conversion between Dream and C types:

| Dream Type | C Type | Notes |
|------------|--------|--------|
| `int` | `int` | 32-bit signed integer |
| `long` | `long long` | 64-bit signed integer |
| `float` | `double` | 64-bit floating point |
| `bool` | `int` | 0 = false, non-zero = true |
| `char` | `char` | Single byte character |
| `string` | `const char*` | Null-terminated string |
| `void*` | `void*` | Generic pointer |

### String Handling

Dream strings are automatically converted to C strings when passed to C functions:

```dream
extern func int system(string command);
extern func int puts(string str);
extern func string getenv(string name);

func string_interop_example() {
    // Dream string automatically converted to const char*
    int result = system("ls -la");
    puts("Hello from C!");
    
    // C string automatically converted to Dream string
    string path = getenv("PATH");
    Console.WriteLine("PATH: " + path);
}
```

**Generated C Code:**
```c
int string_interop_example(void) {
    int result = system("ls -la");  // Dream string literal → const char*
    puts("Hello from C!");
    
    // C string return value wrapped in Dream string
    const char* c_path = getenv("PATH");
    char* path = dr_alloc_string(c_path);  // Convert to managed Dream string
    
    char* temp = dr_string_concat("PATH: ", path);
    dr_console_writeline(temp);
    
    dr_release_string(path);
    dr_release_string(temp);
    return 0;
}
```

### Array Interop

Dream arrays can be passed to C functions as pointers:

```dream
extern func void qsort(void* base, int nmemb, int size, 
                      func int(void*, void*) compare);

func int compare_ints(void* a, void* b) {
    int* x = (int*)a;
    int* y = (int*)b;
    return *x - *y;
}

func array_interop_example() {
    int[] numbers = [64, 34, 25, 12, 22, 11, 90];
    
    // Pass Dream array to C qsort function
    qsort(numbers, numbers.Length, sizeof(int), compare_ints);
    
    for (int i = 0; i < numbers.Length; i++) {
        Console.WriteLine(numbers[i]);
    }
}
```

## Struct and Type Definitions

### C Struct Integration

Define C structs for use in Dream code:

```dream
// Define C struct layout
struct Point {
    float x;
    float y;
}

struct Color {
    byte r, g, b, a;
}

// Declare C functions that use the structs
extern func Point create_point(float x, float y);
extern func float distance(Point a, Point b);
extern func void draw_pixel(Point pos, Color color);

func graphics_example() {
    Point p1 = create_point(10.0, 20.0);
    Point p2 = create_point(30.0, 40.0);
    
    float dist = distance(p1, p2);
    Console.WriteLine("Distance: " + dist);
    
    Color red = Color { r: 255, g: 0, b: 0, a: 255 };
    draw_pixel(p1, red);
}
```

**Generated C Code:**
```c
typedef struct {
    float x;
    float y;
} Point;

typedef struct {
    unsigned char r, g, b, a;
} Color;

extern Point create_point(float x, float y);
extern float distance(Point a, Point b);
extern void draw_pixel(Point pos, Color color);

int graphics_example(void) {
    Point p1 = create_point(10.0f, 20.0f);
    Point p2 = create_point(30.0f, 40.0f);
    
    float dist = distance(p1, p2);
    // Distance converted to string for output...
    
    Color red = {255, 0, 0, 255};
    draw_pixel(p1, red);
    return 0;
}
```

### Complex Data Structures

Handle more complex C data structures:

```dream
// File handle wrapper
struct FILE;  // Opaque type declaration

// Linked list node
struct ListNode {
    int data;
    ListNode* next;
}

extern func ListNode* create_node(int data);
extern func void append_node(ListNode** head, int data);
extern func void print_list(ListNode* head);
extern func void free_list(ListNode* head);

func linked_list_example() {
    ListNode* head = null;
    
    append_node(&head, 10);
    append_node(&head, 20);
    append_node(&head, 30);
    
    print_list(head);
    free_list(head);
}
```

## Function Pointers and Callbacks

### C Callbacks from Dream

Define Dream functions that can be called from C:

```dream
// Function type for C callback
typedef func int CompareFunc(void* a, void* b);

extern func void qsort(void* base, int nmemb, int size, CompareFunc compare);

func int int_compare(void* a, void* b) {
    int x = *(int*)a;
    int y = *(int*)b;
    return x - y;
}

func int string_compare(void* a, void* b) {
    string x = *(string*)a;
    string y = *(string*)b;
    return strcmp(x, y);  // Use C strcmp
}

func callback_example() {
    int[] numbers = [5, 2, 8, 1, 9];
    qsort(numbers, numbers.Length, sizeof(int), int_compare);
    
    string[] names = ["Charlie", "Alice", "Bob"];
    qsort(names, names.Length, sizeof(string), string_compare);
}
```

### Event Handling

Implement C-style event handling with function pointers:

```dream
// Event handler types
typedef func void EventHandler(int event_type, void* data);
typedef func void TimerCallback(int timer_id);

extern func void set_event_handler(EventHandler handler);
extern func int create_timer(int interval, TimerCallback callback);
extern func void process_events();

func void on_event(int event_type, void* data) {
    switch (event_type) {
        case 1: Console.WriteLine("Mouse click event"); break;
        case 2: Console.WriteLine("Keyboard event"); break;
        case 3: Console.WriteLine("Window close event"); break;
    }
}

func void on_timer(int timer_id) {
    Console.WriteLine("Timer " + timer_id + " fired!");
}

func event_example() {
    set_event_handler(on_event);
    int timer = create_timer(1000, on_timer);  // 1 second timer
    
    // Process events for 10 seconds
    for (int i = 0; i < 10; i++) {
        process_events();
        sleep(1000);  // Sleep 1 second
    }
}
```

## Memory Management in Interop

### Safe Memory Handling

Dream provides safe wrappers for C memory operations:

```dream
// Safe C memory allocation wrappers
func void* safe_malloc(int size) {
    void* ptr = malloc(size);
    if (ptr == null) {
        throw new OutOfMemoryException("Failed to allocate " + size + " bytes");
    }
    return ptr;
}

func void safe_free(void* ptr) {
    if (ptr != null) {
        free(ptr);
    }
}

// RAII-style resource management
class CBuffer {
    void* ptr;
    int size;
    
    func CBuffer(int size) {
        this.size = size;
        this.ptr = safe_malloc(size);
    }
    
    ~CBuffer() {  // Destructor automatically called
        safe_free(ptr);
    }
    
    func void* GetPointer() { return ptr; }
    func int GetSize() { return size; }
}

func memory_safety_example() {
    CBuffer buffer = new CBuffer(1024);  // Automatic cleanup when out of scope
    
    // Use buffer.GetPointer() for C interop
    memset(buffer.GetPointer(), 0, buffer.GetSize());
    
    // No need to manually free - destructor handles it
}
```

### Mixing Dream and C Memory

When mixing Dream-managed and C-allocated memory:

```dream
extern func char* strdup(string str);  // C function that allocates memory

func mixed_memory_example() {
    string dream_string = "Hello, World!";
    
    // C function allocates memory - must free manually
    char* c_copy = strdup(dream_string);
    
    // Convert back to Dream string (creates managed copy)
    string dream_copy = new string(c_copy);
    
    // Free the C-allocated memory
    free(c_copy);
    
    Console.WriteLine(dream_copy);
    // dream_copy automatically freed by Dream runtime
}
```

## Advanced Interop Scenarios

### Dynamic Library Loading

Load C libraries at runtime:

```dream
extern func void* dlopen(string filename, int flags);
extern func void* dlsym(void* handle, string symbol);
extern func int dlclose(void* handle);
extern func string dlerror();

// Constants for dlopen flags
const int RTLD_LAZY = 1;
const int RTLD_NOW = 2;

func dynamic_loading_example() {
    // Load math library dynamically
    void* handle = dlopen("libm.so", RTLD_LAZY);
    if (handle == null) {
        Console.WriteLine("Error: " + dlerror());
        return;
    }
    
    // Get function pointer
    typedef func double MathFunc(double);
    MathFunc sin_func = (MathFunc)dlsym(handle, "sin");
    
    if (sin_func != null) {
        double result = sin_func(3.14159 / 2.0);
        Console.WriteLine("sin(π/2) = " + result);
    }
    
    dlclose(handle);
}
```

### Platform-Specific APIs

Access platform-specific C APIs:

```dream
#ifdef WINDOWS
    extern func int MessageBoxA(void* hwnd, string text, string caption, int type);
    extern func void* GetStdHandle(int handle_type);
    extern func int WriteConsoleA(void* handle, string text, int length, int* written, void* reserved);
    
    func windows_specific() {
        MessageBoxA(null, "Hello from Dream!", "Dream Program", 0);
        
        void* console = GetStdHandle(-11);  // STD_OUTPUT_HANDLE
        int written;
        WriteConsoleA(console, "Direct console output", 21, &written, null);
    }
#endif

#ifdef LINUX
    extern func int open(string pathname, int flags);
    extern func int read(int fd, void* buf, int count);
    extern func int write(int fd, void* buf, int count);
    extern func int close(int fd);
    
    func linux_specific() {
        int fd = open("/proc/version", 0);  // O_RDONLY
        if (fd >= 0) {
            char[256] buffer;
            int bytes = read(fd, buffer, 255);
            buffer[bytes] = '\0';
            Console.WriteLine("Kernel version: " + buffer);
            close(fd);
        }
    }
#endif
```

### Interfacing with C++

Call C++ functions through C wrappers:

```dream
// C++ class wrapper (implemented in C++)
extern func void* create_vector();
extern func void delete_vector(void* vector);
extern func void vector_push_back(void* vector, int value);
extern func int vector_size(void* vector);
extern func int vector_get(void* vector, int index);

class CppVector {
    void* cpp_object;
    
    func CppVector() {
        cpp_object = create_vector();
    }
    
    ~CppVector() {
        delete_vector(cpp_object);
    }
    
    func void Add(int value) {
        vector_push_back(cpp_object, value);
    }
    
    func int Size() {
        return vector_size(cpp_object);
    }
    
    func int Get(int index) {
        return vector_get(cpp_object, index);
    }
}

func cpp_interop_example() {
    CppVector vec = new CppVector();
    vec.Add(10);
    vec.Add(20);
    vec.Add(30);
    
    for (int i = 0; i < vec.Size(); i++) {
        Console.WriteLine("vec[" + i + "] = " + vec.Get(i));
    }
}
```

## Build Integration

### Linking with C Libraries

Specify C libraries to link during compilation:

```bash
# Compile with C library linking
zig build -Dlink-libc -Dlink-libm -- myprogram.dr

# Link specific libraries
zig build -Dextra-libs="pthread,ssl,crypto" -- myprogram.dr

# Static linking
zig build -Dstatic-libs="/path/to/libfoo.a,/path/to/libbar.a" -- myprogram.dr
```

### Build Configuration

Configure C interop in `build.zig`:

```zig
// build.zig configuration for C interop
const std = @import("std");

pub fn build(b: *std.Build) void {
    // ... existing build configuration ...
    
    // Add C library linking
    exe.linkLibC();
    exe.linkSystemLibrary("m");        // Math library
    exe.linkSystemLibrary("pthread");  // POSIX threads
    exe.linkSystemLibrary("ssl");      // OpenSSL
    
    // Add include paths
    exe.addIncludePath(.{ .path = "include" });
    exe.addIncludePath(.{ .path = "/usr/local/include" });
    
    // Add library paths
    exe.addLibraryPath(.{ .path = "/usr/local/lib" });
    
    // Static libraries
    exe.addObjectFile(.{ .path = "libs/libcustom.a" });
}
```

### Header Generation

Dream can generate C headers for Dream functions that need to be called from C:

```dream
// Mark functions for C export
[CExport]
func int dream_add(int a, int b) {
    return a + b;
}

[CExport]
func string dream_greet(string name) {
    return "Hello, " + name + "!";
}

// Generated C header (dream_exports.h):
/*
extern int dream_add(int a, int b);
extern const char* dream_greet(const char* name);
*/
```

## Best Practices

### 1. Type Safety

Always use proper type declarations and avoid raw pointers when possible:

```dream
// Good: Use proper types
extern func FILE* fopen(string filename, string mode);
FILE* file = fopen("data.txt", "r");

// Avoid: Raw void pointers without type information
extern func void* raw_function(void* data);
```

### 2. Error Handling

Check return values from C functions and handle errors appropriately:

```dream
extern func int some_c_function(int param);

func safe_c_call() {
    int result = some_c_function(42);
    if (result < 0) {
        throw new Exception("C function failed with code: " + result);
    }
}
```

### 3. Memory Management

Be explicit about memory ownership when mixing Dream and C memory:

```dream
// Clear memory ownership
func string process_c_string(string input) {
    char* c_str = to_c_string(input);    // Dream→C conversion
    char* result = c_process_string(c_str);  // C function allocates
    string dream_result = from_c_string(result);  // C→Dream conversion
    free(result);                        // Free C-allocated memory
    return dream_result;                 // Return Dream-managed string
}
```

### 4. Platform Abstraction

Use conditional compilation for platform-specific code:

```dream
#ifdef WINDOWS
    const string LIBRARY_EXTENSION = ".dll";
#else
    const string LIBRARY_EXTENSION = ".so";
#endif

func string get_library_name(string base_name) {
    return base_name + LIBRARY_EXTENSION;
}
```

## Conclusion

Dream's C interoperability provides powerful capabilities for:

- **Legacy Integration** - Use existing C libraries and codebases
- **System Programming** - Access low-level system APIs
- **Performance** - Leverage optimized C libraries
- **Ecosystem Access** - Use the vast C library ecosystem
- **Gradual Migration** - Incrementally port C code to Dream

The interop system maintains Dream's safety guarantees while providing seamless access to C functionality, making it practical for real-world systems programming tasks.