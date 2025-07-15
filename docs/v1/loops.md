# Loops in Dream

Dream supports looping using the `while`, `do` and `for` keywords.

Syntax
------

```
while (<expression>) <statement>
do <statement> while (<expression>);
for (<init>; <condition>; <increment>) <statement>
```

The `while` loop executes the statement repeatedly while the expression evaluates to a nonzero value. The `do` form always executes the statement at least once and then continues looping while the expression is nonzero. The `for` loop first runs the initialization clause, then repeatedly tests the condition and executes the body followed by the increment clause. Braces may surround the body and may contain multiple statements separated by semicolons.

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

for (int k = 0; k < 3; k = k + 1)
    Console.WriteLine(k);
```



