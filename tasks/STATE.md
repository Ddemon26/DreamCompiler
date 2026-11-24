# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0084 - adopt the final join from three returned appends in a direct scalar initializer**.
- Record: [P0084](passes/P0084-adopt-three-returned-appends.md).
- Parent/fetched approved remote: `196f12c48a8e76621ad733909b9802ff97433b85` (P0083).
- Resolve this pass commit by its exact `Pass-ID: P0084` trailer.
- Delivery requires push and matching `git ls-remote`.

P0083 CI passed on Windows and Ubuntu. In P0084 a direct scalar initializer
adopts the original join from a factory with three proven returned appends.
Eight calls in each allocation probe leave zero joins live.
See [measured tests](MostRecentTestResults.md) and the
[P0083 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45309986340).
Eighteen unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0084 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Measure retention in a complete assignment to an owned scalar string when its
   factory returns three proven appends. If the assignment qualifies for adoption,
   check original-pointer identity and destination replacement during argument evaluation.
3. General expression and argument ownership, async, and exception unwinding remain open.
