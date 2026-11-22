# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0076 - discard the final join from two returned appends**.
- Record: [P0076](passes/P0076-discard-nested-returned-joins.md).
- Parent/fetched approved remote: `33603311d37daf7d5c24e85b4e4ef1a20e55673b` (P0075).
- Resolve this pass commit by its exact `Pass-ID: P0076` trailer.
- Delivery requires push and matching `git ls-remote`.

P0075 CI passed on Windows and Ubuntu. P0076 releases the original final join
after a standalone call that meets the proof for two returned appends. The
caller evaluates the call and its arguments once.
See [measured tests](MostRecentTestResults.md) and the
[P0075 CI run](https://github.com/Ddemon26/DreamCompiler/actions/runs/44969995144).
Eighteen unannotated Dream entries remain skipped.

Next invocation:

1. Verify P0076 publication and both exact-commit CI jobs. Repair any failed baseline.
2. Qualify release of the final join after direct Console output and cover
   both writers. The `returned_join_append_return` audit still retains eight
   final joins after eight `Console.WriteLine(nested());` calls per mode.
3. General expression and argument ownership, async, and exception unwinding remain open.
