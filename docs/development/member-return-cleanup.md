# Borrowed member-return cleanup: P0043 audit and P0044 implementation

P0044 implements direct scalar nonstatic `this.field` string-result cleanup under
the whole-body read-only proof below. Both ordinary/module-entry modes reproduced
retained scoped allocations before the fix; each now checks 38 releases, 21 output
lines and empty pre/post-shutdown registries. Exact C, native checked allocator/
UBSan and 196 emission-only fallback/shape/guard controls pass. See the
[pass record](../../tasks/passes/P0044-borrowed-member-return.md) and
[ownership checkpoint](string-ownership.md#read-only-direct-instance-string-field-returns-p0044).

The P0043 source audit/design below remains the rationale. P0044 found explicit
class parameters and async instance methods are rejected by the existing grammar,
and receiver-name shadowing by semantics. Thus the executable first slice uses
explicit `this`; free/static readers accepting class parameters remain unavailable.
The metadata predicate recognizes depth-1 class bindings without adding syntax.
The audit's class-parameter examples are conceptual counterexamples, not accepted
Dream source. Valid controls use methods and previously declared alias classes;
self-type declarations inside a class are not registered yet. Async guarding is
checked with an accepted free function/static field, without runtime claims.

## What the current paths establish

| Path | Source evidence | Consequence |
| --- | --- | --- |
| Member lookup and shape | [expr.c](../../src/codegen/expr.c): `expr_member`, `primitive_shape`, `ND_FIELD`; [stmt.h](../../src/codegen/stmt.h): `cg_member` | Resolve the declaration, owning class, static flag, pointer marker and array length. A string token alone is insufficient. |
| Borrowed receiver binding | [context.c](../../src/codegen/context.c): `cgctx_push_decl`; [stmt.c](../../src/codegen/stmt.c): `emit_func_impl`, `emit_method` | Parameters and explicit `this` occupy depth 1. Their class pointer is borrowed; that says nothing about field backing. |
| Field reads and assignments | [expr.c](../../src/codegen/expr.c): `ND_FIELD`, `ND_BINOP` | A field read yields the existing pointer. The owned-string assignment helper applies to owned identifier bindings; field assignment emits plain assignment without copy or retain. |
| Local copies | [stmt.c](../../src/codegen/stmt.c): `copies_string_initializer`, `ND_VAR_DECL` | A scalar field-initialized declaration gets an independent copy without changing field ownership. |
| Return qualification | [stmt.c](../../src/codegen/stmt.c): `borrowed_parameter_result`, `parameter_unchanged`, `is_scoped_string_result`, `ND_RETURN` | Direct/conditional scalar parameter results have a whole-body slot-rebinding proof. Member returns leave following local cleanup unreachable. |
| Class cleanup | [stmt.c](../../src/codegen/stmt.c): `emit_scope_cleanup`, `return_releases_binding`; [memory.c](../../src/runtime/memory/memory.c): `dr_release` | Class locals are released without an alias-retain proof. Runtime release frees the payload without traversing fields or destroying their backing. |
| Semantic member typing | [analysis.c](../../src/sem/analysis.c): `analyze_expr` for `ND_FIELD` | Member existence and primitive token typing do not prove scalar shape, ownership, receiver lifetime or nonmutation. |

The [member-copy fixture](../../tests/basics/memory/member_string_copy.dr) checks
copies from literal, managed, null, static and inherited fields, caller backing
survival and constructor effects. It does not qualify member returns. P0041/P0042
return fixtures establish parameter-slot behavior, not transitive field lifetime.

## Why the parameter proof cannot simply be reused

These are source-derived counterexamples for future regression construction;
they were not executed during this documentation pass. Assume a declared class
`Box` with scalar `string value` and a valid caller-owned `box`:

```dream
func string reboundField(Box box) {
    string local = "local";
    box.value = local;
    return box.value;
}
```

The receiver slot is unchanged. Releasing `local` after capturing `box.value`
would invalidate the result and the caller's field. `parameter_unchanged` rejects
assignment to the receiver identifier itself; a field destination passes that
check. Writing `other.value` is equally relevant if `other` aliases `box`.

| Barrier | Example shape | Why a shallow scan is insufficient |
| --- | --- | --- |
| Receiver rebinding | `box = other;` | The incoming object is no longer the receiver. |
| Direct/aliased field write | `box.value = local;` or `other.value = local;` | Backing can become callee-local through either spelling. |
| Free/static/instance call | `store(box, local);` or `other.store(local);` | A callee can mutate the same payload without rebinding `box`. |
| Constructor | `new Writer(box, local);` | Constructor bodies can mutate passed receivers. Allocation syntax is not purity. |
| Class alias local | `Box alias = box;` | No retain is added. Even an earlier nested-block cleanup can free the borrowed receiver. |
| Later/nested effect | Write/call in a later statement, loop update, initializer or finally body | It may execute before an earlier syntactic return on another iteration/path. |
| Different result shape | Static field, string pointer/array, nested/base/call receiver | Token typing and parameter metadata do not establish the selected slice's proof. |

Copying every result would change ownership/allocation behavior and still leave
field/receiver alias bugs unresolved. Comparing only current local string slots
cannot protect already-released nested aliases or the caller's receiver. Neither
is the selected repair.

## Selected first slice: read-only borrowed class receiver

Implement only a direct scalar string instance-field result `receiver.value`
(parentheses may already be removed), in a synchronous function or method outside
an active exception context. Explicit `this.value` may qualify. Resolve inherited
fields through the same owner lookup/casts as ordinary emission.

The receiver must be a direct identifier resolving to a depth-1, zero-indirection
class binding. Resolve the field using its registered class declaration; require
an actual nonstatic string variable member with no pointer marker or array length.
Reject type-name/local/global/struct receivers, nested field/index/call receivers,
`ND_BASE` and conditional/compound results. Do not broaden P0042's conditional
leaf predicate in this slice.

Use a separate whole-body proof with an explicit AST allowlist. Preserve P0041's
less restrictive parameter proof and its accepted call/assignment cases:

- Visit the entire body, including all branches, initializers, loop headers and
  updates, switch selectors/cases and returns, even unreachable code. Reject
  same-name declarations anywhere rather than infer shadowing relationships.
- Reject every assignment/compound assignment and increment/decrement regardless
  of destination. This deliberately rejects harmless scalar updates along with
  alias/receiver/field/index writes; it requires no alias analysis.
- Reject all ordinary/method calls, constructor/new expressions, console operations,
  await, try/throw, nested declarations and unknown/error nodes. The first slice
  needs no interprocedural purity or exception-unwinding inference.
- Allow local declarations only of primitive scalar storage without explicit
  pointers/arrays; recursively qualify initializers. Reject all custom-type locals
  throughout the body, including already-exited nested blocks: their cleanup can
  destroy a borrowed class alias. Other parameters stay borrowed; writes through
  them and calls involving them are already barred.
- Allow literals/identifiers, field/base/index reads, nonmutating unary/binary and
  conditional expressions, blocks, expression statements, conditionals, loops,
  switches, break/continue and returns only if every child qualifies. An absent
  optional child is harmless. Explicitly allow operators; new/unknown operators
  must fail the proof.

For valid incoming storage, no body operation can replace the field with a new
local value, mutate/free its backing or release a local class alias of its receiver.
Independent scalar string copies cannot own that incoming allocation. The caller
must still supply a live receiver and backing; this proof does not repair existing
dangling fields or add null-receiver checks. Synchronous single-threaded reasoning
does not qualify async or concurrent mutation.

Once all predicates hold and qualified locals exist, reuse the collision-safe
temporary to capture the field pointer exactly once before reverse local cleanup.
Release only already-qualified local bindings. Never release/retain the receiver
or field pointer, copy the result or add field destruction. Without qualified locals,
retain direct return. Any failed proof preserves the entire previous return;
do not partially insert cleanup. Preserve binding metadata for other branches,
normal fallthrough and unbraced conditional returns.

The initial restriction excludes independently allocated class locals and apparently
read-only calls too. Relaxation requires a separate proven slice.

## P0043 acceptance contract and P0044 qualification

These were future requirements in P0043. P0044's measured results and the grammar
limits above qualify the implemented slice; broader cases remain future work:

1. Add a real Dream regression and checked Go lifetime group in ordinary/module-entry
   modes. Before fixing the compiler, reproduce scoped allocations retained at a
   read-only member return. Independently count releases/allocations/output; require
   an empty pre/post-shutdown registry, original caller references and live release
   operands for the positive matrix.
2. Execute free/static/instance readers, explicit `this`, inherited owner lookup,
   nested primitive locals and independent snapshots; literal/managed/empty/typed-null
   backing; early/unbraced/alternate paths; binding/lowered-name temporary collisions
   and no-local direct returns. Check caller receiver/backing after every call and
   snapshot survival after later caller replacement.
3. Inspect exact C before host compilation: one capture before reverse releases,
   unchanged owner cast, no receiver/field release or result copy/retain. Preserve
   P0037-P0042 expectations and allocator/refcount assertions.
4. Add emission-only controls for every barrier above. Place writes/calls before
   and after early returns, in nested blocks and expression/header positions; use
   a second aliased parameter. Require both original returns and no new temporary/
   early cleanup. Do not execute dangling-alias examples to obtain output. Test
   exception/async guards without claiming runtime unwinding support.
5. Cover scalar/nonstatic lookup versus static/pointer/array and inherited fields,
   rebound/shadowed/local/struct/nested receivers, class aliases in exited scopes,
   unsupported operators/nodes and conditional member leaves. If grammar/semantics
   reject a proposed fixture earlier, record the limit and use a valid emission
   control without broadening semantics inside this pass.
6. Align quick selectors and CI; run focused tests with `-count=1`, Zig 0.17.1 full
   Dream suite, Python runner tests, supported Windows Go quick and Linux cross-build.
   Inspect native CI separately. Update feature/user/ownership docs with measured
   behavior only after implementation.

Field ownership, class aliases/destructors, pointer/array shape checks, conditional
member results, interprocedural effects, caller consumption and exception/async
cleanup remain separate. See the [ownership contract](string-ownership.md) and
[backlog](../../tasks/TODO.md).
