# Arrays in Dream

Dream now supports simple integer arrays with fixed sizes.

## Syntax

```
int <name>[<size>];
<name>[<index>] = <expression>;
```

Array elements are indexed starting at zero. Only `int` arrays are supported and
initialization at declaration is not yet available.

## Example

```dream
int nums[3];
nums[0] = 1;
nums[1] = nums[0] + 1;
Console.WriteLine(nums[1]);
```
