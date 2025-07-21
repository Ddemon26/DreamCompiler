# Dream Language Reference Materials

This section contains authoritative specifications, version documentation, and complete language reference materials for the Dream programming language.

## Contents

### Core Reference

#### [Language Reference](language-reference.md)  
Complete language specification and programmer's guide:
- **Language Overview** - Philosophy, design principles, and features
- **Learning Paths** - Structured approach to mastering Dream
- **Comprehensive Examples** - Real-world code demonstrating all features
- **Best Practices** - Guidelines for writing effective Dream code

#### [Grammar Specification](grammar/)
Formal language specification using BNF notation:

##### [Main Grammar](grammar/Grammar.md)
Complete formal grammar specification:
- **Program Structure** - Modules, imports, declarations
- **Expressions** - All expression types with precedence rules
- **Statements** - Control flow, declarations, and compound statements
- **Types** - Type system specification and rules

##### [Lexical Grammar](grammar/Lexical-grammar.md)
Token-level language specification:
- **Keywords** - Reserved words and their meanings
- **Operators** - All operator symbols and precedence
- **Literals** - String, numeric, character, and boolean literals
- **Identifiers** - Valid identifier patterns and rules

##### [Syntactic Grammar](grammar/Syntactic-grammar.md)
Context-free grammar for Dream syntax:
- **Production Rules** - Complete BNF specification
- **Precedence Rules** - Operator precedence and associativity
- **Grammar Extensions** - Future language extensions

##### [Unsafe Code Extensions](grammar/Grammar-extensions-for-unsafe-code.md)
Specification for low-level programming features:
- **Pointer Operations** - Direct memory access
- **Inline Assembly** - Assembly code integration  
- **C Interop** - Foreign function interface
- **Memory Management** - Manual memory control

### Version History

#### [Changelog](changelog.md)
Comprehensive version history with detailed release notes:
- **Version 1.1.12 (Current)** - Latest features and improvements
- **Performance Metrics** - Compilation speed and runtime improvements
- **Platform Support** - Compatibility matrix across versions
- **Migration Guides** - Upgrading between versions
- **Breaking Changes** - API changes and deprecations

### Historical Documentation

#### [Version 1.1 Documentation](v1.1/)
Complete documentation for Dream Language v1.1:

**Language Features:**
- [Architecture](v1.1/architecture.md) - v1.1 compiler architecture
- [Arithmetic Operations](v1.1/arithmetic.md) - Mathematical expressions
- [Arrays](v1.1/arrays.md) - Array declaration and manipulation
- [Bitwise Operations](v1.1/bitwise.md) - Binary operations and bit manipulation
- [Comments](v1.1/comments.md) - Code documentation syntax
- [Comparison Operators](v1.1/comparison.md) - Relational and equality operators
- [Conditional Expressions](v1.1/conditional.md) - Ternary operator and conditionals
- [Console Operations](v1.1/console.md) - Input/output operations
- [Control Flow](v1.1/if.md) - If/else conditional statements
- [Logical Operations](v1.1/logical.md) - Boolean logic and short-circuiting
- [Loops](v1.1/loops.md) - For, while, and do-while iterations
- [Return Statements](v1.1/return.md) - Function returns and values
- [Strings](v1.1/strings.md) - String literals and operations
- [Switch Statements](v1.1/switch.md) - Multi-way branching
- [Variables](v1.1/variables.md) - Variable declaration and initialization
- [Vulkan Integration](v1.1/vulkan.md) - Graphics programming support

**Code Examples:**
- [Classes](v1.1/examples/classes.md) - Object-oriented programming examples
- [Functions](v1.1/examples/functions.md) - Function definition examples

#### [Version 1.0 Documentation](v1.0/)
Historical documentation for Dream Language v1.0:

**Core Features:**
- [Architecture](v1.0/architecture.md) - Original compiler design
- [Arithmetic](v1.0/arithmetic.md) - Basic mathematical operations
- [Arrays](v1.0/arrays.md) - Early array implementation
- [Comments](v1.0/comments.md) - Documentation syntax
- [Comparison](v1.0/comparison.md) - Comparison operators
- [Conditionals](v1.0/conditional.md) - Early conditional syntax  
- [Console I/O](v1.0/console.md) - Basic input/output
- [Control Flow](v1.0/if.md) - If/else statements
- [Logical Operations](v1.0/logical.md) - Boolean logic
- [Loops](v1.0/loops.md) - Iteration constructs
- [Parentheses](v1.0/parentheses.md) - Expression grouping
- [Returns](v1.0/return.md) - Function returns
- [Strings](v1.0/strings.md) - String handling
- [Switch](v1.0/switch.md) - Switch statements
- [Variables](v1.0/variables.md) - Variable basics

**Legacy Examples:**
- [Classes](v1.0/examples/classes.md) - Early OOP implementation
- [Functions](v1.0/examples/functions.md) - Function examples

**Version History:**
- [v1.0 Changelog](v1.0/changelog.md) - Early version history

#### [Language Integration](language/)
Language server and IDE integration documentation:
- [Language Server](language/index.md) - IDE integration and IntelliSense

## Reference Usage Guide

### For Language Users
- **Start with [Language Reference](language-reference.md)** - Complete language guide
- **Consult [Grammar](grammar/Grammar.md)** - For syntax questions
- **Check [Changelog](changelog.md)** - For version-specific information

### For Tool Developers  
- **Use [Lexical Grammar](grammar/Lexical-grammar.md)** - For tokenizer implementation
- **Reference [Syntactic Grammar](grammar/Syntactic-grammar.md)** - For parser development
- **Follow [Grammar Extensions](grammar/Grammar-extensions-for-unsafe-code.md)** - For advanced features

### For Compiler Contributors
- **Study [Architecture docs](v1.1/architecture.md)** - Understand compiler design evolution
- **Review [Changelog](changelog.md)** - Understand design decisions and trade-offs
- **Compare versions** - See how features evolved from v1.0 to v1.1

## Grammar Notation

The grammar specifications use standard BNF (Backus-Naur Form) notation:

```bnf
# Production rules
<program> ::= <module_declaration>? <import_statement>* <declaration>*

# Terminal symbols (tokens)
<identifier> ::= [a-zA-Z_][a-zA-Z0-9_]*
<integer_literal> ::= [0-9]+

# Non-terminal symbols (grammar rules)  
<expression> ::= <term> (("+" | "-") <term>)*
<term> ::= <factor> (("*" | "/" | "%") <factor>)*

# Optional elements
<function_declaration> ::= "func" <identifier> "(" <parameter_list>? ")" <return_type>? <block>

# Repetition  
<statement_list> ::= <statement>*     # Zero or more
<parameter_list> ::= <parameter> ("," <parameter>)*  # One or more with separator
```

## Version Compatibility

### Language Compatibility
- **v1.1.x** - Fully backward compatible with v1.0
- **v1.0.x** - Original language specification
- **Future versions** - Breaking changes will be clearly documented

### Compiler Compatibility
- **Cross-version** - v1.1 compiler can build v1.0 code
- **Generated code** - C output is forward compatible
- **Runtime library** - Automatic compatibility management

## Standards Compliance

### Language Design Standards
- **Static typing** - All types resolved at compile time
- **Memory safety** - Automatic memory management with deterministic cleanup
- **Cross-platform** - Consistent behavior across operating systems
- **C interoperability** - Seamless integration with C libraries

### Documentation Standards
- **Completeness** - All language features fully documented
- **Accuracy** - Specifications match implementation exactly  
- **Examples** - Every feature demonstrated with working code
- **Maintenance** - Documentation kept in sync with language changes

## What's Next?

### For New Users
1. **[Language Reference](language-reference.md)** - Start with complete language guide
2. **[Getting Started](../getting-started/)** - Set up development environment
3. **[Fundamentals](../fundamentals/)** - Learn core concepts
4. **[Language Features](../language-features/)** - Master language constructs

### For Advanced Users
1. **[Grammar Specification](grammar/Grammar.md)** - Understand formal specification
2. **[Advanced Topics](../advanced-topics/)** - Memory management and C interop
3. **[Development](../development/)** - Contribute to the language
4. **[Changelog](changelog.md)** - Stay updated with new versions

The Dream language reference materials provide the authoritative specification for all aspects of the Dream programming language, ensuring consistency and enabling tool development and language evolution.