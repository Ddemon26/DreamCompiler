# P0013 - Preserve primitive pointer parameter C signatures

- Date: 2026-11-12 UTC
- Task: T008
- Parent checkpoint: `b83b7a186e2f1ace7eb14b557866c5f08410a75a`
- Commit lookup: exact `Pass-ID: P0013` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Primitive pointer parameters were parsed but emitted as scalars. Before the fix,
the new ordinary/module-entry regression failed all ten expected C signatures in
each mode, before host compilation. `src/codegen/stmt.c` now calls the existing
`emit_type_with_pointer` helper for free-function prototypes, function/static
method definitions and instance methods, including local/explicit base `init`.
`bool*` maps to `int*`; `string*` maps to `const char **`. Existing scalar types and
the helper's Task special case are preserved. No lexer/parser, semantic, module
lowering or runtime code changed; no new shape compatibility policy is claimed.

`codex/go/pointer_parameters_test.go` checks generated C before host compilation,
then exact runtime output in ordinary and module-entry code. The annotated
`tests/advanced/oop/pointer_parameters.dr` covers all five primitive pointers,
borrowed array reads/writes, an alias, static/inherited/instance calls, `this`/`base`
forwarding, local/base construction, explicit `init` and forward free-function
calls. Module mode also passes a dependency function result as a scalar index.
Both quick protocols and CI select the new group. Feature, function, grammar,
tooling and source-map docs describe the qualified boundary and remaining defects.

## Validation

Starting state: clean approved checkout/branch/origin, one registered worktree,
no unpublished agent pass in memory/history and no competing active writer in
the app inventory. Restricted fetch failed DNS; approved fetch succeeded, matching
the parent above. P0012 [CI run 43208505371](https://github.com/Ddemon26/DreamCompiler/actions/runs/43208505371)
completed successfully. Automation tool/config inspected: already local execution
with the approved cwd; unchanged.

Windows: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4. Restricted baseline
could not locate the installed Zig SDK. Compiler checks used approved SDK/cache
access; no SDK/cache content or permissions were changed.

- Unchanged `zig build test`: 142 discovered, 124 passed, 18 skipped, zero
  failures/errors, 38.12s. Report:
  `test_results/test_results_20261112_095735_windows.json`.
- Pre-change `go test -count=1 -v -run '^TestCompiler_PointerParameters$' .`
  from `codex/go`: failed as intended, 0.483s. All ten signature expectations
  failed in each mode; host compilation was not reached.
- `zig build`: passed after implementation.
- First focused run: ordinary signatures passed, runtime exposed incorrect direct
  indexed float/char console formatting (`0`, `90` instead of `2.500000`, `Z`).
  Module free-function assertions used unlowered names; corrected to the actual
  `__dream_module_0_` prefix without relaxing signature assertions.
- Second focused run: typed numeric locals produced correct values; adding a
  borrowed string local exposed block cleanup calling `dr_release` on literal
  storage, panicking on misaligned `DrRef` in both modes. Final fixture prints
  the string element directly. No output assertion was weakened, no type/case
  was removed, and no runtime check was suppressed. These are separate open
  defects, recorded with triggers/acceptance in T009/T010.
- Final focused command above: two modes passed, ten signatures per mode and
  exact output, 1.232s (body 0.80s).
- Final `zig build test`: 143 discovered, 125 passed, 18 skipped, zero failures/
  errors, 35.34s. New annotated entry passed. Report:
  `test_results/test_results_20261112_100109_windows.json`.
- From `codex/go`, `powershell -NoProfile -ExecutionPolicy Bypass -File
  ./build_and_test.ps1 quick`: all eleven groups passed, 13.567s (pointer 0.74s).
  Existing method/constructor scalar diagnostics and all other groups remain
  selected; compiler invocations use `-count=1`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  five passed, 0.037s.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux`: passed.
  Linux binaries were not executed locally; inspect native P0013 CI after push.
- Complete diff review and `git diff --check` passed. All 42 local Markdown link
  occurrences in changed docs resolve; the new parameter anchor, source/fixture
  paths, Go formatting, quick/CI selection and PowerShell UTF-8 BOM were checked.

Ignored evidence: `build/P0013-*.log` contains baseline, pre-change, build,
first/final focused, full suite, quick, Python and cross-build output. The second
focused panic is recorded above; its shared final log was overwritten by success.

## Limits and handoff

The same 18 unannotated skips and existing exclusions remain. Pointer shape,
lifetime, ownership, bounds, null safety, returns and async paths are unqualified;
custom-type/array parameters, field initializers, implicit inherited construction,
all-path return proofs and general CFG/SSA emission remain unsupported/unqualified.
This pass preserves parameter C types and demonstrates compatible borrowed calls;
it does not establish general pointer safety or fix indexed formatting/ownership.

Commit only this pass, push the approved branch and verify the remote SHA before
reporting delivery. Inspect native CI first next run; otherwise take T009, the
borrowed indexed-string local cleanup defect. Do not start another pass here.
