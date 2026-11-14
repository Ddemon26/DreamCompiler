# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0021 - independent storage for call-initialized scalar strings**.
- Record: [P0021](passes/P0021-call-string-ownership.md).
- Parent/fetched approved remote: `daeb57fb8bffe7bd7b3729a8fdc330a60cac9620` (P0020).
- Resolve the pass commit by its exact `Pass-ID: P0021` trailer.
- Publication requires push and matching `git ls-remote`; report that identity
  after delivery. Inspect native CI before selecting the next task.

The 2026-11-14 UTC pass started clean in the approved checkout, attached branch
and origin, one worktree, no unpublished pass or competing active project writer.
Approved fetch matched HEAD after restricted DNS failure. The automation already
uses local execution and approved cwd. P0020
[CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43539278761) passed
native Linux/Windows stability checks and full suite; no baseline repair was needed.

P0021 completes one T011 slice. Direct scalar string call initializers evaluate
once and copy their readable result into independent managed storage. Literal,
borrowed, fresh managed and null results follow the same rule through free/static/
instance/base calls, nested/const/grouped declarations, loop bodies, for headers
and top-level snapshots. Cleanup releases the copy and preserves source ownership.
Fresh managed results and returned locals can remain registered until shutdown;
this pass does not define a return-transfer ABI or change return cleanup.

The ordinary/module-entry regression checks fifteen copy sites, fourteen release
sites, fourteen nonnull checked releases and nineteen exact output lines per mode.
Both modes reproduced invalid live-allocation cleanup before the fix. Borrowed
backing survives nested cleanup; a snapshot survives source replacement. A marker
proves once-only calls. Existing allocator assertions and ownership groups remain.

Baseline: 131 passed/18 skipped/no failures. Final full suite:
**150 discovered, 132 passed, 18 skipped, zero failures/errors**, 33.46s. All
**eighteen Go quick groups passed**, 27.077s; Python **five passed**, 0.046s.
All twelve focused ownership modes passed, 12.745s. Linux compiler cross-build
and both new harness cross-links passed; Linux binaries were not run locally.
Native P0021 runtime awaits CI. See [MostRecentTestResults.md](MostRecentTestResults.md).

Same 18 skips/exclusions remain. Reassignment, return transfer, early return/
exception cleanup, owned body locals skipped by break/continue, fields and
array/pointer-local cleanup remain unqualified. Copies require readable input,
add allocation cost and retain the helper's null-on-allocation-failure contract.
General shape/bounds/operator limits and existing C warnings remain. Windows tools:
Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Approved SDK/cache access was
used without content/permission changes; compiler checks ran sequentially.

Next: verify P0021 delivery and native CI, repairing failures first. Otherwise
choose a bounded **T011 scalar string reassignment** slice: audit/reproduce
literal/borrowed/managed replacement on normal paths, then define and qualify
old-value release and replacement copying in both modes. Return transfer remains
separate. Do not start a second pass here.
