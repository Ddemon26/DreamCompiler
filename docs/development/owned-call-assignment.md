# Owned factory results assigned to existing string slots

P0051 audits the next T011 consumer after [P0050 declaration adoption](owned-call-result-consumption.md).
P0051 records the pre-fix audit and contract below. P0052 implements the complete-
statement boundary; see the implementation evidence at the end. The historical
audit was inspected at `4e0020436ab5997989eb10108ab3f61f43f4e4a7`.

## Current path and failure mechanism

| Stage | Source and finding |
| --- | --- |
| Statement | [stmt.c](../../src/codegen/stmt.c), `ND_EXPR_STMT`, delegates the complete expression to `cg_emit_expr`. For updates separately call the expression emitter. |
| Destination | [context.c](../../src/codegen/context.c), `cgctx_binding`, selects the innermost active binding. `VarBinding.owns_string` and indirection metadata distinguish owned scalar slots from borrowed parameters and pointer/array storage. |
| Assignment | [expr.c](../../src/codegen/expr.c), `ND_BINOP`, recognizes identifier `TK_EQ` destinations with `owns_string` and emits `__dream_string_assign(&slot, rhs)`. Append has its own path. No factory proof is used here. |
| Replacement | [codegen.c](../../src/codegen/codegen.c), generated `__dream_string_assign`, copies `text`, releases the current `*slot`, stores the copy and returns it. RHS evaluation finishes before helper entry. |
| Factory proof | [stmt.c](../../src/codegen/stmt.c), static `adopts_string_call`, is currently used only for declaration initializers. It resolves actual lowered free-function definitions and proves the two-statement independent local result. |
| Semantics | [analysis.c](../../src/sem/analysis.c), `ND_BINOP`, rejects identifier assignment to const variables. Code generation must preserve diagnostics and not infer legal assignment solely from ownership metadata. |

For `slot = factory();`, where the factory satisfies P0050, the original result
is transferred out of the callee. The assignment helper creates a second allocation,
releases the previous destination and stores the copy. No statement consumes the
original result. Repeated assignments therefore accumulate original results even
though the destination itself is replaced and eventually cleaned up.

The existing [P0049/P0050 audit](../../codex/python/owned_result_audit.py)
`other_consumers` case already checks this emitted helper call in both modes.
Its **three nonnull releases and five survivors per mode** are historical measured
evidence from P0050 on 2026-11-17, not a new P0051 run. One survivor comes from
replacement; append, console, discard and argument consumption account for four
others. Do not claim an isolated assignment measurement or allocation identity
from this aggregate probe. Its source-derived tally and exact C identify the debt;
the next implementation needs isolated original-pointer evidence.

## Selected implementation boundary

Select only a complete `ND_EXPR_STMT` whose expression is plain `TK_EQ`, whose
left side is an identifier resolving to an existing owned scalar string binding,
and whose entire right side is a direct call satisfying the unchanged P0050
factory proof. Require string type and zero indirections explicitly. Parentheses
erased by parsing do not introduce another expression context.

Share the existing proof through an internal codegen declaration rather than
duplicating it or widening its body/parameter rules. The statement emitter can
select this consumer before general expression emission; it must not enable
adoption recursively for all assignments in `cg_emit_expr`. For-header updates,
conditions, declarations initialized by assignment, nested/chained assignments,
argument and return expressions keep existing expression behavior in this slice.

Emit an internal adoption helper with the conceptual operation:

```c
static char *adopt(char **slot, char *owned) {
    dr_release(*slot);
    *slot = owned;
    return owned;
}
/* complete statement only */
adopt(&slot, factory(arguments));
```

The final internal name must be checked against emitted source names to avoid
collisions. The RHS appears exactly once. The address operand is a simple identifier
slot and has no value-reading side effect; C argument evaluation order does not
sequence multiple Dream arguments and this slice must not promise that it does.
Helper entry occurs after the factory and its arguments finish. Read and release
the **current** slot at helper entry, including any nested replacement in an
argument; do not capture the old pointer before evaluating the RHS.

In `slot = snapshot(slot);`, the factory first copies the old backing. Only then
may adoption release that backing. The proven result is independently allocated
or null and cannot alias the old live slot. Null releases the old allocation and
stores null; it does not allocate or require a release of the result. Keep the
binding's ownership metadata and all existing block/jump/return cleanup intact.
Top-level slots retain program lifetime, with old values released on replacement.

Borrowed literal/parameter/member/static results and mixed returns must still use
the copying helper. Pointer/array slots, borrowed parameters, field/index targets,
methods, async, extra-body/effectful factories, unresolved or shadowed functions,
append and compound RHS forms retain previous emission or existing diagnostics.
Never release a result merely because it has string type or managed-looking bytes.
Nested owned argument temporaries remain an independent unresolved consumer.

## Required implementation evidence

1. Before changing emission, add ordinary and module-entry/imported-definition
   tests that fail the adoption C assertion. Separately reproduce isolated
   assignment retention with the checked allocator; do not infer original-pointer
   adoption from a final live count alone. Keep the existing regression intact.
2. Require exactly one RHS call and no caller copy/retain. Instrument original
   factory allocation identity, old-slot release operands and reference count one.
   Repeated replacements must have bounded live storage, and final scoped cleanup
   must leave no survivors. Record peak storage appropriate to old and new values
   being simultaneously live during RHS evaluation.
3. Execute literal/empty/escaped/const-local factories, absent and typed-null
   results, parameter snapshots, forward/imported definitions and same-spelled
   borrowed versus owned lowered factories. Check `slot = snapshot(slot)`, separate
   snapshots surviving replacement, and nested argument replacement before helper
   entry. Use a primitive argument effect to establish once-only call evaluation.
4. Exercise scoped/unbraced/loop-body assignments, zero/multiple iterations,
   break/continue, early qualified returns, name collisions, initially null slots
   and top-level lifetime. Test for-header updates as unchanged fallback in this
   first slice. Preserve semantic rejection controls without generated artifacts.
5. Assert unchanged C for borrowed/mixed/method/shape/body barriers and excluded
   expression contexts. Unsafe aliases and dangling/exception controls stay
   emission-only. Preserve P0037-P0050 allocator/refcount assertions; explain any
   genuine migration. The old aggregate other-consumer audit should supersede
   only assignment: expected survivors decrease from five to four, while three
   nonnull releases remain (the adopted original replaces the released copy).
6. Run focused, full Dream, Go quick, Python and Linux cross-build checks, then
   inspect native CI. No new skipped tests or weakened assertions are acceptable.

General assignment-expression ownership, call forwarding, method/mixed result
flow, nested owned arguments, containers/fields/destructors, pointer/class transfer
and async/exception unwinding remain separate work. P0051 reruns no compiler tests;
reuse [P0050 results](../../tasks/MostRecentTestResults.md) with their date and limits.

## P0052 implementation

`cg_emit_stmt` now selects exactly the complete-statement boundary above and reuses
`adopts_string_call` in the same translation unit, without widening its proof.
Instead of adding a global helper, emit a block with a fresh `__dream_owned_N`
temporary, checked against active bindings and registered function names. Capture
the RHS once, release the current destination, and store the original pointer.
This implements the conceptual helper operation while avoiding a new global helper
name collision and leaving recursive expression emission unchanged. The destination
keeps its existing ownership metadata and cleanup. No runtime ABI changes.

Both ordinary and imported/module modes first reproduced three retained originals
from three isolated assignments. After adoption they retain zero, with the same
four nonnull releases. The aggregate audit changes only assignment: three releases
and four survivors instead of five. Borrowed/mixed/initializer audits are unchanged.
The new checked harness repeats ten replacements in each of 10,000 scoped calls:
eleven original allocations/releases per call, in allocation order, peak live count
two and zero survivors. A returned result is the original final pointer; its old
slot was released. The main fixture checks 19 copies/releases, self-snapshot,
nested replacement, once-only primitive effects, null/empty/escaped results,
loops/jumps/unbraced paths, temporary collisions and global lifetime. Existing
initializer controls additionally assert assignment fallback for all 28 unsupported
definitions in both modes. Header/chained/initializer expressions and borrowed
parameters retain previous emission. Native UBSan, Linux cross-links and directly
linked exact output are checked; cross-linking is not Linux execution.

See [P0052](../../tasks/passes/P0052-owned-call-assignment.md) for measured checks.
General assignment expressions and the remaining consumers still require their own
provenance and lifetime audits; no blanket result release is justified.
