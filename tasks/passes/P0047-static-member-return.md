# P0047 - Clean locals before direct read-only static string-field returns

- Date: 2026-11-16 UTC
- Task: T011 direct static scalar string-field return cleanup
- Parent checkpoint: `3e1db012b833c7060217120533d82321d05a9df9`
- Commit lookup: exact `Pass-ID: P0047` trailer
- Delivery branch: `origin/codex/compiler-stabilization`

## Objective and result

Implement the [P0046 contract](../../docs/development/static-member-return-cleanup.md).
Direct `return Type.field;` now captures the owning global pointer once before
reverse local cleanup for an unshadowed registered class type and an actual scalar
static string declaration/owner. `borrowed_static_member_result` is separate from
the literal-only and instance/parameter predicates. The shared member traversal
accepts an explicit guarded name, preserving its node/operator allowlist without
inventing receiver binding metadata. Writes/calls/new/console/await/exceptions,
custom-type/pointer/array locals, shadowing and unknown forms still fail closed.
No field/result copy, retain, global release or field ownership is introduced.

The new annotated [fixture](../../tests/basics/memory/static_member_return.dr) and
[Go group](../../codex/go/static_member_return_test.go) check ordinary/module-entry
lifetime, exact C and fallback barriers. Both modes cover managed/literal/empty/
typed-null backing, inherited/hidden owner selection, free/static/instance readers,
independent snapshots, nested/unbraced/alternate/loop/header/switch paths, later
locals, temporary/global-symbol collisions, no-local returns and caller survival.
The borrowed global is cleared before caller backing exits. Both quick selectors
and CI select the new group. The old direct member group's two static controls
become positive C checks; conditional static controls and all lifetime assertions
are preserved. Feature, function, ownership, source-map and handoff docs are updated.

## Validation

- Approved clean checkout/path/attached branch/origin verified. Available chats,
  preceding memory, all-ref history and one registered worktree showed no unfinished
  pass or competing active writer. Schedule view/TOML already use local execution
  and approved cwd; unchanged. Restricted fetch failed DNS; approved fetch succeeded.
  HEAD and approved fetched branch matched the parent, zero commits on either side.
- P0046 [native CI 43821203292](https://github.com/Ddemon26/DreamCompiler/actions/runs/43821203292)
  completed successfully for the exact parent SHA. Linux job 133144451607 and
  Windows job 133144451865 passed build, Python, Go quick and full Dream suite.
  Restricted metadata request failed network access; approved read succeeded.
- Zig **0.17.1**. Restricted baseline could not locate the installation directory;
  approved installed-toolchain/cache access succeeded without cache/config changes.
  Unchanged `zig build test`: **171 discovered, 153 passed, 18 skipped, zero failures/
  errors**, **48.75s**; `test_results/test_results_20261116_060710_windows.json`.
- Before fix, `go test -count=1 -v -run '^TestCompiler_StaticMemberReturn$' .`
  from `codex/go`: **both modes failed** the empty pre-shutdown registry assertion,
  **2.637s** (group 2.20s). After `zig build` passed, initial both-mode lifetime
  check passed **2.860s** (group 2.43s), **40 nonnull releases** and **23 output lines**
  per mode. Independent count: 36 callee strings plus caller backing/replacement,
  receiver and snapshot. No static-result survivor is expected.
- Final focused static group: **252 subtests passed**, **9.015s**. Each mode checks
  exact C before host compilation, live operands/refcounts, native UBSan, Linux
  harness cross-linking, directly linked exact output and empty pre/post-shutdown
  registries. Controls comprise two lifetime modes, 208 shared controls (206
  fallback/two migrated static positives), 30 expression controls (28 fallback/
  two existing-instance positives), twelve artifact-free semantic rejections.
- Intermediate three-group run passed prior member groups but failed new test
  expectations: an assignment C check omitted existing parentheses; local type
  shadowing was rejected semantically rather than reaching emission. These now
  have correct exact C and explicit diagnostic checks. A later static run found
  primitive parameter shadows use the receiver diagnostic; a broader run found
  indexed class-array field receivers rejected similarly. Final tests assert those
  existing boundaries without changing semantics or removing allocator assertions.
  Final native rebuild passed. No unsafe mutation/alias example is executed.
- `zig build test`: **172 discovered, 154 passed, 18 skipped, zero failures/errors**,
  **46.49s**; `test_results/test_results_20261116_061319_windows.json`.
- `python -m unittest discover -s codex/python -p test_test_manager.py`:
  **five passed**, **0.043s**.
- Supported Windows Go quick protocol from `codex/go`, `powershell -NoProfile
  -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`: **all 38 groups passed**,
  **86.012s**, compiler invocations use `-count=1`. Earlier member/conditional and
  P0037-P0042 lifetime checks passed unchanged except the documented static-control
  migration. The static group again passed all 252 subtests.
- `zig build -Dtarget=x86_64-linux-gnu --prefix build/linux --summary all`:
  **passed, 15/15 steps**. No native P0047 Linux runtime claim from cross-building.
- Reviewed all 18 pass files and full diff; **225 local Markdown paths**, **seven
  fragment anchors**, UTF-8 in 18 files, PowerShell BOM, 38-group selector/actual
  equality, P0001-P0047 index continuity and 23 annotated outputs passed.
  `gofmt -l` is empty; `git diff --check` passed. Tracked lexer, checked allocator
  harness and earlier direct/conditional member Dream fixtures are unchanged.
  Logs and commit message: ignored `build/P0047-*.log` / `build/P0047-commit.txt`.

## Limits and handoff

Same eighteen unannotated skips and prior grammar/operator/shape limits persist.
Valid incoming backing and synchronous single-threaded execution remain preconditions.
Conditional static leaves, struct/variable/this/base/nested/index/call qualifiers,
static pointer/array storage emission, caller consumption/retained owned-result
accumulation, aliases/containers/destructors/fields, pointer/class result transfer
and async/exception unwinding remain separate. Current local type-name shadowing
is rejected as redefinition; primitive parameter/indexed class-array field
receivers are rejected before C. No grammar/semantics were broadened.

Publish and verify remote SHA; inspect native CI and repair failures first. Next
qualify conditional static value leaves under the unchanged read-only proof, with
mixed borrowed/owned/literal lifetime and failed-leaf controls. End after this pass.
