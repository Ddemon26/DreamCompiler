# Console Output in Dream

## Overview

Dream provides `Console.WriteLine` and `Console.Write` for outputting values to the console, similar to C#.

## Syntax

`Console.Write(<expression>);`
: Prints the value without a trailing newline.

`Console.WriteLine(<expression>);`
: Prints the value followed by a newline.

## Examples

```dream
int x = 15;
Console.WriteLine(x);          // Outputs 15
Console.Write("hello");       // Outputs the string "hello" without newline
Console.WriteLine(" world");  // Outputs "hello world" then a newline
```

## Notes

`Console.WriteLine` prints integers using `printf` with the `%ld` format. Values
are cast to `long` in the generated C code to avoid warnings. Strings are
emitted directly as C string literals. Boolean values, including the result of
boolean expressions, are printed as the words `true` or `false`.

