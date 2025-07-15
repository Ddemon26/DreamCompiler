# Logical Operators in Dream

Dream now supports logical conjunction and disjunction using the `&&` and `||` operators.

## Syntax

```
<expression> && <expression>
<expression> || <expression>
```

These expressions evaluate to a nonzero value when the logic holds true. They are typically used in conditions for `if` statements and `while` loops.

## Example

```dream
int a = 1;
int b = 0;
if (a && b) Console.WriteLine(a);
if (a || b) Console.WriteLine(a); // prints 1
```
