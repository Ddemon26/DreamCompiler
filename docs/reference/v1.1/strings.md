# String Literals & Operations

Dream provides comprehensive string support including literals, escape sequences, and concatenation operations.

## String Literals

String literals are enclosed in double quotes and support standard escape sequences:

```dream
string message = "Hello, World!";
string multiline = "Line 1\nLine 2\nLine 3";
string quoted = "She said \"Hello\"";
string path = "C:\\Program Files\\Dream";
```

### Supported Escape Sequences

| Sequence | Description | Output |
|----------|-------------|--------|
| `\"` | Double quote | `"` |
| `\\` | Backslash | `\` |
| `\n` | Newline | Line break |
| `\t` | Tab | Tab character |
| `\r` | Carriage return | CR |

## String Concatenation

Dream supports string concatenation using the `+` operator with automatic type conversion:

### String + String
```dream
string first = "Hello";
string second = "World";
string result = first + " " + second;  // "Hello World"
```

### String + Numeric Types
```dream
string message = "Count: " + 42;        // "Count: 42"
string balance = "Balance: $" + 19.99;  // "Balance: $19.99"
int value = 100;
string text = "Value is " + value;      // "Value is 100"
```

### String Arrays and Concatenation
```dream
string words[3];
words[0] = "Dream";
words[1] = words[0] + " Language";      // "Dream Language"
words[2] = "Version: " + 1;             // "Version: 1"
```

### Mixed Type Concatenation
```dream
int count = 5;
float price = 29.99;
string summary = "Items: " + count + ", Total: $" + price;
// Result: "Items: 5, Total: $29.99"
```

## String Variables

String variables are declared with the `string` type and can be assigned literals or expressions:

```dream
string name = "Alice";
string greeting = "Hello, " + name;
string empty = "";
```

## Runtime Implementation

String operations are implemented using the Dream runtime system:
- `dream_concat(str1, str2)` - Concatenates two strings
- `dream_concat_string_int(str, value)` - Concatenates string with integer
- `dream_concat_string_float(str, value)` - Concatenates string with float
- Memory management via reference-counted allocation (`dr_alloc`)

