# Dream Compiler Changelog

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

Version 1.0.22 (2025-07-15)

* Added logical AND (&&) and OR (||) operators.
* Updated documentation and added a new test case.

Version 1.0.23 (2025-07-15)

* Added logical NOT (!) operator for expressions.
* Updated documentation and added a new test case.

Version 1.0.24 (2025-07-15)

* Added `do-while` loops for post-test iteration.
* Updated documentation and usage guide.
* Added regression test for do-while loops.

Version 1.0.25 (2025-07-15)

* Added `for` loops supporting initialization, condition and increment clauses.
* Updated documentation and usage guide.
* Added regression test for for loops.

Version 1.0.26 (2025-07-15)

* Added `break` and `continue` statements for loops.
* Documented the feature and added a new test case.

Version 1.0.27 (2025-07-16)

* Added `return` statements to exit the program early.
* Updated documentation and added a regression test.

Version 1.0.28 (2025-07-16)

* Updated IntelliJ plugin build configuration to target Java 21.

Version 1.0.29 (2025-07-17)

* Fixed JetBrains syntax highlighting by using the correct language id in plugin.xml.


Version 1.0.30 (2025-07-17)

* Added simple functions with the `func` keyword.
* Updated documentation and added a new test case.

Version 1.0.31 (2025-07-17)

* Added block comment token for syntax highlighting.
* Regenerated VS Code and JetBrains grammars.

Version 1.0.32 (2025-07-18)

* Lexer now supports C-style block comments.
* Documented comments and added a regression test.

Version 1.0.33 (2025-07-18)

* Standardized syntax token scopes so default editor themes highlight Dream files.
* Regenerated VS Code grammar from updated token list.

Version 1.0.34 (2025-07-18)

* Synchronized JetBrains token definitions with `tokens.json`.
* Updated generator to copy tokens into the plugin resources.

Version 1.0.35 (2025-07-18)

* Fixed printf warnings by casting numeric output to `long`.

Version 1.0.36 (2025-07-15)

* Added `string` variable support and a new test.
* Updated tokens and regenerated editor grammars.

Version 1.0.37 (2025-07-18)

* Added support for `else if` chains.
* Updated documentation and added a regression test.

Version 1.0.38 (2025-07-19)

* Functions now accept parameters.
* Updated documentation and added a new test case.

Version 1.0.39 (2025-07-20)

* Introduced `bool` variables and boolean literals.
* Updated documentation, syntax highlighting and added a new test case.

Version 1.0.40 (2025-07-21)

* Added increment (`++`) and decrement (`--`) operators.
* Updated loops documentation and added a new test case.

Version 1.0.41 (2025-07-22)

* Increment and decrement operators can now be used inside expressions.
* Documented the feature and added a new regression test.

Version 1.0.42 (2025-07-23)

* `Console.WriteLine` now prints boolean variables as `true` or `false`.
* Updated documentation to mention boolean output.

Version 1.0.43 (2025-07-24)

* Fixed uninitialized boolean variable tracking in the driver.

Version 1.0.44 (2025-07-25)

* Added operator and punctuation tokens for syntax highlighting.

Version 1.0.45 (2025-07-26)

* Added `switch` statements with `case` and `default` clauses.
* Updated documentation and added a new test case.

Version 1.0.46 (2025-07-27)

* Added ternary conditional operator with `? :`.
* Updated documentation and syntax highlighting.

Version 1.0.47 (2025-07-27)

* Added compound assignment operators (`+=`, `-=`, `*=`, `/=`, `%=`).
* Updated arithmetic documentation and editor token definitions.
* Added regression test for compound assignments.

Version 1.0.48 (2025-07-28)

* Extended syntax highlighting tokens with identifiers, bracket punctuation and documentation comments.
* Fixed block comment handling in the lexer generator.
* Regenerated editor grammars.

Version 1.0.49 (2025-07-29)

* Fixed syntax highlighting regressions for keywords and block comments.
* Regenerated editor grammars.

Version 1.0.50 (2025-07-30)

* Console.WriteLine now prints boolean expressions as 'true' or 'false'.
* Updated documentation and added a new test case.


Version 1.0.51 (2025-07-31)

* Added `Console.Write` for output without a newline.
* Updated documentation and README.
* Added new regression test.
* Regenerated editor grammars.
Version 1.0.52 (2025-07-31)

* Added support for escape sequences in string literals.

* Updated documentation and added a regression test.

Version 1.0.53 (2025-07-31)

* Added string concatenation using the `+` operator.
* Updated documentation and added a new regression test.

Version 1.0.54 (2025-08-01)

* Added float variables and decimal number support.
* Updated documentation and console printing.
* Added new regression test.

Version 1.0.55 (2025-08-02)

* Added bitwise XOR operator (`^`).
* Updated arithmetic documentation and editor token definitions.
* Added new regression test.

Version 1.0.56 (2025-08-03)

* Added bitwise AND (`&`) and OR (`|`) operators.
* Updated arithmetic documentation and syntax highlighting.
* Added new regression tests.

Version 1.0.57 (2025-08-04)

* Added bitwise shift operators (`<<` and `>>`).
* Updated arithmetic documentation and syntax highlighting.
* Added new regression test.

Version 1.0.58 (2025-08-05)

* Added basic `class` definitions.
* Documented the feature and generated syntax highlighting.
* Added a regression test.

Version 1.0.59 (2025-08-06)

* Rewrote expression parsing using a precedence climbing algorithm.
* Documented the new parser architecture.
* Added regression tests for mixed precedence and grouping.

Version 1.0.60 (2025-08-07)

* Added object instantiation for classes and field access via `.`.
* Updated class documentation and added a new regression test.

Version 1.0.61 (2025-08-08)

* Added support for declaring multiple variables in one statement.
* Updated variable documentation and added a new regression test.

Version 1.0.62 (2025-08-09)

* Added `struct` keyword as an alias for `class`.
* Updated grammar, documentation and syntax files.
* Added a regression test for struct declarations.

Version 1.0.63 (2025-08-10)

* Added `char` variables and character literals.
* Added unary bitwise NOT `~` operator.
* Updated documentation, syntax highlighting and added new tests.

Version 1.0.64 (2025-08-11)

* Added fixed-size `int` arrays and indexing syntax.
* Documented arrays and updated grammar specification.
* Added a regression test.

Version 1.0.65 (2025-08-12)

* Added fixed-size `float` arrays.
* Updated array documentation and syntax highlighting.
* Added a regression test for float arrays.

Version 1.0.66 (2025-08-13)

* Added fixed-size arrays for `bool`, `char` and `string` types.
* Updated array documentation and grammar notes.
* Added new regression tests for the additional array types.

Version 1.0.67 (2025-08-14)

* Updated grammar specification to reflect implemented array support.
* Removed arrays from the future work list.

Version 1.0.68 (2025-08-16)

* Functions can now declare an explicit return type after `func`.
* Updated code generation to handle typed returns.
* Added documentation and a regression test for typed return values.

Version 1.0.69 (2025-08-17)

* Implemented object instantiation for classes and structs.
* Field access via `.` is now supported in expressions.
* Updated tasks and added a regression test for object creation.

Version 1.0.70 (2025-08-18)

* Added VS Code language server extension skeleton for IntelliSense.


Version 1.0.71 (2025-08-18)

* Added JetBrains language server plugin skeleton for IntelliSense.

Version 1.0.72 (2025-08-18)

* Implemented basic JetBrains plugin completions and hover docs.
* Gradle build now produces a runnable plugin zip under `assets/jetbrains/build`.
