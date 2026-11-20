# Owned string call results: P0049 audit and P0050 adoption

P0037-P0048 preserve selected owned local allocations across a string return and
clean other qualified locals. At P0049 a caller copied every direct scalar string
call initializer. Its copy was released normally; the original owned result stayed
in the allocator registry until shutdown. The same return type can also describe
a literal, borrowed parameter/field, or a conditional choice between borrowed and
owned storage. Result type and allocation membership cannot establish ownership.

P0049 measured this boundary and defined the first caller adoption slice below.
P0050 implements that proof in `adopts_string_call` without changing the C ABI.
The audit tables describe the pre-adoption checkpoint; current evidence follows.

## Source audit

| Path | Source evidence | P0049 consequence |
| --- | --- | --- |
| Function result model | [ast.h](../../src/parser/ast.h), `Node.as.func`; [context.h](../../src/codegen/context.h), `VarBinding` | Functions have a return type/body but no owned/borrowed result metadata. `owns_string` belongs to local bindings, not call expressions. |
| Direct free-function lookup | [stmt.c](../../src/codegen/stmt.c), `cg_register_program`, `cg_function` | Searches definitions in the registered program by exact function name. A future proof must resolve the actual function and exclude a variable binding with the same spelling. A name or string return type alone is insufficient. |
| Modules | [modules.c](../../src/driver/modules.c), `rewrite_node` | Imported and entry free-function calls become identifiers with lowered names. Use the actual lowered definition/body, not source spelling, an export flag, or a module-name heuristic. |
| Calls and methods | [expr.c](../../src/codegen/expr.c), `expr_member`, `primitive_shape`, `ND_CALL` | Call classification supplies type/shape, not ownership. Method emission resolves receiver, inherited owner and implicit receiver separately; exclude methods from the first adoption slice. |
| Local call initializer | [stmt.c](../../src/codegen/stmt.c), `copies_string_initializer`, `ND_VAR_DECL` | Every direct scalar call initializer emits `dream_string_copy(call())` and marks the destination owned. Caller scope cleanup releases only the copy. |
| Returns | [stmt.c](../../src/codegen/stmt.c), `owned_string_result`, `is_scoped_string_result`, `emit_return_cleanup`, `emit_conditional_return_cleanup`, `ND_RETURN` | An owned local result preserves its allocation/reference; literal and qualified parameter/member results borrow. A conditional function can return either kind on different invocations. Async and active exception exits have different guards. |
| Other consumers | [expr.c](../../src/codegen/expr.c), `ND_BINOP`, `ND_CALL`; [stmt.c](../../src/codegen/stmt.c), `ND_EXPR_STMT`, `ND_CONSOLE_CALL` | Owned identifier replacement copies the RHS, append allocates a concatenation, ordinary arguments/console/discards pass or ignore the pointer. None consumes an owned call result. |
| Runtime | [custom.c](../../src/runtime/extensions/custom.c), `dream_string_copy`, `dream_concat`; [codegen.c](../../src/codegen/codegen.c), generated `__dream_string_assign`, `__dream_string_append` | Copying is independent allocation, not retain/adopt. Replacement/append release the destination's old allocation, without releasing the original RHS call result. Null copies remain null. |
| Allocation is not ownership | [memory.c](../../src/runtime/memory/memory.c), `dr_alloc`, `dr_release`, `dr_release_all` | Managed allocations begin with one reference. `dr_release` expects a live allocation base; it cannot safely inspect literals or interior pointers. Even registry membership would not distinguish a caller-owned borrowed base from a transferred result. Shutdown reclaims outstanding allocations but does not bound accumulation during execution. |

## Reproducible checked evidence

Build with the pinned toolchain, then run this audit with no competing compiler
writer (the driver uses shared output paths):

```sh
zig build
python codex/python/owned_result_audit.py
```

The [audit helper](../../codex/python/owned_result_audit.py) contains four complete
source programs, expected generated C and exact output. Each runs in ordinary and
module-entry modes with an actual import. It uses the unchanged
[checked allocator harness](../../codex/c/indexed_string_harness.c) and real runtime:
every nonnull generated release must target a live base with reference count one;
remaining allocations must retain their original references; shutdown must empty
the registry. It also runs the normally linked program and cross-links the checked
program for Linux on Windows. No generated ownership statement or harness assertion
is removed. The historical counts below document P0049 debt. P0050 supersedes
only `owned_initializers`: it now requires six releases and zero live survivors
in each mode, with direct caller initializers. The other three probes are unchanged.

Measured on Windows, 2026-11-17 UTC, Zig 0.17.1, in **each** mode:

| Program | Nonnull releases | Live before shutdown | Interpretation |
| --- | ---: | ---: | --- |
| `owned_initializers` | 6 | 5 | Three loop calls returning an owned literal local, one parameter snapshot factory and one empty local result leave five original allocations. The caller releases its five nonnull copies and backing; a null local result adds neither allocation nor release. |
| `borrowed_initializers` | 7 | 0 | Literal, unchanged parameter, instance-field and static-field result copies are independent. Four copies, receiver, old backing and replacement backing are released. Borrowed fields are cleared before replacing backing; the snapshot survives. |
| `mixed_initializers` | 4 | 1 | One owned and one borrowed selection from the same function. The unselected local, two caller copies and backing are released; only the selected owned original survives. |
| `other_consumers` | 3 | 5 | Replacement, append, console, discarded call and ordinary argument each leave one original result. The destination's initial, replaced and concatenated allocations are released. |

The audit validates live counts/refcounts and source-derived allocation tallies;
it does not record allocation identities or peak bytes. For the repeated owned
initializer loop, the source plus five-live result demonstrates accumulation even
though each caller destination has left scope. Increasing that loop would add one
survivor per nonnull result. No unbounded stress run or native Linux audit execution
is claimed. The first adoption slice addresses direct initializers only.

## Why blanket release, retain or adoption fails

`func string literal() { return "literal"; }` returns static bytes: releasing them
would read an invalid allocator header. `func string borrowed(string source) {
return source; }` may return the caller's managed base: releasing it consumes the
caller's reference; adopting it creates two owners without retention. The borrowed
field readers have the same problem and can also point at static bytes. Retaining
all results has the same invalid-header problem for literals and does not consume
an already transferred reference.

`mixed` in the audit has a qualified conditional return but no uniform ownership:
one invocation returns its owned local, another its borrowed argument. P0048's
read-only proofs protect backing during callee cleanup; they do not turn borrowed
results into transferable references. A managed-looking pointer, a known callee,
or a cleanup-eligible return is insufficient evidence for caller adoption.

## Selected first implementation slice

Adopt a result only for a **complete direct free-function call initializer of a
scalar string declaration**, when a deliberately small definition proof succeeds.
Keep existing parameter, member, static and conditional return predicates intact.
Use a separate conservative proof of transferred result ownership:

1. Resolve an identifier call to its actual registered free-function definition
   after module lowering. Reject unresolved/external calls, methods/receivers and
   any active caller binding shadowing that identifier. Require synchronous string
   return and a definition body. Do not infer from function name, return type alone,
   or an expression's allocation status.
2. Restrict the entire body to exactly two statements: a scalar nonstatic string
   local declaration followed by a terminal direct return of **that binding**.
   The declaration may be const. Its initializer must be a string literal, null,
   absent (the emitted null initialization), or a reference to a declared scalar
   string parameter. Restrict parameters to scalar primitive shapes. Resolve the
   parameter/local identity; do not confuse same-spelled names or a field/global
   with a parameter. Parentheses already erased by parsing require no new rule.
3. This body always reaches the sole return, creates independent storage or null,
   exposes no alias, and has no intervening effects or exceptional context. Its
   return is already preserved by `owned_string_result`; no callee transformation
   is needed. Reject any extra statement, nested block, alternative/conditional
   return, loop, call/concat/read-line/assignment initializer, write, alias, field,
   custom type, pointer/array, async/exception node or unknown form. Fail closed;
   do not build a general return-flow or recursive call-graph analysis here.
4. At the eligible scalar declaration, emit `string`'s existing C pointer type
   initialized directly by the call once, without the outer `dream_string_copy`.
   Keep `owns_string` true so existing block/jump/qualified-return cleanup owns or
   transfers exactly that original reference. No retain, extra allocation,
   immediate release or new runtime ownership tag is required. Null stays null.
5. All other initializers keep their existing copy behavior. A single unsupported
   definition detail prevents adoption. Replacement, append, console, discarded
   calls, ordinary/nested call arguments, call forwarding returns and compound
   initializers remain separate. Evaluating arguments still follows existing C
   emission; this slice adds no nested temporary cleanup or evaluation ordering.

The selected proof admits useful snapshot factories such as:

```dream
func string snapshot(string source) {
    const string result = source;
    return result;
}
string value = snapshot(backing);
```

The callee's copy remains independent of `backing`; the caller adopts that one
allocation and releases it at its existing scope boundary. Existing C signatures
remain unchanged. Library callers without a visible proven definition continue
to use current rules. General ownership annotations or a tagged return ABI would
be a different project and are not prerequisites for this bounded proof.

## Implementation acceptance (P0049 contract)

- Start with intact pre-fix regressions showing the direct-initializer extra copy
  and retained originals, in ordinary and module-entry/imported-function modes.
  Include definitions declared after use and lowered-name/caller binding guards.
- For eligible factories, assert exact generated C: call once, no outer copy or
  retain, original callee allocation/reference adopted and released once. Check
  allocation identity/count explicitly in the new evidence; the audit's final
  live counts alone would not distinguish adoption from copy-then-release.
- Cover literal, escaped/empty, absent/explicit-null and scalar parameter sources;
  const/grouped declarations, nested scopes, zero/multiple-iteration for headers,
  body locals, top-level storage, break/continue, bare/primitive/string returns and
  temporary-name collisions. Prove caller backing and independent snapshots
  survive replacement and cleanup. Repeated eligible declarations must leave no
  original results at shutdown; null adds no allocation/release.
- Assert unchanged C for every body/initializer/shape/async/exception guard above,
  borrowed literal/parameter/instance/static calls, mixed owned/borrowed and mixed
  owned/literal returns, unresolved/method calls, compound initializers and other
  consumer contexts. Do not execute unsafe alias/dangling/exception controls.
- Preserve P0037-P0048's qualified-return and checked allocator assertions. Their
  caller-copy expectations/counts may change only where the new proof succeeds;
  independently explain each genuine fixture/count migration. Supersede the
  corresponding audit baseline rather than treating expected retention as the
  new feature's acceptance. Run required full/quick/Python/cross checks and CI.

Caller-owned arguments and field backing must already be valid. General return
flow, mixed-provenance calls, method ownership, other consumers, recursive results,
pointer/class transfer, aliases/containers/destructors/fields, async and exception
unwinding remain open. No skipped fixture, grammar or semantic shape is broadened.

## P0050 implemented boundary and evidence

`adopts_string_call` resolves the actual registered lowered free-function body,
rejects caller bindings with the callee name and checks the exact two-statement
definition above. The destination keeps `owns_string`; only the outer copy is
omitted. Existing callee return, jump, block and qualified-return cleanup remain
unchanged. Methods, borrowed/mixed/effectful/recursive factories, unsupported
initializers and all other consumers preserve their previous emission.

The [regression](../../codex/go/owned_call_initializer_test.go),
[Dream fixture](../../tests/basics/memory/owned_call_initializer.dr) and
[checked harness](../../codex/c/owned_call_initializer_harness.c) check actual
ordinary/module-entry/imported definitions, forward definitions, literal/escaped/
empty/const/grouped calls, absent and typed-null parameter copies, nested/header/
loop/break/continue paths, early bare/primitive returns and an adopted local
transferred through a string return. Caller snapshots survive backing replacement.
Each identity probe makes exactly one copy and releases that exact original base
with reference count one. Ten thousand repetitions have peak live count one and
an empty registry after each call. A separately called transfer probe returns the
original allocation without copying or releasing it; the harness releases it once.
Top-level storage keeps program lifetime: exactly its original allocation remains
at shutdown, then the registry is empty. This is distinct from scoped accumulation.

Direct `string result = null` and string declarations from async calls are already
rejected by semantic analysis; the defensive AST proof does not broaden them.
Typed-null parameter snapshots execute instead. String `==` retains its existing
pointer comparison, so empty-string output is checked through Console operations.
Shadowing guards are defensive: source shadowing is already rejected by semantic
or module validation. Unsafe shape/recursive/effect/exception controls are
emission-only, never executed. Array/custom-type parameter syntax remains outside
the grammar. No general flow, ABI, ownership annotations or argument-temp cleanup
is introduced. Nested owned call arguments can still accumulate independently.

P0051 completes the [plain-assignment audit and contract](owned-call-assignment.md):
select complete assignment statements only, preserve once-only RHS evaluation before
old-slot release, and require original-pointer evidence before adoption is implemented.
General assignment-expression and other-consumer behavior remains unchanged. See
[P0050](../../tasks/passes/P0050-owned-call-initializer.md) for measured validation.


P0052 now implements the [complete-statement assignment slice](owned-call-assignment.md).
The other-consumer audit retains four originals per mode instead of five; its three
nonnull releases and all other consumer behavior remain unchanged.

P0053 defined the [discarded complete factory-call statement contract](owned-call-discard.md).
P0054 implements immediate release of the original result after normal call
completion under the same factory proof. The aggregate audit now measures four
nonnull releases and three retained originals per mode, from append, Console
output and argument consumption.

P0064 extends declaration adoption to a [direct append return with a proven
factory operand](string-ownership.md#scalar-initializers-that-adopt-returned-joins-p0064).
The caller releases the same allocation that concatenation created. Later
passes apply the returned-join proof to [assignments](string-ownership.md#complete-assignments-that-adopt-returned-joins-p0065),
[discards](string-ownership.md#standalone-calls-that-discard-returned-joins-p0066),
[direct Console output](string-ownership.md#direct-console-output-of-returned-joins-p0067),
and [Console wrapper arguments](string-ownership.md#returned-joins-passed-to-console-wrappers-p0068).
