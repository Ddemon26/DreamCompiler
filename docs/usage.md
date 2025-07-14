Dream Language Usage Guide
Overview
Dream is a programming language with C#-like, semicolon-based syntax, compiled to x86-64 assembly. This guide covers the current syntax and usage.
Syntax

Variable Declaration: int <identifier>; or int <identifier> = <value>;
Assignment: <identifier> = <expression>;
Arithmetic: <identifier> = <expression> + <expression>;
Console Output: Console.WriteLine(<expression>);

Example Program
int x = 5;
int y = 10;
x = y + 5;          // x = 15
Console.WriteLine(x); // Outputs 15

Compilation

Write a .dr file (e.g., example.dr).
Compile: dream.exe example.dr (generates output.asm).
Assemble: nasm -f win64 output.asm -o output.obj.
Link: gcc output.obj -o dream_exec.exe.
Run: dream_exec.exe.

Notes

Semicolons (;) are required to terminate statements.
Run in CLion or via a batch file for Windows compatibility.