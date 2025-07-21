# Memory Management in Dream

Dream uses a sophisticated reference-counted memory management system that provides automatic memory management while maintaining predictable performance characteristics.

## Overview

The Dream runtime implements automatic memory management through reference counting, eliminating the need for manual memory allocation and deallocation while avoiding garbage collection pauses.

### Key Features

- **Automatic Memory Management** - No manual `malloc`/`free` required
- **Reference Counting** - Deterministic cleanup of unused objects
- **Cycle Detection** - Handles circular references automatically
- **Zero-Copy Operations** - Efficient string and array handling
- **Memory Pool Optimization** - Reduces allocation overhead

## Memory Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Dream Memory System                      │
├─────────────────────────────────────────────────────────────┤
│  User Code                                                  │
│    ↓                                                        │
│  Dream Runtime API (dr_alloc, dr_retain, dr_release)       │
│    ↓                                                        │
│  Reference Counter + Memory Pools                          │
│    ↓                                                        │
│  Platform Memory Manager (malloc/free)                     │
└─────────────────────────────────────────────────────────────┘
```

## Reference Counting System

### Basic Principles

Every Dream object maintains a reference count that tracks how many references point to it:

```dream
// Dream code - memory management is automatic
string message = "Hello World";  // ref_count = 1
string copy = message;           // ref_count = 2
copy = "";                       // ref_count = 1
// When message goes out of scope, ref_count = 0, memory freed
```

### Generated C Code

The Dream compiler generates reference counting calls in the output C code:

```c
/* Generated C code for the above Dream example */
char* message = dr_alloc_string("Hello World");  // Allocates with ref_count = 1
char* copy = dr_retain_string(message);          // Increments to ref_count = 2
dr_release_string(copy);                         // Decrements to ref_count = 1
copy = dr_alloc_string("");                      // New allocation
dr_release_string(message);                      // Automatic cleanup at scope end
```

## Memory API Reference

### Core Allocation Functions

```c
// String allocation and management
char* dr_alloc_string(const char* content);
char* dr_retain_string(char* str);
void dr_release_string(char* str);

// Array allocation and management
void* dr_alloc_array(size_t element_size, size_t count);
void* dr_retain_array(void* array);
void dr_release_array(void* array);

// Generic object allocation
void* dr_alloc_object(size_t size);
void* dr_retain_object(void* obj);
void dr_release_object(void* obj);
```

### Memory Pool Functions

```c
// Pool management for high-frequency allocations
dr_pool_t* dr_create_pool(size_t block_size);
void* dr_pool_alloc(dr_pool_t* pool);
void dr_pool_free(dr_pool_t* pool, void* ptr);
void dr_destroy_pool(dr_pool_t* pool);
```

## Automatic Memory Management Examples

### String Operations

```dream
// All memory management is handled automatically
func string_example() {
    string greeting = "Hello";
    string name = "World";
    string message = greeting + " " + name + "!";
    
    // Intermediate strings are automatically cleaned up
    // Only 'message' remains allocated when function returns
    return message;
}

func main() {
    string result = string_example();
    Console.WriteLine(result);
    // 'result' automatically freed at program end
}
```

**Generated C Memory Calls:**
```c
char* string_example() {
    char* greeting = dr_alloc_string("Hello");      // ref_count = 1
    char* name = dr_alloc_string("World");          // ref_count = 1
    
    // String concatenation creates temporary objects
    char* temp1 = dr_string_concat(greeting, " ");  // temp, ref_count = 1
    char* temp2 = dr_string_concat(temp1, name);    // temp, ref_count = 1
    char* message = dr_string_concat(temp2, "!");   // result, ref_count = 1
    
    // Cleanup temporary objects
    dr_release_string(temp1);   // ref_count = 0, freed
    dr_release_string(temp2);   // ref_count = 0, freed
    dr_release_string(greeting); // ref_count = 0, freed
    dr_release_string(name);     // ref_count = 0, freed
    
    return message;  // Returned object has ref_count = 1
}
```

### Array Management

```dream
func array_example() {
    int[] numbers = [1, 2, 3, 4, 5];
    int[] doubled = new int[numbers.Length];
    
    for (int i = 0; i < numbers.Length; i++) {
        doubled[i] = numbers[i] * 2;
    }
    
    return doubled;  // Original 'numbers' array automatically freed
}
```

**Generated C Memory Calls:**
```c
int* array_example() {
    // Allocate array with reference count
    int* numbers = dr_alloc_array(sizeof(int), 5);
    numbers[0] = 1; numbers[1] = 2; numbers[2] = 3; numbers[3] = 4; numbers[4] = 5;
    
    int* doubled = dr_alloc_array(sizeof(int), 5);
    
    for (int i = 0; i < 5; i++) {
        doubled[i] = numbers[i] * 2;
    }
    
    dr_release_array(numbers);  // Cleanup original array
    return doubled;             // Return with ref_count = 1
}
```

### Class and Object Management

```dream
class Person {
    string name;
    int age;
    
    func Person(string name, int age) {
        this.name = name;
        this.age = age;
    }
    
    func string GetDescription() {
        return name + " is " + age + " years old";
    }
}

func object_example() {
    Person person = new Person("Alice", 30);
    string description = person.GetDescription();
    // 'person' object automatically freed when out of scope
    return description;
}
```

**Generated C Memory Calls:**
```c
typedef struct {
    int ref_count;
    char* name;
    int age;
} Person;

Person* Person_new(char* name, int age) {
    Person* obj = dr_alloc_object(sizeof(Person));
    obj->name = dr_retain_string(name);
    obj->age = age;
    return obj;
}

void Person_release(Person* obj) {
    if (--obj->ref_count == 0) {
        dr_release_string(obj->name);
        dr_release_object(obj);
    }
}

char* object_example() {
    char* name = dr_alloc_string("Alice");
    Person* person = Person_new(name, 30);
    char* description = Person_GetDescription(person);
    
    Person_release(person);  // Automatic cleanup
    dr_release_string(name);
    
    return description;
}
```

## Circular Reference Handling

Dream's memory system includes cycle detection to handle circular references:

```dream
class Node {
    string value;
    Node next;
    
    func Node(string value) {
        this.value = value;
    }
}

func circular_reference_example() {
    Node first = new Node("First");
    Node second = new Node("Second");
    
    first.next = second;
    second.next = first;  // Creates circular reference
    
    // Automatic cycle detection ensures both objects are freed
    // when they go out of scope, despite circular references
}
```

## Memory Optimization Strategies

### 1. String Interning

```dream
// Frequently used strings are automatically interned
string constant1 = "Hello World";
string constant2 = "Hello World";
// Both variables point to same memory location
```

### 2. Copy-on-Write Arrays

```dream
int[] original = [1, 2, 3, 4, 5];
int[] copy = original;        // Shallow copy, shared data
copy[0] = 999;               // Triggers copy-on-write
// 'original' unchanged, 'copy' has its own data
```

### 3. Small Object Optimization

```dream
// Objects under 64 bytes use optimized allocation pools
struct Point {
    float x, y;  // 8 bytes total - uses fast pool allocation
}
```

## Performance Characteristics

### Time Complexity

| Operation | Time Complexity | Notes |
|-----------|----------------|--------|
| Object Allocation | O(1) | Using memory pools |
| Reference Increment | O(1) | Simple counter increment |
| Reference Decrement | O(1) | Simple counter decrement |
| Cycle Detection | O(n) | Only triggered when needed |
| String Concatenation | O(n+m) | Where n, m are string lengths |

### Memory Overhead

- **Reference Count**: 4 bytes per object
- **Pool Metadata**: ~1% of total allocated memory
- **Cycle Detection**: Temporary overhead during collection

## Best Practices

### 1. Minimize Object Creation in Loops

```dream
// Good: Create objects outside loop when possible
string prefix = "Item ";
for (int i = 0; i < 1000; i++) {
    string item = prefix + i;  // Only creates one new string per iteration
    Process(item);
}

// Avoid: Creating multiple temporary objects
for (int i = 0; i < 1000; i++) {
    string item = "Item " + i;  // Creates temporary string objects
    Process(item);
}
```

### 2. Use StringBuilder for Multiple Concatenations

```dream
// Good: Use StringBuilder for multiple string operations
StringBuilder sb = new StringBuilder();
for (int i = 0; i < 100; i++) {
    sb.Append("Item ");
    sb.Append(i);
    sb.Append(", ");
}
string result = sb.ToString();

// Avoid: Multiple string concatenations
string result = "";
for (int i = 0; i < 100; i++) {
    result = result + "Item " + i + ", ";  // Creates many temporary strings
}
```

### 3. Explicit Cleanup for Large Objects

```dream
func process_large_data() {
    LargeObject data = LoadLargeData();
    ProcessData(data);
    // data automatically cleaned up at scope end
    
    // For very large objects, explicit cleanup may be beneficial:
    // data = null;  // Triggers immediate cleanup
}
```

## Memory Debugging and Profiling

### Debug Build Memory Tracking

In debug builds, Dream provides memory usage statistics:

```bash
# Compile with debug memory tracking
zig build run -Ddebug-memory -- myprogram.dr

# Output includes memory statistics:
# Total allocations: 1,247
# Peak memory usage: 2.1 MB
# Active objects at exit: 0
# Memory pools used: 3
```

### Memory Leak Detection

```dream
// Debug builds automatically detect memory leaks
func main() {
    string test = "Memory leak test";
    // If 'test' is not properly cleaned up, debug build reports:
    // Warning: 1 string object(s) not released at program exit
}
```

## Integration with C Code

When interfacing with C libraries, Dream provides manual memory management functions:

```dream
// C interop memory management
extern func void* malloc(int size);
extern func void free(void* ptr);

func unsafe_c_interop() {
    // Manual memory management for C interop
    void* c_buffer = malloc(1024);
    // ... use c_buffer with C functions ...
    free(c_buffer);
}
```

## Conclusion

Dream's reference-counted memory management provides:

- **Predictable Performance** - No garbage collection pauses
- **Automatic Cleanup** - No manual memory management needed
- **Safety** - Prevents memory leaks and dangling pointers
- **Efficiency** - Low overhead reference counting with optimizations

The system is designed to be invisible to the programmer while providing the performance characteristics needed for systems programming applications.