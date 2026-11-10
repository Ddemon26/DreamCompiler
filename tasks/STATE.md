# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0010 - primitive scalar method arguments; validation recovered**.
- Record: [P0010](passes/P0010-method-arguments.md).
- Parent/fetched approved remote: `37d310c626b9af347343baf214a02a46b2e07081` (P0009).
- Resolve the pass commit by its exact `Pass-ID: P0010` trailer.
- Publication requires push and matching `git ls-remote`; report that identity after
  delivery. Inspect native CI before selecting the next task.

Use only `C:/Users/damon/cApps/DreamCompiler`, attached to the integration branch.
The 2026-11-10 recovery resumed the documented uncommitted P0010 files, with one
registered worktree and no competing writer in app inventory. Restricted fetch
failed DNS; approved fetch succeeded and HEAD matched the fetched parent. The
schedule already uses local execution and this directory; no change was needed.
P0009 [CI run 42481471439](https://github.com/Ddemon26/DreamCompiler/actions/runs/42481471439)
was rechecked and remains successful.

The semantic method-call path checks exact primitive scalar parameter types
through existing receiver/member resolution, with one analysis per argument.
Diagnostics identify argument index, method, expected/actual type, and call position
before emission. Inherited/static/instance, `this`/`base`, declared receiver/override
selection, explicit `init`, and module-entry classes are covered. Both quick
protocols and CI include the Go group; two annotated Dream entries cover execution
and rejection. Language/feature docs explain the supported boundary.

Windows recovery on 2026-11-10 UTC: Zig 0.17.1, re2c 3.0, Python 3.13.15, Go 1.27.0.
`zig build test`: **139 discovered, 121 passed, 18 skipped, zero failures/errors**
(47.48s). All **eight Go quick groups passed** (10.964s), including **82 method
argument rejections and two exact-output programs**. Python: **five passed**
(0.052s). Linux cross-build passed; native Linux runtime awaits CI. Restricted
execution could not locate Zig's installed SDK; approved SDK/cache access was used.
The earlier changing SDK/cache file-open errors did not recur. Their cause remains
unproven. No SDK/cache content or permissions, tests, assertions, exclusions, or
implementation changed during recovery. Prior failed attempts remain in the pass
record and historical test checkpoint. See [MostRecentTestResults.md](MostRecentTestResults.md)
and ignored `build/P0010-recovery-*.log`.

The initial new positive fixture exposed that existing `int*` method parameters
emit C `int`. Its scalar fixture was corrected during implementation; a separate
negative case with an excluded pointer parameter checks the second scalar only.
Pointer runtime behavior remains unqualified. The same 18 unannotated entries,
supporting module fixtures, and graphics/concurrency exclusions remain unchanged.
Pointer/array shapes, custom-type arguments, implicit `new` arguments, broader
operators, field initializers, all-path returns, and general CFG/SSA emission remain
deferred. P0009's checked literal bridge is unchanged.

After verified P0010 publication, inspect native CI and fix failures first. The next
bounded feature is primitive constructor argument compatibility and arity through
resolved `init`, including artifact-free diagnostics. Do not start it in this run.
