# DreamCompiler development

Start with the [current checkpoint](../../tasks/STATE.md),
[ordered backlog](../../tasks/TODO.md) and [dated test evidence](../../tasks/MostRecentTestResults.md).
The compiler is experimental; older examples and planned APIs are not evidence of
implemented behavior.

## Working references

- [Developer guide](developer-guide.md): setup, source navigation and contribution checks.
- [Source map](source-map.md): implementation and verification entry points.
- [Compiler architecture](compiler/index.md): the working AST-to-C pipeline.
- [Build commands](compiler/build.md): Zig 0.17.1, bootstrap stages and cross-builds.
- [Testing guide](../../codex/TEST_GUIDE.md): full, focused and Go quick checks.
- [Dogfooding](compiler-dogfooding.md): the first production Dream helper and limits.
- [Optimization boundary](optimization-emission.md) and [string ownership](string-ownership.md).
- [Agent workflow](agent-workflow.md): one bounded pass, durable evidence and verified publication.

Editor plugins live under `assets/`; keep their work separate from compiler work
unless a selected task requires both. Consult their local instructions before changes.
