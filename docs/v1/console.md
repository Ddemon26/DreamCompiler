# Console Output in Dream

## Overview
Dream provides `Console.WriteLine` for outputting values to the console, similar to C#.

## Syntax

`Console.WriteLine(<expression>);`
: Prints the value of a variable, number or quoted string.

## Examples

```dream
int x = 15;
Console.WriteLine(x);          // Outputs 15
Console.WriteLine("hello");   // Outputs the string "hello"
```

## Notes

`Console.WriteLine` prints integers using `printf` so the output appears in
decimal form. Strings are emitted directly as C string literals.

