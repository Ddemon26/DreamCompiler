# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0061 - release proven factory operands in appends passed to Console wrappers**.
- Record: [P0061](passes/P0061-owned-append-wrapper.md).
- Parent/fetched approved remote: `69059dcfe4cd20e6ce2ac2dea24a0a568a6ab621` (P0060).
- Resolve this pass commit by its exact `Pass-ID: P0061` trailer.
- Delivery requires push and matching `git ls-remote`.

The [Console wrapper append rule](../docs/development/string-ownership.md#append-arguments-to-console-wrappers-p0061)
releases a proven factory operand after installing the joined allocation and
before passing the join to a synchronous, one-parameter wrapper that only
prints it. The destination keeps the join for later reads and updates. The
isolated eight-iteration audit reaches 24 nonnull releases and zero survivors
per mode.

P0060 passed exact-commit native Linux and Windows [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44323725290).
See [measured tests](MostRecentTestResults.md). Eighteen unannotated Dream
entries remain skipped.

Next invocation:

1. Verify P0061 publication and inspect exact-commit CI. Repair any failed baseline.
2. Isolate `return (text += owned());` from a synchronous string function with an
   owned local in both modes. Measure the factory operand and verify the caller
   can read the joined result before choosing a release rule.
3. General expression and argument ownership, async, and exception unwinding remain open.
