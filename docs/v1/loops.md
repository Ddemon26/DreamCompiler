Loops in Dream
===============

Dream now supports basic looping using the `while` keyword.

Syntax
------

```
while (<expression>) <statement>
```

The loop executes the statement repeatedly while the expression evaluates to a nonzero value. Braces may surround the body and may now contain multiple statements separated by semicolons.

Example
-------

```
int i = 2;
while (i) {
    Console.WriteLine(i);
    i = i - 1;
}
```


