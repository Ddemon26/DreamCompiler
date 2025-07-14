Dream Compiler Changelog
Version 0.1 (2025-07-14)

Initial compiler implementation in dream.c.
Supported features:
Variable declarations (int x;)
Assignments (x = 5;)
Addition (x = y + z;)
Console output (Console.WriteLine(x);)


Added /docs with variables.md, arithmetic.md, console.md, usage.md.
Added /tests with test_variables.dr, test_arithmetic.dr, test_console.dr.

Version 0.2 (2025-07-15)

* Added support for initializing variables with a value (e.g., `int x = 5;`).
* Console.WriteLine now prints numbers in decimal using printf.

Version 0.3 (2025-07-16)

* Compiler now generates C code (`output.c`) instead of x86-64 assembly.
* Updated documentation and build instructions accordingly.

Version 0.4 (2025-07-17)

* Added basic `if` statement support. Conditions can reference variables or numbers and execute a single statement when true.
* Updated documentation and added new test.

Version 0.5 (2025-07-18)

* Added subtraction operator (-) for arithmetic expressions.
* Updated documentation and usage guide.
* Added new test case for subtraction.

Version 0.6 (2025-07-19)

* Introduced `while` loops for basic iteration.
* Added documentation and usage examples.
* Added regression test for loops.
