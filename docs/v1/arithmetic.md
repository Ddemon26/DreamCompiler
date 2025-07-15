Arithmetic in Dream
Overview
Dream supports basic arithmetic operations. Addition (+) was the first
operator implemented and subtraction (-) is now also available. Multiplication
using the `*` operator has been introduced as well. Division with the `/`
operator is now supported.
Syntax

Addition: <identifier> = <expression> + <expression>;
Adds two expressions (variables or numbers) and assigns the result.

Subtraction: <identifier> = <expression> - <expression>;
Subtracts the right expression from the left expression.

Multiplication: <identifier> = <expression> * <expression>;
Multiplies two expressions together and assigns the result.

Division: <identifier> = <expression> / <expression>;
Divides the left expression by the right expression and assigns the result.



Examples
int x = 5;
int y = 10;
x = y + 5;   // x = 15
x = y - 3;   // x = 7
x = y * 2;   // x = 20
x = y / 5;   // x = 2

Notes

Operands can be variables or literal numbers.
Result is stored in the left-hand variable.