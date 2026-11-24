# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0085 - adopt the final join from three returned appends in a complete scalar assignment**.
- Record: [P0085](passes/P0085-assign-three-returned-appends.md).
- Parent/fetched approved remote: `060f10f37636c739066e51859ed210863f07edc9` (P0084).
- Resolve this pass commit by its exact `Pass-ID: P0085` trailer.
- Delivery requires push and matching `git ls-remote`.

P0084 CI passed on Windows and Ubuntu. P0085 extends adoption of the original
join from three proven returned appends to complete assignments to owned scalar
strings. The assignment releases the destination after argument evaluation,
including a replacement installed while evaluating an argument. Eight calls
in each allocation probe leave zero final joins live.
See [measured tests](MostRecentTestResults.md) and the
[P0084 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45353940033).
Eighteen unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0085 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Measure retention after a standalone call to a factory with three proven returned
   appends. If the discard qualifies for release, check original-pointer identity
   and once-only argument evaluation in ordinary and imported calls.
3. General expression and argument ownership, async, and exception unwinding remain open.
