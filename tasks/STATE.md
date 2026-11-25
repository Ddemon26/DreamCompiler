# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0090 - release the final join from three returned appends in an append initializer**.
- Record: [P0090](passes/P0090-initialize-from-three-returned-appends.md).
- Parent/fetched approved remote: `efc760d5a03637bcafc75f7139b972c7adb06acf` (P0089).
- Resolve this pass commit by its exact `Pass-ID: P0090` trailer.
- Delivery requires push and matching `git ls-remote`.

P0089 CI passed on Windows and Ubuntu. P0090 releases the original final join
from three proven returned appends in `string snapshot = (text += deeper());`.
The checked ordinary and module-entry calls evaluate the operand once, use the
current destination after evaluation, and give the snapshot an independent copy.
See [measured tests](MostRecentTestResults.md) and the
[P0089 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45630310875).
Eighteen unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0090 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Measure retention in `Console.Write(text += deeper());` and
   `Console.WriteLine(text += deeper());` with three proven returned appends.
   Check operand release, once-only argument evaluation, and output from the
   current destination when an argument replaces it.
3. General expression and argument ownership, async, and exception unwinding remain open.
