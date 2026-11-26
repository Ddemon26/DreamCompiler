# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0091 - provision re2c in a required Windows CI step**.
- Record: [P0091](passes/P0091-windows-ci-re2c.md).
- Parent/fetched approved remote: `b75051c2acc2f8235176b777f16246eb2e71874a` (P0090).
- Resolve this pass commit by its exact `Pass-ID: P0091` trailer.
- Delivery requires push and matching `git ls-remote`.

P0090 passed Ubuntu CI. Its Windows job could not run re2c after Chocolatey's
package feed returned HTTP 503. P0091 builds re2c 3.0 from the upstream archive,
checks its checksum, and exposes it to subsequent Windows CI steps only after
the executable runs. See the [P0090 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45691819027)
and [measured tests](MostRecentTestResults.md). Eighteen unannotated Dream entries
remain skipped.

Next invocation:

1. Verify P0091 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Measure retention in `Console.Write(text += deeper());` and
   `Console.WriteLine(text += deeper());` with three proven returned appends.
   Check operand release, once-only argument evaluation, and output from the
   current destination when an argument replaces it.
3. General expression and argument ownership, async, and exception unwinding remain open.
