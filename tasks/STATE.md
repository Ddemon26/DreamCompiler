# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0087 - release the final join from three returned appends after direct Console output**.
- Record: [P0087](passes/P0087-print-three-returned-appends.md).
- Parent/fetched approved remote: `428fe702006a8e90f9e7d6c2afe0d9a009728650` (P0086).
- Resolve this pass commit by its exact `Pass-ID: P0087` trailer.
- Delivery requires push and matching `git ls-remote`.

P0086 CI passed on Windows and Ubuntu. P0087 releases the original final join
from three proven returned appends after a complete `Console.Write` or
`Console.WriteLine` statement. The checked ordinary and imported calls evaluate
the argument once, and each writer reads the original pointer before release.
See [measured tests](MostRecentTestResults.md) and the
[P0086 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45477398152).
Eighteen unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0087 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Measure retention when a complete call to a proven Console wrapper receives
   the result from three returned appends. Check that the original pointer stays
   live through the wrapper and is released after it returns.
3. General expression and argument ownership, async, and exception unwinding remain open.
