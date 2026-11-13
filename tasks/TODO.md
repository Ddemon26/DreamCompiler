# Ordered compiler backlog

Read `STATE.md` first. Current measurements live in `MostRecentTestResults.md`;
implemented language boundaries live in `../codex/FEATURES.md`. Do one bounded pass,
commit and push it, and verify the remote SHA before starting another.

A baseline regression or failed CI takes priority over this queue. Select the first
ready item; if it spans several passes, finish one useful, tested slice and leave the
remaining acceptance criteria open. Update this queue with evidence when work lands.

## T001 - Primitive function return checks (completed in P0005)

P0005 checks exact primitive return types and rejects missing values and void return
expressions before C emission. Methods use their own return context; nested switch/try
bodies and both conditional branches are visited. Four dependency-return cases require
the originating source line and absence of generated artifacts. Matching returns run
in Dream regressions and the module matrix. Numeric policy is documented in
`../docs/language-features/functions.md`.

Recovery validation: 114 Dream passes, 19 skips, no failures/errors; four Go quick
groups passed (23 diagnostic cases and 20 module cases included); five runner tests
passed; Linux cross-build succeeded. See [P0005](passes/P0005-primitive-return-checks.md).
Missing-return proofs, pointer/array shapes, and broader operator validation remain
separate follow-ups. Verify P0005 publication before starting T002.

## T002 — Member-call and static/instance diagnostics (completed in P0006)

Close cases where an unresolved receiver or invalid static/instance access reaches the
C compiler instead of a Dream diagnostic. Cover ordinary code and module entry classes.

Acceptance: a focused negative/positive matrix, no generated artifacts for rejected
programs, useful source locations, and preserved existing inheritance/module behavior.

P0006 implements the receiver/access slice: 15 ordinary rejection cases, five module
cases, and three annotated Dream entries. Recovery on 2026-11-06 UTC passed the full
Dream suite (117 passed, 19 skipped, zero failures/errors), all four Go quick groups,
five Python runner tests, and Linux cross-build. The earlier intermittent Zig SDK/cache
file-open errors did not recur; no tests or caches were changed to obtain the pass.
Verify [P0006](passes/P0006-member-access-diagnostics.md) publication before new work.
After delivery, proceed to T003; method argument type/shape checks remain follow-up work.

## T003 — Close a skipped-regression gap (completed in P0007)

Choose one of the 19 entry files skipped at the P0006 checkpoint, verify its intended behavior against
the implemented language, and make it an honest executable or rejection regression.
Fix the real compiler defect if needed. Do not annotate an unsupported program as passing,
move it out merely to improve counts, or change expected behavior without an explanation.

Acceptance: one newly meaningful checked behavior, a recorded before/after result, and
an accurate explanation of the remaining skips. Supporting module files remain fixtures.

P0007 makes `tests/optimization/test_loop_opt.dr` an executable `-O2` regression
with independently calculated results 112 and 8. A four-setting Go matrix exposed
heap corruption and nontermination in SCCP/LICM/loop analysis; the real optimizer
paths are fixed, and the matrix runs in both quick protocols and CI. Full Windows
suite: 118 passed, 18 skipped, zero failures/errors, versus the unchanged baseline's
117 passed and 19 skipped. Go quick, five runner tests, and Linux cross-build passed.
The other 18 entries remain unannotated and unverified, with no new exclusions.
Verify [P0007](passes/P0007-loop-optimization-stability.md) publication, then take T004.

## T004 — Connect optimization to emitted code (first bounded slice completed in P0009)

Before P0009, the driver optimized and discarded a CFG, then emitted C from the AST.
The selected task was a small end-to-end optimization slice with preserved behavior.
Do not describe optimizer source files alone as working generated-code optimizations.

Acceptance for the first slice: behavior-equivalence tests at supported optimization
levels, evidence the transformed representation affects emitted code, and explicit
fallback/error behavior for unsupported lowering. Split larger SSA/backend work into
subsequent passes with durable handoffs.

P0007 supplies a loop execution baseline at the default setting and `-O1`/`-O2`/`-O3`,
and repairs the crashes/stalls it exposed. It does not connect optimized IR to C or
validate IR semantics. First define a bounded supported lowering and its correctness
requirements; current non-SSA propagation and CFG transformations need qualification
before their results can be emitted. Fusion/empty-loop elimination/derived induction
recognition are still analysis placeholders and must not claim transformations.

P0008 completes the prerequisite source audit and bounded
[design](../docs/development/optimization-emission.md), without changing compiler
behavior. It selects literal-only `+`/`-`/`*` expression IR with checked nonnegative
intermediates, restricted SCCP, and an atomic literal replacement consumed by the
existing AST emitter. Unsupported expressions retain AST emission; internal errors
must fail explicitly. Replace the unsafe discarded whole-program pipeline call
rather than running it on fallback source. Generated-C assertions before host
compilation are required alongside exact runtime output at all four settings.

P0009 implements that restricted bridge and completes the first slice's emitted-code
and equivalence criteria. Generated C is inspected before host compilation, exact
output matches at default/`-O1`/`-O2`/`-O3`, and fallback/side effects, function/method/
module bodies, bounds, object emission, and controlled internal failures are checked.
The unsafe discarded whole-program pipeline is removed from the driver. General
CFG/SSA emission and extending eligible expressions remain separate future work;
no loop or variable propagation is claimed. Full Windows suite: 119 passed, 18
skipped, zero failures/errors; all seven Go quick groups, five Python runner tests,
and Linux cross-build passed. See [P0009](passes/P0009-literal-emission.md).

Next: verify P0009 publication and inspect CI, then take T005. Repair any failed
baseline first; do not expand the bridge to unqualified mutable-variable IR.

## T005 — Primitive method argument compatibility (completed in P0010)

Extend the existing method arity checks to reject incompatible primitive scalar
arguments before C emission. Use the actual receiver/method resolution path and
the documented numeric compatibility policy; preserve existing valid method calls.
Include ordinary source, inherited/static/instance methods, and module entry classes.

Acceptance: focused positive/negative matrix with useful source positions and no
new artifacts for rejection, exact output for valid calls, required full checks,
and explicit remaining limits for arrays, pointers, and other argument shapes.

P0010 implements this scalar boundary with 82 ordinary/module-entry rejection cases
and two exact-output programs, plus two annotated Dream regressions. Both new Dream
entries, the focused matrix, Python runner tests, and Linux cross-build passed.
Recovery on 2026-11-10 UTC passed the complete Dream suite (121 passed, 18 skipped,
zero failures/errors), all eight Go quick groups, five Python runner tests, and
the Linux cross-build. The earlier changing Zig SDK/cache errors did not recur;
no exclusions, weakened assertions, SDK/cache changes, or implementation edits
were used. See [P0010](passes/P0010-method-arguments.md) for historical failures
and recovery evidence. Verify publication and inspect native CI before new work.

## T006 - Primitive constructor arguments (completed in P0011)

Check `new Type(...)` arity and exact primitive scalar arguments against the
class's own instance `init`, following the actual emitter's selection. Constructors
are not inherited implicitly; classes without local `init` and structs take zero
arguments. Reject formerly ignored arguments and static constructor methods before
artifacts. Preserve explicit `base.init(...)` behavior and valid construction.

Acceptance: all primitive mismatches, arity/default/local/inherited selection,
nested arguments analyzed once, ordinary/module-entry diagnostic positions without
artifacts, exact positive output, full checks, and documented shape limits.

P0011 implements this boundary, with 80 rejection cases and two exact-output
programs plus two annotated Dream entries. Final full Windows suite: 123 passed,
18 skipped, zero failures/errors; all nine Go quick groups, five runner tests and
Linux cross-build passed. Two prior full runs hit changing SDK/cache file-open
errors; no tests/caches/implementation changed to obtain the final passing run.
See [P0011](passes/P0011-constructor-arguments.md). Verify publication and inspect
native CI before T007; fix any failed baseline first.

## T007 - Guarantee parser progress on unsupported parameter syntax (completed in P0012)

A new P0011 fixture initially used a custom-type parameter (`func init(Child child)`),
which the current primitive-parameter grammar does not accept. Both ordinary and
module-entry compilation timed out at 10 seconds. The constructor regression now
uses the supported primitive field expression `new Child(1.5).value` and retains
the same nested-constructor diagnostic and once-only assertions.

Acceptance: reproduce the unsupported parameter stall and repair parser
recovery so malformed/unsupported declarations terminate with useful Dream errors
and no artifacts. Add timeout-bounded ordinary/module regressions; do not silently
claim custom-type parameter support or broaden the constructor contract.

P0012 reproduces both ten-second stalls, recovers invalid parameter signatures and
balanced bodies, and guarantees progress for stray statement delimiters. All 75
bounded rejection cases pass across ordinary/module-entry/dependency paths, with
positions, continued declaration diagnostics and no artifacts. Full Windows suite:
124 passed, 18 skipped, zero failures/errors; all ten Go quick groups, five runner
tests and Linux cross-build passed. Custom-type and array parameters remain
unsupported. See [P0012](passes/P0012-parser-parameter-recovery.md). Verify delivery
and native CI before T008.

## T008 - Preserve primitive pointer method parameter emission (completed in P0013)

P0010 exposed method `int*` parameters emitted as scalar C `int`; the pointer
positive fixture therefore failed host compilation. Parser acceptance and deferred
shape checks do not establish working pointer arguments. Reproduce this defect,
then take a bounded signature-emission slice through the actual static/instance
method and constructor paths. Keep array/custom-type support and broader shape
compatibility separate unless qualification is required for correctness.

Acceptance: inspect generated signatures before host compilation, require exact
runtime output for valid pointer method/constructor calls in ordinary and module
entry sources, retain scalar argument diagnostics, run required checks and document
any remaining pointer shape/ownership limits. Repair failed baseline/CI first.

P0013 preserves pointer markers through the common type emitter for function
prototypes/definitions and instance methods (including constructors). The new
ordinary/module-entry matrix checks ten generated signatures per mode before
host compilation, then exact output across all five primitive pointer types,
array mutations, aliases, inheritance, local/base construction and forwarding.
One annotated Dream entry is added. Scalar compatibility policy is unchanged;
shape, ownership, async and pointer return paths remain unqualified. See
[P0013](passes/P0013-pointer-parameter-emission.md). Verify publication and native
CI before the next task; repair failed checks first.

## T009 - Correct cleanup of strings borrowed from indexed storage (completed in P0014)

P0013's initial fixture used `string textValue = s[0]` inside a static method
with `string* s` pointing to an array containing a literal. Both ordinary and
module-entry executions printed the expected text, then panicked in `dr_release`
on a misaligned `DrRef`. Block cleanup releases every string local, including
borrowed literal storage. P0013 prints the indexed string directly and retains
its original exact output; it does not qualify borrowed string local ownership.

Acceptance: reproduce this runtime failure, establish the actual literal/owned/
borrowed string contract, and fix the relevant initializer/cleanup path without
removing releases or suppressing runtime checks indiscriminately. Check ordinary
and module-entry exact output, nested scopes and dynamically allocated backing
strings with meaningful lifetime evidence. Run required checks and document the
remaining ownership limits. Keep this separate from general pointer shapes.

P0014 gives scalar string declarations initialized directly from indexing their
own managed copy. Normal cleanup remains intact; the allocator is unchanged.
The ordinary/module-entry matrix validates emitted C, exact output, live managed
release operands, nested blocks, dynamic/literal/null backing and once-only index
evaluation. Helper checks prove a copy survives release of its backing allocation.
The original uninstrumented fixture happened to pass this run; the checked live
registry reproduced the invalid literal cleanup deterministically in both modes.
See [P0014](passes/P0014-indexed-string-ownership.md) and the
[bounded ownership contract](../docs/development/string-ownership.md). Broader
ownership is deferred to T011. Verify publication and native CI before T010.

## P0015 baseline repair before T010

P0014 native CI failed Linux ownership-harness compilation: strict C11 hid the
POSIX `strdup` declaration because the harness omitted the normal runtime's
`_GNU_SOURCE` flag. P0015 aligns that flag and cross-links both harness modes for
Linux on Windows. The new checks reproduced the same four errors before the fix;
focused, full Dream, twelve Go quick groups, Python and Linux cross-build passed.
See [P0015](passes/P0015-linux-string-harness.md). Verify delivery and native CI
before T010; repair failed checks first. No T010 implementation was started.

## T010 - Preserve indexed primitive types for console formatting (completed in P0016)

P0013's initial direct `Console.WriteLine(r[0])` / `Console.WriteLine(c[0])`
inside pointer-taking methods emitted `%d`, producing `0` and `90` rather than
`2.500000` and `Z`. Its final signature fixture copies the numeric elements to
typed scalar locals and keeps all original output assertions. The same backend
classification should be audited for local arrays and scalar-string indexing.

Acceptance: regress generated format selection and exact runtime output for
indexed primitive arrays/pointer parameters in ordinary and module-entry code;
preserve existing member indexing and distinguish scalar string indexing from
string array elements. Avoid inferring general shape compatibility from this fix.

P0016 preserves array/pointer layers in backend bindings and consumes them during
indexed classification. Direct float/char output selects `%f`/`%c`; string array
or pointer elements remain strings, while scalar string indexing prints characters.
The ordinary/module-entry matrix checks 18 C format snippets and exactly 41 output
lines per mode, all five primitives, member/this/base storage, free/static paths,
numeric aliases, nested string indexing and once-only index effects. Full Windows
suite: 127 passed, 18 skipped, no failures/errors; thirteen Go quick groups, five
Python tests and Linux cross-build passed. See [P0016](passes/P0016-indexed-console.md).
Semantic shape/bounds/operator/ownership qualification remains separate. Verify
publication and native CI before T011; repair failed checks first.

## T011 - Qualify remaining string ownership paths (literal/identifier slices completed)

P0014 qualifies scalar declarations initialized directly from an index expression;
P0017/P0018 add literals and identifiers. Source audit still finds unconditional
block release of string bindings and plain C assignment elsewhere. For-header
declarations, call/member initializers, reassignment, array/pointer-local cleanup,
return transfer, early exits and field ownership
remain unqualified; do not infer these from indexed-copy success. The memory
guide's old pools/cycle/alias examples are design material, not implemented APIs.

Acceptance: choose one bounded ownership boundary, reproduce it with checked
live-allocation evidence, retain meaningful cleanup, qualify ordinary/module
behavior and document transfer/lifetime limits. Start after T010 and native CI.

P0017 reproduces invalid literal cleanup with the checked live registry in both
ordinary/module-entry modes and gives direct scalar literal declarations managed
copies. Empty/escaped/const/parenthesized literals, nested scopes, loop bodies,
free/static/instance functions and top-level storage are covered. Seven copy sites,
eight real releases, ten output lines and unchanged concatenation emission are
checked; existing indexed coverage is preserved. Full suite: 128 passed, 18 skipped,
zero failures/errors; fourteen Go quick groups, five Python tests and Linux
cross-build passed. See [P0017](passes/P0017-literal-string-ownership.md).

P0018 reproduces nested identifier alias cleanup failure with the live registry
in both modes and gives scalar identifier declarations independent managed copies.
Nine identifier-copy sites, ten release sites, twelve nonnull releases and nineteen
output lines per mode cover nested chains, literal/managed parameters, const/grouped
bindings, free/static/instance functions, loops, top-level snapshots and null.
The source remains live after nested cleanup; existing helper checks also prove
copy lifetime after backing release. Full suite: 129 passed, 18 skipped, no
failures/errors; fifteen Go quick groups, five Python tests and Linux cross-build
passed. See [P0018](passes/P0018-identifier-string-ownership.md).

Broader T011 remains open. Call/member/for-header initializers, reassignment,
return/field ownership and early-exit cleanup are unqualified. Verify P0018
delivery/native CI first; next reproduce scalar member-expression initializer
cleanup from literal/managed fields and qualify source/copy lifetimes in both modes.

## Later work

- Extend T005 to pointer/array and other method argument shapes later; analyze
  class field declaration initializers. P0006 covers receiver/access diagnostics only.
- Extend constructor argument shapes beyond T006's scalar boundary only after
  qualification. P0013 preserves primitive pointer signatures; shape checking and
  indexed formatting and broader ownership above remain open.

- Prove required returns on all control-flow paths; extend pointer/array return shapes
  and general operator operand validation beyond the primitive scalar checks in P0005.
- Honor instance field declaration initializers. P0005's initial method fixture printed
  `(null)` for `string label = "label"`; constructor assignment works and is used by
  the final return regression. This existing limitation is outside T001.

- Broaden module exports beyond functions; design namespaces and type ownership first.
- Repair or retire the unsupported legacy Go/cgo suites with replacement coverage.
- Virtual dispatch, access controls, and implicit base construction.
- Async/concurrency and graphics remain experimental, outside the supported baseline.
