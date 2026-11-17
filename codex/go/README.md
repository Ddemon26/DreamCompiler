# DreamCompiler Go Test Suite

## Current compiler stability checks

From this directory, run `make quick-validate` on Linux or
`powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 quick`
on Windows. These commands build with Zig 0.17.1 and run the CLI regressions for
error exits, parser recovery, warnings, stable AST allocation, modules, primitive method/constructor arguments,
literal emission/failures, and loop execution at the
default optimization setting and `-O1`/`-O2`/`-O3`. The loop fixture must print
`112` and `8` at each setting; this does not prove optimized IR reaches emitted C.
They require no cgo library or downloaded Go dependencies. Use `-count=1` when
running these Go tests directly:
Go's cache does not track changes to the external compiler executable.

`TestCompiler_Dogfood` checks the production Dream source normalizer and the
function-only `--emit-library-c` mode. Separately linked C checks 97,912 inputs
against the C bootstrap seed and independent expected output, buffer boundaries,
repeated calls and forward function linkage. It also checks bootstrap/production
emission equality, invalid-source/output handling, preservation of existing output,
and production LF/CRLF/CRCRLF entry sources. Windows cross-links the harness for
Linux; native Linux execution is checked by CI. See the
[dogfooding contract](../../docs/development/compiler-dogfooding.md).

`TestCompiler_MethodArguments` checks all primitive type mismatches plus receiver
resolution, arity, nested expressions, and artifact-free diagnostics in ordinary
and module-entry sources. Valid calls require exact runtime output. Pointer/array
shapes remain outside that scalar contract.

`TestCompiler_ConstructorArguments` checks exact primitive types, arity, nested
expressions, and artifact-free diagnostics in ordinary and module-entry sources,
plus exact runtime output. It follows the emitter's local instance `init` rule:
inherited constructors are explicit, and classes without local `init` and structs
take zero arguments. Pointer/array shapes and custom types remain unqualified.

`TestCompiler_ParserRecovery` bounds each malformed-input compilation to ten
seconds in ordinary, module-entry and dependency-loader paths. It checks custom
parameters, malformed lists, EOF and stray expression delimiters, useful positions,
continued diagnostics after invalid methods and absence of generated artifacts.
It does not implement custom-type or array parameters.

`TestCompiler_PointerParameters` checks C signatures before host compilation and
exact runtime output in ordinary/module-entry sources. It covers all five primitive
pointer parameter types, local/base construction, static/inherited/instance calls,
`this`/`base` forwarding, array mutation, an alias and forward free-function calls.
It does not qualify pointer shape checks or ownership. See the documented
[pointer limits](../../docs/language-features/functions.md#primitive-pointer-parameter-emission).

`TestCompiler_IndexedStrings` checks indexed scalar string initialization in
ordinary/module-entry programs: generated copy/release calls, exact linked output,
and a harness that validates cleanup against the real allocator's live registry.
It covers nested blocks, literal/dynamic backing, null and once-only index effects.
The helper also proves independent lifetime after backing storage is freed. Its
C11 harness build uses the runtime's `_GNU_SOURCE` feature macro so Linux exposes
the `strdup` declaration used by exception support. On Windows both modes also
cross-link the harness for `x86_64-linux-gnu`; those binaries are not executed.
Both quick protocols and CI select it. See the bounded
[ownership contract](../../docs/development/string-ownership.md).

`TestCompiler_LiteralStrings` reuses the same checked allocator and linking paths
for scalar literal declarations: seven emitted copy sites, eight local releases,
ten exact output lines, empty/escaped/const literals, nested blocks, loop bodies,
free/static/instance functions and top-level storage. Concatenation initialization
keeps its existing allocation without an extra copy. Both quick protocols and CI
select it. See the call-initializer boundary below; broader ownership remains open.

`TestCompiler_IdentifierStrings` adds scalar identifier initializers through the
same ordinary/module-entry harness and direct linker: nine identifier-copy sites,
ten release sites, twelve nonnull runtime releases and nineteen output lines.
It covers borrowed literal/managed parameters, nested alias chains, const/grouped
bindings, free/static/instance functions, loop bodies, top-level snapshots and null.
Sources remain live after nested copies are cleaned up. Both quick protocols and
CI select it. Assignment and return transfer remain
outside this boundary.

`TestCompiler_MemberStrings` checks scalar member initializers through the same
ordinary/module-entry harness and direct linker: ten member-copy sites, thirteen
release sites, sixteen nonnull releases and twenty-seven output lines. Literal,
managed and null fields, inherited `this`/`base`, static fields, nested/const/grouped
bindings, loops, top-level snapshots and once-only temporary receiver construction
are covered. Sources remain live after nested cleanup; snapshots survive field
replacement. Both quick protocols and CI select it. Field ownership,
assignment and return transfer remain unqualified.

`TestCompiler_ForStrings` checks supported primitive for-header declaration scope
and string ownership: nine copy sites, unchanged concatenation, ten loop-exit
release sites, nine nonnull releases and thirteen exact output lines in both modes.
Zero/multiple iterations, null, borrowed sources, member/index expressions,
once-only indexing, break/continue, condition/update visibility, later name reuse
and top-level cleanup are included. Four driver diagnostic cases require useful
positions and no artifacts for string/integer header uses after a loop. Both quick
protocols and CI select this group. Return/exception exits remain unqualified;
`TestCompiler_LoopExitStrings` below qualifies owned scalar body locals.

`TestCompiler_CallStrings` checks fifteen call-copy sites, fourteen block/header release sites,
fifteen nonnull releases and nineteen output lines in each mode (P0022 also releases
the replaced top-level backing). Literal, empty,
borrowed, fresh managed, local-returned and null results, nested/const/grouped
declarations, static/instance/base methods, loops and top-level snapshots are
covered. The call runs once; cleanup preserves borrowed backing. Both quick
protocols and CI select it. Calls copy without transferring ownership; original
managed results may stay registered until shutdown. Return/early-exit cleanup and
general ownership remain unqualified.

`TestCompiler_AssignedStrings` checks plain `=` on qualified owned scalar
bindings in both modes: thirty-nine nonnull checked releases and thirty-three
exact output lines. Generated-C checks require copying before old-value release,
assignment result values, null defaults and unchanged borrowed parameter/field/index
destinations. Literal/managed/borrowed/member/index/call sources, self/nested/chained
assignment, empty/null values, once-only effects, nested source lifetime, free/static/
instance methods, loops, header updates and top-level replacement are covered.
Both quick protocols and CI select it. Compound assignment, ownership transfer,
parameter/field/index destination ownership and early-exit cleanup remain open.

`TestCompiler_StringContainers` checks normal block cleanup of string arrays and
borrowed `string*` locals in four array/pointer ordinary/module-entry modes. The
real checked allocator requires eight and seven nonnull releases, respectively,
and thirteen/eleven exact output lines. Generated-C assertions forbid releases of
containers and aliases while requiring owned scalar and class-instance cleanup.
Nested/unused storage, literal/managed/null elements, alias chains and rebinding,
null pointers, loop bodies and static/instance methods are covered. Managed sources
survive container cleanup and element replacement. All four modes failed the live
registry assertion before the fix; existing allocator assertions remain. Both
quick protocols and CI select this group. Owning containers, element destruction,
dangling-alias checks and return/exception exits remain unqualified.

`TestCompiler_LoopExitStrings` requires thirty-seven nonnull checked releases and
sixteen exact output lines in ordinary/module-entry modes. Before the fix both
modes failed the release total, exposing locals retained until shutdown. Exact
generated-C cleanup sequences check reverse order and target boundaries before
each break/continue, excluding later declarations, surviving outer bindings,
borrowed containers/parameters and for headers. While/do/for, expression for
initializers, unbraced conditional jumps, nested loop/switch targets, null/empty
strings, header replacement updates, free/static/instance paths and top-level
loops are covered. The prior allocator/refcount/shutdown assertions and ownership
matrices remain intact. Both quick protocols and CI select this group. Unsupported
string initializers and return/exception/finally
unwinding remain unqualified.

`TestCompiler_LoopExitClasses` extends jump cleanup evidence to independent class
locals: 27 class and six string releases, thirteen output lines and an empty live
registry before shutdown in both modes. Exact C sequences check mixed reverse
order, nested loop/switch targets, later declarations and surviving outer/header
storage. While/do/for, normal exits, methods and top-level loops are covered.
Both modes failed pre-shutdown reclamation before P0031. Class aliases, pointers,
arrays, destructors/field ownership and return/exception exits remain unqualified.
Both quick protocols and CI select this group.

`TestCompiler_ConditionalStrings` qualifies scalar `?:` declaration copying in
ordinary/module-entry modes: nineteen emitted copy sites, twenty-two nonnull
checked releases and twenty-four exact output lines, selected-branch effects,
source/copy lifetime, typed null values, replacement and loop-exit cleanup.
Both quick protocols and CI select it. Literal null/string branch typing and
result ownership transfer remain outside this boundary; see the
[ownership contract](../../docs/development/string-ownership.md).

`TestCompiler_AssignmentInitializers` qualifies plain `=` scalar declaration
initializers in both modes: seventeen emitted copy sites, thirty nonnull checked
releases and twenty-three exact output lines. Source/copy replacement, self/nested
assignment, borrowed destinations, null/empty results, managed operands, methods,
top-level snapshots and loop/header/jump cleanup are covered. Call/index effects
run once. Both modes failed live-registry cleanup before P0032; prior allocator
assertions remain unchanged. Both quick protocols and CI select this group.
Other compound operators and return ownership remain unqualified.

`TestCompiler_AppendedStrings` qualifies owned scalar string `+=` in both modes:
49 nonnull checked releases, 26 exact output lines, generated helper/copy/jump
sequences, self/nested updates, null as empty text, once-only call/index effects,
borrowed/member/managed sources, loop headers, methods and top-level snapshots.
Ten positioned, artifact-free cases reject const updates and int/float/bool/char
operands. Before P0033, both modes failed harness C compilation on pointer `+=`.
Both quick protocols and CI select it; temporary/result ownership is unchanged.

`TestCompiler_ScalarReturnCleanup` checks synchronous primitive scalar returns
in both modes: 47 nonnull releases (21 classes, 26 strings), 23 output lines,
evaluation-before-cleanup sequences and an empty registry before shutdown.
Nested locals, string headers, unbraced conditionals, loops/switches, recursion,
methods, all four result types, field/index reads, once-only effects, borrowed
caller survival, temporary-name collisions and normal exception completion are
covered. Active exception exits and the unused owned-string result have emission-only
checks (the latter updated for P0037's direct-local preservation). Both quick
protocols and CI select it; result consumption and exception unwinding remain
separate. See the [ownership contract](../../docs/development/string-ownership.md).

`TestCompiler_LocalStringReturn` checks direct known-owned string-local returns in
both modes: 45 nonnull releases (20 classes, 25 strings), 21 output lines and
exactly 17 returned allocations live before shutdown, each with its original
reference. Before P0037, 44 allocations remained. Exact C checks preserve the
returned binding and reverse-order cleanup of the other locals. Nested/const/null/
empty results, loops/switches, headers, unbraced paths, append/replacement, recursion,
methods, caller survival, name collisions and normal exception completion are
covered. Borrowed/compound results and active exception exits have emission controls only;
the unused direct-literal control now checks P0038 cleanup emission.
No result copy/consumption is introduced; shutdown still reclaims every retained
result. Both quick protocols and CI select this group.

`TestCompiler_LiteralStringReturn` checks direct string-literal returns in both
modes: 51 nonnull releases (18 classes, 33 strings), 19 output lines and an empty
registry before shutdown. Both modes failed that lifetime assertion before P0038.
Mixed/nested locals, null/const/empty storage, later declarations, unbraced paths,
loops/switches, headers/updates, zero/multiple iterations, recursion, methods,
temporary-name collisions and normal exception completion are covered. Results
include parenthesized, empty and escaped literals; caller backing/receiver/snapshot
survive. Exact C checks require reverse cleanup order and unchanged borrowed and
active-exception emission; the unused all-literal conditional now checks P0039
cleanup. No literal result allocation or retain is added. Both quick protocols
and CI select the group.

`TestCompiler_ConditionalLiteralReturn` checks nested literal-only conditional
returns in both modes: 55 nonnull releases (18 classes, 37 strings), 34 output lines
and an empty registry before shutdown. Both modes failed that assertion before
P0039. All four value leaves execute, with labels and counters checking selected-only,
once-only condition evaluation before cleanup of local strings/classes and headers.
Unbraced paths, later declarations, null/const/empty locals, loop updates, methods,
temporary collisions, caller survival and normal exception completion are covered.
Exact C checks preserve unsupported leaves in either recursive branch and active
exception exits. No returned copy or retain is added. Both quick protocols and CI
select the group; earlier allocator/refcount/shutdown assertions remain intact.

`TestCompiler_ConditionalLocalReturn` checks literal/owned-local conditional trees
in both modes: 64 nonnull releases (18 classes, 46 strings), 36 output lines and
exactly 14 selected allocations with their original reference before shutdown.
The initial fixture reproduced 57 live allocations instead of 12 before P0040.
Exact C sequences require once-only selection before reverse cleanup; candidate
pointer comparisons preserve only the selected allocation. Nested/repeated leaves,
null/const/empty slots, equal-content independent allocations, append in a condition,
headers/updates, unbraced paths, methods, name collisions and caller snapshots are
covered. Unsupported/rebound leaves and active exception returns keep their
emission; P0042 qualifies unchanged borrowed scalar parameter leaves. P0037/P0039
unused owned-conditional controls check the new cleanup with unchanged runtime
assertions. Both quick protocols and CI select this group.

`TestCompiler_BorrowedParameterReturn` checks direct unchanged scalar string
parameter returns in both modes: 58 nonnull releases (20 classes, 38 strings),
17 output lines, and an empty registry before/after shutdown. Caller backing and
snapshots survive; no result copy/retain is added. Nested/header/switch scopes,
unbraced paths, recursion, methods, temporary collisions, null/empty storage and
normal exception completion execute. Exact C checks require reverse cleanup and
preserve rebound/active-exception emission; P0042 qualifies conditional leaves.
Twenty-four additional cases check twelve placements of parameter rebinding in both modes, including
later statements and loop updates. Both quick protocols and CI select this group.
P0037/P0038 unused borrowed controls now check qualified cleanup; their runtime
counts remain unchanged. Rebound/compound results and alias lifetime stay separate.

`TestCompiler_ConditionalBorrowedReturn` checks nested literal/owned-local/
unchanged-parameter trees in both modes: 75 checked releases (19 classes, 56
strings), 39 output lines and exactly five retained owned results. Before P0042,
both modes left 62 live allocations instead. Exact C requires capture before
reverse cleanup, owned-candidate comparisons and no borrowed copy/retain/release.
All nested paths, selected-only/once-only conditions, repeated/null/empty leaves,
headers/updates, methods, collisions, local replacement in a condition, completed
exceptions and caller backing/snapshot survival execute. Unsupported/rebound and
active-exception returns retain their emission. Forty-eight additional cases
check twelve rebinding placements in either nested branch in both modes, sharing
the direct-parameter helper without weakening its assertions. Earlier unused
conditional controls require the new emission with unchanged runtime assertions.
Both quick protocols and CI select the group.

`TestCompiler_IndexedConsole` checks generated formats before linking and exact
output in ordinary/module-entry programs. All five primitive array/pointer element
types, local arrays, numeric aliases, free functions/static methods, inherited member
arrays, scalar string fields/parameters/locals/literals/calls and nested string
indexing are covered. A side-effecting index must run once; Write and WriteLine
share the element format. Both quick protocols and CI select it. This qualifies
backend format selection, not general shape, bounds, string ownership or operator typing.

The legacy `minimal_tests` and `temp_disabled` directories still import cgo
directly in test files and cannot be run with `go test ./...`. The older general
integration cases also contain unsupported syntax and incomplete fixtures; they
are not part of the stability checks.

Install Go and Zig 0.17.1 on PATH outside the checkout. The launchers now expose
only the supported quick checks and help. `make` defaults to `quick-validate`;
`build_and_test.ps1` defaults to `quick`. Run `make help` or
`powershell -NoProfile -ExecutionPolicy Bypass -File ./build_and_test.ps1 help`
for usage. The PowerShell launcher resolves build/test directories from its own
location, so calling it from another working directory selects the same checks.

The launchers preserve build and test failure exit codes and do not install Go
modules, build cgo libraries, clean caches or advertise unsupported benchmark/fuzz
coverage. Retired actions/targets fail instead of falling through to recursive
tests. The two obsolete standalone library builders have been removed. Legacy
cgo source fixtures, disabled benchmark/fuzz files and their checksums remain for
a separate replacement-coverage audit. See the
[testing guide](../TEST_GUIDE.md) for the complete validation procedure.

### Proven owned factory initializers (P0050)

`TestCompiler_OwnedCallInitializer` checks direct scalar string declaration
adoption from a deliberately narrow actual-definition proof. Ordinary/module and
imported forward definitions run with original-pointer/refcount checks: 10,000
scoped declarations keep only one live result and leave none after each call.
A separate string-return probe transfers that original to the C harness. The
main fixture makes 16 allocations/15 scoped releases in ordinary mode, 17/16 in
module mode (one independent imported borrowed copy); only the program-lifetime
global remains until shutdown. Same-spelled imported borrowed and entry-owned
factories prove selection by lowered definition. Exact linked/checked output,
Linux harness cross-linking, emission-only unsafe fallbacks and artifact-free
semantic rejections run through the supported quick/CI selector. See the
[contract](../../docs/development/owned-call-result-consumption.md).

The manual `python codex/python/owned_result_audit.py` probes now supersede only
P0049's eligible owned-initializer live count (five to zero per mode). Borrowed,
mixed and other-consumer counts remain unchanged. Prior P0037-P0048 lifetime
fixtures/assertions are unchanged; their more complex functions fail the narrow
factory proof. Cross-linking is distinct from native Linux execution in CI.
