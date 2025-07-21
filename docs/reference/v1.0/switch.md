# Switch Statements

Dream now supports `switch` statements for selecting between multiple constant cases.

## Syntax

```dream
switch (<expression>) {
    case <number>: <statement>
    case <number>: {
        <statements>
    }
    default: <statement>
}
```

Only integer case labels are allowed. Each case executes its body and automatically
breaks; fallthrough between cases is not supported. The optional `default` block
runs when no case matches.
