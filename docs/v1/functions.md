# Functions in Dream

Dream allows defining reusable blocks of code using the `func` keyword. A function may specify a return type immediately after `func`. If omitted the function returns nothing (`void`).

## Syntax

```
func [returnType] <name>(<type> <param>[, <type> <param>]*) {
    <statements>
}
```

Call a function using its name followed by `()` and optionally arguments.

```
<name>(<args>);
```

## Example

```dream
func int Add(int a, int b) {
    return a + b;
}

Console.WriteLine(Add(2, 3));

func string Hello() {
    return "hi";
}

Console.WriteLine(Hello());
```
