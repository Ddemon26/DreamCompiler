# Variables in Dream

## Overview
The Dream language supports variable declarations and assignments with a C#-like syntax. Three types are now available: `int` for integers, `string` for string values and `bool` for boolean flags.

## Syntax

**Declaration**: `int <identifier>;`, `string <identifier>;` or `bool <identifier>;`
: Declares a variable. Integer and boolean variables default to `0` while string variables default to `null`.

**Assignment**: `<identifier> = <expression>;`
: Assigns a value (number or expression) to a variable.

## Examples

```dream
int x;        // Declares x, initialized to 0
x = 42;       // Assigns 42 to x
int y = 10;   // Declares and assigns 10 to y
bool flag = true; // Boolean variable initialized to 1
```

## Notes

Integer and boolean variables are stored as local `long` values in the generated C code. `string` variables compile to `const char*` pointers.
When printed with `Console.WriteLine`, boolean variables appear as `true` or `false`.
