# Functions in Dream

Dream allows defining reusable blocks of code using the `func` keyword. Functions now support parameters and return an integer value using the `return` statement.

## Syntax

```
func <name>(<type> <param>[, <type> <param>]*) {
    <statements>
}
```

Call a function using its name followed by `()` and optionally arguments.

```
<name>(<args>);
```

## Example

```dream
func Add(int a, int b) {
    return a + b;
}

Console.WriteLine(Add(2, 3));
```
