# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0064 - adopt proven returned joins in scalar string initializers**.
- Record: [P0064](passes/P0064-adopt-returned-joins.md).
- Parent/fetched approved remote: `25bda9d61e635a6f385f929ae186a19ff7e5889b` (P0063).
- Resolve this pass commit by its exact `Pass-ID: P0064` trailer.
- Delivery requires push and matching `git ls-remote`.

P0063 [CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/44477695219)
passed on Windows and Linux. P0064 lets a scalar declaration adopt the join
from a proven two-statement append factory. Scope cleanup releases the original
allocation. See [measured tests](MostRecentTestResults.md).
Eighteen unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0064 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Measure join allocation identity and release order for `target = joined();`
   as a complete assignment to an owned scalar in ordinary and module-entry modes.
3. General expression and argument ownership, async, and exception unwinding remain open.
