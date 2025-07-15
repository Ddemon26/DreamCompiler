# Variables in Dream

## Overview
The Dream language supports variable declarations and assignments with a C#-like syntax. Two types are now available: `int` for integers and `string` for string values.

## Syntax

**Declaration**: `int <identifier>;` or `string <identifier>;`
: Declares a variable. Integer variables default to `0` while string variables default to `null`.

**Assignment**: `<identifier> = <expression>;`
: Assigns a value (number or expression) to a variable.

## Examples

```dream
int x;        // Declares x, initialized to 0
x = 42;       // Assigns 42 to x
int y = 10;   // Declares and assigns 10 to y
```

## Notes

Integer variables are stored as local `long` values in the generated C code. `string` variables compile to `const char*` pointers.
