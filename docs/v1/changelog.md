Dream Compiler Changelog
Version 1.0.1 (2025-07-14)

Initial compiler implementation in dream.c.
Supported features:
Variable declarations (int x;)
Assignments (x = 5;)
Addition (x = y + z;)
Console output (Console.WriteLine(x);)


Added /docs with variables.md, arithmetic.md, console.md, usage.md.
Added /tests with test_variables.dr, test_arithmetic.dr, test_console.dr.

Version 1.0.2 (2025-07-14)

* Added support for initializing variables with a value (e.g., `int x = 5;`).
* Console.WriteLine now prints numbers in decimal using printf.

Version 1.0.3 (2025-07-14)

* Compiler now generates C code (`output.c`) instead of x86-64 assembly.
* Updated documentation and build instructions accordingly.

Version 1.0.4 (2025-07-14)

* Added basic `if` statement support. Conditions can reference variables or numbers and execute a single statement when true.
* Updated documentation and added new test.

Version 1.0.5 (2025-07-14)

* Added subtraction operator (-) for arithmetic expressions.
* Updated documentation and usage guide.
* Added new test case for subtraction.

Version 1.0.6 (2025-07-14)

* Introduced `while` loops for basic iteration.
* Added documentation and usage examples.
* Added regression test for loops.

Version 1.0.7 (2025-07-14)

* Console.WriteLine now accepts quoted string literals.
* Updated documentation and usage guide.
* Added new test case for string output.

Version 1.0.8 (2025-07-14)

* Added multiplication operator (*) for arithmetic expressions.
* Updated documentation and usage guide.
* Added regression test for multiplication.

Version 1.0.9 (2025-07-14)

* Added division operator (/) for arithmetic expressions.
* Updated documentation and usage guide.
* Added regression test for division.


Version 1.0.10 (2025-07-14)

* Added less-than (<) and greater-than (>) comparison operators.
* Updated documentation and usage guide.
* Added regression test for comparisons.

Version 1.0.11 (2025-07-14)

* Documentation site now supports optional dark mode.

Version 1.0.12 (2025-07-14)

* Restructured documentation index with category submenus.
* Added version selection dropdown.

Version 1.0.13 (2025-07-14)

* Added equality operators (== and !=) for expressions.
* Updated documentation and usage guide.
* Added regression test for equality.

Version 1.0.14 (2025-07-14)

* Added <= and >= comparison operators.
* Updated documentation and usage guide.
* Added regression test for the new operators.

Version 1.0.15 (2025-07-14)

* Added modulo operator (%) for arithmetic expressions.
* Updated documentation and usage guide.
* Added regression test for modulo.

Version 1.0.16 (2025-07-14)

* Reverted to generating `build/bin/dream.c` so the C file lives inside
  the build directory. The compilation command now matches this path.

Version 1.0.17 (2025-07-14)

* Added `else` clauses to `if` statements.
* Updated documentation and added a new test case.

Version 1.0.18 (2025-07-14)

* Added unary minus support for numeric expressions.
* Updated arithmetic documentation and added a new test case.

Version 1.0.19 (2025-07-15)

* Reorganized source files into subdirectories (`lexer`, `parser`, `ast`, `codegen`, `driver`).
* Updated build script and documentation to reflect the new layout.

Version 1.0.20 (2025-07-15)

* Added parentheses for grouping expressions.
* Documented the feature and added a new test case.

Version 1.0.21 (2025-07-15)

* Braced blocks can now contain multiple statements in `if` and `while`.
* Updated documentation and added regression tests.
