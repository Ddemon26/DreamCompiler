# Scalar string initialization checkpoint

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
this cleanup, and cleanup of owned body locals skipped by break/continue remains
unqualified.

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
`TestCompiler_CallStrings` check fifteen copy sites, fourteen release sites,
fourteen nonnull runtime releases and nineteen exact output lines in each mode.
Literal/empty/borrowed/fresh/local-returned/null results, nested copies, static/
instance/base calls, zero/multiple loop iterations, const/grouped declarations
and top-level snapshots are included. An output marker proves once-only call
evaluation. Borrowed backing survives nested cleanup; the global snapshot survives
source replacement. Both modes failed the live-allocation assertion before the
fix. Existing registry/refcount assertions are preserved, including empty registry
after shutdown. Both quick protocols and CI select this group.

P0015 repairs the harness's Linux C11 build by matching `build.zig`'s
`_GNU_SOURCE` feature macro for exception support. Windows runs also cross-link
both modes for Linux before executing the native harness. Cross-linking checks
headers and linking; Linux runtime evidence still requires native CI.

This is not a complete string ownership system. Other initializer forms, string
reassignment, alias retention, array/pointer-local cleanup, function returns,
early return/exception cleanup, body-local cleanup skipped by break/continue,
compound expressions and field
ownership remain unqualified. Existing concatenations/read-line expressions are not
wrapped in copies by this pass; their temporary/result ownership is unchanged.
Pointer shape compatibility and general indexed expression typing remain separate
work. P0016 qualifies direct indexed primitive console formatting, including scalar
string characters; it does not establish semantic argument/return shapes.
