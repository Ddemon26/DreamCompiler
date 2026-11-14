# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0026 - repository tooling and operational documentation cleanup**.
- Record: [P0026](passes/P0026-repository-cleanup.md).
- Parent/fetched approved remote: `52de683a1752e1a6b22b5ecfe673fdc72f23cc76` (P0025).
- Resolve the pass commit by its exact `Pass-ID: P0026` trailer.
- Publication requires push and matching `git ls-remote`; inspect CI before new work.

The user requested repository pruning ahead of T011. The pass started clean in
the approved primary checkout, attached branch/origin, one worktree and no competing
active project chat. Approved fetch matched HEAD. P0025
[CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43649932934) passed
native Linux/Windows builds, stability checks and suites for the exact parent SHA.

P0026 removes eleven proven obsolete files: a downloaded 66 MB Go installer,
root scratch programs, superseded runner/runtime scripts, an unusable cgo workflow
wrapper and redundant stale setup/status documents. A narrow root installer ignore
rule prevents reintroduction. The pass record inventories each deletion and evidence.
No history rewrite or recursive cache purge was used.

Operational build, architecture, development and testing guides now describe the
actual supported commands and pipeline. Compiler testing delegates to the maintained
[codex testing guide](../codex/TEST_GUIDE.md). The current Go coverage descriptions
remain without the stale cgo tutorials. README uses the maintained Markdown index
and the approved existing-checkout schedule policy. No compiler or plugin code,
fixtures, test exclusions, build graph, supported runner or CI selector changed.

Verification: **367 protected file hashes unchanged** and **154 test entries with
identical categories** before/after. The full suite passed **136/154**, with the
same **18 skips**, zero failures/errors, 51.72s. All **22 Go quick groups passed**,
41.955s; Python **five passed**, 0.044s. Revised/new Markdown's **184 local links**
resolved in the final check; added handoff links, retired-entry references,
paths and diff were checked. See [results](MostRecentTestResults.md).
No C/build inputs changed, so no new cross-build was required. Native P0026 CI
awaits publication. SDK/cache access used normal approval without content changes.

P0025's Dream production normalizer and C-only host bootstrap remain intact.
This cleanup does not qualify new language features or eliminate the same ownership,
shape/operator, return/unwinding, allocation-failure and skipped-test limitations.
Historical records, versioned references, unique legacy cgo tests, deferred examples
and experimental compiler code remain. The HTML docs viewer and broader language/
API tutorials need a separate path/accuracy audit; this is one bounded cleanup.

Next: verify P0026 publication/native CI and repair failures first. Otherwise
resume T011 conditional string initializer ownership using checked live-registry
and branch-evaluation evidence, unless the user chooses another cleanup slice.
Do not start a second pass here.
