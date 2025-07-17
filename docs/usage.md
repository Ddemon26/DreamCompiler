# Dream Language Usage Guide

## Overview

Dream is a C‑style, semicolon‑terminated language that transpiles to C code.

---

## Syntax

### Declarations

```text
int    <identifier>;
int    <identifier> = <value>;
bool   <identifier>;
string <identifier>;
```

### Assignment

```text
<identifier> = <expression>;
```

### Arithmetic

```text
<expression> +  <expression>
<expression> -  <expression>
<expression> *  <expression>
<expression> /  <expression>
<expression> %  <expression>
```

### Comparison

```text
<expression>  <  <expression>
<expression>  >  <expression>
<expression> <= <expression>
<expression> >= <expression>
<expression> == <expression>
<expression> != <expression>
```

### Logical

```text
!<expression>
<expression> && <expression>
<expression> || <expression>
```

### Conditional (ternary) operator

```text
<condition> ? <when‑true> : <when‑false>
```

### Console output

```text
Console.WriteLine(<expression>);
```

* `<expression>` may be a variable, number, or quoted string.
* String literals accept `\n`, `\t`, `\\`, and `\"`.
* Strings concatenate with `+`.

### Control flow

```text
// Conditional
if (<expression>) <statement> [else <statement>]

// Loops
while (<expression>)          <statement>
do    <statement> while (<expression>)
for   (<init>; <condition>; <increment>) <statement>

// Switch
switch (<expression>) {
    case <number>:
        <statement>
        ...
}
```

### Functions

```text
func <name>(<type> <param>[, <type> <param>]*) {
    <statements>
}
```

### Return

```text
return            // no value
return <expression>
```

### Increment / decrement

```text
identifier++   ++identifier
identifier--   --identifier
```

### Comments

```text
// single‑line
/* block */
```

---

## Example program

```text
int x = 5;
int y = 10;

x = y + 5;       // 15
x = y - 3;       // 7
x = y / 2;       // 5
x = y % 3;       // 1

Console.WriteLine(x);        // 15
Console.WriteLine("hi");     // hi

if (x)          Console.WriteLine(y);
if (x && y)     Console.WriteLine(x);
if (x || y)     Console.WriteLine(y);

while (x)       Console.WriteLine(y);
```

---

## Compilation

1. Save your code in a file such as `example.dr`.
2. Build and run:

   ```bash
   zig build run -- example.dr
   ```

    * The compiler uses the `CC` environment variable for the C back‑end.
    * If `CC` is unset, **gcc** is used by default.

---

## Notes

* Every statement **must** end with a semicolon (`;`).
* On Windows, run the compiler from **CLion** or a batch script for smoother integration.