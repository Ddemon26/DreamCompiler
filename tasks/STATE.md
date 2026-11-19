# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0062 - restore CI Python setup on Ubuntu 26.04**.
- Record: [P0062](passes/P0062-ci-python-setup.md).
- Parent/fetched approved remote: `9e257f5e40c3edbf12f7fb93bf24700bb6aeb675` (P0061).
- Resolve this pass commit by its exact `Pass-ID: P0062` trailer.
- Delivery requires push and matching `git ls-remote`.

P0061 [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44376422082)
failed during Python 3.8 setup on Ubuntu 26.04. P0062 selects Python 3.13,
available for both CI platforms, and updates checkout and Python setup to the
v6 actions. See [measured tests](MostRecentTestResults.md). Eighteen
unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0062 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Isolate `return (text += owned());` from a synchronous string function with an
   owned local in both modes. Measure the factory operand and verify the caller
   can read the joined result before choosing a release rule.
3. General expression and argument ownership, async, and exception unwinding remain open.
