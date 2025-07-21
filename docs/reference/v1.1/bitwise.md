# Bitwise Operators (v1.1.12)

Dream supports comprehensive bitwise operations for low-level programming and efficient bit manipulation. These operators work at the bit level and are essential for systems programming, embedded development, and performance-critical applications.

## Bitwise Operators

| Operator | Name | Description | Example |
|----------|------|-------------|---------|
| `&` | Bitwise AND | Sets bit if both bits are 1 | `a & b` |
| `\|` | Bitwise OR | Sets bit if either bit is 1 | `a \| b` |
| `^` | Bitwise XOR | Sets bit if bits are different | `a ^ b` |
| `~` | Bitwise NOT | Inverts all bits | `~a` |
| `<<` | Left Shift | Shifts bits left | `a << n` |
| `>>` | Right Shift | Shifts bits right | `a >> n` |

## Bitwise AND (&)

Sets each bit to 1 if both corresponding bits are 1:

```dream
int a = 12;    // Binary: 1100
int b = 10;    // Binary: 1010
int result = a & b;  // Binary: 1000 = 8

Console.WriteLine("12 & 10 = " + result);  // Output: 12 & 10 = 8

// Practical example: Check if number is even
func bool isEven(int number) {
    return (number & 1) == 0;  // Check if least significant bit is 0
}

int num = 42;
if (isEven(num)) {
    Console.WriteLine(num + " is even");
}

// Mask operations
int flags = 0b11010110;
int mask = 0b00001111;   // Mask to get lower 4 bits
int lower4Bits = flags & mask;  // Result: 0b00000110 = 6
```

## Bitwise OR (|)

Sets each bit to 1 if at least one corresponding bit is 1:

```dream
int a = 12;    // Binary: 1100
int b = 10;    // Binary: 1010
int result = a | b;  // Binary: 1110 = 14

Console.WriteLine("12 | 10 = " + result);  // Output: 12 | 10 = 14

// Setting specific bits
int flags = 0b00000000;
int setBit2 = 0b00000100;  // Bit 2
int setBit5 = 0b00100000;  // Bit 5

flags = flags | setBit2;   // Set bit 2
flags = flags | setBit5;   // Set bit 5
// flags is now 0b00100100 = 36

// Combining permissions example
int READ_PERMISSION = 0b001;   // 1
int WRITE_PERMISSION = 0b010;  // 2
int EXEC_PERMISSION = 0b100;   // 4

int userPermissions = READ_PERMISSION | WRITE_PERMISSION;  // 3
int adminPermissions = READ_PERMISSION | WRITE_PERMISSION | EXEC_PERMISSION;  // 7
```

## Bitwise XOR (^)

Sets each bit to 1 if the corresponding bits are different:

```dream
int a = 12;    // Binary: 1100
int b = 10;    // Binary: 1010
int result = a ^ b;  // Binary: 0110 = 6

Console.WriteLine("12 ^ 10 = " + result);  // Output: 12 ^ 10 = 6

// XOR properties and applications
int x = 25;
int y = 30;

// Swapping without temporary variable
x = x ^ y;  // x = 25 ^ 30
y = x ^ y;  // y = (25 ^ 30) ^ 30 = 25
x = x ^ y;  // x = (25 ^ 30) ^ 25 = 30
// Now x = 30, y = 25

// Toggle specific bits
int value = 0b10110010;
int toggleMask = 0b00110000;  // Toggle bits 4 and 5
int toggled = value ^ toggleMask;  // Result: 0b10000010

// Simple encryption (XOR cipher)
func string simpleEncrypt(string text, char key) {
    string encrypted = "";
    for (int i = 0; i < text.length; i++) {
        char encryptedChar = text[i] ^ key;
        encrypted = encrypted + encryptedChar;
    }
    return encrypted;
}
```

## Bitwise NOT (~)

Inverts all bits (1 becomes 0, 0 becomes 1):

```dream
int a = 12;    // Binary: 00001100 (8-bit representation)
int result = ~a;  // Binary: 11110011 = -13 (two's complement)

Console.WriteLine("~12 = " + result);  // Output: ~12 = -13

// Creating masks
int fullMask = ~0;  // All bits set to 1 (typically 0xFFFFFFFF)
int clearLower4 = ~0b00001111;  // Clear lower 4 bits mask

// Complement operations
int flags = 0b11010110;
int inverted = ~flags;  // Invert all flags

// One's complement
func int onesComplement(int value) {
    return ~value;
}
```

## Left Shift (<<)

Shifts bits to the left, filling with zeros from the right:

```dream
int a = 5;     // Binary: 101
int result = a << 2;  // Binary: 10100 = 20

Console.WriteLine("5 << 2 = " + result);  // Output: 5 << 2 = 20

// Left shift is equivalent to multiplication by 2^n
int value = 7;
int shifted1 = value << 1;  // 7 * 2^1 = 14
int shifted2 = value << 2;  // 7 * 2^2 = 28
int shifted3 = value << 3;  // 7 * 2^3 = 56

// Fast power of 2 multiplication
func int multiplyByPowerOf2(int num, int power) {
    return num << power;
}

int result1 = multiplyByPowerOf2(10, 3);  // 10 * 8 = 80

// Creating bit patterns
int bit0 = 1 << 0;  // 1
int bit1 = 1 << 1;  // 2
int bit2 = 1 << 2;  // 4
int bit3 = 1 << 3;  // 8

// Array indexing optimization
func int fastArrayIndex(int base, int elementSize, int index) {
    // For powers of 2 element sizes, use shift instead of multiplication
    if (elementSize == 4) {
        return base + (index << 2);  // index * 4
    } else if (elementSize == 8) {
        return base + (index << 3);  // index * 8
    }
    return base + (index * elementSize);
}
```

## Right Shift (>>)

Shifts bits to the right. For positive numbers, fills with zeros; for negative numbers, behavior depends on implementation:

```dream
int a = 20;    // Binary: 10100
int result = a >> 2;  // Binary: 101 = 5

Console.WriteLine("20 >> 2 = " + result);  // Output: 20 >> 2 = 5

// Right shift is equivalent to division by 2^n (for positive numbers)
int value = 64;
int shifted1 = value >> 1;  // 64 / 2^1 = 32
int shifted2 = value >> 2;  // 64 / 2^2 = 16
int shifted3 = value >> 3;  // 64 / 2^3 = 8

// Fast power of 2 division
func int divideByPowerOf2(int num, int power) {
    return num >> power;
}

int result1 = divideByPowerOf2(100, 2);  // 100 / 4 = 25

// Extracting specific bits
int value32bit = 0xABCD1234;
int upperByte = (value32bit >> 24) & 0xFF;  // Extract bits 24-31
int lowerByte = value32bit & 0xFF;          // Extract bits 0-7

// Binary search optimization
func int binarySearchStep(int range) {
    return range >> 1;  // Divide range by 2
}
```

## Compound Bitwise Assignment

Combine bitwise operations with assignment:

```dream
int flags = 0b00001010;

// Compound AND
flags &= 0b11110000;  // Clear lower 4 bits
// flags is now 0b00000000

flags = 0b11111111;
flags &= ~0b00001111;  // Clear lower 4 bits using complement
// flags is now 0b11110000

// Compound OR
flags |= 0b00000101;  // Set bits 0 and 2
// flags is now 0b11110101

// Compound XOR
flags ^= 0b11111111;  // Toggle all bits
// flags is now 0b00001010

// Compound shifts
int value = 100;
value <<= 2;  // Multiply by 4, value is now 400
value >>= 1;  // Divide by 2, value is now 200
```

## Practical Applications

### Flag Management
```dream
// Permission system using bit flags
int NONE = 0b0000;
int READ = 0b0001;
int WRITE = 0b0010;
int EXECUTE = 0b0100;
int DELETE = 0b1000;

func void managePermissions() {
    int userPermissions = NONE;
    
    // Grant permissions
    userPermissions |= READ;      // Grant read
    userPermissions |= WRITE;     // Grant write
    
    // Check permissions
    if ((userPermissions & READ) != 0) {
        Console.WriteLine("User has read permission");
    }
    
    if ((userPermissions & EXECUTE) == 0) {
        Console.WriteLine("User does not have execute permission");
    }
    
    // Revoke permissions
    userPermissions &= ~WRITE;    // Revoke write permission
    
    // Toggle permissions
    userPermissions ^= DELETE;    // Toggle delete permission
}
```

### Bit Manipulation Utilities
```dream
func int setBit(int number, int position) {
    return number | (1 << position);
}

func int clearBit(int number, int position) {
    return number & ~(1 << position);
}

func int toggleBit(int number, int position) {
    return number ^ (1 << position);
}

func bool getBit(int number, int position) {
    return (number & (1 << position)) != 0;
}

func int countSetBits(int number) {
    int count = 0;
    while (number != 0) {
        count += number & 1;
        number >>= 1;
    }
    return count;
}

// Usage examples
int value = 42;  // Binary: 101010

value = setBit(value, 0);     // Set bit 0: 101011 = 43
value = clearBit(value, 1);   // Clear bit 1: 101001 = 41
value = toggleBit(value, 3);  // Toggle bit 3: 100001 = 33

bool bit2Set = getBit(value, 2);  // Check if bit 2 is set: false
int numBits = countSetBits(value); // Count set bits: 2
```

### Color Manipulation
```dream
// RGB color represented as 24-bit integer: 0xRRGGBB
func int createColor(int red, int green, int blue) {
    return (red << 16) | (green << 8) | blue;
}

func int getRed(int color) {
    return (color >> 16) & 0xFF;
}

func int getGreen(int color) {
    return (color >> 8) & 0xFF;
}

func int getBlue(int color) {
    return color & 0xFF;
}

func int setAlpha(int color, int alpha) {
    // Convert RGB to ARGB
    return (alpha << 24) | (color & 0xFFFFFF);
}

// Usage
int red = 255, green = 128, blue = 64;
int color = createColor(red, green, blue);  // 0xFF8040

int extractedRed = getRed(color);     // 255
int extractedGreen = getGreen(color); // 128
int extractedBlue = getBlue(color);   // 64

int colorWithAlpha = setAlpha(color, 200);  // 0xC8FF8040
```

### Hash Functions
```dream
func int simpleHash(string text) {
    int hash = 0;
    for (int i = 0; i < text.length; i++) {
        hash = (hash << 5) - hash + text[i];  // hash * 31 + char
        hash &= 0x7FFFFFFF;  // Keep positive (clear sign bit)
    }
    return hash;
}

func int djb2Hash(string text) {
    int hash = 5381;
    for (int i = 0; i < text.length; i++) {
        hash = ((hash << 5) + hash) + text[i];  // hash * 33 + char
    }
    return hash;
}
```

## Performance Considerations

### Optimization Techniques
```dream
// Fast modulo for powers of 2
func int fastModulo(int value, int modPowerOf2) {
    return value & (modPowerOf2 - 1);
}

// Check if number is power of 2
func bool isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// Find next power of 2
func int nextPowerOfTwo(int n) {
    if (n <= 1) return 1;
    
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    
    return n;
}

// Fast absolute value
func int fastAbs(int x) {
    int mask = x >> 31;  // -1 if negative, 0 if positive
    return (x + mask) ^ mask;
}
```

## Best Practices

### Readability and Maintenance
```dream
// Use named constants for bit masks
int STATUS_ACTIVE = 0b00000001;
int STATUS_LOCKED = 0b00000010;
int STATUS_VERIFIED = 0b00000100;

// Document bit patterns
func void setUserStatus(int userId, int status) {
    // Status bits:
    // Bit 0: Active (1 = active, 0 = inactive)
    // Bit 1: Locked (1 = locked, 0 = unlocked)
    // Bit 2: Verified (1 = verified, 0 = unverified)
    
    updateUserStatus(userId, status);
}

// Use helper functions for complex operations
func bool hasAllPermissions(int userPerms, int requiredPerms) {
    return (userPerms & requiredPerms) == requiredPerms;
}

func bool hasAnyPermission(int userPerms, int checkPerms) {
    return (userPerms & checkPerms) != 0;
}
```

### Error Prevention
```dream
// Validate bit positions
func int safeBitSet(int number, int position) {
    if (position < 0 || position >= 32) {
        throw "Invalid bit position: " + position;
    }
    return number | (1 << position);
}

// Avoid undefined behavior with large shifts
func int safeLeftShift(int value, int positions) {
    if (positions < 0 || positions >= 32) {
        return 0;  // or throw exception
    }
    return value << positions;
}
```

Bitwise operations in Dream provide powerful tools for low-level programming, optimization, and efficient data manipulation while maintaining type safety and code clarity.