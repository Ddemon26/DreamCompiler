# Dream.dr Grammar Specification

*Version 0.2  – 15 July 2025*

> **Scope** This document formally defines the lexical structure and context‑free grammar accepted by the **Dream** 
> compiler ( `DreamCompiler` ).  The language is deliberately close to **C# 12** but pares away advanced features 
> such as namespaces, generics, attributes and async code.  Unless stated otherwise, Dream behaves exactly like C# for the constructs that the compiler currently supports.

---

## 1 Lexical structure

### 1.1 Character set & encoding

Dream source files are encoded in **UTF‑8**.  Each Unicode code point is significant; the compiler makes no attempt to normalise different code‑point sequences.

### 1.2 Line terminators

`CR`, `LF` or the two‑character sequence `CR LF` are *line terminators*.  They may appear inside string literals 
only when escaped (see § 1.6).

### 1.3 Whitespace

```
WhiteSpace ::=  " " | "\t" | "\r" | "\n" | \u000B | \u000C
```

Whitespace between tokens is ignored except that it may not appear inside multi‑character punctuators (e.g. the two characters of `>=`).

### 1.4 Comments

```
SingleLineComment   ::= "//" InputCharacters? LineTerminator
MultiLineComment    ::= "/*" MultiLineCommentChars? "*/"
DocComment          ::= "///" InputCharacters? LineTerminator
```

Comments are replaced by a single whitespace character prior to tokenisation.

### 1.5 Identifiers

Identifiers follow the C# rules:\*\*

```
Identifier          ::= IdentifierStart IdentifierPart*
IdentifierStart     ::= LetterCharacter | "_"
IdentifierPart      ::= IdentifierStart | DecimalDigitCharacter
```

Dream is *case‑sensitive*.

### 1.6 Literals

```
BooleanLiteral      ::= "true" | "false"
IntegerLiteral      ::= DecimalDigits
StringLiteral       ::= "\"" StringCharacters* "\""
```

(A future release will add `float` and `char` literals.)

### 1.7 Keywords

```
keyword ::=
    "break" | "bool" | "case" | "class" | "continue" | "default" |
    "do" | "else" | "false" | "for" | "func" | "if" | "int" | "return" |
    "string" | "switch" | "true" | "while"
```

### 1.8 Operators & punctuators

```
Punctuator ::= "{" | "}" | "(" | ")" | "[" | "]" | ";" | "," | "."
Operator   ::= "++" | "--" | "+=" | "-=" | "*=" | "/=" | "%=" |
               "+"  | "-"  | "*"  | "/"  | "%"  | "^" | "<<" | ">>" |
               "<=" | ">=" | "==" | "!=" | "<"  | ">" |
               "&&" | "||" | "&"  | "|"  | "!"  | "=" | "?"
```

---

## 2 Notation

The grammar is written in **EBNF** with the following meta‑symbols: `::=` *is defined as* ‖ `|` *alternative* ‖ `{ … }` 
*zero or more* ‖ `[ … ]` *optional* ‖ `…*` *one or more*.

Tokens (terminals) appear in **bold** or in `code font`.

---

## 3 Syntax grammar

### 3.1 Compilation unit

```
CompilationUnit     ::= { UsingDirective } { TopLevelDeclaration } EOF
UsingDirective      ::= "using" Identifier ";"
TopLevelDeclaration ::= ClassDeclaration
                     |  FunctionDeclaration
                     |  GlobalVariableDeclaration
```

### 3.2 Types

```
Type                ::= PredefinedType | Identifier
PredefinedType      ::= "int" | "bool" | "string" | "float"
```

### 3.3 Declarations

#### 3.3.1 Class

```
ClassDeclaration    ::= "class" Identifier "{" { ClassMemberDeclaration } "}"
ClassMemberDeclaration ::= FunctionDeclaration | VariableDeclaration | ";"
```

#### 3.3.2 Function

```
FunctionDeclaration ::= "func" [ Type ] Identifier
                         "(" [ ParameterList ] ")" Block
ParameterList       ::= Parameter { "," Parameter }
Parameter           ::= Type Identifier
```

If the **Type** is omitted the function returns `void`.

#### 3.3.3 Variables

```
GlobalVariableDeclaration ::= VariableDeclaration
VariableDeclaration      ::= Type VariableDeclarator { "," VariableDeclarator } ";"
VariableDeclarator       ::= Identifier [ "=" Expression ]
```

### 3.4 Statements

```
Statement           ::= Block
                     | VariableDeclaration
                     | IfStatement
                     | WhileStatement
                     | DoStatement
                     | ForStatement
                     | SwitchStatement
                     | BreakStatement
                     | ContinueStatement
                     | ReturnStatement
                     | ExpressionStatement

Block               ::= "{" { Statement } "}"
ExpressionStatement ::= Expression ";"

IfStatement         ::= "if" "(" Expression ")" Statement [ "else" Statement ]
WhileStatement      ::= "while" "(" Expression ")" Statement
DoStatement         ::= "do" Statement "while" "(" Expression ")" ";"
ForStatement        ::= "for" "(" [ ForInit ] ";" [ Expression ] ";" [ ForUpdate ] ")" Statement
ForInit             ::= VariableDeclaration | ExpressionList
ForUpdate           ::= ExpressionList
ExpressionList      ::= Expression { "," Expression }

SwitchStatement     ::= "switch" "(" Expression ")" "{" { SwitchSection } "}"
SwitchSection       ::= [ "case" ConstantExpression ":" | "default" ":" ] { Statement }

BreakStatement      ::= "break" ";"
ContinueStatement   ::= "continue" ";"
ReturnStatement     ::= "return" [ Expression ] ";"
```

### 3.5 Expressions (operator‑precedence form)

```
Expression          ::= Assignment
Assignment          ::= Conditional [ AssignmentOperator Assignment ]
AssignmentOperator  ::= "=" | "+=" | "-=" | "*=" | "/=" | "%="
Conditional         ::= LogicalOr [ "?" Expression ":" Conditional ]
LogicalOr           ::= LogicalAnd { "||" LogicalAnd }
LogicalAnd          ::= BitwiseOr { "&&" BitwiseOr }
BitwiseOr           ::= BitwiseXor { "|" BitwiseXor }
BitwiseXor          ::= BitwiseAnd { "^" BitwiseAnd }
BitwiseAnd          ::= Equality { "&" Equality }
Equality            ::= Relational { ("==" | "!=") Relational }
Relational          ::= Shift { ("<" | "<=" | ">" | ">=") Shift }
Shift               ::= Additive { ("<<" | ">>") Additive }
Additive            ::= Multiplicative { ("+" | "-") Multiplicative }
Multiplicative      ::= Unary { ("*" | "/" | "%") Unary }
Unary               ::= ("+" | "-" | "!" | "++" | "--") Unary | Postfix
Postfix             ::= Primary { PostfixPart }
PostfixPart         ::= "++" | "--" | MemberAccess | Invocation
MemberAccess        ::= "." Identifier
Invocation          ::= "(" [ ArgumentList ] ")"
ArgumentList        ::= Expression { "," Expression }
Primary             ::= Literal | Identifier | "(" Expression ")" | ConsoleCall
ConsoleCall         ::= "Console" "." ("WriteLine" | "Write") Invocation
```

### 3.6 Operator precedence and associativity

| Precedence | Category       | Operators               | Associativity |               |               |
| ---------- | -------------- |-------------------------| ------------- | ------------- | ------------- |
| 15         | Postfix        | `x++ x-- f() a.b`       | left‑to‑right |               |               |
| 14         | Unary          | `++x --x +x -x !x`      | right‑to‑left |               |               |
| 13         | Multiplicative | `*  /  %`               | left‑to‑right |               |               |
| 12         | Additive       | `+  -`                  | left‑to‑right |               |               |
| 11         | Shift          | `<<  >>`                | left‑to‑right |               |               |
| 10         | Relational     | `<  <=  >  >=`          | left‑to‑right |               |               |
| 9          | Equality       | `==  !=`                | left‑to‑right |               |               |
| 8          | Bitwise AND    | `&`                     | left‑to‑right |               |               |
| 7          | Bitwise XOR    | `^`                     | left‑to‑right |               |               |
| 6          | Bitwise OR     | \`                      | \`            | left‑to‑right |               |
| 5          | Logical AND    | `&&`                    | left‑to‑right |               |               |
| 4          | Logical OR     | \`                      |               | \`            | left‑to‑right |
| 3          | Conditional    | `?:`                    | right‑to‑left |               |               |
| 2          | Assignment     | `=  +=  -=  *=  /=  %=` | right‑to‑left |               |               |
| 1          | Sequence       | `,`                     | left‑to‑right |               |               |

---

## 4 Semantic differences from C# (current version)

1. **Top‑level functions and variables** are permitted; there is no concept of a namespace yet.
2. The `func` keyword replaces C#’s return‑type‑first syntax.  A *type* may optionally follow `func` to specify the return type; omit it for `void`.
3. Only the primitive types `int`, `bool`, `string` (and experimental `float`) are implemented today.
4. All data is **passed by value**; there are no references or pointers.
5. `Console.Write`/`WriteLine` are *built‑ins* rather than part of a standard library.
6. The compiler currently emits C, so semantics align with **32‑bit signed integers** and **zero‑terminated UTF‑8 strings**.

---

## 5 Future work (non‑normative)

- Arrays, structs and generics
- Access modifiers (`public`, `private`, …)
- Exception handling (`try`‑`catch`)
- Attributes and annotations
- Modular compilation & namespaces

---

## 6 Change log

| Version | Date       | Notes                                                |
|---------|------------| ---------------------------------------------------- |
| 0.2     | 2025‑07‑15 | Initial public draft extracted from compiler source. |

