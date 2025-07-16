# Arrays in Dream

Dream now supports fixed-size arrays of both integers and floats.

## Syntax

```
int <name>[<size>];
float <name>[<size>];
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
