# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0027 - unsupported Go build entry-point cleanup**.
- Record: [P0027](passes/P0027-go-tooling-cleanup.md).
- Parent/fetched approved remote: `57c8edf0edc7c05fc5385d2748ceb3011413fb49` (P0026).
- Resolve the pass commit by its exact `Pass-ID: P0027` trailer.
- Publication requires push and matching `git ls-remote`; inspect CI before new work.

The user requested another cleanup pass ahead of T011. It started clean in the
approved primary checkout/branch/origin, one worktree and no competing active
project chat. Approved fetch matched HEAD. P0026
[CI](https://github.com/Ddemon26/DreamCompiler/actions/runs/43652638299) was initially
pending; both native jobs passed before this pass edited files.

P0027 removes the two obsolete Go C-library builder scripts and reduces the main
Makefile/PowerShell launcher to the supported quick protocol and help. Defaults
now select quick. The Make build/Go recipe is unchanged; the PowerShell script
resolves its working directories and returns build/test failure codes, stopping
before Go on build failure. Retired actions fail instead of installing modules or
invoking absent cgo APIs, unsupported benchmark/fuzz modes or recursive tests.
No launcher purges caches. The Go README and testing guide describe the contract.

The exact 22-group selectors remain identical to the original and CI. **474
protected file hashes** remain unchanged, including all compiler/runtime sources,
active/deferred fixtures, docs/Pages assets, plugins, unique Go test/C/header/
checksum files, build inputs, supported Python runner/config and CI. No assertions
or exclusions changed. The full suite passed **136/154 with the same 18 skips**,
zero failures/errors, 48.74s. All **22 real Go quick groups passed**, 36.863s;
Python **five passed**, 0.029s. Seven controlled launcher probes, PowerShell
parsing/BOM, GNU Make help/dry-runs and retired-target rejection passed. See
[results](MostRecentTestResults.md); 95 local Markdown links also passed. No new cross-build was required because no
compiler/C/build inputs changed. Native P0027 CI awaits publication.

The Pages metadata reports a built site sourced from `main:/docs`; HTML/CSS
cannot be treated as unused and were retained. Unique legacy cgo test sources,
fixture-local Makefile, disabled benchmark/fuzz code and checksums also remain
pending replacement-coverage decisions. The site needs a separate navigation/
accuracy repair. Prior ownership/shape/operator/unwinding limits remain.

Next: verify P0027 publication/native CI and repair failures first. Otherwise
resume T011 conditional-string initializer ownership with live-registry and
branch-effect evidence, unless the user chooses another cleanup slice.
Do not start a second pass here.
