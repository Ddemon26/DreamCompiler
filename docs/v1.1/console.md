# Console I/O

Dream's standard library exposes simple console functions.

- `Console.Write` and `Console.WriteLine` output text.
- `Console.ReadLine` reads a line from standard input.

```dream
Console.Write("Enter name: ");
string name = Console.ReadLine();
Console.WriteLine("Hello, " + name);
```

