# String storage checkpoint

P0014 qualifies one ownership boundary: a scalar `string` declaration whose
initializer is an index expression, for example `string text = values[i]`.
The generated code calls `dream_string_copy` once on the selected element.
This applies to ordinary programs and module entries, including pointer-taking
methods and nested blocks. A top-level declaration also copies the element.

P0017 extends this boundary to scalar declarations initialized directly from a
string literal, including empty/escaped literals, parentheses and `const` bindings.
For example, `string text = "literal"` now calls `dream_string_copy("literal")`.
Literal storage has no managed header; copying allows the existing block release
to remain valid. Loop-body declarations copy and release once per iteration.
This also applies to free functions, static/instance methods and top-level
declarations. Copies add an allocation; concatenation initializers retain their
existing allocation and are not wrapped in another copy.

P0018 also copies scalar identifier initializers, for example `string copy = source`.
The source may be a local, a top-level binding or a borrowed string parameter;
it must designate readable string storage or null. Parentheses and `const` bindings
follow the same rule. Each declaration owns a separate copy, so cleaning up a
nested alias does not release its source. This does not retain an alias reference
or transfer parameter ownership. Reassignment remains separate.

P0019 extends scalar declaration copying to member expressions:
`string copy = object.field`, `this.field`, `base.field`, and static field access. The receiver
is evaluated once; parentheses and `const` bindings use the same path. Fields
may borrow literal, managed or null storage. Copying leaves field/source ownership
unchanged and gives the declaration its own storage for normal cleanup. It does
not implement field initialization, replacement or destruction ownership.

P0020 applies the same declaration path to supported primitive `for` headers.
For example, `for (string text = source; condition; update) { ... }` creates a
loop scope containing the declaration and the C loop, then releases `text` after
the loop. Literal/index/identifier/member initializers copy as above; concatenation
initializers keep their existing managed allocation. Initialization runs once,
even when the condition is initially false. The binding is visible in condition,
body and update, and unavailable afterward; separate loops may reuse its name.
Existing restrictions on shadowing an enclosing binding remain unchanged.

Header storage is released once on normal termination or a break targeting that
loop. Continue preserves it through the update and subsequent iterations. This
also applies at top level, before shutdown. Owning-result and exception exits can
bypass this cleanup; P0036 qualifies primitive scalar returns below. P0024 qualifies
owned scalar body-local cleanup on break/continue as described below.

P0021 extends scalar declaration copying to direct function and method calls:
`string text = getText()` evaluates the call once and copies its readable result.
Literal, borrowed managed, fresh managed and null results use the same rule;
static, instance and base methods, parentheses, const declarations, loop bodies,
for headers and top-level declarations follow this path. Cleanup releases the
declaration's copy, leaving the returned pointer's ownership unchanged. This is
not a return-transfer ABI: a fresh managed result, or a local whose return bypasses
cleanup, remains in the allocation registry until shutdown unless otherwise
released by its owner. This can retain storage and adds an extra allocation.
The callee must return storage that is still readable when the caller copies it.
P0037 qualifies cleanup around direct owned-local results below, preserving the
returned allocation's existing lifetime. There is no general result consumption,
dangling-result repair or ownership inference.

P0049's [caller-consumption audit and implementation contract](owned-call-result-consumption.md)
measures retained owned originals separately from safe borrowed call copies and
mixed return provenance. It selects a conservative direct free-function factory
initializer adoption slice for the next pass. Compiler behavior remains unchanged;
no call result is adopted or consumed yet. The reproducible checked audit is
[owned_result_audit.py](../../codex/python/owned_result_audit.py).

P0022 qualifies plain `=` when the destination is an identifier bound to a known
owned scalar declaration. The emitter records this for the copied initializer
forms above, concatenation and read-line initializers, and declarations without
an initializer. The latter now start as null. Parameters do not acquire this flag;
fields, indexed destinations, pointer/array bindings and compound operators keep
their existing emission. Other initializer forms remain unqualified.

`text = replacement` evaluates the replacement once, copies its readable bytes
(or preserves null), releases the slot's current managed allocation, stores the
copy and yields that pointer as the assignment expression's value. Copying first
makes `text = text`, a call borrowing `text`, and nested/chained assignment safe.
Nested replacement of the same slot releases the value current after the inner
assignment. Normal block/for-header cleanup later releases the final copy;
top-level final storage remains for shutdown. An initially null slot needs no
release. Allocation failure follows `dream_string_copy`: the new value is null
and the old value is released. Source ownership is unchanged. Fresh call,
concatenation and read-line results used as replacements can remain registered
until shutdown; this pass does not introduce temporary ownership transfer.

P0033 qualifies `text += suffix` for the same known-owned mutable scalar identifier
destinations when the operand is a scalar string expression or literal null.
The operand evaluates once before the helper reads the slot's current value.
Concatenation allocates independent managed storage before releasing that value;
the helper stores and yields the new pointer. Self-append is safe. Nested updates
read the value current after the inner update: `text += (text += "B")` with
initial `"A"` produces `"ABAB"`. A declaration initialized by `+=` copies the
complete result once, so its storage survives later source replacement or append.
Normal replacement, block/header cleanup and break/continue apply to that copy.

For this append operation, null on either side contributes empty text. Null plus
null and empty plus empty produce a nonnull managed empty string. Borrowed
sources remain readable, and fresh concatenation or unproven call results can
remain registered until shutdown. Proven factory operands are released in
[complete append statements](#append-of-proven-factory-results-p0058),
[direct append initializers](#append-initializers-with-proven-factory-results-p0059),
[direct Console output of append results](#console-output-of-append-results-p0060),
and [arguments to Console wrappers](#append-arguments-to-console-wrappers-p0061).
Allocation failure and size limits follow `dream_concat`. Const
identifier updates and non-string/non-null operands receive Dream diagnostics
before artifacts. Numeric `+=` retains its existing emission. Parameter/field/index
destinations, pointer/array operands, conditional operands, other compound operators
and general operator/shape checking remain outside this qualification.

P0023 distinguishes string container bindings from scalar strings during normal
block cleanup. A local `string values[N]` is emitted as a C array of borrowed
string pointers; a local `string* alias = values` borrows the array storage.
Neither binding is passed to `dr_release` when its block ends. Pointer aliases
and element writes do not copy, retain, release or transfer their targets.
Rebinding a pointer leaves its previous target's ownership unchanged. Null and
unused pointer/array declarations likewise need no scalar cleanup; their existing
initialization behavior is unchanged, so unused storage must not be read.

Elements may borrow literal, managed or null string pointers. Managed backing must
outlive every use through the container; destroying the container does not release
its elements. A scalar declaration such as `string copy = values[0]` still owns
an independent copy and receives normal cleanup. Owned scalar and class-instance
cleanup are preserved. This qualifies normal block exit in free functions,
static/instance methods, nested blocks and loop bodies in ordinary/module-entry
programs. It does not add owning containers, heap-array allocation, pointer-result
ownership, dangling-alias checks or automatic element destruction.

P0024 releases qualified owned scalar strings before `break` or `continue`, in
reverse declaration order through the scopes the jump leaves. A break targets
the nearest loop or switch; a continue targets the nearest loop, including from
inside a switch. Nested loops preserve enclosing body locals. Switch breaks
release case locals while preserving the surrounding loop body. Declarations
after the jump are excluded, and single-statement conditional jumps keep cleanup
inside the condition.

For-header storage survives body cleanup and continue/update, then receives its
existing single loop-exit release. Normal fall-through cleanup remains intact;
the early jump bypasses it, so each executed path releases a local once. Empty
and null strings follow the same rule, while borrowed parameters, arrays and
explicit pointer aliases receive no release. This applies to while/do-while/for,
free/static/instance functions and top-level loop bodies in ordinary/module-entry
programs. It does not qualify class-instance early-exit cleanup, unsupported
string initializer forms, return ownership, or exception/finally unwinding when
a jump crosses an exception context.

P0030 extends declaration copying to scalar conditional (`?:`) initializers.
The emitter wraps the complete conditional in one `dream_string_copy`, so the
condition runs once and only its selected branch evaluates. Literal, borrowed,
managed and null-valued string branches all produce independent declaration
storage (or null). Nested/parenthesized/const declarations, free/static/instance
functions, loop bodies, for headers and top-level declarations follow this rule.
These declarations also receive the existing owned-scalar replacement and
break/continue cleanup. Replacing or releasing the selected source leaves the
copy readable. Source ownership does not transfer: fresh concatenation/call
results selected inside a conditional can remain registered until shutdown.
Both branches must satisfy the existing semantic type rules; literal `null`
paired with a string is currently rejected. A typed string parameter or indexed
string element carrying null is covered. No conditional type-policy expansion,
return transfer or dangling-source repair is introduced.

P0032 extends declaration copying to plain assignment expressions, for example
`string copy = (source = "next")`. The complete assignment evaluates once, then
the declaration copies its readable result into independent managed storage.
Owned scalar identifier destinations retain P0022's replacement copy/release
ordering; parameter, field and indexed destinations retain their existing borrowed
assignment behavior. Their ownership does not transfer to the new declaration.
Self/nested assignment, parentheses, const declarations, methods, top-level and
for-header declarations use this path. These declarations also receive owned
scalar replacement and break/continue cleanup. Source replacement and copy
replacement leave the other allocation readable. Null results remain null.
Fresh managed assignment operands can still remain registered until shutdown;
compound assignment and destination ownership beyond P0022 remain separate.

P0034 releases qualified owned scalar string locals before a bare `return;` in
synchronous void functions and static/instance methods. Cleanup runs in reverse
declaration order across all active local scopes, including for headers, and stays
inside unbraced conditional returns. Parameters, `this`, borrowed arrays/pointers
and declarations not yet reached receive no release. Fall-through paths retain
their existing cleanup; a taken return bypasses it, avoiding repeated release.

Returns inside an active try, catch or finally retain their previous emission:
they require exception-context unwinding before this cleanup can be qualified.
A return after the context has completed uses the new path. Async functions,
value returns and temporary/result ownership remain
outside this boundary. In particular, returning a string local still leaves its
existing storage readable for the caller; this pass does not add value transfer.

P0035 adds scalar class locals to this bare-return path, interleaving class and
owned-string releases in reverse declaration order. Body/header locals occupy
scopes deeper than the function parameter scope; this excludes borrowed `this`
and parameters. Class arrays and explicit pointers are excluded from this new
path. This qualifies fresh independent class instances, matching their normal
block releases, without adding alias retention, field destruction or destructors.
The compiler still rejects custom-type for-header declarations and direct class
null initializers; the regression uses supported string headers and fresh instances.

## Storage contract

| Storage | Current implementation | Indexed scalar initializer |
| --- | --- | --- |
| String literal | C static storage, no `DrRef` header | Copy bytes into a new managed allocation |
| Concatenation/read-line result | `dr_alloc` storage, initially one reference | Copy bytes; backing reference count is unchanged |
| String array element or `string*` element | Borrowed pointer; may designate either storage kind | Copy bytes rather than retaining an unknown header |
| Null element | Null pointer | Preserve null; `dr_release(NULL)` is already safe |

`dream_string_copy` accepts a valid null-terminated string or null, allocates
`strlen(text) + 1` bytes and copies the terminator. It returns null if allocation
fails. A successful result has one reference and does not alias its input.
The input must remain readable while copying; no pointer/bounds checks are added.

Existing fall-through block cleanup releases the new local allocation. Top-level
copies remain in the allocator's registry until `dr_release_all` at shutdown.
The allocator's release/retain implementations are unchanged: only live managed
allocation addresses may be passed to them. Reading a header before an arbitrary
literal or interior pointer is invalid; the fix does not mask such errors.

## Evidence and limits

The [Dream regression](../../tests/basics/memory/indexed_string_copy.dr) checks
literal and dynamically allocated backing, nested blocks, null elements, a
top-level snapshot, and an index with a once-only side effect. The
[Go matrix](../../codex/go/indexed_strings_test.go) checks generated copy/release
code before compiling it, then requires exact output through both the direct
compiler linker and a checked-allocator harness in ordinary/module-entry modes.
The [C harness](../../codex/c/indexed_string_harness.c) checks release operands
against the real live-allocation registry before any header access, four nonnull
local releases, shutdown cleanup, and helper ownership for literal, empty, null,
managed and interior backing storage, including use after backing is freed.

The [literal regression](../../tests/basics/memory/literal_string_copy.dr) and
`TestCompiler_LiteralStrings` reuse the harness in both modes, checking seven
literal-copy sites, unchanged concatenation emission, exactly eight nonnull local
releases and ten output lines. Top-level storage remains live until shutdown;
the registry must be empty afterward. Empty, escaped, nested and repeated literal
declarations are included. Before P0017, both modes failed the live-registry
assertion while cleaning up a literal.

The [identifier regression](../../tests/basics/memory/identifier_string_copy.dr)
and `TestCompiler_IdentifierStrings` check nine identifier-copy sites and ten
release sites, exactly twelve nonnull runtime releases and nineteen output lines
in each mode. Nested alias chains, managed and literal parameter backing,
parenthesized/const bindings, free/static/instance functions, loop-body copies,
top-level copies and a null identifier are included. Source strings are used after
nested copies are released; the live registry and single-reference assertions
ensure cleanup releases independent allocations. Helper checks above also prove
a copy remains readable after its managed backing is freed. Both modes failed the
live-registry assertion before the identifier change. Both quick protocols and CI
select this matrix alongside the indexed and literal checks.

The [member regression](../../tests/basics/memory/member_string_copy.dr) and
`TestCompiler_MemberStrings` check ten member-copy sites, thirteen release sites,
sixteen nonnull runtime releases and twenty-seven output lines in each mode.
Literal/managed/null fields, inherited `this`/`base` access, static fields,
const/grouped declarations, nested blocks, loop-body and top-level copies are
covered. Managed backing and fields remain readable after copies are released;
a top-level copy also survives field replacement. A constructor output marker
proves a temporary receiver runs once. Helper checks above establish copy lifetime
after backing release. Before P0019, both modes failed the live-registry assertion
on literal field cleanup. Both quick protocols and CI select this matrix.

The [for-header regression](../../tests/basics/memory/for_string_copy.dr) and
`TestCompiler_ForStrings` check nine copy sites, unchanged concatenation emission,
ten header release sites and exactly nine nonnull runtime releases, with thirteen
output lines in each mode. Zero/multiple iterations, borrowed managed sources,
member/index/null values, once-only indexing, break, continue, header use in the
condition/update, reused names and top-level cleanup are covered. Generated C
must release each header immediately after its loop, within the wrapper scope.
Before P0020, reused names failed semantic analysis; after semantic scope repair,
both harness modes failed Linux C compilation on nine out-of-scope release operands.
Four ordinary/module-entry diagnostic cases now reject string and integer header
uses after the loop, with source positions and no generated artifacts.

The [call regression](../../tests/basics/memory/call_string_copy.dr) and
`TestCompiler_CallStrings` check fifteen copy sites, fourteen block/header release sites,
fifteen nonnull runtime releases and nineteen exact output lines in each mode
after P0022 adds release of the replaced top-level backing (fourteen in P0021).
Literal/empty/borrowed/fresh/local-returned/null results, nested copies, static/
instance/base calls, zero/multiple loop iterations, const/grouped declarations
and top-level snapshots are included. An output marker proves once-only call
evaluation. Borrowed backing survives nested cleanup; the global snapshot survives
source replacement. Both modes failed the live-allocation assertion before the
fix. Existing registry/refcount assertions are preserved, including empty registry
after shutdown. Both quick protocols and CI select this group.

The [assignment regression](../../tests/basics/memory/assigned_string_copy.dr) and
`TestCompiler_AssignedStrings` require thirty-nine nonnull checked releases and
thirty-three exact output lines in both modes. They check replacement copy/release
ordering in generated C, managed/literal/borrowed/member/index/call sources,
self/chained/nested assignment and its result, null/empty values, null defaults,
source survival and copies surviving nested source cleanup. Index and call markers
prove once-only evaluation. Free/static/instance paths, loop bodies, header updates
and top-level replacement are included. Parameter rebinding and field/index
writes retain their existing borrowed emission. Both modes failed live-registry
cleanup before the fix; no allocator assertions were removed. Both quick protocols
and CI select this group. The exact release total checks old-value reclamation
as well as final block/header cleanup; shutdown still requires an empty registry.

The [array regression](../../tests/basics/memory/array_string_storage.dr) and
[pointer regression](../../tests/basics/memory/pointer_string_storage.dr), selected
by `TestCompiler_StringContainers`, require eight and seven nonnull checked
releases respectively, with thirteen and eleven exact output lines in both modes.
Generated C must omit container/alias releases and retain scalar and class-instance
releases. Nested/unused storage, literal/managed/null elements, pointer alias chains
and rebinding, null pointers, repeated loop bodies and static/instance methods are
covered. Managed sources survive container exit and element replacement; scalar
copies still have one reference and are reclaimed. All four modes failed the live
registry assertion before P0023. Both quick protocols and CI select this group;
the harness's release/refcount/shutdown assertions are unchanged.

The [loop-exit regression](../../tests/basics/memory/loop_exit_strings.dr) and
`TestCompiler_LoopExitStrings` require thirty-seven nonnull checked releases and
sixteen exact output lines in both modes. Before P0024 both modes failed the
release total, exposing allocations retained until shutdown. Generated C must
release exactly the active owned strings before each jump, excluding later
declarations, surviving outer locals and headers. Repeated while/do/for bodies,
switch break/continue and normal completion, nested loops, a loop inside a switch,
null/empty strings, borrowed containers, unbraced conditionals, for-header
replacement updates, free/static/instance methods and top-level loops are covered.
The existing allocator/refcount/shutdown assertions and all prior ownership
matrices remain intact. Both quick protocols and CI select this group.

The [conditional regression](../../tests/basics/memory/conditional_string_copy.dr)
and `TestCompiler_ConditionalStrings` check nineteen emitted conditional-copy
sites, twenty-two nonnull checked releases and twenty-four exact output lines in
each mode. Both modes failed the live-registry assertion before the fix. Branch
markers and an index increment require once-only selected evaluation and no
unselected call; nested branches, empty/null strings, borrowed/managed sources,
source replacement, independent snapshots, scalar replacement, header cleanup,
continue/break and static/instance/top-level paths are covered. Both quick
protocols and CI select the group. The harness now prints actual/expected release
counts on mismatch while preserving every lifetime/refcount/shutdown assertion.

The [assignment-initializer regression](../../tests/basics/memory/assignment_initializer_copy.dr)
and `TestCompiler_AssignmentInitializers` check seventeen emitted assignment-copy
sites, thirty nonnull checked releases and twenty-three exact output lines in
ordinary/module-entry modes. Both modes failed the live-registry assertion before
P0032. Self/nested assignment, source/copy replacement, const bindings, borrowed
parameter/static/instance-field/index destinations, null/empty results, managed
operands, zero/multiple loop iterations, header cleanup, break/continue and
top-level snapshots are covered. A call marker and index increment require
once-only effects. The existing allocator/refcount/shutdown assertions remain
unchanged. Both quick protocols and CI select this group.

The [append regression](../../tests/basics/memory/appended_string_copy.dr) and
`TestCompiler_AppendedStrings` require 49 nonnull checked releases and 26 exact
output lines per mode, including old-value reclamation, source/copy lifetime,
self/nested append, null/empty operands, call/index effects, member/borrowed/fresh
sources, methods, zero/multiple loop iterations, header updates, break/continue,
top-level snapshots and unchanged numeric operators. Ten positioned, artifact-free
cases reject const append and int/float/bool/char operands. Both modes failed checked
harness C compilation on invalid pointer `+=` before P0033; no runtime lifetime
claim is made for those unbuildable programs. Existing registry/refcount/shutdown
assertions remain intact. Both quick protocols and CI select the matrix.

P0015 repairs the harness's Linux C11 build by matching `build.zig`'s
`_GNU_SOURCE` feature macro for exception support. Windows runs also cross-link
both modes for Linux before executing the native harness. Cross-linking checks
headers and linking; Linux runtime evidence still requires native CI.

P0031 extends the same jump boundaries to class locals, matching their normal
block cleanup. The [class regression](../../tests/basics/memory/loop_exit_classes.dr)
and `TestCompiler_LoopExitClasses` require reverse-order cleanup at each jump,
thirty-three nonnull releases (27 class instances and six strings), thirteen output
lines and an empty live-allocation registry **before** `dr_release_all`. Both modes
failed that pre-shutdown assertion before the fix. Repeated while/do/for bodies,
mixed class/string scopes, later declarations, nested loop/switch targets, surviving
outer instances, string headers, normal exits, static/instance methods and top-level
loops are covered. The fixture uses fresh, independent class allocations; it does
not establish alias retention, class pointer/array ownership, destructors or field
destruction. Value-return and exception/finally exits remain separate. Both quick protocols
and CI select the new matrix; all prior allocator assertions remain intact.

The [void-return regression](../../tests/basics/memory/void_return_strings.dr)
and `TestCompiler_VoidReturnStrings` require exact reverse-order return sequences,
40 nonnull checked releases (39 strings and one normally cleaned class instance),
13 output lines and an empty registry before shutdown in both modes. Both modes
failed that pre-shutdown assertion before P0034. Nested/const/null/empty locals,
later declarations, true/false unbraced paths, while/do/for/switch returns,
zero/multiple iterations, header updates, recursion, methods and borrowed caller
survival are covered. An unused function checks that return emission inside
try/catch/finally remains unchanged; these unsafe exit paths are not executed or
qualified. A normally completed exception context followed by a return is executed.
Both quick protocols and CI select the matrix; existing allocator assertions remain.

The [class void-return regression](../../tests/basics/memory/void_return_classes.dr)
and `TestCompiler_VoidReturnClasses` require 40 nonnull releases (30 class
instances and ten strings), ten exact output lines, reverse-order C sequences and
an empty registry before shutdown in both modes. Both modes reproduced retained
allocations before P0035. Nested mixed scopes, later declarations, true/false
unbraced returns, while/do/for/switch exits, zero/multiple iterations, recursion,
static/instance methods, a constructor's bare return, caller receiver survival and
normal exception completion are covered. Unused active exception-return paths
are checked only for unchanged emission. Both quick protocols and CI select the
matrix; all existing allocator/refcount/shutdown assertions remain unchanged.

P0036 adds cleanup for synchronous `int`, `float`, `bool` and `char` returns
outside active exception contexts. The result is evaluated once into a temporary
of the declared C return type before reverse-order owned-string/scalar-class
cleanup. This preserves local field reads and expression effects; no primitive
result ownership transfer is required. Only returns with qualified locals need
a temporary. Name selection avoids active bindings and identifiers in the lowered
expression, including module-lowered function names. Borrowed parameters/receivers
and containers retain their existing lifetime; emitter bindings remain available
to other branches. This uses the same cleanup predicate as bare void returns.

The [scalar-return regression](../../tests/basics/memory/scalar_return_cleanup.dr)
and `TestCompiler_ScalarReturnCleanup` require 47 nonnull releases (21 class
instances and 26 strings), 23 output lines, evaluation-before-cleanup C sequences
and an empty registry before shutdown in both modes. Both modes reproduced live
allocations before the fix. Mixed/nested scopes, null/const/empty strings, later
declarations, true/false unbraced paths, while/do/for/switch exits, zero/multiple
iterations, recursion, primitive result types, methods, receiver survival,
index/assignment effects, temporary-name collisions and normal exception
completion are checked. Unused active exception returns and an owning string
result have unchanged-emission checks only. Both quick protocols and CI select
the group; existing allocator/refcount/shutdown assertions remain intact.

## Direct owned-local string returns (P0037)

The bounded result contract is `return local;` in a synchronous string function
or static/instance method, outside active exception contexts, where `local` is
a known-owned scalar string in an active body/header scope. Parentheses are
transparent in the AST. `owns_string` records independent allocation ownership
for the qualified declaration/replacement forms above; a null slot also qualifies.
Parameters, top-level bindings, arrays and explicit pointers do not qualify.

The emitter saves the pointer once in a collision-free `const char *` temporary,
releases every other qualified string/scalar-class local in reverse declaration
order, then returns the saved pointer. It skips exactly the returned binding,
without changing emitter metadata, so untaken branches and normal block cleanup
still release that binding. An unbraced conditional return keeps this complete
sequence inside its condition. No result copy, retain or allocator change is added.

This preserves the existing result lifetime: the returned allocation remains live
with its original reference until shutdown. Caller declarations still copy the
result; they do not consume or release that original reference. Recursion can
therefore retain a returned allocation at each level. This pass reclaims the other
locals; it does not introduce an owning return ABI or eliminate retained results.
The returned string is independent of the locals being released, including a
caller copy of an earlier returned allocation. Allocation failure remains null.

P0037's rule requires a direct known-owned identifier. Borrowed parameter,
member, indexed, call, assignment, concatenation and conditional *return
expressions* retain prior emission, even when they mention owned locals. Their
source/alias lifetime needs separate qualification; a universal release could free
the result's backing, while universally copying results would add retained storage.
Active try/catch/finally returns and async exits also keep their prior emission.
P0038 separately qualifies direct literals below; P0040 additionally qualifies
conditional owned-local/literal trees below; P0041 qualifies unchanged direct
scalar string-parameter results. Broader borrowed-source and result-
consumption policies remain open; never apply this predicate to an arbitrary
string expression.

The [regression](../../tests/basics/memory/local_string_return.dr) and
`TestCompiler_LocalStringReturn` reproduce **44 live allocations before shutdown**
in both ordinary/module-entry modes before the fix. Afterward, exact return/cleanup
sequences, **45 nonnull releases** (20 classes, 25 strings), **21 output lines**
and **17 remaining returned allocations**, each with one reference, are required.
The shutdown registry must still be empty. Mixed/nested scopes, const/empty/null
results, later declarations, unbraced true/false paths, loops/switches, headers,
replacement/append, recursion, free/static/instance methods, name collisions and
normal exception completion are covered. Caller backing and receiver survive;
a caller snapshot survives backing replacement. Active exception exits remain
unchanged-emission controls and are not executed here. Unused literal/conditional
controls check P0038/P0040, and the direct borrowed control checks P0041 cleanup;
runtime counts are unchanged.
The existing empty-before-shutdown checks remain intact for earlier fixtures.

## Direct literal string returns (P0038)

A synchronous string function or static/instance method returning a direct string
literal now releases all qualified string/scalar-class locals in reverse declaration
order, including active body/header scopes. Parentheses are transparent. Empty and
escaped literals follow the same rule. The literal's C storage has static lifetime,
so cleanup cannot invalidate the result. No result allocation, copy, retain or
consumption is added. Caller declarations retain their existing copy behavior.

When cleanup is needed, the existing collision-safe temporary captures the literal
pointer before releases. Unbraced returns remain conditional and emitter metadata
remains available for other paths. With no qualified locals, emission stays a direct
return. P0038 qualified only `ND_STRING`; P0039 extends this to literal-only
conditional trees below. P0041 qualifies unchanged direct parameter results.
Other borrowed/member/index/call/assignment/concatenation results,
async exits and returns inside active try/catch/finally remain separate.

The [literal-return regression](../../tests/basics/memory/literal_string_return.dr)
and `TestCompiler_LiteralStringReturn` failed the empty-registry-before-shutdown
assertion in both modes before the fix. Both now require **51 nonnull releases**
(18 class instances and 33 strings), **19 exact output lines** including an empty
line, and an empty registry before and after shutdown. Exact C sequences check
reverse cleanup order, no returned-literal copy, temporary collisions and unchanged
active-exception emission. Unused literal-only conditional and borrowed controls
check P0039/P0041 cleanup; runtime counts are unchanged. Mixed/nested/null/const/empty locals,
later declarations, true/false unbraced paths, while/do/for/switch, header updates,
zero/multiple iterations, recursion, methods, caller snapshot/backing/receiver
survival, escaped results and normal exception completion execute. Both quick
protocols and CI select the group. Existing allocator/refcount assertions remain.

## Conditional literal string returns (P0039)

A synchronous string return also qualifies when its expression is a `?:` tree
whose value leaves are all string literals. Both branches are checked recursively,
including branches that a constant condition would not select. Parenthesized,
empty and escaped literals qualify. P0039 preserved prior emission for every
nonliteral value leaf; P0040 additionally qualifies owned-local leaves below.

Conditions may call functions, mutate state and read owned local strings, class
fields or loop headers. The existing collision-safe temporary captures the selected
static pointer before reverse-order cleanup. Each reached condition executes once;
unselected conditional branches do not execute. No returned allocation, copy,
retain or consumption is introduced, and caller declarations still copy results.
Unbraced returns keep cleanup conditional, other paths retain their binding
metadata, and functions without qualified locals keep a direct return. Async and
active try/catch/finally returns remain outside this contract.

The [conditional regression](../../tests/basics/memory/conditional_literal_return.dr)
and `TestCompiler_ConditionalLiteralReturn` failed pre-shutdown registry emptiness
in both modes before the fix. Each now requires **55 nonnull releases** (18 class
instances, 37 strings), **34 output lines**, and an empty registry before and after
shutdown. All four nested literal leaves execute; labels and effect counts check
selected-only, once-only condition evaluation. Null/const/empty locals, later
declarations, unbraced true/false paths, loop/header updates, static/instance methods,
temporary-name collisions, caller backing/snapshot/receiver survival and normal
exception completion are covered. Exact C checks require evaluation before reverse
cleanup and unchanged emission for unsupported leaves in either recursive branch
and active exception exits. The unused owned-local control now checks P0040
cleanup; its runtime counts remain unchanged. Both quick protocols and CI select
the group. Earlier allocator/refcount, retained-result and shutdown assertions
remain intact.

## Conditional owned-local string returns (P0040)

Synchronous string returns outside active exception contexts now qualify nested
`?:` trees when every value leaf is a literal or a known-owned scalar string local
under P0037's binding predicate. Every branch must qualify, including branches
under constant conditions. P0042 additionally qualifies unchanged borrowed scalar
parameters below. Top-level bindings, arrays, explicit pointers, members, indexes,
calls and other compound value leaves do not qualify;
one unsupported leaf preserves the entire return's previous emission.

The collision-safe temporary captures the selected pointer once before cleanup.
For each owned binding that occurs as a value leaf, reverse-order cleanup emits a
pointer comparison and releases it only when its current pointer differs from
the result. Other qualified strings and scalar classes are released normally.
Independently owned local slots have distinct live allocations even when contents
match. Conditions can replace a candidate before selection; comparisons use the
current slot and never evaluate conditions again. Repeated leaves do not duplicate
releases. A selected null omits only no-op null releases; a selected literal keeps
no allocation. All-literal trees retain P0039 emission. Binding metadata and
unbraced conditional control flow remain intact.

The selected owned allocation retains its original reference/lifetime until
shutdown, matching P0037. No result allocation, copy, retain or consumption is
introduced; caller declarations still make independent copies. General result
ownership and retained-result accumulation remain unresolved.

The [regression](../../tests/basics/memory/conditional_local_return.dr) and
`TestCompiler_ConditionalLocalReturn` check both ordinary/module-entry modes:
**64 nonnull releases** (18 classes, 46 strings), **36 output lines**, and exactly
**14 selected allocations** with one reference before shutdown, then an empty
registry. The initial fixture reproduced **57 live allocations instead of 12**
before the fix; later cases strengthen candidate replacement/equal-content checks.
All four nested leaves, selected-only condition effects, body/header scopes,
normal/early loop paths and header updates, null/const/empty candidates, repeated
leaves, equal-content independent slots, append in a condition, later declarations,
unbraced paths, static/instance methods, name collisions, caller snapshots/backing
and normal exception completion execute. Exact C checks require evaluation before
reverse cleanup and preserve unsupported/active-exception emission. P0037/P0039's
unused owned-conditional controls check the new emission with unchanged runtime
assertions. Both quick protocols and CI select the new group.

## Unchanged borrowed string-parameter returns (P0041)

A direct identifier result now qualifies when it resolves to a borrowed scalar
string parameter and a whole-body scan establishes that parameter is unchanged.
The synchronous function/static/instance method captures the pointer once, releases
all qualified owned string/scalar-class locals in reverse order, and returns the
saved pointer. The caller's backing storage remains independent of those locals.
No result allocation, copy, retain or consumption is added; caller declarations
still copy. No qualified locals means the existing direct return remains.

The scan checks assignments and increment/decrement throughout the body, including
later statements, loop conditions/updates, nested expressions and exception bodies.
Same-name declarations/catch bindings and unknown/nested declarations conservatively
disqualify the proof. Writes to other bindings do not disqualify this parameter.
Dream has no parsed address-of expression for exposing a scalar parameter slot;
by-value calls cannot rebind that slot. This is not alias analysis. A rebound
parameter can point at owned local storage, so its returns preserve prior emission.
P0042 extends this proof to conditional value leaves below. Rebound/member/index/
call results and active exception/async exits remain separate; existing dangling
aliases are not repaired.

The [regression](../../tests/basics/memory/borrowed_parameter_return.dr) and
`TestCompiler_BorrowedParameterReturn` first reproduced a nonempty pre-shutdown
registry in both modes. Each now checks **58 nonnull releases** (20 classes,
38 strings), **17 exact output lines**, and an empty registry before and after
shutdown. Nested/const/null/empty locals, later declarations, true/false unbraced
returns, header updates, switch scopes, recursion, static/instance methods,
temporary collisions, normal exception completion and caller snapshots/backing
survival execute. Literal, empty, null and managed parameter storage are covered.
Exact C checks require capture before reverse cleanup and unchanged unsupported/
active-exception returns; **24 additional ordinary/module-entry cases** check
fallback for twelve placements of rebinding. P0037/P0038 unused borrowed controls
now check the qualified emission with unchanged runtime assertions. Both quick
protocols and CI select the group; earlier allocator/refcount checks remain intact.

## Conditional borrowed string-parameter returns (P0042)

A synchronous conditional string result outside active exception contexts now
qualifies when every value leaf is a literal, a known-owned scalar string local,
or a borrowed scalar string parameter proven unchanged by P0041's whole-body scan.
The scan includes return conditions, later statements and loop updates. Every
parameter used as a value leaf must pass, including leaves under constant
conditions. One unsupported/rebound leaf preserves the entire previous return.
Writes to parameters that are not value leaves do not block the result proof.

The selected pointer is captured once before reverse-order cleanup. Only owned
local value candidates receive P0040's pointer comparison; borrowed parameters
are never released or retained. A selected borrowed value leaves no retained
local allocation. Caller backing keeps its original lifetime and caller declarations
still copy results. Selected owned results keep their original reference until
shutdown; general result consumption and retained-result accumulation stay open.

The [regression](../../tests/basics/memory/conditional_borrowed_return.dr) and
`TestCompiler_ConditionalBorrowedReturn` reproduced **62 live allocations instead
of five** in both modes before the fix. Each now checks **75 nonnull releases**
(19 classes, 56 strings), **39 output lines**, exactly **five selected owned results**
with their original reference before shutdown and an empty registry afterward.
All nested value paths, two borrowed parameters, repeated leaves, null/empty/const
storage, later declarations, unbraced paths, header updates, methods, temporary
collisions, condition-side owned replacement and normal exception completion
execute. Labels/effect counts require selected-only, once-only evaluation, and
caller backing/snapshot survival is checked after return and backing replacement.
Exact C requires capture before reverse cleanup and preserves call/assignment/
concatenation/rebound-condition/active-exception returns. **48 additional cases**
check twelve placements of rebinding with a borrowed leaf nested on either side,
in both ordinary/module-entry modes. P0039/P0040/P0041 unused conditional controls
now require the qualified cleanup; their runtime assertions remain unchanged.
Both quick protocols and CI select the group; existing allocator/refcount/shutdown
assertions and the 24 direct-parameter rebinding cases remain intact.

## Read-only direct instance string-field returns (P0044)

P0044 implements the [P0043 member-return proof](member-return-cleanup.md) for
direct `this.field` results in synchronous methods. Resolve the depth-1 borrowed
class binding and the actual nonstatic scalar string declaration, including its
inherited owner. Capture the field pointer once before reverse local cleanup;
add no copy, retain, receiver release or field destruction. No cleanup is inserted
when there are no qualified local owners.

The separate whole-body AST/operator allowlist rejects all writes, increment/
decrement, calls/new/console/await, exceptions, nested/unknown nodes, receiver-name
shadowing and every custom-type/pointer/array local, including already-exited
blocks. This protects backing from aliased writes and the receiver from unretained
class-alias cleanup. Independent primitive scalar locals/read expressions and
qualified control flow are permitted. P0041/P0042's less restrictive parameter
proof remains unchanged. Failed proof keeps the entire original return.

The [regression](../../tests/basics/memory/borrowed_member_return.dr) and
`TestCompiler_BorrowedMemberReturn` in
[member_return_test.go](../../codex/go/member_return_test.go) reproduced a nonempty
pre-shutdown registry in both ordinary/module-entry modes before the fix. Each
now requires **38 nonnull releases** (one receiver, 37 strings), **21 output lines**,
and empty pre/post-shutdown registries with live release operands and original
reference counts. Literal/managed/empty/typed-null backing, independent snapshots,
nested/alternate/unbraced paths, later declarations, while/do/for/header/switch
returns, read-only operations, inherited fields, temporary collisions and no-local
returns execute. Caller receiver/backing and snapshot survival are checked.
Exact C requires owner-preserving capture before reverse releases and forbids
receiver/result retain/release/copy. **196 emission-only controls** cover mutations
and calls before/after early returns throughout body/header/expression positions,
aliases in exited scopes, result shape and exception/async guards. No unsafe alias
example is executed. Quick protocols/CI select the new group.

Explicit class parameters and async instance methods are rejected by today's
grammar; receiver shadowing is rejected by semantics. Self-type declarations
inside a class are not registered yet, so alias/constructor controls use a prior
class. The async control uses an accepted free function/static field. These tests
do not broaden grammar or qualify async runtime behavior. Unknown AST/operators
fail closed in source; source syntax cannot construct arbitrary unknown nodes.
P0045 extends the same proof to conditional member leaves below.
P0047-P0048 qualify type-qualified static reads below; other static qualifiers,
local/struct/nested/base instance receivers, broader field/class-alias ownership,
caller consumption and exception/async cleanup remain separate. Incoming receiver and backing must already be live.

## Conditional read-only instance string-field returns (P0045)

Conditional trees may mix proven scalar nonstatic `this.field` reads, literals,
known-owned scalar locals and unchanged borrowed scalar parameters. Every value
leaf must qualify, including constant unselected branches. Any member leaf requires
P0044's separate whole-body read-only proof with every write/call/alias barrier
unchanged. Effectful conditions therefore preserve the entire previous return.
The less restrictive P0041/P0042 parameter-only proof remains unchanged.

Capture the selected pointer exactly once before reverse local cleanup, preserving
inherited owning-class casts. Only owned-local value candidates receive pointer
comparisons to preserve selected storage. Member/parameter/literal leaves retain
their existing caller/static lifetime without result copy/retain, receiver release
or field destruction. No-local methods keep direct returns. Unsupported leaves,
local/struct/nested/base instance receivers and async/active exception exits keep
previous emission; no partial cleanup is inserted. P0048 adds type-qualified
static leaves below.

The [fixture](../../tests/basics/memory/conditional_member_return.dr) and
`TestCompiler_ConditionalMemberReturn` in
[conditional_member_return_test.go](../../codex/go/conditional_member_return_test.go)
initially reproduced **50 live allocations instead of four** in both modes before
the fix, before adding the final two-field reader calls.
Each mode now requires **59 nonnull releases** (one receiver, 58 strings), **27 output
lines**, **four selected owned results** with original reference counts before
shutdown and an empty registry afterward. Selected local/for-header/empty results,
null/empty/literal/managed member backing, nested branches, independent snapshot
survival after caller replacement, inherited fields, temporary collisions, unbraced/
loop/header/switch paths and no-local returns are checked. Generated C requires
capture before reverse cleanup with no result copy/retain or receiver release.

The shared 196 emission controls also run on conditional member trees. Fourteen
additional negative controls check effectful conditions and unselected call/concat/
assignment/index leaves; two nested-else member controls qualify. Unsafe alias
programs remain emission-only. The old direct group's newly eligible conditional
control was replaced in P0045 by a mixed instance/static result; P0048 migrates
that result to positive C coverage. Conditional positives have separate lifetime/C
coverage. Existing allocator,
reference-count, direct-return and parameter-result assertions remain intact.
Both quick protocols and CI select the new group. Explicit class parameters,
async instance methods and semantic receiver shadowing remain unsupported.
Selected owned-result accumulation and caller consumption remain separate.

## Direct read-only static string-field returns (P0047)

P0047 implements the [P0046 contract](static-member-return-cleanup.md) for direct
`return Type.field;` on an unshadowed registered class type and an actual scalar
static string declaration/owning class. A separate predicate shares the existing
whole-body read-only traversal using an explicit guarded name. It adds no fake
receiver binding and does not change the instance/parameter proofs or literal-only
predicate. All writes/calls/new/console/await/exceptions, custom-type/pointer/array
locals, shadowing and unknown nodes/operators still block the proof.

Capture the owning global pointer once before reverse local cleanup. No result
copy/retain, global/receiver release or field ownership is added. Inherited and
hidden owner selection, collisions, free/static/instance readers and no-local
returns preserve normal expression emission. Incoming backing must be live;
callers must clear borrowed static slots before releasing their backing.

The [fixture](../../tests/basics/memory/static_member_return.dr) and
[Go group](../../codex/go/static_member_return_test.go) reproduced retained callee
allocations in both modes before the fix. Each mode now requires 40 nonnull releases
(one receiver, 39 strings), 23 output lines, live release/refcount checks, independent
snapshot/caller backing survival and empty pre/post-shutdown registries. Exact C
requires capture before reverse cleanup. Of 252 subtests, two are lifetime modes,
208 shared controls include two newly eligible direct-static positives, 30 expression
controls include two existing-instance positives, and twelve assert semantic rejection
without artifacts. Primitive parameter and indexed class-array field receivers are
rejected before emission; local type-name shadowing is rejected in active/exited/later/
header scopes. No grammar or semantic boundary is broadened. Quick/CI selectors align.

P0048 extends these proven static reads to conditional value leaves, as described
below. Struct/variable/this/base/nested/index/call static qualifiers, static pointer/
array shape emission and async/exception exits remain outside this slice.
See [P0047](../../tasks/passes/P0047-static-member-return.md) for check results.

## Conditional read-only static string-field returns (P0048)

Conditional trees may mix proven static fields, instance fields, unchanged scalar
parameters, known-owned scalar locals and literals. Each static leaf reuses
`borrowed_static_member_result`; the complete function body must pass the unchanged
member read-only allowlist, including conditions and unselected branches. Capture
the result once before reverse cleanup and preserve only selected owned storage.
No result copy/retain, global release or field ownership is added. One unsupported
leaf/effect preserves the complete prior return; async/active exception exits
retain prior emission. The literal-only predicate remains unchanged.

The [fixture](../../tests/basics/memory/conditional_static_return.dr) and
[Go group](../../codex/go/conditional_static_return_test.go) check both modes with
67 nonnull releases, 35 output lines and four original-reference owned survivors
until shutdown; all storage is gone afterward. Exact C verifies inherited/hidden
owners, mixed receiver leaves, free/static/instance readers, nested/alternate/
for-header/loop/switch paths, collisions and no-local returns. Caller backing and
independent snapshots survive; borrowed globals are cleared before backing exits.
All write/call/alias/shape barriers and failed-leaf/async/exception controls remain.
See the [static contract](static-member-return-cleanup.md) and
[P0048](../../tasks/passes/P0048-conditional-static-return.md).

## Direct proven-owned call initializers (P0050)

A complete scalar string declaration may adopt a direct synchronous free-function
result when its actual lowered body is exactly one scalar string local initialized
from a literal, absent/null AST or scalar string parameter, followed by a direct
return of that binding. All parameters have scalar primitive shapes; caller
shadowing and unsupported definitions prevent adoption. Omit the caller copy and
keep the destination owned, so existing cleanup releases or transfers the original
reference. No runtime ABI, return proof or other consumer changes. See the
[contract and measured identity/lifetime evidence](owned-call-result-consumption.md).
Top-level declarations retain program lifetime until shutdown. Direct literal-null
and async-result string declarations remain rejected; typed-null copies execute.
Historical caller-copy statements above describe their original pass checkpoints;
P0050 supersedes only actually proven factory initializers.

This is not a complete string ownership system. Other initializer forms/compound
operators, parameter/field/index destination ownership, alias retention,
owning arrays/pointers and automatic element destruction, general string/pointer/class result transfer,
exception cleanup, class alias/container ownership,
compound expressions and field ownership remain unqualified. Concatenation and
read-line initializers keep their existing managed allocation; as replacements
they are copied, leaving temporary/result ownership unchanged.
Pointer shape compatibility and general indexed expression typing remain separate
work. P0016 qualifies direct indexed primitive console formatting, including scalar
string characters; it does not establish semantic argument/return shapes.


## Complete proven-owned factory assignments (P0052)

Complete assignment statements to already-owned scalar string identifier slots
reuse P0050's actual-definition factory proof. Capture the result once in a fresh
statement-local temporary, release the current destination after RHS completion,
and store the original pointer. Self-snapshot and nested argument replacement
therefore finish before destination cleanup. Other assignment expression contexts,
borrowed/mixed/method results and shape/body barriers retain previous emission.
See the [boundary and evidence](owned-call-assignment.md).

## Discarded proven-owned factory results (P0054)

A complete direct call statement releases a proven factory's original result
after its arguments and the call finish. The emitter uses the same
actual-definition proof as scalar declaration and assignment adoption. It emits
no caller copy, retain or scoped binding. Borrowed or unproven calls, for headers,
compound expressions and nested owned arguments keep their existing lifetime.
See the [boundary and measured identity evidence](owned-call-discard.md).

## Proven factory results passed to Console wrappers (P0056)

A complete statement such as `show(getText());` releases a proven factory
result after `show` returns normally when the actual synchronous void free
function has one scalar string parameter and its whole body calls
`Console.Write` or `Console.WriteLine` on that parameter. The caller passes the
original pointer and releases it after the read. See the
[argument boundary and measured behavior](owned-call-argument.md).

## Direct Console output of proven factory results (P0057)

A complete `Console.Write(makeText());` or `Console.WriteLine(makeText());`
statement captures a direct call that passes the same actual-definition
factory proof. It sends the original pointer to the selected runtime writer
and calls `dr_release` immediately after the writer returns. The writer uses
the pointer for synchronous output. The same statement-local naming rules as
the Console wrapper apply, including for an unbraced loop or branch body.

Each writer's isolated eight-call probe went from zero releases and eight live
originals to eight releases and zero live originals in ordinary and module-entry
compilation. The checked regression also exercises 10,000 calls to each writer
per mode, with a peak of one live allocation. The proof requires the entire
output operand to be an eligible factory call. The null-result case is checked
at emission because the runtime formats its argument with `%s`.
See the [P0057 pass record](../../tasks/passes/P0057-direct-console-ownership.md).

## Append of proven factory results (P0058)

A complete `text += makeText();` statement captures the operand once when
`text` is a known-owned scalar string identifier and the entire operand passes
the existing [factory proof](owned-call-result-consumption.md). The append helper
reads the destination after the call finishes, so a factory argument can replace
the destination before concatenation. The helper copies both strings into a new
allocation, releases the old destination, and installs the joined string. The
statement then releases the original factory result. Null contributes empty text
and its release is a no-op.

The checked regression exercises 10,000 repetitions in each mode, along with
destination replacement during argument evaluation, a once-only side effect,
null operands, borrowed self-append, and temporary name shadowing. The isolated
eight-call audit now finds 24 nonnull releases and zero survivors in each mode.
See the [P0058 pass record](../../tasks/passes/P0058-owned-call-append.md).
P0069 extends complete statements to [proven returned joins](#complete-appends-with-proven-returned-joins-p0069).

## Append initializers with proven factory results (P0059)

In `string snapshot = (text += makeText());`, when `text` is an owned scalar
string identifier and `makeText()` passes the factory proof, the append helper
consumes the original factory result. It reads the current destination after
argument evaluation, installs the joined allocation, and releases the factory
result. The declaration copies the joined value into its own managed allocation.
Changing the destination or releasing the snapshot leaves the other value readable.
The same rule applies to const snapshots and declarations in a `for` initializer.
Null still contributes empty text.

Each eight-iteration audit went from 32 nonnull releases and eight live factory
allocations to 40 releases and zero live allocations. The checked regression
exercises 10,000 repetitions per mode, including operand effects, borrowed
self-append, null, and zero-iteration header cleanup. See the
[P0059 pass record](../../tasks/passes/P0059-owned-append-initializer.md).
P0070 extends this initializer rule to [proven returned joins](#append-initializers-with-proven-returned-joins-p0070).

## Console output of append results (P0060)

In a complete `Console.Write(text += makeText());` or
`Console.WriteLine(text += makeText());` statement, the same owned destination
and factory proof allow the append helper to consume the original operand. It
finishes the append and releases that operand before the writer receives the
joined string. The destination owns the join, so later reads and replacements
still work. A null operand contributes empty text, and a borrowed operand uses
the ordinary append path.

The eight-iteration audit measures 24 nonnull releases and zero survivors in
each mode, up from 16 releases and eight survivors. The stress regression also
checks both writers, an argument that replaces the destination and increments a
counter once, null, and borrowed self-append.
See the [P0060 pass record](../../tasks/passes/P0060-owned-append-console.md).
P0071 extends this rule to [operands that return a proven join](#console-output-of-appends-with-proven-returned-joins-p0071).

## Append arguments to Console wrappers (P0061)

In a complete `view(text += makeText());` statement, `view` must be a synchronous
free function with a void return, one scalar string parameter, and a body that
only prints that parameter through `Console.Write` or `Console.WriteLine`.
When the destination owns its string and the operand passes the factory proof,
the append helper reads the destination after argument evaluation, installs
the join, and releases the factory operand. The wrapper reads the join and the
destination keeps it for later reads and updates.

Eight isolated calls per mode moved from 16 releases and eight live
allocations to 24 releases and zero live allocations. The two-statement
wrapper control follows general expression emission and retains its original
factory allocation until shutdown. General calls and nested expressions remain
open.
See the [P0061 pass record](../../tasks/passes/P0061-owned-append-wrapper.md).
P0072 extends this wrapper rule to [proven returned joins](#append-arguments-with-proven-returned-joins-p0072).

## Direct append returns with proven factory operands (P0063)

In `return (text += makeText());`, a synchronous string function can consume
the factory operand when `text` is a known-owned local scalar string and the
entire operand passes the existing factory proof. After argument evaluation,
the append helper reads the current local, creates the join, replaces the old
local value and releases the factory operand. The return hands the original
joined pointer to the caller. Null contributes empty text, and a borrowed
operand follows ordinary append emission.

At P0063 the eight-call audit measured 24 nonnull releases and eight live joins
in each mode. Before P0063 it measured 16 releases and 16 live allocations,
including eight factory operands. Cleanup of other locals at these returns,
nested result expressions, async and exception unwinding remain open.
See the [P0063 pass record](../../tasks/passes/P0063-owned-append-return.md).

## Scalar initializers that adopt returned joins (P0064)

In `string snapshot = joined();`, the declaration can take the original
joined allocation. The resolved synchronous free function must have exactly one
mutable scalar string local declared from a literal, an omitted initializer,
or a scalar string parameter, followed by a direct return appending to that
local. The entire append operand must meet the original factory proof.
Lowered imports qualify under the same checks. P0074 extends direct declarations
to [one additional returned append](#scalar-initializers-that-adopt-a-join-of-a-returned-join-p0074).

The caller evaluates the call once and normal declaration cleanup releases
the original join. The eight-call audit now sees 24 releases and zero survivors
in each mode. The checked regression verifies the exact allocation base and
an empty allocation registry after each of 10,000 calls per mode.
See the [P0064 pass record](../../tasks/passes/P0064-adopt-returned-joins.md).

## Complete assignments that adopt returned joins (P0065)

In `target = joined();`, a complete assignment to an already-owned scalar
string can adopt an original join under the one-append proof from P0064. The
call runs once in a statement-local temporary. Once the call finishes, the
assignment releases the current destination and installs the returned pointer.
In `target = joined(target)`, the callee can copy the old contents while they
are live. In `target = joined(target = "new")`, the outer assignment releases
the value installed by the inner assignment.

The eight-call audit measures 32 nonnull releases per mode, with survivors
down from eight to zero. Each mode checks 10,000 scopes with two assignments
each, verifying the release order, original allocation bases, and a peak of
four live allocations. Assignments to owned scalars inside headers,
initializers and chained expressions use the string copying helper. P0075
extends complete assignments to
[one additional returned append](#complete-assignments-that-adopt-two-returned-appends-p0075).
See the [P0065 pass record](../../tasks/passes/P0065-assign-returned-joins.md).

## Standalone calls that discard returned joins (P0066)

In a standalone `joined();` statement, the caller can release the original
join immediately after the call returns. The same two-statement factory proof
used for declarations and assignments applies, including to lowered imports.
The call and its arguments are evaluated once. The append helper releases
both operands before the caller releases the join at reference count one.

The isolated eight-call audit in each mode went from 16 releases and eight
live joins to 24 releases and zero survivors. The checked harness also verifies
the original allocation and an empty registry after each of 10,000 calls per
mode. Calls in `for` headers, conditional expressions and nested arguments
continue to use general expression emission.
See the [P0066 pass record](../../tasks/passes/P0066-discard-returned-joins.md).

## Direct Console output of returned joins (P0067)

A complete `Console.Write(joined());` or `Console.WriteLine(joined());`
statement can consume the original join when the resolved function passes the
same two-statement proof as a standalone discard. The call runs once, including
its arguments. The Console writer reads the original joined allocation, then
the caller releases that allocation. The proof also applies to lowered imports.

Conditional operands and unproven join factories use general expression emission.
See the [P0067 pass record](../../tasks/passes/P0067-print-returned-joins.md).

## Returned joins passed to Console wrappers (P0068)

A complete `view(joined());` statement releases the original join after
`view` returns when both the factory and the wrapper pass their ownership
checks. The synchronous void wrapper takes one scalar string and its entire
body prints that parameter with `Console.Write` or `Console.WriteLine`.
The caller evaluates the factory and its arguments once, then passes the
original allocation to the wrapper before releasing it. Imported functions
use the same checks after module lowering.

Eight isolated wrapper calls per mode now make 24 releases and leave no
live joins. The checked harness verifies the original pointer is live through
both writers and is released at reference count one. Conditional arguments,
forwarding wrappers, unproven factories, and calls inside larger expressions
continue to use general expression emission.
See the [P0068 pass record](../../tasks/passes/P0068-wrap-returned-joins.md).

## Complete appends with proven returned joins (P0069)

In a complete `text += joined();` statement to an owned scalar string
identifier, the caller consumes the operand if it satisfies the
[returned join proof](#scalar-initializers-that-adopt-returned-joins-p0064).
The caller evaluates the call once, appends its original result to the
value currently in the destination, then releases the original result.
The destination owns the newly concatenated allocation. Argument evaluation
can replace the destination before the append.

Eight isolated statements per mode go from 32 releases and eight live joins
to 40 releases and zero live joins.
See the [P0069 pass record](../../tasks/passes/P0069-append-returned-joins.md).

## Append initializers with proven returned joins (P0070)

In `string snapshot = (text += joined());`, an append to an owned scalar string
identifier can consume an operand that satisfies the
[returned join proof](#scalar-initializers-that-adopt-returned-joins-p0064).
The helper reads the destination after the call's arguments run, installs the
concatenation, and releases the original returned join. The declaration then
copies the installed value into its own managed allocation. Const snapshots
and `for`-header declarations follow the same rule.

The [checked regression](../../tests/basics/memory/owned_returned_join_append_initializer.dr)
verifies that changing the destination leaves the snapshot readable and that
releasing the snapshot leaves the destination readable. It also covers a
destination replaced during argument evaluation, a once-only side effect,
borrowed self-append, and cleanup after a zero-iteration loop.
See the [P0070 pass record](../../tasks/passes/P0070-initialize-from-returned-join-appends.md).

## Console output of appends with proven returned joins (P0071)

In a complete `Console.Write(text += joined());` or
`Console.WriteLine(text += joined());` statement, an append to an owned scalar
string identifier can consume an operand that satisfies the
[returned join proof](#scalar-initializers-that-adopt-returned-joins-p0064).
The append helper reads the destination after evaluating the call, installs
the concatenation, and releases the original returned join before the writer
receives the new value. The destination owns that value for later reads and
updates. Borrowed operands follow the ordinary append path.

The [checked fixture](../../tests/basics/memory/owned_returned_join_append_console.dr)
exercises both writers, destination replacement during argument evaluation,
a once-only increment, and borrowed self-append in ordinary and module-entry
programs. See the [P0071 pass record](../../tasks/passes/P0071-print-appends-with-returned-joins.md).

## Append arguments with proven returned joins (P0072)

In a complete `view(text += joined());` statement, an append to an owned scalar
string identifier consumes the operand when it satisfies the
[returned join proof](#scalar-initializers-that-adopt-returned-joins-p0064)
and the callee satisfies the [Console wrapper proof](#append-arguments-to-console-wrappers-p0061).
The caller evaluates the operand once, appends it to the current destination,
and releases the original join before the wrapper receives the new value.
The destination owns that value for subsequent reads and updates.

Borrowed operands and wrappers with additional statements follow ordinary
expression emission. The [checked fixture](../../tests/basics/memory/owned_returned_join_append_wrapper.dr)
covers both writers, replacement during operand evaluation, and a once-only
side effect in ordinary and module-entry programs. See the
[P0072 pass record](../../tasks/passes/P0072-wrap-appends-with-returned-joins.md).

## Direct append returns with proven returned joins (P0073)

In `return (text += joined());`, a synchronous string function can consume
the operand if `text` is an owned scalar string local and `joined()` meets
the [returned join proof](#scalar-initializers-that-adopt-returned-joins-p0064).
The helper reads the current destination after evaluating the call, installs
the concatenation, and releases the operand. The caller receives the new
concatenation. An unproven operand follows ordinary append emission.

At P0073, a caller that copied the final result left the original registered
until shutdown. P0074 qualifies direct scalar initializers for adoption.
The [checked fixture](../../tests/basics/memory/owned_returned_join_append_return.dr)
verifies caller access, destination replacement during argument evaluation,
and a once-only side effect in both compiler modes. See the
[P0073 pass record](../../tasks/passes/P0073-return-appends-with-returned-joins.md).
P0083 extends this return rule to
[two proven returned appends](#direct-append-returns-with-two-returned-appends-p0083).

## Scalar initializers that adopt a join of a returned join (P0074)

In `string snapshot = nested();`, the initializer adopts the final join when
`nested()` declares one qualifying mutable scalar string local and directly
returns `return (local += joined());`. The operand call must meet the
[one-append factory proof](#scalar-initializers-that-adopt-returned-joins-p0064).
Both functions must satisfy the existing binding and parameter checks, including
for imported functions. Each append releases its operand and the caller's scope
cleanup releases the original final join at reference count one.

P0074 qualified two returned appends. P0084 extends direct initializer adoption
to three. A conditional initializer keeps its independent copy. See the
[P0074 pass record](../../tasks/passes/P0074-adopt-nested-returned-joins.md) and
[current initializer rule](#scalar-initializers-that-adopt-three-returned-appends-p0084).

## Complete assignments that adopt two returned appends (P0075)

A complete `target = nested();` statement can adopt the final allocation
with the factory proof bounded at two returned appends.
The destination must be an already-owned scalar string identifier.
The right-hand side evaluates once. After the call, the assignment releases
the current value and installs the original final join. If an argument
replaces the destination, the outer assignment releases that replacement.

Assignment expressions keep their existing fallback. P0085 extends the
[complete assignment proof](#complete-assignments-that-adopt-three-returned-appends-p0085)
to a third returned append. See the
[P0075 pass record](../../tasks/passes/P0075-assign-nested-returned-joins.md).

## Standalone calls that discard two returned appends (P0076)

A complete `nested();` statement uses the factory proof bounded at two
returned appends.
The caller evaluates the call and its arguments once and releases the original
final join immediately after normal return. This also applies to imported
functions and parenthesized standalone calls.

Loop headers, conditional expressions, nested arguments, and calls beyond two
returned appends follow ordinary expression emission. See the
[P0076 pass record](../../tasks/passes/P0076-discard-nested-returned-joins.md).

## Direct Console output of two returned appends (P0077)

In a complete `Console.Write(nested());` or `Console.WriteLine(nested());`
statement, the caller uses the factory proof bounded at two returned appends.
The factory and its arguments run once. The writer reads the original final
join, and the caller releases it immediately after the writer returns.

This rule covers ordinary and imported factories. Appends have their own
[Console rule](#console-output-of-appends-with-two-returned-appends-p0081).
P0087 extends direct output to the
[three-append proof](#direct-console-output-of-three-returned-appends-p0087). See the
[P0077 pass record](../../tasks/passes/P0077-print-nested-returned-joins.md).

## Console wrappers for two returned appends (P0078)

A complete `view(nested());` statement uses the
[two-append factory proof](#scalar-initializers-that-adopt-a-join-of-a-returned-join-p0074)
when the resolved wrapper's entire body prints its sole scalar string parameter
with `Console.Write` or `Console.WriteLine`. The factory and its arguments
evaluate once. The wrapper receives the original final join, and the caller
releases it after the wrapper returns. The rule also covers imported definitions.

Compound arguments and consumers that fail the
[wrapper proof](owned-call-argument.md#p0056-implementation-boundary) follow
ordinary emission. P0088 extends the factory limit to
[three returned appends](#console-wrappers-for-three-returned-appends-p0088).
See the [P0078 pass record](../../tasks/passes/P0078-wrap-nested-returned-joins.md).

## Complete append statements with two returned appends (P0079)

A complete `text += nested();` statement accepts the existing
[two-append factory proof](#scalar-initializers-that-adopt-a-join-of-a-returned-join-p0074)
when `text` is a known-owned scalar string identifier. The caller evaluates
the factory and its arguments once and uses the original final join as the
operand. The append then reads the current destination, installs the new
concatenation, and releases the operand. If an argument replaces `text`, the
new concatenation begins with that replacement.

The rule applies to complete statements in ordinary and module-entry code. P0089
extends the limit to [three returned appends](#complete-append-statements-with-three-returned-appends-p0089).
See the [P0079 pass record](../../tasks/passes/P0079-append-nested-returned-joins.md).

## Append initializers with two returned appends (P0080)

In `string snapshot = (text += nested());`, the append accepts the
[two-append factory proof](#scalar-initializers-that-adopt-a-join-of-a-returned-join-p0074)
when `text` is a known-owned scalar string identifier. The factory and its
arguments run once. The append reads the current destination, installs the
concatenation, and releases the factory's original final join. The declaration
copies the installed value into independent managed storage.

The same rule applies to const snapshots and `for`-header declarations.
Borrowed operands still use the ordinary append helper. Both ordinary and
module-entry regressions check replacement during argument evaluation,
snapshot survival after destination replacement, and destination survival
after snapshot cleanup. P0090 extends this rule to
[three returned appends](#append-initializers-with-three-returned-appends-p0090).
See the
[P0080 pass record](../../tasks/passes/P0080-initialize-from-nested-join-appends.md).

## Console output of appends with two returned appends (P0081)

In `Console.WriteLine(text += nested());` and `Console.Write(text += nested());`,
an append to a known-owned scalar string accepts the
[two-append factory proof](#scalar-initializers-that-adopt-a-join-of-a-returned-join-p0074).
The factory and its arguments run once. The append reads the current
destination, installs the concatenation, and releases the factory's original
final join before the writer runs. The writer reads the new concatenation,
which stays owned by the destination after output.

The ordinary and module-entry regressions cover both writers, argument
evaluation that replaces the destination, and borrowed self-append through
the ordinary append helper. P0092 applies the same rule to
[three returned appends](#console-output-of-appends-with-three-returned-appends-p0092).
See the
[P0081 pass record](../../tasks/passes/P0081-print-appends-with-nested-joins.md).

## Console wrapper append arguments with two returned appends (P0082)

A complete `view(text += nested());` call accepts the
[two-append factory proof](#scalar-initializers-that-adopt-a-join-of-a-returned-join-p0074)
when `text` is an owned scalar string and `view` meets the
[Console wrapper proof](owned-call-argument.md#p0056-implementation-boundary)
for either writer. The operand and its arguments run once. The append uses
the current destination after operand evaluation, installs the concatenation,
and releases the original final join before calling the wrapper. The
destination keeps the concatenation after the wrapper returns.

Borrowed operands and wrappers with extra statements use ordinary append
emission. The [checked fixture](../../tests/basics/memory/owned_nested_join_append_wrapper.dr)
exercises both writers, replacement of the destination during argument
evaluation, borrowed self-append, and the extra-statement wrapper in ordinary
and module-entry modes. See the
[P0082 pass record](../../tasks/passes/P0082-wrap-appends-with-nested-joins.md).

## Direct append returns with two returned appends (P0083)

In `return (text += nested());` from a synchronous string function, the
append accepts the
[two-append factory proof](#scalar-initializers-that-adopt-a-join-of-a-returned-join-p0074)
when `text` is an owned scalar local and the return is outside an exception
region. The operand and its arguments run once. The append reads the current
local, installs the concatenation, and releases the factory's original final
join. The installed value survives the return and the caller can copy it.

Borrowed operands and operands beyond two proven returned appends use ordinary
append emission. A direct scalar initializer can adopt the resulting third join
when the entire factory meets the proof. The regression covers argument
evaluation that replaces the destination, an unbraced conditional return, caller
reads, and both fallback cases in ordinary and module-entry modes. See the
[checked fixture](../../tests/basics/memory/owned_nested_join_append_return.dr),
[P0083 pass record](../../tasks/passes/P0083-return-appends-with-nested-joins.md),
and [current initializer rule](#scalar-initializers-that-adopt-three-returned-appends-p0084).

## Scalar initializers that adopt three returned appends (P0084)

In `string snapshot = extended();`, a direct initializer adopts the factory's
original final join when `extended()` meets the factory proof with three returned
appends. The outer factory declares one qualifying mutable scalar local and
returns its append of a call that meets the two-append proof. Each returned
append releases its operand, and scope cleanup releases the original final join
at reference count one. Imported functions use the same proof on their lowered
definitions. The call and each argument evaluate once.

Calls beyond three returned appends and conditional initializers keep their
copies. See the
[P0084 pass record](../../tasks/passes/P0084-adopt-three-returned-appends.md).

## Complete assignments that adopt three returned appends (P0085)

In `target = extended(target);`, a complete `=` statement to an owned scalar
string takes the original final join when the factory meets the
[three-append proof](#scalar-initializers-that-adopt-three-returned-appends-p0084).
The temporary holds the result while the assignment releases the current slot.
For `target = extended(target = "new");`, the argument replaces the first
join and the outer assignment releases `"new"` after the call. Scope cleanup
releases the second original join. The proof also applies to imported calls.

Assignment expressions and calls with four returned appends keep their ordinary
emission. See the [assignment contract](owned-call-assignment.md) and the
[P0085 pass record](../../tasks/passes/P0085-assign-three-returned-appends.md).

## Standalone calls that discard three returned appends (P0086)

A complete call statement such as `deeper(count++);` now uses the existing
factory proof with a limit of three returned appends. The caller passes the
original final join to `dr_release` as soon as the factory returns. Each
argument is evaluated once. The rule applies to resolved ordinary and imported
factory definitions.

Calls in loop headers, conditional expressions and nested arguments still
follow ordinary expression emission. Standalone calls with four returned
appends also follow ordinary emission. See the
[P0086 pass record](../../tasks/passes/P0086-discard-three-returned-appends.md).

## Direct Console output of three returned appends (P0087)

In `Console.Write(deeper(count++));` and `Console.WriteLine(deeper(count++));`,
the complete statement uses the [three-append factory proof](#scalar-initializers-that-adopt-three-returned-appends-p0084)
on the resolved ordinary or imported factory. The call and its arguments
evaluate once. The writer reads the original final join while it is live,
and the caller releases that pointer immediately after the writer returns.

Four returned appends and compound operands use ordinary emission. Appended
factory operands still use the two-append limit. See the
[P0087 pass record](../../tasks/passes/P0087-print-three-returned-appends.md).

## Console wrappers for three returned appends (P0088)

A complete statement such as `view(deeper(count++));` applies the
[three-append factory proof](#scalar-initializers-that-adopt-three-returned-appends-p0084)
to its sole argument when the resolved wrapper passes the
[Console wrapper proof](owned-call-argument.md#p0056-implementation-boundary).
The caller evaluates the factory and its arguments once. The wrapper passes
the original final join to `Console.Write` or `Console.WriteLine` while it is
live, and the caller releases the pointer immediately after the wrapper returns.
The rule covers ordinary and imported definitions.

Four returned appends, conditional arguments, and forwarding wrappers follow
ordinary emission. An append expression passed to a wrapper still accepts
factory operands with up to two proven returned appends. See the
[P0088 pass record](../../tasks/passes/P0088-wrap-three-returned-appends.md).

## Complete append statements with three returned appends (P0089)

In a complete `text += deeper();` statement, a direct factory operand can
qualify under the [three-append proof](#scalar-initializers-that-adopt-three-returned-appends-p0084)
when `text` is an owned mutable scalar string identifier. The caller evaluates
the factory and its arguments once, then appends its original final join to
the current value of `text`. If an argument replaces `text`, the concatenation
starts with the replacement. The caller releases the original join after
installing the concatenation in `text`.

Borrowed operands, conditional operands, and calls beyond three returned
appends use ordinary emission. P0090 applies the three-append proof to
[scalar declaration initializers](#append-initializers-with-three-returned-appends-p0090).
Append expressions used in output, wrapper calls, and returns use the
two-append proof. See the
[P0089 pass record](../../tasks/passes/P0089-append-three-returned-appends.md).

## Append initializers with three returned appends (P0090)

In `string snapshot = (text += deeper());`, an owned mutable scalar string
destination can consume an operand that satisfies the
[three-append factory proof](#scalar-initializers-that-adopt-three-returned-appends-p0084).
The factory call and its arguments evaluate once. The append reads the current
destination after evaluation, installs the concatenation, and releases the
factory's original final join. The declaration copies the installed value
into independent managed storage.

The rule covers ordinary and module-entry declarations, including const
snapshots and `for` headers. Borrowed and conditional operands and calls
beyond three returned appends use ordinary append emission. See the
[P0090 pass record](../../tasks/passes/P0090-initialize-from-three-returned-appends.md).

## Console output of appends with three returned appends (P0092)

Complete `Console.Write(text += deeper());` and
`Console.WriteLine(text += deeper());` statements accept the
[three-append factory proof](#scalar-initializers-that-adopt-three-returned-appends-p0084)
for an owned mutable scalar string destination. The factory and its arguments
evaluate once. The append reads the current destination, installs the new
concatenation, and releases the factory's original final join before the
writer reads the concatenation. The destination owns the printed value after
the writer returns.

The ordinary and module-entry regressions cover both writers, destination
replacement during argument evaluation, and borrowed self-append. Borrowed
operands and calls beyond the three-append proof use ordinary append emission.
See the [P0092 pass record](../../tasks/passes/P0092-print-appends-with-three-returned-appends.md).
