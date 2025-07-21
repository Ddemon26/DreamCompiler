# Loops

Dream.dr implements three looping constructs.

- `while` evaluates its condition before each iteration.
- `do` executes the body at least once before checking the condition.
- `for` combines initialiser, condition and increment in one statement.

`break` exits the nearest loop and `continue` skips to the next iteration.

```dream
var sum = 0;
for (var i = 0; i < 10; i++) {
    if (i == 5) continue;
    sum += i;
    if (sum > 40) break;
}
```

