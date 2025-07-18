# Dream Compiler Features

## Implemented Features

- Primitive types: `int`, `float`, `bool`, `char`, and `string`
- Variable declarations with optional initialisers
- Implicitly typed local variables using `var`
- Basic arithmetic operators `+`, `-`, `*`, `/`, `%`, unary minus and unary plus
- Logical operators `&&`, `||`, and `!`
- Comparison operators `<`, `<=`, `>`, `>=`, `==`, and `!=`
- Simple assignment using `=`
- Parentheses for expression grouping
- `if`/`else` statements
- Nested block statements
- `while` loops
- `do` loops
- `for` loops
- `break` statements
- `continue` statements
- `return` statements
- Console output via `Console.Write` and `Console.WriteLine`
- Console input via `Console.ReadLine`
- String and character literals with escape sequences
- Line (`//`) and block (`/* */`) comments
- Basic C code generation for programs
- Bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`)
- Compound assignment operators (`+=`, `-=`, `*=`, `/=`, `%=` and bitwise variants)
- Increment/decrement operators `++` and `--`
- Multiple variable declarations separated by commas
- String concatenation with `+`
- Ternary conditional operator `?:`
- `switch` statements
- Arrays of primitive types
- Function declarations with parameters and typed return values
- Object construction for classes and structs
- Instance methods on classes (emitted as C functions with a `this` pointer)
- Exception handling with `try`/`catch` statements and `throw`

## Missing Features

- The following language constructs appear in the documentation or tests but are not yet fully implemented:


These features need implementing before they will compile without diagnostics.
