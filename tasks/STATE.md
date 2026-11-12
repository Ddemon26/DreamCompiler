# Current project handoff

- Integration branch: `codex/compiler-stabilization` on public `Ddemon26/DreamCompiler`.
- Current pass: **P0012 - parser parameter recovery and statement progress**.
- Record: [P0012](passes/P0012-parser-parameter-recovery.md).
- Parent/fetched approved remote: `2e28d5661d54920236027176ed44d670ec07820a` (P0011).
- Resolve the pass commit by its exact `Pass-ID: P0012` trailer.
- Publication requires push and matching `git ls-remote`; report that identity after
  delivery. Inspect native CI before selecting the next task.

The 2026-11-12 pass started clean in `C:/Users/damon/cApps/DreamCompiler`, attached
on the approved branch/origin, with one registered worktree and no competing
active writer in the app inventory. Memory/history showed P0011 delivered and no
unfinished pass. Restricted fetch failed DNS; approved fetch succeeded and HEAD
matched the fetched parent. P0011 [CI run 43022236461](https://github.com/Ddemon26/DreamCompiler/actions/runs/43022236461)
passed native Linux and Windows, including stability and Dream suites. The schedule
already has local execution and the correct cwd; unchanged.

Unsupported custom-type parameters previously stalled at ten seconds in both
ordinary and module-entry compilation. Parameter error recovery now discards the
invalid signature and balanced body, leaving enclosing braces and following
declarations to their callers. Custom-type parameters produce a source-positioned
error plus an explanatory hint. A statement progress guard prevents repeated
parsing of a stray expression delimiter at program/block/switch scope. No custom
or array parameter support is added; no semantic/backend/runtime/lexer edits.

The CLI matrix checks 75 bounded rejections across ordinary, module-entry and
imported-dependency paths: malformed parameters, EOF, delimiters, positions,
continued diagnostics at following declarations and no artifacts. One annotated
Dream rejection covers the original trigger. Both quick protocols and CI include
the group; feature, function, grammar and tooling docs state the supported boundary.

Windows toolchain: Zig 0.17.1, re2c 3.0, Python 3.13.17, Go 1.27.4.
Unchanged baseline: 141 discovered, 123 passed, 18 skipped, zero failures/errors,
43.58s. Final `zig build test`: **142 discovered, 124 passed, 18 skipped, zero
failures/errors**, 38.82s. All **ten Go quick groups passed** (12.752s), including
75 parser rejections (0.99s). Focused parser matrix passed in 1.907s. Python:
**five passed** (0.044s). Linux cross-build passed; P0012 native runtime awaits CI.

Restricted execution could not locate Zig's SDK; compiler checks used approved
SDK/cache access without SDK/cache content or permission changes. No changing
SDK/cache file-open failures recurred in approved baseline/final checks. Initial
focused failures were fixture position expectations: an enclosing brace on line 4
and differing driver/module EOF normalization. Fixtures now require the correct
explicit line, preserving all assertions/cases. See [MostRecentTestResults.md](MostRecentTestResults.md)
and ignored `build/P0012-*.log` for exact outcomes, including pre-change timeouts.

The same 18 unannotated skips and existing module/graphics/concurrency exclusions
remain. Pointer emission/shape compatibility, custom/array parameters, field
initializers, implicit inherited construction, all-path returns and general CFG/SSA
emission remain unqualified. Recovery does not prove termination on arbitrary
malformed input or unbounded nesting.

Next: verify P0012 publication and inspect native CI, repairing failures first.
Otherwise take **T008**: reproduce and preserve primitive pointer parameters in
emitted method/constructor signatures, with generated-C and exact runtime checks
before expanding shape compatibility. Do not start another pass in this run.
