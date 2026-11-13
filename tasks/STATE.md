# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0015 - restore Linux indexed-string harness builds**.
- Record: [P0015](passes/P0015-linux-string-harness.md).
- Parent/fetched approved remote: `d156d5cab31304eb2cec695d83ab0f4083acac35` (P0014).
- Resolve the pass commit by its exact `Pass-ID: P0015` trailer.
- Publication requires push and matching `git ls-remote`; report that identity
  after delivery. Inspect native CI before selecting the next task.

The 2026-11-13 pass started clean in the approved primary checkout on the attached
branch/origin, one registered worktree, no unpublished pass or competing active
project writer in app inventory. Approved fetch matched HEAD after restricted DNS
failure. Automation already uses local execution and the approved cwd; unchanged.

P0014 [CI run 43326685373](https://github.com/Ddemon26/DreamCompiler/actions/runs/43326685373)
failed Linux harness compilation because `exception.c` could not see `strdup` in
strict C11; Windows was cancelled. P0015 fixes that priority baseline failure by
adding the normal runtime build's `_GNU_SOURCE` flag to the Go ownership harness.
Both ordinary/module-entry modes also cross-link for Linux on Windows, catching
the original CI failure locally. Existing generated-C, checked allocator lifetime,
sanitizer and exact output assertions remain intact. Compiler/runtime behavior,
string ownership and indexed formatting are unchanged.

Before the flag fix, both new Linux cross-link checks reproduced the CI failure.
After it, both focused modes passed in 2.846s. Final full suite ran alone:
**144 discovered, 126 passed, 18 skipped, zero failures/errors**, 34.21s.
All **twelve Go quick groups passed**, 17.419s. Python: **five passed**, 0.045s.
Linux compiler cross-build passed. Native P0015 runtime awaits CI; cross-linked
Linux harnesses were not executed locally. See [MostRecentTestResults.md](MostRecentTestResults.md).
The provisional initial baseline briefly overlapped focused reproduction; it was
replaced by the final isolated suite above. No assertions or exclusions changed.

The same 18 skips and previous limitations remain. Indexed scalar string copies
are still the P0014 ownership boundary; other initializers, reassignment, returns,
early exits, fields and general lifetime/shape safety remain T011. Harness
const-discard warnings remain. Windows: Zig 0.17.1, re2c 3.0, Python 3.13.17,
Go 1.27.4. SDK/cache access used normal approvals without content/permission changes.

Next: verify P0015 delivery and inspect native CI; repair failures first. Otherwise
take **T010**, indexed primitive console formatting. Do not start a second pass here.
