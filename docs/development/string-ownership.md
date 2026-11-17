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

For this append operation, null on either side contributes empty text; null plus
null and empty plus empty produce a nonnull managed empty string. This does not
change null handling for ordinary `+` or `=`. Operand ownership is unchanged:
borrowed sources remain readable, and fresh concatenation/call results can remain
registered until shutdown. Allocation failure/size limits follow the existing
`dream_concat` implementation; no new recovery policy is established. Const
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
