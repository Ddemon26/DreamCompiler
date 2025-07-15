# Dream Compiler Architecture

This document describes the layout of the `src/` directory and ways to avoid monolithic "god" files.

## Layout

The compiler source is split into focused directories:

```
src/
  lexer/
    lexer.c
    lexer.h
  parser/
    expression.c
    statement.c
    parser.h
  ast/
    node.c
    node.h
  codegen/
    codegen.c
    codegen.h
  driver/
    main.c
```

Dividing the code this way keeps each component focused and reduces the risk of oversized modules.

