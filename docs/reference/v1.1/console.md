# Console I/O

Dream's standard library exposes simple console functions.

- `Console.Write` and `Console.WriteLine` output text.
- `Console.ReadLine` reads a line from standard input.
- `Console.Read` reads a single character from standard input.

When a complete `Write` or `WriteLine` statement takes the direct result of a
[proven string factory](../../development/string-ownership.md#direct-console-output-of-proven-factory-results-p0057),
the compiler releases that result after printing it.

In `Console.WriteLine(text += makeText());` or `Console.Write(text += makeText());`,
an owned scalar `text` keeps the joined value. If `makeText()` passes the factory
proof, the compiler releases the original operand after appending and before
printing the join. The rule includes factories that return a proven join.
See [string append ownership](../../development/string-ownership.md#console-output-of-appends-with-proven-returned-joins-p0071).

```dream
Console.Write("Enter name: ");
string name = Console.ReadLine();
Console.WriteLine("Hello, " + name);
```

When building the compiler itself, these console methods are provided via
`console_debug.h`. In debug mode they write to `stderr` (or read from `stdin`),
while in release builds they compile to no-ops so the compiler remains silent.

