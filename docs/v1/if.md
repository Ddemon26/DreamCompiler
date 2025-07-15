# If Statements in Dream

Dream supports conditional execution using the `if` keyword. `else` and `else if` clauses provide alternate branches.

Syntax
- -----

```
if (<expression>) <statement>
else if (<expression>) <statement>
else <statement>
```

The `else` section is optional. Each branch may be a single statement or a block enclosed in braces. Braced blocks can contain multiple statements separated by semicolons.

Example
- ------

```
int x = 1;
if (x) {
    Console.WriteLine(x);
    Console.WriteLine(0);
} else {
    Console.WriteLine(0);
}
```

This prints `1` because `x` is nonzero.

You can chain conditions using `else if`:

```
int y = 0;
if (y == 1)
    Console.WriteLine(1);
else if (y == 0)
    Console.WriteLine(2);
else
    Console.WriteLine(3);
```

