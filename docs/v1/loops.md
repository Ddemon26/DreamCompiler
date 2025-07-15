# Loops in Dream

Dream supports looping using the `while` and `do` keywords.

Syntax
------

```
while (<expression>) <statement>
do <statement> while (<expression>);
```

The `while` loop executes the statement repeatedly while the expression evaluates to a nonzero value. The `do` form always executes the statement at least once and then continues looping while the expression is nonzero. Braces may surround the body and may contain multiple statements separated by semicolons.

Example
-------

```
int i = 2;
while (i) {
    Console.WriteLine(i);
    i = i - 1;
}

int j = 0;
do {
    j = j + 1;
} while (j < 3);
```



