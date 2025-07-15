Comparison Operators in Dream
=============================

Dream now supports basic comparison operators for building conditions.

Syntax
------

```
<expression> < <expression>
<expression> > <expression>
```

These expressions evaluate to a nonzero value when true and zero otherwise.
They are typically used in `if` statements and `while` loops.

Example
-------

```
int x = 5;
int y = 3;
if (x > y) Console.WriteLine(x); // prints 5
```

