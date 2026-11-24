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

## T011 - Qualify remaining string ownership paths (initializer/header/assignment/container/jump/conditional/class-jump/assignment-initializer/append slices completed)

P0014 qualifies scalar declarations initialized directly from an index expression;
P0017/P0018/P0019 add literals, identifiers and members; P0020 scopes for-header
declarations and qualifies their loop-exit cleanup; P0021 copies direct call results
without transferring ownership; P0022 qualifies plain replacement on known-owned
scalar identifiers; P0023 excludes borrowed array/pointer bindings from scalar
block cleanup; P0024 qualifies owned scalar cleanup on break/continue; P0030
adds scalar conditional initializer copies; P0031 adds class-local jump cleanup;
P0032 adds plain assignment-expression initializer copies; P0033 qualifies owned
scalar string `+=`; P0034 qualifies owned scalar cleanup on bare synchronous
void return; P0035 adds scalar class locals; P0036 qualifies primitive scalar
returns. Source audit still finds plain C assignment elsewhere. Other compound operators, owning containers, return transfer, other early exits and
field ownership remain unqualified; do not infer these from indexed-copy success. The memory
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

P0019 reproduces invalid literal field cleanup in both modes and copies direct
scalar member initializers, including inherited `this`/`base` and static fields.
Ten member-copy sites, thirteen release sites, sixteen nonnull releases and
twenty-seven output lines per mode cover literal/managed/null sources, nested
cleanup, const/grouped declarations, loops, top-level snapshots and once-only
temporary receiver construction. Sources survive nested cleanup; a snapshot
survives field replacement. Full suite: 130 passed, 18 skipped, zero failures/
errors; sixteen Go quick groups, five Python tests and Linux cross-build passed.
See [P0019](passes/P0019-member-string-ownership.md).

P0020 scopes for-header bindings in semantic analysis and C emission, reuses normal
declaration copying, and releases each header after its loop. Nine copy sites, ten
release sites, nine nonnull checked releases and thirteen output lines per mode
cover zero/multiple iterations, borrowed/member/index/null sources, once-only
indexing, header visibility in condition/update, break/continue, name reuse and
top-level cleanup. Four positioned/artifact-free cases reject post-loop uses.
Full suite: 131 passed, 18 skipped, zero failures/errors; seventeen Go quick groups,
five Python tests and Linux cross-build passed. See
[P0020](passes/P0020-for-header-string-ownership.md).

P0021 reproduces invalid call-result cleanup in both modes and gives direct scalar
call initializers independent managed copies. Fifteen copy sites, fourteen release
sites, fourteen nonnull releases and nineteen output lines cover literal/empty/
borrowed/fresh/local-returned/null results, static/instance/base calls, nested/
const/grouped declarations, loops and top-level snapshots. Calls evaluate once;
borrowed backing survives cleanup. Full suite: 132 passed, 18 skipped, zero
failures/errors; eighteen Go quick groups, five Python tests and Linux cross-build
passed. See [P0021](passes/P0021-call-string-ownership.md). Original managed results
can remain registered until shutdown; return transfer is not implemented.

P0022 copies plain `=` replacements before releasing a known-owned scalar
identifier's old value, preserving self/chained/nested assignment and expression
results. No-initializer scalar strings now start as null. Thirty-nine nonnull
checked releases and thirty-three output lines in both modes cover literal,
borrowed/managed/member/index/call replacement, source/copy lifetime, null/empty,
once-only effects, static/instance paths, loops, header updates and top-level
replacement. The call fixture's release count increases to fifteen to require
old top-level reclamation. Full suite: 133 passed, 18 skipped, no failures/errors;
nineteen Go quick groups, five Python tests and Linux cross-build passed. See
[P0022](passes/P0022-assigned-string-ownership.md). Replacement temporaries can
remain registered until shutdown; parameter/field/index destinations, other
initializers, compound assignment and return transfer remain unqualified.

P0023 reproduces invalid container/alias cleanup in all four array/pointer
ordinary/module-entry modes. `emit_scope_cleanup` now uses existing indirection
metadata to release string scalars, preserving class-instance cleanup. Local
arrays and explicit string pointers borrow their elements/targets without
automatic element destruction. The two annotated fixtures require eight/seven
nonnull checked releases and thirteen/eleven exact output lines, with forbidden
container/alias release assertions and preserved owned-scalar releases. Nested
and unused storage, alias rebinding, null, loop bodies and static/instance methods
are covered. Full suite: 135 passed, 18 skipped, no failures/errors; twenty Go quick
groups, five Python tests and Linux cross-build passed. See
[P0023](passes/P0023-borrowed-string-containers.md).

P0024 reproduces missing body-local releases in both modes, then tracks loop/switch
target binding boundaries to release exited owned scalar strings in reverse order
before break/continue. Headers survive continue/update and receive their existing
loop-exit cleanup. Exact C sequences, thirty-seven nonnull checked releases and
sixteen output lines cover while/do/for, nested loop/switch targets, switch normal
completion, null/empty locals, borrowed containers, later declarations, unbraced
conditionals, header replacement updates, methods and top-level loops. All prior
allocator/refcount/shutdown assertions remain. Full suite: 136 passed, 18 skipped,
no failures/errors; twenty-one Go quick groups, five Python tests and Linux
cross-build passed. See [P0024](passes/P0024-loop-exit-strings.md).

P0030 reproduces invalid conditional-result cleanup in ordinary/module-entry modes
and copies the selected result of scalar `?:` declaration initializers. Nineteen
emitted copy sites, twenty-two nonnull checked releases and twenty-four output
lines cover literal/empty/borrowed/managed/typed-null branches, nested/const forms,
once-only condition/branch/index evaluation, source replacement and independent
snapshots, scalar replacement, header cleanup, break/continue and method/top-level
paths. Full Windows suite: 137 passed, 18 skipped, no failures/errors; twenty-three
Go quick groups, five runner tests and Linux cross-build passed. See
[P0030](passes/P0030-conditional-string-ownership.md). Fresh selected managed
results can remain registered until shutdown. Literal null/string branch typing
is still rejected; return transfer and general expression ownership remain open.

P0031 reproduces retained class locals before shutdown in both modes, then applies
normal class-local cleanup to the existing break/continue scope boundaries.
Exact C sequences, 27 class and six string releases, thirteen output lines and
an empty pre-shutdown registry cover mixed scopes, later declarations, while/do/for,
nested loop/switch targets, surviving outer instances/string headers, normal exits,
methods and top-level loops. Full Windows suite: 138 passed, 18 skipped, no failures/
errors; 24 Go quick groups, five runner tests and Linux cross-build passed. See
[P0031](passes/P0031-loop-exit-classes.md). Class alias/container ownership,
destructors/field destruction and return/exception exits remain unqualified.

P0032 reproduces invalid assignment-result cleanup in both modes and copies the
result of plain `=` scalar string declaration initializers. Seventeen emitted
copy sites, thirty nonnull checked releases and twenty-three exact output lines
cover self/nested assignment, source/copy replacement, borrowed parameter/field/
index destinations, null/empty values, managed operands, const bindings,
once-only call/index effects, methods, top-level snapshots and loop/header/jump
cleanup. Full Windows suite: 139 passed, 18 skipped, no failures/errors;
25 Go quick groups, five runner tests and Linux cross-build passed. See
[P0032](passes/P0032-assignment-initializer-ownership.md). Destination ownership
is unchanged; fresh managed operands can remain registered until shutdown.

P0033 replaces invalid C pointer `+=` with an owned-scalar string/null append
helper. Both modes require 49 checked releases, 26 exact output lines, self/nested
updates, independently copied results, once-only effects, null-as-empty policy,
borrowed/member/fresh operands, header/jump cleanup, methods and top-level snapshots.
Ten positioned, artifact-free cases reject const updates and non-string operands.
Numeric compound operators retain their emission; managed operands retain ownership.
Full Windows suite: 142 passed, 18 skipped, zero failures/errors; all 26 Go quick
groups, five runner tests and Linux cross-build passed. See
[P0033](passes/P0033-owned-string-append.md) for exact validation and limits.

P0034 reproduces retained scoped allocations on bare void return in both modes,
then releases qualified owned scalar strings in reverse declaration order across
active local/header scopes. Each mode checks 40 nonnull releases (39 strings and
one normal class release), 13 output lines and an empty registry before shutdown.
Nested/const/null/empty locals, later declarations, true/false unbraced paths,
while/do/for/switch returns, header updates, zero/multiple iterations, recursion,
methods and borrowed caller survival are covered. Active try/catch/finally returns
retain their emission and are not executed/qualified; return after a completed
context executes. Full Windows suite: 143 passed, 18 skipped, zero failures/errors;
27 Go quick groups, five runner tests and Linux cross-build passed. See
[P0034](passes/P0034-void-return-strings.md).

P0035 reproduces retained class locals on bare void return in both modes, then
adds scalar class releases to the synchronous void-return path. Parameter-scope
bindings, including borrowed `this`, and class pointers/arrays are excluded.
Each mode requires 40 nonnull releases (30 class instances, ten strings), ten
output lines, reverse-order C sequences and an empty pre-shutdown registry.
Mixed/nested scopes, later declarations, unbraced true/false returns, loops,
switches, string headers, zero/multiple iterations, recursion, methods, constructor
return and normal exception completion are covered. Active exception returns have
emission checks only. Full Windows: 144 passed, 18 skipped, no failures/errors;
28 quick groups, five runner tests and Linux cross-build passed. See
[P0035](passes/P0035-void-return-classes.md).

P0036 reproduces retained scoped allocations on primitive value returns in both
modes, then evaluates synchronous int/float/bool/char results once into typed
temporaries before reverse-order string/scalar-class cleanup. Each mode requires
47 nonnull releases (21 classes, 26 strings), 23 output lines, exact evaluation/
cleanup sequences and an empty pre-shutdown registry. Nested scopes, headers,
loops/switches, recursion, methods, field/index reads, effects, caller survival,
temporary-name collisions and normal exception completion are covered. Active
exception exits and owning string results have unchanged-emission controls only.
Full Windows: 145 passed, 18 skipped, no failures/errors; 29 quick groups, five
runner tests and Linux cross-build passed. See [P0036](passes/P0036-scalar-return-cleanup.md).

P0037 reproduces 44 live allocations before shutdown in both modes, then preserves
the original allocation/reference for a direct known-owned scalar string-local
result while releasing other qualified string/class locals in reverse order.
Each mode requires 45 nonnull releases (20 classes, 25 strings), 21 output lines
and exactly 17 returned allocations before shutdown, then an empty registry.
Nested scopes, const/null/empty results, loops/switches, headers, unbraced paths,
append/replacement, recursion, methods, name collisions, caller survival and normal
exception completion are covered. No result copy/consumption is added; other string
result forms and active exception exits retain their emission. Full Windows:
146 passed, 18 skipped, no failures/errors; 30 quick groups, five runner tests and
Linux cross-build passed. See [P0037](passes/P0037-local-string-return.md).

P0038 reproduces retained scoped allocations before shutdown on direct literal
string returns in both modes, then releases all qualified string/class locals
before returning the static pointer. No result allocation or retain is added.
Each mode requires 51 nonnull releases (18 classes, 33 strings), 19 output lines,
exact C sequences and an empty pre-shutdown registry. Parenthesized/empty/escaped
literals, nested/header scopes, unbraced paths, loops/switches, header updates,
zero/multiple iterations, recursion, methods, collisions, caller survival and normal
exception completion execute; borrowed/compound/active-exception controls remain
emission-only. Full Windows: 147 passed, 18 skipped, zero failures/errors;
31 Go quick groups, five runner tests and Linux cross-build passed. See
[P0038](passes/P0038-literal-string-return.md).

P0039 reproduces retained scoped allocations on conditional string returns in both
modes, then qualifies nested `?:` results only when every value leaf is a literal.
Reached conditions execute once before reverse cleanup; nonliteral leaves preserve
the entire return's prior emission. Each mode checks 55 nonnull releases (18 classes,
37 strings), 34 output lines and an empty pre-shutdown registry. All four nested
leaves, selected-only effects, owned local/field/header reads, unbraced paths,
null/const/empty locals, later declarations, loop updates, methods, collisions,
caller survival and normal exception completion are covered. Full Windows: 148
passed, 18 skipped, zero failures/errors; 32 Go quick groups, five runner tests and
Linux cross-build passed. See
[P0039](passes/P0039-conditional-literal-return.md) for validation and limits.

P0040 reproduces 57 live allocations instead of 12 on conditional owned-local
returns in both modes. It qualifies literal/known-owned-local value trees, captures
once, then compares candidate slot pointers during reverse cleanup to preserve
only the selected allocation. The strengthened matrix checks 64 nonnull releases
(18 classes, 46 strings), 36 output lines, exactly 14 selected allocations with
original references before shutdown, and an empty registry afterward. Nested/
repeated/null/const/empty leaves, equal-content independent slots, append in a
condition, header updates, unbraced paths, methods, collisions, normal exception
completion and caller survival execute. Exact C preserves unsupported leaves and
active exception exits. Full Windows: 149 passed, 18 skipped, zero failures/errors;
33 quick groups, five runner tests and Linux cross-build passed. See
[P0040](passes/P0040-conditional-local-return.md).

P0041 reproduces retained scoped allocations on direct borrowed string-parameter
returns in both modes. A whole-body unchanged-parameter proof qualifies capture
before reverse cleanup without a result copy/retain. Each mode requires 58 nonnull
releases (20 classes, 38 strings), 17 output lines and an empty registry before/
after shutdown, with caller backing/snapshot survival. Twenty-four additional
cases preserve prior returns for twelve placements of rebinding, including later
statements and loop updates. Full Windows: 150 passed, 18 skipped, zero failures/
errors; 34 quick groups, five runner tests and Linux cross-build passed. See
[P0041](passes/P0041-borrowed-parameter-return.md).

P0042 extends conditional result cleanup to unchanged borrowed scalar parameter
leaves using P0041's whole-body proof. Both modes reproduced 62 live allocations
instead of five; each now requires 75 checked releases (19 classes, 56 strings),
39 output lines, five selected owned results with original references and an empty
registry after shutdown. Forty-eight additional cases check twelve rebinding
placements nested on either side in both modes; earlier runtime and direct-return
assertions remain intact. See [P0042](passes/P0042-conditional-borrowed-return.md)
and the current test snapshot for measured full/quick/cross results.

P0043 completes the requested direct borrowed member-result source audit and
[implementation contract](../docs/development/member-return-cleanup.md).
Receiver-slot immutability does not prove field backing: aliased writes/calls and
unretained class alias cleanup require separate barriers. The first slice selects
direct scalar instance fields on borrowed class parameters/explicit this with a
whole-body read-only proof; every write/call/new, custom-type/pointer/array local,
exception/async, shadowing or unknown node preserves previous emission. No compiler
behavior changed and no compiler tests were rerun; reuse dated P0042 evidence.

P0044 implements direct scalar nonstatic `this.field` string-result cleanup under
that read-only proof, preserving inherited owner casts and the existing parameter
proof. Each checked mode requires 38 releases, 21 output lines, empty pre/post-
shutdown registries and caller backing/snapshot survival; 196 emission-only controls
preserve mutations/calls/aliases/shapes/exception/async fallback. Full Windows suite:
152 passed, 18 skipped, no failures/errors; all 36 quick groups, five runner tests
and Linux cross-build 15/15 passed. See [P0044](passes/P0044-borrowed-member-return.md).
P0043's conceptual explicit class-parameter cases remain rejected by grammar, as do
async instance methods; semantic receiver shadowing is also rejected. No syntax
was broadened. Valid controls use explicit this and previously declared alias types.

P0045 qualifies conditional result trees with those proven `this.field` leaves,
mixing literals, known-owned scalar locals and unchanged scalar parameters. The
read-only proof and every write/call/alias barrier remain unchanged. An initial
both-mode matrix retained 50 allocations instead of four before the fix. Final
coverage adds a two-field inherited reader; each mode checks 59 releases, 27 output
lines and four selected owned results until shutdown. The shared 196 emission
controls plus fourteen expression rejections and two nested-else positives pass.
See [P0045](passes/P0045-conditional-member-return.md) and the current results.

P0046 audits static string-field backing and defines a direct class type-qualified
[implementation contract](../docs/development/static-member-return-cleanup.md).
Global slot lifetime does not establish pointee lifetime. Require an unshadowed
class qualifier, actual static scalar declaration/owner and the existing whole-body
read-only barriers; capture once before reverse cleanup without result copy/retain
or global release. Both-mode lifetime/C/output and unsafe emission-only acceptance
are specified. No compiler behavior changed or compiler tests were rerun; reuse
P0045 dated 2026-11-16 UTC. Its exact-parent native Linux/Windows CI passed.

P0047 implements the direct unshadowed class type-qualified static scalar string
result slice under that contract. A separate predicate shares the unchanged
read-only traversal via an explicit guarded name; capture once before reverse
cleanup without result copy/retain, global release or fake receiver bindings.
Both modes first reproduced retained locals and now check 40 releases, 23 output
lines, inherited/hidden owner C emission, caller/snapshot survival and empty
pre/post-shutdown registries. The 252-subtest group includes all write/call/alias/
shape barriers, migrated direct-static positives and twelve artifact-free semantic
rejections. Existing parameter/member/lifetime assertions are preserved. Full
Windows: 154 passed, 18 skipped, zero failures/errors; 38 quick groups, five runner
tests and Linux cross-build 15/15 passed. See [P0047](passes/P0047-static-member-return.md).

P0048 qualifies those conditional static value leaves alongside proven instance,
parameter, owned-local and literal leaves. All whole-body barriers remain intact.
Both pre-fix modes fail the complete test's capture/cleanup C assertion; final
lifetime evidence checks 67 nonnull releases, 35 outputs and four original-reference
owned survivors until shutdown in each mode. All 246 new-group subtests pass;
unsafe body/leaf/shape/guard controls and migrated positive static controls retain
prior allocator/refcount assertions. See
[P0048](passes/P0048-conditional-static-return.md) and the current measured snapshot.

P0049 completes that caller-consumption source audit and
[bounded adoption contract](../docs/development/owned-call-result-consumption.md).
Eight real compiler/checked allocator probes cover ordinary/module-entry modes:
each mode retains five owned initializer originals, zero borrowed control results,
one mixed-return original and five results in other consumers. Generated C,
nonnull release operands/refcounts, exact output and shutdown registries are checked.
Compiler behavior is unchanged. Select only complete scalar string declaration
initializers calling a proven synchronous free-function factory: exactly one
scalar string declaration from literal/null/absent/scalar parameter plus its
terminal direct local return. All other definitions and consumers retain current
behavior. Require actual lowered definitions, no caller shadowing and original
allocation identity evidence; no blanket release, retain or call-type inference.

P0050 implements that exact factory proof and adopts the original allocation for
complete direct scalar string declaration initializers, keeping destination ownership
and every callee return proof intact. Actual lowered definitions, primitive scalar
parameter shapes and caller binding guards are required. Both modes check 10,000
original-pointer releases at peak live count one, string transfer, snapshots,
headers/jumps/returns and program-lifetime top-level storage; all 74 focused
subtests pass. Same-spelled imported borrowed/entry-owned factories take different
paths. The eight audit probes now require zero owned-initializer survivors;
borrowed/mixed/other-consumer counts remain unchanged. No prior ownership fixture,
release count or refcount assertion changed. Full Windows: 156 passes/18 skips,
zero failures/errors; 40 Go quick groups, five runner tests and Linux 15/15 pass.
See [P0050](passes/P0050-owned-call-initializer.md).

P0051 completes the [plain assignment source audit and contract](../docs/development/owned-call-assignment.md).
Only complete assignment statements with identifier owned scalar destinations and
P0050's unchanged factory proof are selected. RHS finishes once before releasing
the current slot; nested argument replacement and self-snapshot require explicit
identity/order evidence. General expression contexts retain current emission.
Compiler behavior is unchanged; P0050 dated 2026-11-17 evidence is reused without
rerunning compiler checks. P0050 native Linux/Windows CI passed.

P0052 implements complete-statement assignment adoption under that proof, using a
fresh statement-local RHS temporary before current-slot release. Both-mode isolated
retention falls from three to zero originals with unchanged four releases; repeated
replacement checks original-pointer release order, refcount one and peak live count
two. Existing definition fallback controls now also assert assignment fallback;
only eligible assignment C/audit expectations migrate. See
[P0052](passes/P0052-owned-call-assignment.md) for full measured checks.

P0053 completes the [discarded factory-call audit and contract](../docs/development/owned-call-discard.md).
Select only complete direct call statements under the unchanged actual-definition
factory proof. Evaluate once and release the original after normal completion;
borrowed/mixed/method/shape/body barriers and other consumers retain current emission.
No compiler behavior changed or compiler checks reran; reuse P0052 dated 2026-11-17.
P0052 exact-commit Linux/Windows CI passed. Isolated pre-fix retention, original
allocation identity/refcount and bounded repeated-call evidence remain required.

P0054 implements the complete-statement discard slice with the same proof. In each
mode the isolated eight-call audit moves from zero releases/eight survivors to eight
releases/zero survivors. The original-pointer harness checks 10,000 repetitions
at peak live count one, null and argument effects, and exact checked/linked output.
The aggregate audit now has four releases and three survivors for append, Console
output and argument consumption. See [P0054](passes/P0054-owned-call-discard.md).

P0055 measures isolated `view(owned())` at zero releases and eight retained
originals per mode. The static-store control shows a synchronous void callee can
let a parameter escape without rebinding it. The sibling control preserves both
C argument expressions and checks the sibling side effect once. The
[argument contract](../docs/development/owned-call-argument.md) selects a single
scalar string parameter and an exact Console-write body. Compiler emission is
unchanged. See [P0055](passes/P0055-owned-call-argument-audit.md).

P0056 implements the complete-statement rule after normal return from the
Console wrapper. Imported definitions are evaluated independently of
same-spelled local functions. Both modes check 10,000 original-pointer
releases with peak live count one and no survivors. The isolated audit is
now eight releases and zero survivors, and the aggregate probe is five
releases and two survivors. Static-field escape and sibling-argument probes
retain their prior measurements. See [P0056](passes/P0056-owned-call-argument.md).

P0057 releases a proven factory result immediately after it is printed by a
complete `Console.Write` or `Console.WriteLine` statement. In each mode, each
writer's isolated audit moves from zero releases/eight survivors to eight
releases/zero survivors. The aggregate reaches six releases and one survivor.
See [P0057](passes/P0057-direct-console-ownership.md).

P0058 releases a proven factory operand after a complete owned scalar string
`+=` statement copies it and installs the joined result. In each mode, the
isolated audit goes from 16 releases and eight survivors to 24 releases and
zero survivors. The aggregate reaches seven releases and zero survivors. See
[P0058](passes/P0058-owned-call-append.md).

P0059 releases a proven factory operand when the append directly initializes
an owned scalar string declaration. The destination keeps the joined storage
and the initializer takes an independent copy. The isolated eight-iteration audit
goes from 32 releases and eight survivors to 40 releases and zero survivors in
each mode. See [P0059](passes/P0059-owned-append-initializer.md).

P0060 releases a proven factory operand when a complete `Console.Write` or
`Console.WriteLine` statement prints the append result directly. The destination
keeps the installed join. In each mode the eight-iteration audit goes from
16 releases and eight survivors to 24 releases and zero survivors. See
[P0060](passes/P0060-owned-append-console.md).

P0061 releases a proven factory operand when the append directly supplies the
sole argument to a synchronous void wrapper that only prints its one scalar
string parameter. The join remains destination-owned. The eight-call audit
moves from 16 releases and eight survivors to 24 releases and zero survivors in
each mode. The two-statement wrapper control retains one original factory
operand. See [P0061](passes/P0061-owned-append-wrapper.md).

P0062 addresses the failed P0061 CI baseline. Ubuntu 26.04 could not install
Python 3.8, and the workflow now uses Python 3.13 on both platforms with the
v6 checkout and Python setup actions. See [P0062](passes/P0062-ci-python-setup.md).

P0063 releases a proven factory operand when a synchronous string function
directly returns an append to a known-owned local. The isolated eight-call audit
moves from 16 releases and 16 survivors to 24 releases and eight survivors in
each mode. The eight joins remain registered after the caller copies them.
See [P0063](passes/P0063-owned-append-return.md).

P0064 measures and fixes the extra copy in `string snapshot = joined();`
when the resolved function declares one qualifying scalar local and returns
an append of a proven factory result to that local. The original join is
released at declaration scope exit in 10,000 checked calls per mode. The
eight-call audit now has zero survivors. See
[P0064](passes/P0064-adopt-returned-joins.md).

P0065 extends the same proof to `target = joined();` as a complete assignment
to an already-owned scalar. It installs the original join after evaluating the
call and releasing the current destination. In each mode, 10,000 scopes check
both a snapshot of the old value and a nested argument replacement. The isolated
eight-call audit retains zero original joins, down from eight, with 32 releases
per mode. See [P0065](passes/P0065-assign-returned-joins.md).

P0066 releases the original join when the entire statement is a call to a
proven join factory. The isolated eight-call audit goes from 16 releases and
eight survivors to 24 releases and zero survivors per mode. Ordinary and
imported calls each pass 10,000 checks of release identity and once-only
argument evaluation. See [P0066](passes/P0066-discard-returned-joins.md).

P0067 releases the proven original join after direct `Console.Write` and
`Console.WriteLine` statements. In both modes, eight isolated calls go from
16 releases and eight surviving joins to 24 releases and zero survivors.
The checked regression verifies original-pointer identity through each writer
and once-only argument evaluation. See [P0067](passes/P0067-print-returned-joins.md).

P0068 releases a proven returned join after a complete call to a proven
one-parameter Console wrapper. Eight isolated calls in each mode go from
16 releases and eight live joins to 24 releases and zero live joins. See
[P0068](passes/P0068-wrap-returned-joins.md).

P0069 releases a proven returned join after a complete `text += joined();`
statement to an owned scalar string. Eight isolated appends per mode go from
32 releases and eight live joins to 40 releases and zero live joins. See
[P0069](passes/P0069-append-returned-joins.md).

P0070 releases a proven returned join in `string snapshot = (text += joined());`.
The destination and snapshot remain independently readable across replacement
and scope exit. The eight-initializer audit in each mode goes from 48 releases
and eight live joins to 56 releases and zero live joins.
See [P0070](passes/P0070-initialize-from-returned-join-appends.md).

P0071 releases the original returned join in a complete
`Console.Write(text += joined());` or `Console.WriteLine(text += joined());`
statement. The destination keeps the printed value. Checked ordinary and
module-entry runs cover replacement during operand evaluation, post-print
updates, and borrowed self-append.
See [P0071](passes/P0071-print-appends-with-returned-joins.md).

P0072 releases a proven returned join in `view(text += joined());` when the
entire statement calls a proven one-parameter Console wrapper. The updated
destination stays readable after the call. A wrapper with two statements keeps
its original operand until shutdown in the checked control.
See [P0072](passes/P0072-wrap-appends-with-returned-joins.md).

P0073 releases the intermediate join in `return (text += joined());` from an
owned local in a synchronous string function. The caller can read the final
value in both modes. In the isolated audit, eight calls release eight more
allocations and leave only their eight final results registered.
See [P0073](passes/P0073-return-appends-with-returned-joins.md).

P0074 lets a direct scalar initializer adopt the final result of two qualifying
returned appends. The eight-call audit leaves zero allocations live instead of
eight, and checked ordinary and imported calls release the original pointer.
See [P0074](passes/P0074-adopt-nested-returned-joins.md).

P0075 applies the same two-append proof to complete assignments to already-owned
scalar strings. The eight-call audit leaves zero allocations live instead of
eight. Checked ordinary and imported calls release the original allocations in
order, including when a nested argument replaces the destination.
See [P0075](passes/P0075-assign-nested-returned-joins.md).

P0076 applies the two-append proof to standalone calls. The eight-call audit
leaves zero final joins live instead of eight. Checked ordinary and imported
calls evaluate the argument once and release the original final pointer.
See [P0076](passes/P0076-discard-nested-returned-joins.md).

P0077 applies the two-append proof to complete direct Console statements.
Both writers read the original final join before its release. The eight-call
audit leaves zero final joins live instead of eight. See
[P0077](passes/P0077-print-nested-returned-joins.md).

P0078 applies the two-append proof to complete calls to proven Console wrappers.
The caller releases the original final join after the wrapper returns. The
eight-call audit covers both writers and leaves zero final joins live per mode.
See [P0078](passes/P0078-wrap-nested-returned-joins.md).

P0079 releases the original final join after a complete `text += nested();`
statement installs the new concatenation. The checked fixtures cover argument
evaluation that replaces the destination before the append reads it.
Eight isolated appends made 56 releases and left zero joins live per mode,
compared with 48 releases and eight live joins on P0078.
See [P0079](passes/P0079-append-nested-returned-joins.md).

P0080 applies the two-append proof in `string snapshot = (text += nested());`.
The destination and snapshot have independent storage. The checked fixtures
exercise replacement during operand evaluation, borrowed self-append, const
snapshots, and `for`-header cleanup. Eight isolated initializers left zero
factory joins live in each mode, down from eight on P0079.
See [P0080](passes/P0080-initialize-from-nested-join-appends.md).

P0081 applies the two-append proof to `Console.WriteLine(text += nested());`
and `Console.Write(text += nested());`. Both writers read the installed
concatenation after the factory operand is released. Each eight-call audit
left zero factory joins live per mode, down from eight on P0080.
See [P0081](passes/P0081-print-appends-with-nested-joins.md).

P0082 applies the two-append proof to `view(text += nested());` with a proven
wrapper for either Console writer. The append releases the original final
join before the wrapper runs, and the destination survives the call. Each
eight-call audit left zero factory joins live per mode, down from eight on
P0081. See [P0082](passes/P0082-wrap-appends-with-nested-joins.md).

P0083 applies the two-append proof to `return (text += nested());` from an
owned local in a synchronous string function. The append releases the operand
and returns the installed concatenation. Each eight-call audit left eight
final results live per mode, down from sixteen total joins on P0082.
See [P0083](passes/P0083-return-appends-with-nested-joins.md).

P0084 lets direct scalar initializers adopt the original join from three
proven returned appends. The allocator checks the release pointer in ordinary
and imported calls. After eight calls per mode, zero final joins remained
live, down from eight on P0083. See
[P0084](passes/P0084-adopt-three-returned-appends.md).

P0085 lets a complete assignment to an owned scalar string adopt the original
third join. The checked release order covers an operand that uses the current
destination and one that replaces it during argument evaluation. Eight calls
per mode leave zero final joins live, down from eight on P0084. See
[P0085](passes/P0085-assign-three-returned-appends.md).

P0086 releases the original third join after a standalone call. Eight calls
per mode made 56 releases and left zero final joins live, compared with 48
releases and eight live joins on P0085. The checked ordinary and imported
calls verify original-pointer identity and once-only argument evaluation.
See [P0086](passes/P0086-discard-three-returned-appends.md).

Broader T011 remains open. Verify P0086 publication and native CI. Then
measure retention when complete direct `Console.Write` and `Console.WriteLine`
statements print a factory result from three proven returned appends. Check
that both writers read the original pointer before any release.
Static pointer/array shape emission, class-parameter syntax and
local/struct/nested/base receivers remain separate.
Keep general borrowed/member/index/call/compound result lifetime and other caller
consumption, pointer/class result transfer, aliases/containers/destructors/fields,
async and exception unwinding separate. String-index return typing remains
limited; custom-type for headers and direct class null initializers remain
unsupported. Fixture probes did not broaden grammar or semantics.
T012/T013 pruning and T014 website/release work are completed maintenance history;
do not repeat those tasks from stale scheduled-run memory.

## T012 - Prune obsolete repository tooling and operational docs (completed in P0026)

Explicit user cleanup priority. P0026 removes eleven unused/superseded files:
root scratch programs, a downloaded Go installer, obsolete runtime/runner scripts,
an unusable legacy validation wrapper and redundant stale setup/status docs.
Operational build, testing, architecture and development guides now describe the
actual supported entry points. README uses the current local-checkout workflow.
The pass record lists each deletion and its evidence.

Acceptance: no active repository references to deleted entry points; unchanged
compiler/build/fixture/plugin/runner inputs and test discovery; working full suite,
Go quick checks and runner regressions; verified documentation paths and publication.
See [P0026](passes/P0026-repository-cleanup.md) for results and retained boundaries.
Legacy cgo suites and their unique test sources, the historical HTML documentation
viewer, and language/API tutorial accuracy need separate audits. Do not purge these
based only on missing references or claim skipped coverage is now qualified.

## T013 - Retire unsupported Go build entry points (completed in P0027)

Explicit follow-up cleanup priority. P0027 removes the two unused standalone C
library builders and reduces the Go Makefile/PowerShell launcher to the supported
quick protocol and help. Default invocations select quick; retired modes fail
instead of launching absent cgo APIs, module installation, benchmarks or fuzzing.
The existing 22-group selectors and unique legacy sources/checksums are preserved.

Acceptance: selector equality with CI, successful real quick/full/runner checks,
controlled build/test failure propagation, directory handling, missing-tool/help/
retired-action probes, parser/dry-run checks and verified publication. See
[P0027](passes/P0027-go-tooling-cleanup.md) for evidence. The docs HTML/CSS were
retained because repository metadata shows an active Pages site from `main:/docs`;
they cannot be treated as unused from a local reference scan. Audit their paths
separately, and qualify replacement coverage before pruning unique cgo fixtures.

## T014 - Modernize the Pages website (completed in P0028/P0029)

The owner approved the Source first homepage, Guide page reader, Workbench palette,
and square corners. P0028 implements them with real Markdown routing, 21 sidebar
targets, heading links, code copying, mobile navigation and themes. Eight Node
checks, all sidebar renderings, browser interaction/overflow/sanitization checks,
two exact-output examples and protected source hashes passed. See
[P0028](passes/P0028-docs-website.md) for evidence and untested limits.

P0029 found PR #231 already merged outside the automation, with successful native
integration/merged-main CI and Pages deployment. The live homepage, stylesheet,
four modules and all 21 sidebar documents returned HTTP 200 and matched P0028's
committed bytes; MIME and query/deep-link shell checks passed. Eight Node checks
also passed. Browser inventory was empty, so deployed rendering/interactions were
not rerun; P0028's dated local browser qualification remains separate. See
[P0029](passes/P0029-docs-release-verification.md). The release gate is resolved;
resume T011 after the latest integration publication/CI verification. Future
`main` merges, changed Pages settings or another hosted copy still require new
authorization. Legacy tutorial/API content and body-link accuracy remain separate.

## Later work

### Compiler dogfooding (explicit user priority, first slice in P0025)

P0025 migrates production entry-file newline normalization to Dream. A C-only
host seed emits function-library C; the target compiler links the Dream helper
without the seed. The repaired build tracks source/output dependencies and keeps
the bootstrap executable runnable on the host during cross-builds. The focused
Go/CI group checks differential bytes, linkage, output handling and both stages.
See the [contract](../docs/development/compiler-dogfooding.md) and
[pass](passes/P0025-compiler-dogfooding.md) for measured validation and limits.

This is one helper, not a self-hosted frontend. Lexer migration needs a selected
token/storage interface and equivalence tests; custom-type parameters, shared
module types, compiler containers/ownership and host I/O remain larger prerequisites.
Do not expand to a lexer/parser rewrite inside this pass. Continue the current bounded T011
ownership task above after the latest pass publication/native CI verification.

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
