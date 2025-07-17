# Dream Compiler Features

## Implemented Features

- Primitive types: `int`, `float`, `bool`, `char`, and `string`
- Variable declarations and assignments
- Arrays of all primitive types
- Arithmetic operators `+`, `-`, `*`, `/`, `%` and unary minus
- Bitwise operators `&`, `|`, `^`, `~`, `<<`, and `>>`
- Comparison operators `<`, `<=`, `>`, `>=`, `==`, and `!=`
- Logical operators `&&`, `||`, and `!`
- Increment and decrement operators `++` and `--`
- Compound assignments `+=`, `-=`, `*=`, `/=`, `%=`, and reserved tokens
  `&=`, `|=`, `^=`, `<<=`, `>>=`
- Ternary conditional operator `? :`
- Additional reserved tokens `??`, `??=`, `=>`, `::`, `->`
- Parentheses for expression grouping
- If/else statements including `else if` chains
- `switch` statements with `case` and `default`
- `while`, `do-while`, and `for` loops
- `break` and `continue` statements
- `return` statements
 - Functions with parameters and typed return values
- Classes and structs with field access and object instantiation
- Console output via `Console.Write` and `Console.WriteLine`
- String literals with escape sequences and concatenation using `+`
- Line (`//`) and block (`/* */`) comments
- Basic C code generation for programs
- Object file output via `--emit-obj`

## Planned Features

- Generics for types and functions
- Access modifiers such as `public` and `private`
- Exception handling with `try`/`catch`
- Attributes and annotations
- Modular compilation and namespaces
