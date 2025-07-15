# Arithmetic in Dream

## Overview

Dream supports basic arithmetic operations. Addition (`+`) was the first
operator implemented and subtraction (`-`) is now also available. Multiplication
using the `*` operator has been introduced as well. Division with the `/`
operator is now supported. Modulo with the `%` operator is now available too.
Bitwise XOR with the `^` operator is now supported.

## Syntax

**Addition**: `<identifier> = <expression> + <expression>;`
: Adds two expressions (variables or numbers) and assigns the result.

**Subtraction**: `<identifier> = <expression> - <expression>;`
: Subtracts the right expression from the left expression.

**Multiplication**: `<identifier> = <expression> * <expression>;`
: Multiplies two expressions together and assigns the result.

**Division**: `<identifier> = <expression> / <expression>;`
: Divides the left expression by the right expression and assigns the result.

**Modulo**: `<identifier> = <expression> % <expression>;`
: Stores the remainder of dividing the left expression by the right.

**Bitwise XOR**: `<identifier> = <expression> ^ <expression>;`
: Computes the bitwise exclusive-or of two expressions.

**Compound assignment**: `<identifier> <op>= <expression>;`
: Shorthand for `<identifier> = <identifier> <op> <expression>` using
  `+=`, `-=`, `*=`, `/=` and `%=`.

## Examples

```dream
int x = 5;
int y = 10;
x = y + 5;   // x = 15
x = y - 3;   // x = 7
x = y * 2;   // x = 20
x = y / 5;   // x = 2
x = y % 4;   // x = 2
x = y ^ 3;   // bitwise XOR
x += 3;      // x = x + 3
```

## Notes

Operands can be variables or literal numbers.
The result is stored in the left-hand variable.
Negative numbers can be written by prefixing a value with `-`, e.g. `int x = -5;`.
Float literals use a decimal point, for example `float f = 1.5;`.
Parentheses may be used to group sub-expressions, e.g. `int x = (2 + 3) * 4;`.
