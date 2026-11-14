# Compiler architecture

DreamCompiler is experimental. The working pipeline is:

```text
Dream entry source
  -> source newline normalization (Dream helper, built with the C seed)
  -> re2c lexer
  -> parser and AST
  -> module expansion when --multi-file is enabled
  -> semantic analysis
  -> bounded literal arithmetic folding when optimization is requested
  -> AST-to-C emission
  -> host C compiler and Dream runtime
```

The lexer, parser, semantic analyzer and C emitter remain C11. The
[source map](../source-map.md) identifies their implementation files. Generated
C preserves source mappings with `#line` directives.

The driver uses the common AST emitter. General CFG/SSA emission is disabled;
optimizer source files alone do not establish working transformations.
[Literal optimization](../optimization-emission.md) documents the qualified
literal-only SCCP bridge and unchanged AST fallback for unsupported expressions.
There is no qualified general loop optimization or mutable-variable propagation.

[Modules](../../advanced-topics/modules.md) support sibling imports and exported
functions. Imported classes, structs and globals remain unsupported. The older
backend abstraction in `src/codegen/backend.c` and `module.c` is experimental;
it is not the working multi-file driver.

[Storage ownership](../string-ownership.md) describes the qualified string paths
and remaining lifetime limitations. Do not infer complete automatic cleanup,
cycle collection or alias safety from the runtime's reference-counting API.

## Read next

- [Build commands](build.md) and [testing](testing.md).
- [Compiler dogfooding and bootstrap stages](../compiler-dogfooding.md).
- [Feature boundaries](../../../codex/FEATURES.md).
- [Agent workflow](../agent-workflow.md) and [current checkpoint](../../../tasks/STATE.md).
- [Editor integration](intellisense.md), maintained separately under `assets/`.
