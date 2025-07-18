# Classes in Dream

Dream now supports simple class definitions with fields and methods.
A class declares a name and a list of members. Methods are emitted as
regular C functions taking a `this` pointer. The `struct` keyword remains
an alias for `class`.

```
class Person {
    int age;
    string name;
}
```

Class definitions do not require a trailing semicolon. Variables of a class
type are references (implemented as pointers), so `new` allocates storage.
Field access still uses the `.` operator in Dream source.

```
class Person {
    int age;
    string name;
}

Person p = new Person();
p.age = 30;
Console.WriteLine(p.age);
```

You can also create an instance using `new` and assign it to a variable in a single statement:

```
Person q = new Person();
q.age = 42;
Console.WriteLine(q.age);
```
