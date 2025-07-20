```markdown
# Multi-file Projects

Dream supports compiling projects spread across multiple `.dr` files. This allows for better code organization by separating concerns into different modules.

The compiler can take multiple source files as input, compile them, and link them into a single executable.

## Example

Let's create a simple project with two files: `math.dr` which contains a utility function, and `main.dr` which is the main entry point.

### `math.dr`

This file defines a simple `add` function. By default, functions are public and can be imported by other files.

```dream
// file: math.dr
func add(int a, int b) -> int {
    return a + b;
}
```

### `main.dr`

This file imports `math.dr` to use the `add` function. The `import` statement makes the contents of `math.dr` available.

```dream
// file: main.dr
import "math.dr";

func main() {
    int result = add(10, 5);
    Console.WriteLine("The result is: " + result);
}
```

## Compilation

To compile a multi-file project, you provide all the source files to the compiler. The `zig build run` command can be used to compile and execute the project.

```bash
# Place both main.dr and math.dr in the same directory
zig build run -- main.dr math.dr
```

This command will compile both `main.dr` and `math.dr`, link them together, and run the resulting executable.

### Expected Output

```
The result is: 15
```