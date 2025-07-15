# Dream Language Usage Guide

## Overview

Dream is a programming language with C#-like, semicolon-based syntax compiled to C code. This guide covers the current syntax and usage.

## Syntax

Variable Declaration: int <identifier>; or int <identifier> = <value>; or bool <identifier>; or string <identifier>;
Assignment: <identifier> = <expression>;
Arithmetic: <identifier> = <expression> + <expression> or <expression> - <expression> or <expression> * <expression> or <expression> / <expression> or <expression> % <expression>;
Comparison: <expression> < <expression> or <expression> > <expression> or <expression> <= <expression> or <expression> >= <expression> or <expression> == <expression> or <expression> != <expression>;
Logical: !<expression> or <expression> && <expression> or <expression> || <expression>;
Conditional Operator: <expression> ? <expression> : <expression>;
Console Output: Console.WriteLine(<expression>);
    <expression> may be a variable, number, or quoted string.
    String literals support escape sequences such as `\n`, `\t`, `\\` and `\"`.
    Strings can be concatenated using `+`.
Conditional: if (<expression>) <statement> [else <statement>]
Loop: while (<expression>) <statement> or do <statement> while (<expression>) or for (<init>; <condition>; <increment>) <statement>;
Switch: switch (<expression>) { case <number>: <statement> ... }
Function: func <name>(<type> <param>[, <type> <param>]*) { <statements> }
Return: return [<expression>];
Increment/Decrement: <identifier>++ | ++<identifier> | <identifier>-- | --<identifier>
Comments: // single line or /* block */

## Example Program

int x = 5;
int y = 10;
x = y + 5;          // x = 15
x = y - 3;          // x = 7
x = y / 2;          // x = 5
x = y % 3;          // x = 1
Console.WriteLine(x);       // Outputs 15
Console.WriteLine("hi"); // Outputs "hi"
if (x) Console.WriteLine(y);
if (x && y) Console.WriteLine(x); // Prints if both nonzero
if (x || y) Console.WriteLine(y); // Prints if either nonzero
while (x) Console.WriteLine(y);

## Compilation

Write a `.dr` file (e.g., `example.dr`).
Compile and run:
```
zig build run -- example.dr
```
The compiler uses the `CC` environment variable for the back-end C compilation step. If `CC` is not set, `gcc` is used by default.

## Notes

Semicolons (;) are required to terminate statements.
Run in CLion or via a batch file for Windows compatibility.


