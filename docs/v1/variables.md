# Variables in Dream

## Overview

The Dream language supports variable declarations and assignments with a C#-like syntax. Four types are now available: `int` for integers, `float` for decimal numbers, `string` for text values and `bool` for boolean flags.

## Syntax

**Declaration**: `int <identifier>;`, `float <identifier>;`, `string <identifier>;` or `bool <identifier>;`
: Declares a variable. Integer and boolean variables default to `0` while string variables default to `null`.
  Multiple variables of the same type may be declared in a single statement by separating them with commas:
  `int a, b = 1, c;`.

**Assignment**: `<identifier> = <expression>;`
: Assigns a value (number or expression) to a variable.

## Examples

```dream
int x;        // Declares x, initialized to 0
x = 42;       // Assigns 42 to x
int y = 10;   // Declares and assigns 10 to y
float z = 1.5; // Float variable
bool flag = true; // Boolean variable initialized to 1
int a, b = 2, c; // Multiple variables in one statement
```

## Notes

Integer and boolean variables are stored as local `long` values in the generated C code. `float` variables map to `double`. `string` variables compile to `const char*` pointers. When printed with `Console.WriteLine`, boolean variables appear as `true` or `false`.
