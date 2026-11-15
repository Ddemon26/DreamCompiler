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
also applies at top level, before shutdown. Return and exception exits can bypass
this cleanup. P0024 qualifies owned scalar body-local cleanup on break/continue
as described below.

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
No return cleanup, dangling-result repair or ownership inference is introduced.

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
destruction. Return and exception/finally exits remain separate. Both quick protocols
and CI select the new matrix; all prior allocator assertions remain intact.

This is not a complete string ownership system. Other initializer forms, compound
assignment, parameter/field/index destination ownership, alias retention,
owning arrays/pointers and automatic element destruction, function returns,
early return/exception cleanup, class alias/container ownership,
compound expressions and field ownership remain unqualified. Concatenation and
read-line initializers keep their existing managed allocation; as replacements
they are copied, leaving temporary/result ownership unchanged.
Pointer shape compatibility and general indexed expression typing remain separate
work. P0016 qualifies direct indexed primitive console formatting, including scalar
string characters; it does not establish semantic argument/return shapes.
