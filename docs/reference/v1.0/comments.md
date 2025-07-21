# Comments in Dream

Dream supports two forms of comments that are ignored by the compiler.

## Single-line comments

Start with `//` and extend to the end of the line.

## Block comments

Begin with `/*` and end with `*/`. They may span multiple lines but cannot be nested.

## Example

```dream
int x = 5; // single line

/* multi-line
   comment example */
Console.WriteLine(x);
```
