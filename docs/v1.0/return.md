# Return Statement in Dream

The `return` statement exits the program immediately.

## Syntax

```
return;
return <expression>;
```

When encountered, the generated C program returns from `main` with the given value or zero if omitted.

## Example

```dream
Console.WriteLine(1);
return;
Console.WriteLine(2); // not executed
```
