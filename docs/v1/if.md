# If Statements in Dream

Dream supports conditional execution using the `if` keyword and an optional `else` clause.

Syntax
------

```
if (<expression>) <statement>
else <statement>
```

The `else` section is optional. Each branch may be a single statement or a block enclosed in braces. Braced blocks can contain multiple statements separated by semicolons.

Example
-------

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

