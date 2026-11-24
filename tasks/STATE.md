# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0086 - discard the final join from three returned appends**.
- Record: [P0086](passes/P0086-discard-three-returned-appends.md).
- Parent/fetched approved remote: `df949be00f946d46f6369d741a1e832111f2c64c` (P0085).
- Resolve this pass commit by its exact `Pass-ID: P0086` trailer.
- Delivery requires push and matching `git ls-remote`.

P0085 CI passed on Windows and Ubuntu. P0086 releases the original final join
after a standalone call to a factory with three proven returned appends. The
checked ordinary and imported calls evaluate the argument once and release
the original final pointer before returning to the probe.
See [measured tests](MostRecentTestResults.md) and the
[P0085 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/45411129575).
Eighteen unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0086 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Measure retention when complete `Console.Write` and `Console.WriteLine`
   statements print a factory result from three proven returned appends.
   Check that both writers read the original pointer before any release.
3. General expression and argument ownership, async, and exception unwinding remain open.
