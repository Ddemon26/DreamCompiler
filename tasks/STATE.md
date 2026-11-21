# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0074 - adopt the final join in a scalar initializer after two returned appends**.
- Record: [P0074](passes/P0074-adopt-nested-returned-joins.md).
- Parent/fetched approved remote: `6f5bd7427fe78e84513321820dd35e8f336aaad3` (P0073).
- Resolve this pass commit by its exact `Pass-ID: P0074` trailer.
- Delivery requires push and matching `git ls-remote`.

P0073 CI passed on Windows and Ubuntu. P0074 adopts the final join in a direct
scalar initializer when the call meets the proof for two returned appends.
Normal scope exit releases the original final pointer.
See [measured tests](MostRecentTestResults.md) and the
[P0073 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44907693639).
Eighteen unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0074 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Measure final-result retention in `target = nested();` when `target` is an
   owned scalar and `nested()` qualifies for the two-append initializer proof.
   Check original-pointer identity and replacement ordering if assignment can adopt it.
3. General expression and argument ownership, async, and exception unwinding remain open.
