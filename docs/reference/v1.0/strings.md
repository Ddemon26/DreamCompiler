# String Literals

Dream string literals support common C-style escape sequences. Use backslashes to include special characters:

- `\n` - newline
- `\t` - tab
- `\\` - backslash
- `\"` - double quote

Example:

```dream
Console.WriteLine("Line 1\nLine 2");
Console.WriteLine("She said \"hi\"");
```

Strings can be concatenated using the `+` operator:

```dream
string greeting = "Hello, " + "world";
Console.WriteLine(greeting);
```
