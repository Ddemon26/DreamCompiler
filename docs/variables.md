# Variables

Dream.dr supports `int`, `bool` and `string` variables. Declarations use C-style syntax:

```dream
int x = 1;
bool flag;
string message = "hi";
```

Multiple variables may be declared in one statement separated by commas.

Local variables may also use `var` for simple type inference when an initializer is present:

```dream
var count = 5; // inferred as int
```

