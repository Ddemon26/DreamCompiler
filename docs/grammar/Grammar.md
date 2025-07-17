# Dream.dr Grammar Specification

*Version 0.3 – 16 July 2025*

> **Scope** This document formally defines the lexical structure and context‑free grammar accepted by the **Dream** compiler (`DreamCompiler`). It builds on the *core* Dream subset (a deliberately pared‑down, C#‑like language) and now incorporates — for reference and forward compatibility — the complete lexical and syntactic productions drawn from the upstream C# 12 grammar, including *unsafe* constructs. Only the *core* subset is implemented in Dream v0.3 unless otherwise marked; everything else is parsed at most for graceful error recovery and will usually produce diagnostics.

Sources: core grammar from the v0.2 draft; extended lexical grammar (§6) and syntactic grammar (§7‑22) from the C# 12 specification; unsafe extensions (§23).

---

## What's new in v0.3

* Integrated **Lexical grammar** additions (numeric literal forms, verbatim & interpolated strings, `null`, preprocessor directives, extended operator set, full keyword lists).
* Integrated **Syntactic grammar** additions (namespaces, generics, attributes, patterns, lambdas, queries, etc.) as forward‑looking references.
* Integrated **Unsafe grammar** additions (pointer types & ops, `fixed`, address‑of, fixed‑size buffers) as reserved constructs.
* Added missing core tokens (`float`, `char`, `void` return via omission) and clarified built‑in `Console.Write`/`WriteLine`.
* Cleaned up operator table formatting; added bitwise NOT `~` (reserved).

---

### Status legend

| Status      | Meaning                                  |
| ----------- | ---------------------------------------- |
| Implemented | Feature is fully supported in Dream v0.3 |
| Partial     | Parsed but not yet code‑generated        |
| Reserved    | Token reserved; parser rejects           |

---

## 1 Lexical structure (core + extensions)

Dream source files are UTF‑8; each Unicode code point is significant (no normalisation). Line terminators are `CR`, `LF`, or `CR LF` and may appear in string literals only when escaped. Whitespace (Unicode class Zs plus HT/VT/FF/CR/LF) is ignored between tokens except inside multi‑char punctuators. Comments (`//`, `/*...*/`, `///` doc) are replaced by a single space before tokenisation. Identifiers follow C# Unicode rules (letter/underscore start; letters, digits, connector, combining, formatting thereafter; escaped identifiers with `@`). Literals include boolean, integer (decimal/hex/binary with `_` separators & suffixes), real (decimal forms, exponent, suffix), char (escape sequences, `\x`, `\u`, `\U`), regular & verbatim string literals, interpolated strings (`$"...{expr}..."`, `@"..."`, `$@"..."`), and `null`. Preprocessor directives beginning with `#` at line start are tokenised separately. See *Appendix A* for the full lexical reference.

### 1.1 Reserved keywords

Dream reserves the following tokens (union of the core subset and the full C# keyword + contextual keyword sets). Only the *core* subset is currently recognised semantically; the rest are reserved and will generally produce diagnostics if used where unsupported.

**Core keywords**: `break`, `bool`, `case`, `char`, `class`, `struct`, `continue`, `default`, `do`, `else`, `false`, `for`, `func`, `if`, `int`, `return`, `string`, `switch`, `true`, `while`, `float`.

**Additional reserved keywords (future)**: `abstract`, `as`, `base`, `byte`, `catch`, `checked`, `const`, `decimal`, `delegate`, `double`, `enum`, `event`, `explicit`, `extern`, `fixed`, `foreach`, `goto`, `implicit`, `in`, `interface`, `internal`, `is`, `lock`, `long`, `namespace`, `new`, `null`, `object`, `operator`, `out`, `override`, `params`, `private`, `protected`, `public`, `readonly`, `ref`, `sbyte`, `sealed`, `short`, `sizeof`, `stackalloc`, `static`, `this`, `throw`, `typeof`, `uint`, `ulong`, `unchecked`, `unsafe`, `ushort`, `using`, `virtual`, `void`, `volatile`, *contextual*: `add`, `alias`, `ascending`, `async`, `await`, `by`, `descending`, `dynamic`, `equals`, `from`, `get`, `global`, `group`, `into`, `join`, `let`, `nameof`, `notnull`, `on`, `orderby`, `partial`, `remove`, `select`, `set`, `unmanaged`, `value`, `var`, `when`, `where`, `yield`.

### 1.2 Operators & punctuators (union)

The combined token set recognised by the lexer includes all C# operators and punctuators; Dream core implements a subset (marked below).

| Token                               |      Core?     | Notes                                            |                                  |                        |
| ----------------------------------- | :------------: | ------------------------------------------------ | -------------------------------- | ---------------------- |
| `{` `}` `(` `)` `[` `]` `;` `,` `.` |   Implemented  | Block / grouping / list separators.              |                                  |                        |
| `+` `-` `*` `/` `%`                 |   Implemented  | Arithmetic.                                      |                                  |                        |
| `++` `--`                           |   Implemented  | Pre/Post inc/dec.                                |                                  |                        |
| `+=` `-=` `*=` `/=` `%=`            |   Implemented  | Compound assignment.                             |                                  |                        |
| `<` `<=` `>` `>=`                   |   Implemented  | Relational.                                      |                                  |                        |
| `==` `!=`                           |   Implemented  | Equality.                                        |                                  |                        |
| `&` \`                              |     ` `^\`     | Implemented                                      | Bitwise; also logical with bool. |                        |
| `&&` \`                             |                | \`                                               | Implemented                      | Short‑circuit logical. |
| `!`                                 |   Implemented  | Logical negation.                                |                                  |                        |
| `=`                                 |   Implemented  | Simple assign.                                   |                                  |                        |
| `? :`                               |   Implemented  | Conditional operator.                            |                                  |                        |
| `<<` `>>`                           |   Implemented  | Shift.                                           |                                  |                        |
| `~`                                 |     Partial    | Bitwise complement (reserved).                   |                                  |                        |
| `??` `??=`                          |     Partial    | Null‑coalescing (future).                        |                                  |                        |
| `=>`                                |     Partial    | Lambda / switch expr / property bodies (future). |                                  |                        |
| `::`                                |     Partial    | Alias qualifier (future).                        |                                  |                        |
| `->`                                |     Partial    | Pointer member access (unsafe).                  |                                  |                        |
| `&=` \`                             | =` `^=` `<<=\` | Partial                                          | Compound assign (future).        |                        |
| *right\_shift\_assignment*          |     Partial    | Lexer tokenisation detail.                       |                                  |                        |

---

## 2 Core syntax grammar (Dream subset)

> This section is normative for Dream v0.3. Everything shown here is implemented unless specifically marked otherwise.

### 2.1 Compilation unit

```ebnf
CompilationUnit     ::= { UsingDirective } { TopLevelDeclaration } EOF                // Implemented
UsingDirective      ::= "using" Identifier ";"                                        // Partial (parsed; ignored until modules)
TopLevelDeclaration ::= ClassDeclaration                                              // Implemented
                     |  FunctionDeclaration                                           // Implemented
                     |  GlobalVariableDeclaration                                     // Implemented
```

### 2.2 Types

```ebnf
Type                ::= PredefinedType | Identifier                                   // Implemented
PredefinedType      ::= "int" | "bool" | "string" | "float" | "char"                  // Implemented
```

### 2.3 Declarations

#### 2.3.1 Class and Struct

```ebnf
ClassDeclaration    ::= "class" Identifier "{" { ClassMemberDeclaration } "}"         // Partial (parsed; fields & funcs code‑gen)
StructDeclaration   ::= "struct" Identifier "{" { ClassMemberDeclaration } "}"        // Partial
ClassMemberDeclaration ::= FunctionDeclaration | VariableDeclaration | ";"            // Implemented
```

#### 2.3.2 Function

```ebnf
FunctionDeclaration ::= "func" [ Type ] Identifier "(" [ ParameterList ] ")" Block    // Implemented
ParameterList       ::= Parameter { "," Parameter }                                   // Implemented
Parameter           ::= Type Identifier                                               // Implemented
```

If the **Type** is omitted the function returns `void`. (Internally represented as a zero‑sized pseudo‑type.)

#### 2.3.3 Variables

```ebnf
GlobalVariableDeclaration ::= VariableDeclaration                                     // Implemented
VariableDeclaration      ::= Type VariableDeclarator { "," VariableDeclarator } ";"   // Implemented
VariableDeclarator       ::= Identifier [ "[" Number "]" ] [ "=" Expression ]         // Implemented – supports arrays of primitive types
```

### 2.4 Statements

```ebnf
Statement           ::= Block                                                         // Implemented
                     | VariableDeclaration                                            // Implemented
                     | IfStatement                                                    // Implemented
                     | WhileStatement                                                 // Implemented
                     | DoStatement
      // Implemented
      // Planned
                     | ForStatement                                                   // Planned
                     | SwitchStatement                                                // Partial (basic cases only)
                     | BreakStatement                                                 // Implemented
                     | ContinueStatement                                              // Implemented
                     | ReturnStatement                                                // Implemented
                     | ExpressionStatement                                            // Implemented

Block               ::= "{" { Statement } "}"                                         // Implemented
ExpressionStatement ::= Expression ";"                                                // Implemented

IfStatement         ::= "if" "(" Expression ")" Statement [ "else" Statement ]        // Implemented
WhileStatement      ::= "while" "(" Expression ")" Statement                          // Implemented
DoStatement        ::= "do" Statement "while" "(" Expression ")" ";"
      // Implemented
      // Planned
ForStatement        ::= "for" "(" [ ForInit ] ";" [ Expression ] ";" [ ForUpdate ] ")" Statement // Planned
ForInit             ::= VariableDeclaration | ExpressionList                          // Planned
ForUpdate           ::= ExpressionList                                                // Planned
ExpressionList      ::= Expression { "," Expression }                                 // Planned

SwitchStatement     ::= "switch" "(" Expression ")" "{" { SwitchSection } "}"         // Partial
SwitchSection       ::= [ "case" ConstantExpression ":" | "default" ":" ] { Statement }// Planned

BreakStatement      ::= "break" ";"                                                   // Implemented
ContinueStatement   ::= "continue" ";"                                                // Implemented
ReturnStatement     ::= "return" [ Expression ] ";"                                   // Implemented
```

### 2.5 Expressions (operator‑precedence form)

```ebnf
Expression          ::= Assignment                                                     // Implemented
Assignment          ::= Conditional [ AssignmentOperator Assignment ]                  // Implemented
AssignmentOperator  ::= "=" | "+=" | "-=" | "*=" | "/=" | "%="                         // Implemented
Conditional         ::= LogicalOr [ "?" Expression ":" Conditional ]                   // Implemented
LogicalOr           ::= LogicalAnd { "||" LogicalAnd }                                 // Implemented
LogicalAnd          ::= BitwiseOr { "&&" BitwiseOr }                                   // Implemented
BitwiseOr           ::= BitwiseXor { "|" BitwiseXor }                                  // Implemented
BitwiseXor          ::= BitwiseAnd { "^" BitwiseAnd }                                  // Implemented
BitwiseAnd          ::= Equality { "&" Equality }                                      // Implemented
Equality            ::= Relational { ("==" | "!=") Relational }                        // Implemented
Relational          ::= Shift { ("<" | "<=" | ">" | ">=") Shift }                      // Implemented
Shift               ::= Additive { ("<<" | ">>") Additive }                            // Implemented
Additive            ::= Multiplicative { ("+" | "-") Multiplicative }                  // Implemented
Multiplicative      ::= Unary { ("*" | "/" | "%") Unary }                              // Implemented
Unary               ::= ("+" | "-" | "!" | "++" | "--") Unary | Postfix                // Implemented
Postfix             ::= Primary { PostfixPart }                                        // Implemented
PostfixPart         ::= "++" | "--" | MemberAccess | Invocation                        // Implemented
MemberAccess        ::= "." Identifier                                                 // Implemented
Invocation          ::= "(" [ ArgumentList ] ")"                                       // Implemented
ArgumentList        ::= Expression { "," Expression }                                  // Implemented
Primary             ::= Literal | Identifier | "(" Expression ")" | ConsoleCall        // Implemented
ConsoleCall         ::= "Console" "." ("WriteLine" | "Write") Invocation               // Implemented
```

### 2.6 Operator precedence and associativity

(The table is advisory; parser uses productions above.)

| Prec | Category       | Operators          | Assoc  |        |        |
| ---: | -------------- | ------------------ | ------ | ------ | ------ |
|   15 | Postfix        | `x++ x-- f() a.b`  | L‑to‑R |        |        |
|   14 | Unary          | `++x --x +x -x !x` | R‑to‑L |        |        |
|   13 | Multiplicative | `* / %`            | L‑to‑R |        |        |
|   12 | Additive       | `+ -`              | L‑to‑R |        |        |
|   11 | Shift          | `<< >>`            | L‑to‑R |        |        |
|   10 | Relational     | `< <= > >=`        | L‑to‑R |        |        |
|    9 | Equality       | `== !=`            | L‑to‑R |        |        |
|    8 | Bitwise AND    | `&`                | L‑to‑R |        |        |
|    7 | Bitwise XOR    | `^`                | L‑to‑R |        |        |
|    6 | Bitwise OR     | \`                 | \`     | L‑to‑R |        |
|    5 | Logical AND    | `&&`               | L‑to‑R |        |        |
|    4 | Logical OR     | \`                 |        | \`     | L‑to‑R |
|    3 | Conditional    | `?:`               | R‑to‑L |        |        |
|    2 | Assignment     | `= += -= *= /= %=` | R‑to‑L |        |        |
|    1 | Sequence       | `,`                | L‑to‑R |        |        |

---

## 3 Semantic differences from C# (current version)

1. **Top‑level functions and variables** are permitted; no namespaces yet; any `using` tokens are ignored other than for parsing (Appendix B).
2. The `func` keyword replaces C#’s return‑type‑first syntax; optional type yields typed return; omitted implies `void` (no value).
3. Core primitive types implemented: `int`, `bool`, `float`, `char`, `string`.
4. All data is passed **by value**; no references/pointers yet (unsafe grammar reserved).
5. `Console.Write`/`WriteLine` are built‑ins; they do not require namespace qualification.
6. Target C code‑gen uses 32‑bit signed integers and zero‑terminated UTF‑8 strings.
7. Control‑flow semantics follow C# where implemented; `switch` currently supports only constant integral cases without pattern guards.

---

## 4 Future work (non‑normative)

* Generics.
* Access modifiers (`public`, `private`, …).
* Exception handling (`try`‑`catch`).
* Attributes and annotations.
* Modular compilation & namespaces.
* Lambdas / delegates / events.
* Pattern matching & richer `switch`.
* Unsafe code (pointers, fixed buffers).
* Many additional C# constructs as listed in Appendices.

---

## 5 Change log

| Version | Date       | Notes                                                                                            |
| ------- | ---------- | ------------------------------------------------------------------------------------------------ |
| 0.3     | 2025‑07‑16 | Integrated lexical, syntactic & unsafe appendices; keyword & operator union; status annotations. |
| 0.2     | 2025‑07‑15 | Initial public draft extracted from compiler source.                                             |

---

## Appendix A – Lexical reference summary

Additional lexical productions available in the upstream grammar but not (yet) implemented in Dream: extended Unicode identifier classes; numeric literals with digit separators & type suffixes; real literals (with exponent & suffix); char & string escape sequences incl verbatim strings; interpolated strings; `null` literal; full keyword & contextual keyword sets; full operator/punctuator set; preprocessor directives. See **Lexical grammar.md** in the repo for full details.

---

## Appendix B – Syntactic reference summary

Highlights of major constructs present in the upstream C# syntactic grammar but not yet implemented in Dream: namespaces & `using` directives; rich type system (reference/value/nullable/array/tuple/delegate); generics (type parameters, constraints); patterns; named & `ref`/`in`/`out` arguments; interpolated strings; null‑conditional operators; object / collection / anonymous / array / delegate creation; `typeof`, `sizeof`, `checked`/`unchecked`, `default` literal; lambda & anonymous methods; query expressions; full statement suite (`try`, `lock`, `using`, `yield`, `foreach`, `goto`, etc.); class/struct/interface/enum/delegate member forms. See **Syntactic grammar.md** for the authoritative productions.

---

## Appendix C – Unsafe extensions summary

The unsafe grammar introduces pointer types (`T*`, `void*`), pointer indirection (`*x`), pointer member access (`p->f`), pointer element access (`p[i]`), address‑of (`&x`), the `fixed` statement, and fixed‑size buffer declarations (`fixed T name[N];`). Dream currently treats `unsafe` as reserved and emits a diagnostic if used. See **Grammar extensions for unsafe code.md** for details.