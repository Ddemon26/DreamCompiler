# Dream Language API Reference

This comprehensive API reference covers all built-in classes, functions, and interfaces available in the Dream programming language.

## Table of Contents

1. [Console API](#console-api)
2. [Math API](#math-api)
3. [String API](#string-api)
4. [File System API](#file-system-api)
5. [Collections API](#collections-api)
6. [DateTime API](#datetime-api)
7. [Exception Handling](#exception-handling)
8. [Memory Management API](#memory-management-api)
9. [Threading API](#threading-api)
10. [Utility Functions](#utility-functions)

---

## Console API

The `Console` class provides methods for input and output operations to the console.

### Console Output Methods

#### `Console.Write(value)`
Writes a value to the console without a newline.

**Overloads:**
```dream
static func void Write(string text);
static func void Write(int value);
static func void Write(float value);
static func void Write(bool value);
static func void Write(char value);
static func void Write(object value);
```

**Example:**
```dream
func main() {
    Console.Write("Hello ");
    Console.Write("World");        // Output: Hello World
    Console.Write(42);            // Output: Hello World42
    Console.Write(3.14);          // Output: Hello World423.14
}
```

#### `Console.WriteLine(value)`
Writes a value to the console followed by a newline.

**Overloads:**
```dream
static func void WriteLine(string text);
static func void WriteLine(int value);
static func void WriteLine(float value);
static func void WriteLine(bool value);
static func void WriteLine(char value);
static func void WriteLine(object value);
static func void WriteLine();  // Empty line
```

**Example:**
```dream
func main() {
    Console.WriteLine("First line");
    Console.WriteLine(123);
    Console.WriteLine(true);
    Console.WriteLine();  // Empty line
    Console.WriteLine("After empty line");
}
```

**Output:**
```
First line
123
true

After empty line
```

#### `Console.WriteFormat(format, args...)`
Writes formatted text to the console.

**Signature:**
```dream
static func void WriteFormat(string format, params object[] args);
static func void WriteLineFormat(string format, params object[] args);
```

**Format Specifiers:**
- `{0}, {1}, {2}...` - Positional arguments
- `{0:d}` - Integer formatting
- `{0:f}` - Float formatting (default 2 decimal places)
- `{0:f4}` - Float with 4 decimal places
- `{0:x}` - Hexadecimal formatting

**Example:**
```dream
func main() {
    string name = "Alice";
    int age = 30;
    float height = 5.75;
    
    Console.WriteFormat("Name: {0}, Age: {1}, Height: {2:f1}m\n", name, age, height);
    // Output: Name: Alice, Age: 30, Height: 5.8m
    
    Console.WriteLineFormat("Hex value: 0x{0:x}", 255);
    // Output: Hex value: 0xff
}
```

### Console Input Methods

#### `Console.ReadLine()`
Reads a line of input from the console.

**Signature:**
```dream
static func string ReadLine();
static func string ReadLine(string prompt);
```

**Example:**
```dream
func main() {
    Console.WriteLine("Enter your name:");
    string name = Console.ReadLine();
    
    int age = Convert.ToInt(Console.ReadLine("Enter your age: "));
    
    Console.WriteLine($"Hello {name}, you are {age} years old!");
}
```

#### `Console.ReadKey()`
Reads a single key press from the console.

**Signature:**
```dream
static func char ReadKey();
static func char ReadKey(bool intercept);
```

**Parameters:**
- `intercept` - If true, the pressed key is not displayed

**Example:**
```dream
func main() {
    Console.WriteLine("Press any key to continue...");
    char key = Console.ReadKey(true);
    Console.WriteLine($"You pressed: {key}");
}
```

### Console Configuration

#### `Console.Clear()`
Clears the console screen.

**Signature:**
```dream
static func void Clear();
```

#### `Console.SetTitle(title)`
Sets the console window title.

**Signature:**
```dream
static func void SetTitle(string title);
```

**Example:**
```dream
func main() {
    Console.SetTitle("My Dream Application");
    Console.WriteLine("Title set!");
}
```

#### `Console.SetCursorPosition(x, y)`
Sets the cursor position in the console.

**Signature:**
```dream
static func void SetCursorPosition(int left, int top);
static func Point GetCursorPosition();
```

**Example:**
```dream
func main() {
    Console.SetCursorPosition(10, 5);
    Console.WriteLine("This text appears at position (10, 5)");
    
    Point pos = Console.GetCursorPosition();
    Console.WriteLine($"Current position: {pos.X}, {pos.Y}");
}
```

#### Console Colors

**Enumerations:**
```dream
enum ConsoleColor {
    Black, DarkBlue, DarkGreen, DarkCyan, DarkRed, DarkMagenta,
    DarkYellow, Gray, DarkGray, Blue, Green, Cyan, Red, Magenta,
    Yellow, White
}
```

**Methods:**
```dream
static func void SetForegroundColor(ConsoleColor color);
static func void SetBackgroundColor(ConsoleColor color);
static func void SetColor(ConsoleColor foreground, ConsoleColor background);
static func void ResetColor();
```

**Example:**
```dream
func main() {
    Console.SetForegroundColor(ConsoleColor.Red);
    Console.WriteLine("This text is red");
    
    Console.SetColor(ConsoleColor.Yellow, ConsoleColor.Blue);
    Console.WriteLine("Yellow text on blue background");
    
    Console.ResetColor();
    Console.WriteLine("Back to normal colors");
}
```

---

## Math API

The `Math` class provides mathematical functions and constants.

### Mathematical Constants

```dream
static class Math {
    static const double PI = 3.14159265358979323846;
    static const double E = 2.71828182845904523536;
    static const double TAU = 6.28318530717958647693;  // 2 * PI
}
```

### Basic Mathematical Functions

#### `Math.Abs(value)`
Returns the absolute value of a number.

**Overloads:**
```dream
static func int Abs(int value);
static func long Abs(long value);
static func float Abs(float value);
static func double Abs(double value);
```

**Example:**
```dream
func main() {
    Console.WriteLine(Math.Abs(-42));      // Output: 42
    Console.WriteLine(Math.Abs(-3.14));    // Output: 3.14
    Console.WriteLine(Math.Abs(5));        // Output: 5
}
```

#### `Math.Min(a, b)` and `Math.Max(a, b)`
Returns the minimum or maximum of two values.

**Overloads:**
```dream
static func int Min(int a, int b);
static func float Min(float a, float b);
static func double Min(double a, double b);
static func int Max(int a, int b);
static func float Max(float a, float b);
static func double Max(double a, double b);
```

**Example:**
```dream
func main() {
    Console.WriteLine(Math.Min(5, 3));     // Output: 3
    Console.WriteLine(Math.Max(5, 3));     // Output: 5
    Console.WriteLine(Math.Min(2.5, 2.1)); // Output: 2.1
}
```

#### `Math.Clamp(value, min, max)`
Clamps a value between a minimum and maximum.

**Signature:**
```dream
static func int Clamp(int value, int min, int max);
static func float Clamp(float value, float min, float max);
static func double Clamp(double value, double min, double max);
```

**Example:**
```dream
func main() {
    Console.WriteLine(Math.Clamp(15, 0, 10));   // Output: 10
    Console.WriteLine(Math.Clamp(-5, 0, 10));   // Output: 0
    Console.WriteLine(Math.Clamp(7, 0, 10));    // Output: 7
}
```

### Power and Root Functions

#### `Math.Pow(base, exponent)`
Returns base raised to the power of exponent.

**Signature:**
```dream
static func double Pow(double base, double exponent);
```

**Example:**
```dream
func main() {
    Console.WriteLine(Math.Pow(2, 3));     // Output: 8
    Console.WriteLine(Math.Pow(4, 0.5));   // Output: 2 (square root)
    Console.WriteLine(Math.Pow(27, 1.0/3.0)); // Output: 3 (cube root)
}
```

#### `Math.Sqrt(value)`
Returns the square root of a number.

**Signature:**
```dream
static func double Sqrt(double value);
```

**Example:**
```dream
func main() {
    Console.WriteLine(Math.Sqrt(16));      // Output: 4
    Console.WriteLine(Math.Sqrt(2));       // Output: 1.4142135623731
    Console.WriteLine(Math.Sqrt(0));       // Output: 0
}
```

### Trigonometric Functions

#### Basic Trigonometric Functions
```dream
static func double Sin(double radians);
static func double Cos(double radians);
static func double Tan(double radians);
```

#### Inverse Trigonometric Functions
```dream
static func double Asin(double value);    // Arc sine
static func double Acos(double value);    // Arc cosine
static func double Atan(double value);    // Arc tangent
static func double Atan2(double y, double x); // Arc tangent of y/x
```

**Example:**
```dream
func main() {
    double angle = Math.PI / 4.0;  // 45 degrees in radians
    
    Console.WriteLine(Math.Sin(angle));    // Output: 0.7071067811865476
    Console.WriteLine(Math.Cos(angle));    // Output: 0.7071067811865476
    Console.WriteLine(Math.Tan(angle));    // Output: 1.0
    
    // Convert back
    double result = Math.Asin(0.7071067811865476);
    Console.WriteLine(result * 180 / Math.PI); // Output: ~45 degrees
}
```

### Logarithmic and Exponential Functions

```dream
static func double Log(double value);      // Natural logarithm (base e)
static func double Log10(double value);    // Base-10 logarithm
static func double Log2(double value);     // Base-2 logarithm
static func double Exp(double value);      // e raised to the power of value
```

**Example:**
```dream
func main() {
    Console.WriteLine(Math.Log(Math.E));     // Output: 1.0
    Console.WriteLine(Math.Log10(100));      // Output: 2.0
    Console.WriteLine(Math.Log2(8));         // Output: 3.0
    Console.WriteLine(Math.Exp(1));          // Output: 2.718281828459045 (e)
}
```

### Rounding Functions

```dream
static func double Floor(double value);    // Round down to nearest integer
static func double Ceiling(double value);  // Round up to nearest integer
static func double Round(double value);    // Round to nearest integer
static func double Round(double value, int digits); // Round to specified decimal places
```

**Example:**
```dream
func main() {
    double value = 3.7;
    
    Console.WriteLine(Math.Floor(value));    // Output: 3
    Console.WriteLine(Math.Ceiling(value));  // Output: 4
    Console.WriteLine(Math.Round(value));    // Output: 4
    
    Console.WriteLine(Math.Round(3.14159, 2)); // Output: 3.14
}
```

---

## String API

Dream provides extensive string manipulation capabilities through built-in methods and the `String` class.

### String Properties

```dream
class string {
    int Length { get; }           // Number of characters in string
    bool IsEmpty { get; }         // True if string is empty
    bool IsWhiteSpace { get; }    // True if string contains only whitespace
}
```

### String Indexing and Access

```dream
// Access individual characters
string text = "Hello";
char first = text[0];            // 'H'
char last = text[text.Length - 1]; // 'o'

// String slicing
string substring = text[1:4];    // "ell"
string suffix = text[2:];        // "llo"
string prefix = text[:3];        // "Hel"
```

### String Comparison

```dream
class string {
    bool Equals(string other);
    bool EqualsIgnoreCase(string other);
    int CompareTo(string other);
    int CompareToIgnoreCase(string other);
}
```

**Example:**
```dream
func main() {
    string a = "hello";
    string b = "Hello";
    string c = "world";
    
    Console.WriteLine(a.Equals(b));           // false
    Console.WriteLine(a.EqualsIgnoreCase(b)); // true
    Console.WriteLine(a.CompareTo(c));        // -1 (a < c)
}
```

### String Search Methods

```dream
class string {
    bool Contains(string value);
    bool StartsWith(string value);
    bool EndsWith(string value);
    int IndexOf(string value);
    int IndexOf(string value, int startIndex);
    int LastIndexOf(string value);
    int[] FindAll(string pattern);
}
```

**Example:**
```dream
func main() {
    string text = "The quick brown fox jumps over the lazy dog";
    
    Console.WriteLine(text.Contains("fox"));      // true
    Console.WriteLine(text.StartsWith("The"));    // true
    Console.WriteLine(text.EndsWith("dog"));      // true
    Console.WriteLine(text.IndexOf("the"));       // 31 (second occurrence)
    Console.WriteLine(text.LastIndexOf("the"));   // 31
}
```

### String Modification Methods

```dream
class string {
    string ToUpper();
    string ToLower();
    string Trim();
    string TrimStart();
    string TrimEnd();
    string Substring(int start);
    string Substring(int start, int length);
    string Replace(string oldValue, string newValue);
    string Remove(int startIndex);
    string Remove(int startIndex, int count);
    string Insert(int startIndex, string value);
}
```

**Example:**
```dream
func main() {
    string text = "  Hello World  ";
    
    Console.WriteLine("'" + text.Trim() + "'");           // 'Hello World'
    Console.WriteLine(text.ToUpper());                    // '  HELLO WORLD  '
    Console.WriteLine(text.Replace("World", "Dream"));    // '  Hello Dream  '
    Console.WriteLine(text.Substring(2, 5));              // 'Hello'
    Console.WriteLine(text.Insert(7, "Beautiful "));      // '  Hello Beautiful World  '
}
```

### String Splitting and Joining

```dream
class string {
    string[] Split(char separator);
    string[] Split(string separator);
    string[] Split(char[] separators);
    string[] Split(string[] separators, StringSplitOptions options);
}

static class String {
    static string Join(string separator, string[] values);
    static string Join(char separator, string[] values);
    static string Concat(params string[] values);
}
```

**Example:**
```dream
func main() {
    string csv = "apple,banana,cherry,date";
    string[] fruits = csv.Split(',');
    
    for (string fruit in fruits) {
        Console.WriteLine("Fruit: " + fruit);
    }
    
    string rejoined = String.Join(" | ", fruits);
    Console.WriteLine(rejoined); // apple | banana | cherry | date
    
    string sentence = String.Concat("I", " like", " fruits!");
    Console.WriteLine(sentence); // I like fruits!
}
```

### String Formatting and Interpolation

```dream
// String interpolation
string name = "Alice";
int age = 30;
string message = $"Hello {name}, you are {age} years old!";

// String formatting
string formatted = String.Format("Name: {0}, Age: {1}", name, age);

// Advanced formatting
double value = 123.456789;
string formatted2 = String.Format("Value: {0:F2}", value); // Value: 123.46
```

### StringBuilder Class

For efficient string building operations:

```dream
class StringBuilder {
    // Constructors
    StringBuilder();
    StringBuilder(int capacity);
    StringBuilder(string value);
    
    // Properties
    int Length { get; set; }
    int Capacity { get; set; }
    
    // Methods
    StringBuilder Append(string value);
    StringBuilder Append(char value);
    StringBuilder Append(int value);
    StringBuilder Append(float value);
    StringBuilder AppendLine(string value);
    StringBuilder AppendFormat(string format, params object[] args);
    
    StringBuilder Insert(int index, string value);
    StringBuilder Remove(int startIndex, int length);
    StringBuilder Replace(string oldValue, string newValue);
    StringBuilder Replace(char oldChar, char newChar);
    
    StringBuilder Clear();
    string ToString();
}
```

**Example:**
```dream
func main() {
    StringBuilder sb = new StringBuilder();
    
    sb.AppendLine("Header");
    sb.Append("Item 1: ");
    sb.Append(42);
    sb.AppendLine();
    
    for (int i = 1; i <= 5; i++) {
        sb.AppendFormat("Line {0}: Value = {1}\n", i, i * 10);
    }
    
    string result = sb.ToString();
    Console.WriteLine(result);
}
```

---

## File System API

The `File` and `Directory` classes provide comprehensive file system operations.

### File Class

#### File Reading Operations

```dream
static class File {
    // Read entire file content
    static string ReadAllText(string path);
    static string ReadAllText(string path, Encoding encoding);
    static string[] ReadAllLines(string path);
    static byte[] ReadAllBytes(string path);
    
    // Stream reading
    static StreamReader OpenText(string path);
    static FileStream OpenRead(string path);
}
```

**Example:**
```dream
func main() {
    // Read entire file as string
    string content = File.ReadAllText("config.txt");
    Console.WriteLine("File content:");
    Console.WriteLine(content);
    
    // Read file line by line
    string[] lines = File.ReadAllLines("data.txt");
    foreach (string line in lines) {
        Console.WriteLine("Line: " + line);
    }
    
    // Read binary file
    byte[] data = File.ReadAllBytes("image.jpg");
    Console.WriteLine($"File size: {data.Length} bytes");
}
```

#### File Writing Operations

```dream
static class File {
    // Write entire content
    static void WriteAllText(string path, string content);
    static void WriteAllText(string path, string content, Encoding encoding);
    static void WriteAllLines(string path, string[] lines);
    static void WriteAllBytes(string path, byte[] bytes);
    
    // Append content
    static void AppendAllText(string path, string content);
    static void AppendAllLines(string path, string[] lines);
    
    // Stream writing
    static StreamWriter CreateText(string path);
    static FileStream Create(string path);
    static FileStream OpenWrite(string path);
}
```

**Example:**
```dream
func main() {
    // Write text to file
    string content = "Hello, World!\nThis is a test file.";
    File.WriteAllText("output.txt", content);
    
    // Write lines to file
    string[] lines = ["Line 1", "Line 2", "Line 3"];
    File.WriteAllLines("lines.txt", lines);
    
    // Append to existing file
    File.AppendAllText("output.txt", "\nAppended line");
    
    Console.WriteLine("Files written successfully!");
}
```

#### File Information and Management

```dream
static class File {
    // File existence and properties
    static bool Exists(string path);
    static long GetSize(string path);
    static DateTime GetCreationTime(string path);
    static DateTime GetLastWriteTime(string path);
    static DateTime GetLastAccessTime(string path);
    
    // File operations
    static void Copy(string sourceFileName, string destFileName);
    static void Copy(string sourceFileName, string destFileName, bool overwrite);
    static void Move(string sourceFileName, string destFileName);
    static void Delete(string path);
    
    // Attributes
    static FileAttributes GetAttributes(string path);
    static void SetAttributes(string path, FileAttributes fileAttributes);
}
```

**Example:**
```dream
func main() {
    string filePath = "example.txt";
    
    if (File.Exists(filePath)) {
        Console.WriteLine($"File size: {File.GetSize(filePath)} bytes");
        Console.WriteLine($"Created: {File.GetCreationTime(filePath)}");
        Console.WriteLine($"Modified: {File.GetLastWriteTime(filePath)}");
        
        // Create backup
        File.Copy(filePath, filePath + ".backup");
        
        // Delete original
        File.Delete(filePath);
    } else {
        Console.WriteLine("File does not exist");
    }
}
```

### Directory Class

```dream
static class Directory {
    // Directory existence and creation
    static bool Exists(string path);
    static DirectoryInfo CreateDirectory(string path);
    static void Delete(string path);
    static void Delete(string path, bool recursive);
    
    // Directory listing
    static string[] GetFiles(string path);
    static string[] GetFiles(string path, string searchPattern);
    static string[] GetDirectories(string path);
    static string[] GetDirectories(string path, string searchPattern);
    static string[] GetFileSystemEntries(string path);
    
    // Directory operations
    static void Move(string sourceDirName, string destDirName);
    static string GetCurrentDirectory();
    static void SetCurrentDirectory(string path);
    static string GetParent(string path);
}
```

**Example:**
```dream
func main() {
    string dirPath = "TestDirectory";
    
    // Create directory
    if (!Directory.Exists(dirPath)) {
        Directory.CreateDirectory(dirPath);
        Console.WriteLine($"Created directory: {dirPath}");
    }
    
    // List files in current directory
    string[] files = Directory.GetFiles(".", "*.dr");
    Console.WriteLine("Dream files in current directory:");
    foreach (string file in files) {
        Console.WriteLine($"  {file}");
    }
    
    // List subdirectories
    string[] dirs = Directory.GetDirectories(".");
    Console.WriteLine("Subdirectories:");
    foreach (string dir in dirs) {
        Console.WriteLine($"  {dir}");
    }
}
```

### Path Class

```dream
static class Path {
    // Path manipulation
    static string Combine(params string[] paths);
    static string GetDirectoryName(string path);
    static string GetFileName(string path);
    static string GetFileNameWithoutExtension(string path);
    static string GetExtension(string path);
    static string GetFullPath(string path);
    static string GetTempPath();
    static string GetTempFileName();
    
    // Path validation
    static bool IsPathRooted(string path);
    static bool HasExtension(string path);
    static char[] GetInvalidFileNameChars();
    static char[] GetInvalidPathChars();
    
    // Path constants
    static readonly char DirectorySeparatorChar;
    static readonly char AltDirectorySeparatorChar;
    static readonly char PathSeparator;
}
```

**Example:**
```dream
func main() {
    string fileName = "document.txt";
    string directory = "Documents";
    string subDir = "Projects";
    
    // Combine paths
    string fullPath = Path.Combine(directory, subDir, fileName);
    Console.WriteLine($"Full path: {fullPath}");
    
    // Extract path components
    Console.WriteLine($"Directory: {Path.GetDirectoryName(fullPath)}");
    Console.WriteLine($"Filename: {Path.GetFileName(fullPath)}");
    Console.WriteLine($"Extension: {Path.GetExtension(fullPath)}");
    Console.WriteLine($"Name without extension: {Path.GetFileNameWithoutExtension(fullPath)}");
    
    // Generate temp file
    string tempFile = Path.GetTempFileName();
    Console.WriteLine($"Temp file: {tempFile}");
}
```

---

## Collections API

Dream provides a comprehensive collections framework with generic types.

### List<T> Class

```dream
class List<T> {
    // Constructors
    List();
    List(int capacity);
    List(IEnumerable<T> collection);
    
    // Properties
    int Count { get; }
    int Capacity { get; set; }
    T this[int index] { get; set; }
    
    // Adding elements
    void Add(T item);
    void AddRange(IEnumerable<T> collection);
    void Insert(int index, T item);
    void InsertRange(int index, IEnumerable<T> collection);
    
    // Removing elements
    bool Remove(T item);
    void RemoveAt(int index);
    void RemoveRange(int index, int count);
    int RemoveAll(Predicate<T> match);
    void Clear();
    
    // Searching
    bool Contains(T item);
    int IndexOf(T item);
    int IndexOf(T item, int startIndex);
    int LastIndexOf(T item);
    T Find(Predicate<T> match);
    List<T> FindAll(Predicate<T> match);
    
    // Sorting and manipulation
    void Sort();
    void Sort(IComparer<T> comparer);
    void Reverse();
    T[] ToArray();
}
```

**Example:**
```dream
func main() {
    List<string> fruits = new List<string>();
    
    // Add items
    fruits.Add("apple");
    fruits.Add("banana");
    fruits.Add("cherry");
    
    // Insert at specific position
    fruits.Insert(1, "orange");
    
    // Iterate through list
    Console.WriteLine("Fruits:");
    for (int i = 0; i < fruits.Count; i++) {
        Console.WriteLine($"{i + 1}. {fruits[i]}");
    }
    
    // Search for items
    if (fruits.Contains("banana")) {
        Console.WriteLine("Found banana!");
    }
    
    int index = fruits.IndexOf("cherry");
    Console.WriteLine($"Cherry is at index: {index}");
    
    // Remove items
    fruits.Remove("orange");
    fruits.RemoveAt(0);
    
    Console.WriteLine($"Final count: {fruits.Count}");
}
```

### Dictionary<K, V> Class

```dream
class Dictionary<K, V> {
    // Constructors
    Dictionary();
    Dictionary(int capacity);
    Dictionary(IEqualityComparer<K> comparer);
    
    // Properties
    int Count { get; }
    V this[K key] { get; set; }
    ICollection<K> Keys { get; }
    ICollection<V> Values { get; }
    
    // Adding and updating
    void Add(K key, V value);
    bool TryAdd(K key, V value);
    void Set(K key, V value);  // Add or update
    
    // Retrieving values
    bool TryGetValue(K key, out V value);
    V GetValue(K key);
    V GetValueOrDefault(K key);
    V GetValueOrDefault(K key, V defaultValue);
    
    // Removing items
    bool Remove(K key);
    bool Remove(K key, out V value);
    void Clear();
    
    // Checking existence
    bool ContainsKey(K key);
    bool ContainsValue(V value);
}
```

**Example:**
```dream
func main() {
    Dictionary<string, int> ages = new Dictionary<string, int>();
    
    // Add key-value pairs
    ages.Add("Alice", 30);
    ages.Add("Bob", 25);
    ages.Add("Charlie", 35);
    
    // Access values
    Console.WriteLine($"Alice is {ages["Alice"]} years old");
    
    // Safe access with TryGetValue
    if (ages.TryGetValue("David", out int age)) {
        Console.WriteLine($"David is {age} years old");
    } else {
        Console.WriteLine("David not found");
    }
    
    // Iterate through dictionary
    foreach (var kvp in ages) {
        Console.WriteLine($"{kvp.Key}: {kvp.Value}");
    }
    
    // Check if key exists
    if (ages.ContainsKey("Bob")) {
        ages["Bob"] = 26;  // Update Bob's age
    }
    
    Console.WriteLine($"Total people: {ages.Count}");
}
```

### Set<T> Class

```dream
class Set<T> {
    // Constructors
    Set();
    Set(IEqualityComparer<T> comparer);
    Set(IEnumerable<T> collection);
    
    // Properties
    int Count { get; }
    
    // Adding and removing
    bool Add(T item);
    void AddRange(IEnumerable<T> collection);
    bool Remove(T item);
    void Clear();
    
    // Set operations
    void UnionWith(IEnumerable<T> other);
    void IntersectWith(IEnumerable<T> other);
    void ExceptWith(IEnumerable<T> other);
    void SymmetricExceptWith(IEnumerable<T> other);
    
    // Set queries
    bool Contains(T item);
    bool IsSubsetOf(IEnumerable<T> other);
    bool IsSupersetOf(IEnumerable<T> other);
    bool Overlaps(IEnumerable<T> other);
    bool SetEquals(IEnumerable<T> other);
    
    // Conversion
    T[] ToArray();
    List<T> ToList();
}
```

**Example:**
```dream
func main() {
    Set<int> evenNumbers = new Set<int>([2, 4, 6, 8, 10]);
    Set<int> primeNumbers = new Set<int>([2, 3, 5, 7, 11]);
    
    Console.WriteLine("Even numbers:");
    foreach (int num in evenNumbers) {
        Console.Write(num + " ");
    }
    Console.WriteLine();
    
    // Check membership
    if (evenNumbers.Contains(6)) {
        Console.WriteLine("6 is even");
    }
    
    // Set operations
    Set<int> evenPrimes = new Set<int>(evenNumbers);
    evenPrimes.IntersectWith(primeNumbers);
    
    Console.WriteLine("Even prime numbers:");
    foreach (int num in evenPrimes) {
        Console.Write(num + " ");
    }
    Console.WriteLine();
    
    // Union operation
    Set<int> combined = new Set<int>(evenNumbers);
    combined.UnionWith(primeNumbers);
    Console.WriteLine($"Combined set size: {combined.Count}");
}
```

---

## DateTime API

The `DateTime` struct represents dates and times with comprehensive manipulation capabilities.

### DateTime Structure

```dream
struct DateTime {
    // Constructors
    DateTime(int year, int month, int day);
    DateTime(int year, int month, int day, int hour, int minute, int second);
    DateTime(long ticks);
    
    // Properties
    int Year { get; }
    int Month { get; }
    int Day { get; }
    int Hour { get; }
    int Minute { get; }
    int Second { get; }
    int Millisecond { get; }
    DayOfWeek DayOfWeek { get; }
    int DayOfYear { get; }
    long Ticks { get; }
    
    // Static properties
    static DateTime Now { get; }
    static DateTime UtcNow { get; }
    static DateTime Today { get; }
    static DateTime MinValue { get; }
    static DateTime MaxValue { get; }
}
```

### DateTime Methods

```dream
struct DateTime {
    // Arithmetic operations
    DateTime Add(TimeSpan value);
    DateTime AddYears(int value);
    DateTime AddMonths(int value);
    DateTime AddDays(double value);
    DateTime AddHours(double value);
    DateTime AddMinutes(double value);
    DateTime AddSeconds(double value);
    DateTime AddMilliseconds(double value);
    
    // Subtraction
    TimeSpan Subtract(DateTime value);
    DateTime Subtract(TimeSpan value);
    
    // Comparison
    int CompareTo(DateTime value);
    bool Equals(DateTime value);
    
    // Formatting
    string ToString();
    string ToString(string format);
    string ToShortDateString();
    string ToLongDateString();
    string ToShortTimeString();
    string ToLongTimeString();
}
```

**Example:**
```dream
func main() {
    // Create specific date
    DateTime birthday = new DateTime(1990, 5, 15);
    Console.WriteLine($"Birthday: {birthday.ToShortDateString()}");
    
    // Current date and time
    DateTime now = DateTime.Now;
    Console.WriteLine($"Current time: {now.ToString("yyyy-MM-dd HH:mm:ss")}");
    
    // Calculate age
    TimeSpan age = now.Subtract(birthday);
    int years = (int)(age.TotalDays / 365.25);
    Console.WriteLine($"Age: {years} years");
    
    // Date arithmetic
    DateTime nextWeek = now.AddDays(7);
    DateTime lastMonth = now.AddMonths(-1);
    
    Console.WriteLine($"Next week: {nextWeek.ToShortDateString()}");
    Console.WriteLine($"Last month: {lastMonth.ToShortDateString()}");
    
    // Day of week
    Console.WriteLine($"Today is: {now.DayOfWeek}");
}
```

### TimeSpan Structure

```dream
struct TimeSpan {
    // Constructors
    TimeSpan(long ticks);
    TimeSpan(int hours, int minutes, int seconds);
    TimeSpan(int days, int hours, int minutes, int seconds);
    TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds);
    
    // Properties
    int Days { get; }
    int Hours { get; }
    int Minutes { get; }
    int Seconds { get; }
    int Milliseconds { get; }
    long Ticks { get; }
    double TotalDays { get; }
    double TotalHours { get; }
    double TotalMinutes { get; }
    double TotalSeconds { get; }
    double TotalMilliseconds { get; }
    
    // Static methods
    static TimeSpan FromDays(double value);
    static TimeSpan FromHours(double value);
    static TimeSpan FromMinutes(double value);
    static TimeSpan FromSeconds(double value);
    static TimeSpan FromMilliseconds(double value);
}
```

**Example:**
```dream
func main() {
    // Create time spans
    TimeSpan oneHour = TimeSpan.FromHours(1);
    TimeSpan thirtyMinutes = TimeSpan.FromMinutes(30);
    TimeSpan twoAndHalfHours = oneHour.Add(oneHour).Add(thirtyMinutes);
    
    Console.WriteLine($"Duration: {twoAndHalfHours.TotalHours} hours");
    Console.WriteLine($"In minutes: {twoAndHalfHours.TotalMinutes}");
    
    // Measuring elapsed time
    DateTime start = DateTime.Now;
    
    // Simulate some work
    for (int i = 0; i < 1000000; i++) {
        // Some computation
    }
    
    DateTime end = DateTime.Now;
    TimeSpan elapsed = end.Subtract(start);
    
    Console.WriteLine($"Operation took: {elapsed.TotalMilliseconds} ms");
}
```

This comprehensive API reference provides detailed documentation with practical examples for all major Dream language APIs. Each section includes complete method signatures, usage examples, and best practices for effective development.