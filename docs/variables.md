Variables in Dream
Overview
The Dream language supports variable declarations and assignments with a C#-like syntax, using the int type for integers.
Syntax

Declaration: int <identifier>;
Declares a variable with an initial value of 0.


Assignment: <identifier> = <expression>;
Assigns a value (number or expression) to a variable.



Examples
int x;        // Declares x, initialized to 0
x = 42;       // Assigns 42 to x
int y = 10;   // Declares and assigns 10 to y

Notes

Variables are stored on the stack in the generated assembly.
Only int type is supported currently.