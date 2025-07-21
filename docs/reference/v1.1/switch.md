# Switch Statements

`switch` provides multi-way branching on integer values with full support for `break` statements and multiple statements per case.

## Basic Syntax

```dream
switch (expression) {
    case constant1:
        // statements
        break;
    case constant2:
        // statements
        break;
    default:
        // statements
        break;
}
```

## Multiple Statements per Case

Each case can contain multiple statements before the `break`:

```dream
int score = 85;
switch (score / 10) {
    case 10:
    case 9:
        Console.WriteLine("Grade: A");
        Console.WriteLine("Excellent work!");
        break;
    case 8:
        Console.WriteLine("Grade: B");
        Console.WriteLine("Good job!");
        break;
    case 7:
        Console.WriteLine("Grade: C");
        Console.WriteLine("Satisfactory");
        break;
    default:
        Console.WriteLine("Grade: F");
        Console.WriteLine("Please study more");
        break;
}
```

## Fall-through Behavior

Without `break` statements, cases will fall through to the next case (C-style behavior):

```dream
int value = 2;
switch (value) {
    case 1:
        Console.WriteLine("One");
    case 2:
        Console.WriteLine("Two");  // This will execute
    default:
        Console.WriteLine("Other"); // This will also execute
}
// Output: "Two\nOther"
```

## Break Statements

Break statements are fully supported and will exit the switch:

```dream
switch (value) {
    case 1:
        Console.WriteLine("one");
        break;  // Exit switch here
    case 2:
        Console.WriteLine("two");
        break;  // Exit switch here
    default:
        Console.WriteLine("other");
        break;  // Exit switch here (optional for last case)
}
```

## Nested Constructs

Switch statements can contain other control flow constructs:

```dream
for (int i = 0; i < 3; i++) {
    Console.WriteLine("Loop iteration: " + i);
    
    switch (i) {
        case 0:
            Console.WriteLine("First iteration");
            break;
        case 1:
            Console.WriteLine("Second iteration");
            break;
        default:
            Console.WriteLine("Default case");
            break;
    }
}
```

## Implementation Notes

- Switch expressions are evaluated once at the beginning
- Cases must be compile-time constants (literals or constant expressions)
- The `default` case is optional but recommended
- Multiple statements per case are fully supported
- Break statements generate proper C `break;` statements

