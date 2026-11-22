# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0075 - adopt the final join in a complete scalar assignment after two returned appends**.
- Record: [P0075](passes/P0075-assign-nested-returned-joins.md).
- Parent/fetched approved remote: `5e4d598570a64e4a36ee23cb010a62bf688d7740` (P0074).
- Resolve this pass commit by its exact `Pass-ID: P0075` trailer.
- Delivery requires push and matching `git ls-remote`.

P0074 CI passed on Windows and Ubuntu. P0075 adopts the final join in a complete
assignment to an already-owned scalar when the call meets the proof for two
returned appends. After evaluating the call, the assignment releases the
current destination and installs the original final pointer.
See [measured tests](MostRecentTestResults.md) and the
[P0074 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44939209746).
Eighteen unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0075 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Measure final-result retention in a standalone `nested();` call when
   `nested()` qualifies for the two-append proof. Check original-pointer release
   and once-only argument evaluation if the discard path qualifies.
3. General expression and argument ownership, async, and exception unwinding remain open.
