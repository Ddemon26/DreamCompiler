# Classes in Dream

Dream now supports simple class definitions to group related variables.
A class declares a name and a list of fields. Only variable declarations
are allowed inside the class body.
The `struct` keyword is an alias for `class` and behaves the same way.

```
class Person {
    int age;
    string name;
}
```

Class definitions do not require a trailing semicolon. You can now declare
variables of a class type to create objects. Field access uses the `.` operator.
Methods are still unimplemented; classes are emitted as C `struct` types.

```
class Person {
    int age;
    string name;
}

Person p;
p.age = 30;
Console.WriteLine(p.age);
```

You can also create an instance using `new` and assign it to a variable in a single statement:

```
Person q = new Person();
q.age = 42;
Console.WriteLine(q.age);
```
