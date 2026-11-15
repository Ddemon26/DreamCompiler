# Implementation status

Dream is an experimental compiler for a C#-style language. It translates a tested
subset of Dream to C and builds native programs with the Dream runtime and Zig.
The website's examples introduce that path; they are not a complete language contract.

## Check the current evidence

- [Feature inventory](../../codex/FEATURES.md): implemented behavior and known gaps.
- [Current checkpoint](../../tasks/STATE.md): the latest project handoff.
- [Dated test results](../../tasks/MostRecentTestResults.md): checked platforms,
  failures, and skipped cases.
- [Ordered backlog](../../tasks/TODO.md): work still to be qualified.

A passing example does not establish all parameter shapes, lifetime rules, or
control-flow paths for a feature. Use the regressions and their pass records when
you need a more precise boundary.

## The compilation path

The production compiler uses the lexer, parser, semantic analysis, and AST-to-C
emitter. The optimization bridge is limited to qualified literal expressions;
general optimized CFG/SSA emission is not implemented. See the
[architecture](compiler/index.md) and [optimization contract](optimization-emission.md).

Entry-file newline normalization is the first production Dream helper. This is
[a dogfooding checkpoint](compiler-dogfooding.md), not a self-hosted frontend.

## Important boundaries

- Function exports work across source files; shared imported types and globals
  remain unsupported. See [modules](../advanced-topics/modules.md).
- String storage has several qualified ownership paths, with other cases still
  open. See the [ownership contract](string-ownership.md).
- Broader pointer and array shape checks, return ownership, and early-exit
  unwinding remain separate qualification work.
- Concurrency, graphics, and planned APIs in older examples remain experimental
  or unqualified.

## Historical material

The versioned references and extended grammar contain historical or planned
syntax. They are retained as reference material, not advertised as current stable
releases. Other tutorial and API pages also need further accuracy review.

Use the feature inventory and current tests to establish behavior. The homepage
does not publish platform pass percentages or performance promises.
