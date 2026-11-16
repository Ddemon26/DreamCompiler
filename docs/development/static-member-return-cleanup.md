# Static string-field return cleanup: P0046 audit

This is a source audit and implementation contract, not implemented return
support. P0045 qualifies direct and conditional read-only instance fields;
static field results still bypass early local cleanup. Static storage duration
keeps the pointer slot alive, but does not keep its pointed-to string alive.

## Current paths and lifetime

| Path | Source evidence | Consequence |
| --- | --- | --- |
| Static declaration | [stmt.c](../../src/codegen/stmt.c): `emit_type_decl` | Emits a separate `Type_field` C global, with an optional raw initializer. No managed field copy/retain or field destruction is installed. Uninitialized C globals are zero-initialized. |
| Inherited lookup | [analysis.c](../../src/sem/analysis.c): `sem_find_member`; [stmt.c](../../src/codegen/stmt.c): `cg_member`, `cg_type_decl` | Returns the first matching declaration and its actual owner. Do not assume the qualifier declares the field. |
| Field expression | [expr.c](../../src/codegen/expr.c): `expr_type`, `is_type_reference`, `ND_FIELD` | Static members emit the owning type's global symbol. The instance receiver expression is not emitted for a resolved static field. A variable binding can shadow a type spelling. |
| Field assignment | [expr.c](../../src/codegen/expr.c): `ND_BINOP` | Owned replacement applies to owned identifier bindings; a static field destination remains a raw C assignment. The global can point at callee-local storage. |
| Local snapshots | [stmt.c](../../src/codegen/stmt.c): `copies_string_initializer`, `ND_VAR_DECL`; [custom.c](../../src/runtime/extensions/custom.c): `dream_string_copy` | Scalar member-initialized locals own independent copies; null copies remain null. Releasing a snapshot does not release the original field backing. |
| Current result qualification | [stmt.c](../../src/codegen/stmt.c): `borrowed_member_result`, `is_static_string_result`, `is_scoped_string_result`, `ND_RETURN` | Member proof explicitly excludes static fields. Despite its name, `is_static_string_result` recognizes literals and literal-only conditional trees, not static fields. |
| Body proof | [stmt.c](../../src/codegen/stmt.c): `member_body_readonly`, `parameter_unchanged` | The member allowlist rejects all writes/calls, custom-type/pointer/array locals, exceptions and unknown nodes/operators. Parameter-slot immutability alone cannot protect a global's pointee. |
| Binding and release | [context.c](../../src/codegen/context.c): `cgctx_binding`, `cgctx_has_var`; [memory.c](../../src/runtime/memory/memory.c): `dr_release`, `dr_release_all` | Context lookup distinguishes variables from type names. Managed lifetime follows allocations/refcounts, not references stored in global fields. |

The [member-copy fixture](../../tests/basics/memory/member_string_copy.dr) and
[Go checks](../../codex/go/indexed_strings_test.go) cover static snapshots and
caller backing survival. The [member-return controls](../../codex/go/member_return_test.go)
preserve static and mixed instance/static fallback. Neither establishes working
static-result early cleanup. Static pointer/array declarations use a separate
emitter that does not preserve their shape; exclude them from this return slice
rather than repairing static storage emission incidentally.

## Why a global slot is insufficient

These are source-derived counterexamples, not executed regressions. Given a
previously declared `Store` with `static string text`, this body must not gain
cleanup merely because `Store.text` names a global:

```dream
func string unsafeReader() {
    string local = "local";
    Store.text = local;
    return Store.text;
}
```

Capturing the pointer and releasing `local` invalidates the result and global.
An assignment spelled through a derived type or instance can reach the same
owning symbol. A helper call or constructor can perform the write indirectly.
An earlier nested scope can leave the global already dangling before return;
preserving only active local slots cannot repair it. Later statements, loop
updates, conditions and unselected conditional arms also need scanning.

The caller can likewise install a pointer and release/rebind its backing before
entry. A read-only callee cannot establish its prior lifetime. The selected
contract requires valid incoming backing (or null), and synchronous single-threaded
execution. It adds no global ownership, alias analysis or concurrency guarantee.

## Selected implementation slice: direct class type-qualified field

1. Accept only a direct `ND_FIELD` string result whose object is an `ND_IDENT`
   resolving to a registered class type with **no variable binding** of that
   spelling in the current context. Resolve the actual member and owning
   declaration with normal lookup. Require `ND_VAR_DECL`, `is_static`,
   `TK_KW_STRING`, no pointer marker and no array length. Inherited class
   qualifiers may qualify; preserve the emitter's owning symbol.
2. Require synchronous function/static or instance method context, a complete
   function body and no active exception exit. Use the member whole-body AST and
   operator allowlist unchanged in substance. Reject all assignments/compound
   assignments, increment/decrement, calls/new/console/await/try/throw, nested
   declarations, custom-type/pointer/array locals and unknown nodes/operators.
   Reject declarations shadowing the type qualifier anywhere in the body,
   including later and exited scopes; inspect every initializer and child.
3. Share the read-only traversal by making its guarded name explicit, or add a
   separate equivalent proof. A type reference has no receiver `VarBinding`:
   do not fabricate borrowed-parameter ownership/depth metadata. Keep existing
   instance receiver/declaration guards and parameter proof unchanged. Do not
   add static fields to the literal-only predicate.
4. With qualified active locals, capture the pointer once in the existing
   collision-safe temporary before reverse cleanup. Add no field/result copy,
   retain, release or field destruction. Keep context bindings for other branches
   and unbraced returns. With no cleanup candidates retain direct return.
   A failed predicate preserves the entire previous return.

The first implementation excludes conditional static leaves, even when both
arms look safe; mixing static with instance/local/parameter leaves is a later
slice. Also exclude struct qualifiers and variable/`this`/`base`/nested/index/call
receivers, implicit field identifiers and pointer/array fields. Existing instance
and conditional-member support must retain its prior behavior and controls.

Rejecting harmless numeric writes, independent class locals and read-only calls
is intentional. Relaxing those barriers requires a separate proof. General
static initialization and pointer/array emission remain separate too.

## Acceptance for the implementation pass

- Add an annotated Dream fixture and checked Go group in ordinary/module-entry
  modes. First reproduce retained callee locals on a valid direct static return
  with a live-allocation registry. Independently count allocations, nonnull
  releases and output; retain live operand, refcount and shutdown assertions.
  No static-result owned survivors are expected in the direct slice.
- Cover literal/managed/empty/typed-null incoming backing; free functions and
  static/instance readers; inherited and hidden field owner selection; nested
  locals, unbraced/alternate/loop/header/switch paths, later declarations,
  temporary/lowered-symbol collisions and no-local returns. Use accepted grammar;
  record earlier parser/semantic rejections without broadening syntax.
- Before host compilation, inspect exact C for the owning global symbol, one
  capture before reverse cleanup, and no result copy/retain/global release.
  Check exact output, original caller backing/refcounts after calls, independent
  snapshot survival after later caller replacement, and empty pre/post-shutdown
  registries after caller allocations are cleaned. Clear borrowed static slots
  before their caller backing leaves scope; clearing a slot adds no release.
- Add emission-only barriers before/after early returns and in all nested/
  expression/header positions. Include direct/derived/instance-spelled writes,
  local backing replacement, unrelated writes, calls/new, aliases, shadowing,
  unsupported receiver/field shapes, mixed conditional leaves and async/exception
  guards. Do not execute dangling examples. Existing unsupported static controls
  that become eligible must move to positive lifetime/C coverage; retain their
  other assertions and preserve unsupported mixed conditional controls.
- Align both quick protocols and CI. Run focused external-compiler tests with
  `-count=1`, Zig 0.17.1 full Dream suite, Python runner tests, supported Windows
  Go quick and Linux cross-build. Native Linux runtime evidence requires CI.
  Keep P0037-P0045 lifetime checks and the eighteen skips intact.

See the [ownership contract](string-ownership.md),
[instance member proof](member-return-cleanup.md) and
[P0046 record](../../tasks/passes/P0046-static-member-return-audit.md).
