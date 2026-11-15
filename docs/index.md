# DreamCompiler documentation

DreamCompiler is an experimental compiler for a C#-style language. It translates
a tested subset of Dream to C, then links native programs with the Dream runtime.
Use the [quick-start guide](getting-started/quick-start.md) for your first program.
Use Zig **0.17.1** and re2c to build it. The lexer, parser, semantics and emitter
are C11; entry-file newline normalization is the first production Dream helper.

## Build and try an existing regression

From the repository root:

```sh
zig build
zig build run -- tests/basics/io/console.dr
./dream
```

On Windows run `.\dream.exe` for the last step. Compilation produces
`build/bin/dream.c` and `dream`/`dream.exe`; it does not produce a program named
after the source. See the [README](../README.md) for prerequisites.

## Current implementation and evidence

- [Feature inventory](../codex/FEATURES.md): supported behavior and known gaps.
- [Current checkpoint](../tasks/STATE.md) and [backlog](../tasks/TODO.md).
- [Dated test results](../tasks/MostRecentTestResults.md): checked platforms and skips.
- [Build commands](development/compiler/build.md) and [testing guide](../codex/TEST_GUIDE.md).
- [Compiler architecture](development/compiler/index.md) and [source map](development/source-map.md).
- [Dogfooding checkpoint](development/compiler-dogfooding.md): one helper, not full self-hosting.

Function modules work, but shared imported types/globals are unsupported. General
SSA/CFG emission, complete ownership, concurrency and graphics remain unqualified
or experimental. Platform percentages and release promises in older material are
not current measurements.

## Language and development guides

- [Introduction](getting-started/intro.md) and [language syntax](getting-started/usage.md).
- [Types](fundamentals/type-system.md), [variables](fundamentals/variables.md),
  [functions](language-features/functions.md) and [operators](language-features/operators.md).
- [Modules](advanced-topics/modules.md) and [memory boundaries](advanced-topics/memory.md).
- [Developer guide](development/developer-guide.md) and [agent workflow](development/agent-workflow.md).
- [Versioned references](reference/README.md) and [grammar](reference/grammar/Grammar.md).

Versioned examples and extended grammar include historical or planned syntax;
check them against the feature inventory and regressions before relying on them.
The website presents these Markdown guides through a searchable topic navigation
and article reader. Additional tutorial/API pages still need an accuracy audit.
See the [website maintenance guide](development/docs-site.md) for routes and design rules.
