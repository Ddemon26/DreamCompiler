# Functions in Dream

Dream allows defining reusable blocks of code using the `func` keyword. Functions currently take no parameters and return an integer value using the `return` statement.

## Syntax

```
func <name>() {
    <statements>
}
```

Call a function using its name followed by `()`.

```
<name>();
```

## Example

```dream
func GetFive() {
    return 5;
}

Console.WriteLine(GetFive());
```
