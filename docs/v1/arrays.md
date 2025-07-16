# Arrays in Dream

Dream now supports fixed-size arrays of integers, floats, booleans,
characters and strings.

## Syntax

```
int <name>[<size>];
float <name>[<size>];
bool <name>[<size>];
char <name>[<size>];
string <name>[<size>];
<name>[<index>] = <expression>;
```

Array elements are indexed starting at zero. Initialization at declaration is
not yet available.

## Example

```dream
int nums[3];
nums[0] = 1;
nums[1] = nums[0] + 1;
Console.WriteLine(nums[1]);
```

```dream
float values[2];
values[0] = 1.5;
values[1] = values[0] + 2.5;
Console.WriteLine(values[1]);
```

```dream
bool flags[2];
flags[0] = true;
flags[1] = false;
Console.WriteLine(flags[0]);
```

```dream
char letters[2];
letters[0] = 'A';
letters[1] = 'B';
Console.WriteLine(letters[1]);
```

```dream
string words[2];
words[0] = "hi";
words[1] = words[0] + " there";
Console.WriteLine(words[1]);
```
