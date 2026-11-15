# DreamCompiler developer guide

DreamCompiler is a C11 implementation with one production helper written in Dream.
Zig **0.17.1** builds the tools and links emitted C; re2c generates the lexer.
The [source map](source-map.md) identifies working components, including the
experimental backends that are not used by the driver.

## Start from the checkpoint

Read [AGENTS.md](../../AGENTS.md), [STATE](../../tasks/STATE.md),
[TODO](../../tasks/TODO.md), the latest [pass record](../../tasks/passes/README.md),
and [test results](../../tasks/MostRecentTestResults.md). The
[workflow](agent-workflow.md) defines branch synchronization, one-writer checks,
validation and publication. Agent passes use the approved existing checkout and
`codex/compiler-stabilization` branch; do not create isolated worktrees for them.

## Build and inspect

```sh
zig version
zig build
zig build run -- tests/basics/io/console.dr
```

Run `dream` (`dream.exe` on Windows) to execute the generated program. The
[build guide](compiler/build.md) documents supported steps and cross-builds.
The [bootstrap contract](compiler-dogfooding.md) explains how the C seed builds
the Dream helper linked into the production compiler.

## Make one verified change

Reproduce the issue through the actual parser, semantic analyzer, emitter and
runtime paths. Add a meaningful regression and preserve existing assertions.
Edit `src/lexer/lexer.re` or `tokens.def` for lexer changes, then let the build
regenerate C. Keep unsupported syntax and deferred experiments explicit.

Use the [testing guide](../../codex/TEST_GUIDE.md) for focused checks and the full
required protocols. Compiler tests share outputs, so run them sequentially. Keep
Linux and Windows working; a cross-build alone does not prove native behavior.
Inspect generated C where runtime output could hide an emission defect.

The [feature inventory](../../codex/FEATURES.md),
[optimization contract](optimization-emission.md) and
[string ownership contract](string-ownership.md) describe qualified boundaries.
Do not claim complete SSA optimization, C interop, concurrency or graphics from
planned examples or source files alone.

Update the feature/user docs when behavior changes. Record exact evidence,
remaining skips and a concrete handoff in the next pass. Inspect the diff and
run `git diff --check`, then commit and verify the approved remote SHA as required
by the workflow. Preserve historical pass records rather than rewriting evidence.
