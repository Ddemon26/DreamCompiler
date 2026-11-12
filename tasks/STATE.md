# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0013 - preserve primitive pointer parameter signatures**.
- Record: [P0013](passes/P0013-pointer-parameter-emission.md).
- Parent/fetched approved remote: `b83b7a186e2f1ace7eb14b557866c5f08410a75a` (P0012).
- Resolve the pass commit by its exact `Pass-ID: P0013` trailer.
- Publication requires push and matching `git ls-remote`; report that identity
  after delivery. Inspect native CI before selecting the next task.

The 2026-11-12 pass started clean in the approved primary checkout, attached to
the correct branch/origin, with one registered worktree, no unpublished agent
pass and no competing active writer in the app inventory. Restricted fetch failed
DNS; approved fetch succeeded and matched HEAD. P0012
[CI run 43208505371](https://github.com/Ddemon26/DreamCompiler/actions/runs/43208505371)
passed. The automation already uses local execution and the approved cwd; unchanged.

`src/codegen/stmt.c` now uses the existing pointer-aware type emitter in free
function prototypes, function/static method definitions and instance methods,
including `init`. Previously these paths dropped the pointer marker. No parser,
semantic, module lowering or runtime implementation was changed. Scalar diagnostics
retain their existing policy. The helper's existing Task special case is unchanged;
async and task behavior are not qualified by this pass.

The new ordinary/module-entry Go matrix inspects ten signatures per mode before
host compilation and requires exact runtime output for all five primitive pointer
types, borrowed array reads/writes, an alias, static/inherited/instance calls,
`this`/`base` forwarding, local/base construction, explicit `init` and forward
free-function calls. One annotated Dream entry is added. Both quick protocols
and CI select the group; docs state the bounded signature/ownership boundary.

Windows toolchain: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.
Unchanged baseline: 142 discovered, 124 passed, 18 skipped, zero failures/errors,
38.12s. Pre-change pointer checks failed all ten signatures in both modes. Final
focus passed (1.232s). Final full suite: **143 discovered, 125 passed, 18 skipped,
zero failures/errors**, 35.34s. All **eleven Go quick groups passed**, 13.567s.
Python: **five passed**, 0.037s. Linux cross-build passed; native P0013 runtime
awaits CI. Restricted SDK lookup failed; approved SDK/cache access passed without
content/permission changes. See [MostRecentTestResults.md](MostRecentTestResults.md).

Initial fixtures exposed two existing defects: direct indexed float/char console
formatting is wrong, and a local string borrowed from indexed storage is released
incorrectly. The final fixture uses typed numeric locals and direct string reads,
preserving all output assertions; module signature expectations use lowered names.
These are scope corrections, not fixes to the two defects. T009/T010 record both.

The same 18 unannotated skips and existing exclusions remain. Pointer shapes,
ownership, lifetime, bounds, pointer returns, custom/array parameters, async,
field initializers, implicit inherited construction, all-path returns and general
CFG/SSA emission remain unqualified or unsupported.

Next: verify P0013 delivery and inspect native CI; repair failures first. Otherwise
take **T009**: reproduce and correct borrowed indexed-string local cleanup under
an explicit literal/owned/borrowed string contract. Do not start a second pass here.
