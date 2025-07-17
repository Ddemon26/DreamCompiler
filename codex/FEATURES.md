# Dream Compiler Features

## Implemented Features

- Primitive types: `int`, `float`, `bool`, `char`, and `string`
- Variable declarations with initialisers
- **Not working:** basic arithmetic operators `+`, `-`, `*`, `/`, `%` and unary minus (tests fail)
- **Not working:** comparison operators `<`, `<=`, `>`, `>=`, `==`, and `!=` (produce warnings)
- Simple assignment using `=`
- Parentheses for expression grouping
- `if`/`else` statements
- Nested block statements
- `while` loops
- `break` statements
- `continue` statements
- `return` statements
- Console output via `Console.Write` and `Console.WriteLine`
- String and character literals with escape sequences
- Line (`//`) and block (`/* */`) comments
- Basic C code generation for programs

## Missing Features

The following language constructs appear in the documentation or tests but are not yet implemented:

- Arrays of any type
- Bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`) and compound assignments
- Increment/decrement operators `++` and `--`
- Logical operators `&&`, `||`, and `!`
- Ternary conditional operator `?:`
- `do`/`for` loops
- `switch` statements
- Function declarations with parameters and typed return values
- Classes, structs and object construction
- String concatenation

These features need implementing before they will compile without diagnostics.
