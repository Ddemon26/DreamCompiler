# Conditional Operator

Dream supports the ternary conditional operator for concise expressions.

## Syntax

```dream
<condition> ? <trueExpr> : <falseExpr>
```

The condition is evaluated first. If it is nonzero, the `trueExpr` value is returned; otherwise the `falseExpr` value is used.

## Example

```dream
int a = 1;
int b = a ? 2 : 3;
Console.WriteLine(b); // prints 2
```
