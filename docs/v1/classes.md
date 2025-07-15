# Classes in Dream

Dream now supports simple class definitions to group related variables.
A class declares a name and a list of fields. Only variable declarations
are allowed inside the class body.

```
class Person {
    int age;
    string name;
}
```

Class definitions do not require a trailing semicolon. Methods and object
instantiation are not yet implemented; classes are currently emitted as
C `struct` types.
