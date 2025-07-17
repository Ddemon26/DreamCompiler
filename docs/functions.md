# Functions

Dream supports defining functions using the `func` keyword. A return type may be specified before the function name; omitting it defaults to `void`.

```dream
func PrintOne() {
    Console.WriteLine(1);
}

func int Add(int a, int b) {
    return a + b;
}

Console.WriteLine(Add(2, 3));
```
