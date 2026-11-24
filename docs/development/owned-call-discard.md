# Discarded proven-owned factory results

P0053 audited this T011 consumer at parent
`5a6e4ac3835f031e245f9d2050cbb5e905e10e4e`. P0054 implements the
selected boundary below. It builds on the unchanged
[factory proof](owned-call-result-consumption.md) and
[complete assignment implementation](owned-call-assignment.md).

## Source audit

| Stage | Finding at the audited parent |
| --- | --- |
| Statement AST | [stmt.inc](../../src/parser/stmt.inc), final expression-statement construction, stores the expression in `ND_EXPR_STMT`. A direct call can therefore be distinguished from an enclosing assignment, conditional or other consumer. |
| Statement emission | [stmt.c](../../src/codegen/stmt.c), `cg_emit_stmt` / `ND_EXPR_STMT`, selects owned assignment and Console cases, otherwise emits the expression followed by a semicolon. A discarded direct factory result has no destination binding or release. |
| Call emission | [expr.c](../../src/codegen/expr.c), `ND_CALL`, emits callee and arguments recursively. It adds no result cleanup. Field/base calls have a separate receiver path. |
| Actual definition | [stmt.c](../../src/codegen/stmt.c), `adopts_string_call`, resolves an identifier through `cg_function`, rejects caller shadowing, async and unsupported parameter/body/result shapes, and proves the independent local result. The proof is already available in the statement emitter. |
| Modules | [modules.c](../../src/driver/modules.c), `rewrite_node`, rewrites call identifiers and walks expression statements. The same proof must inspect actual lowered definitions, including imported definitions, rather than source names. |
| Allocation | [custom.c](../../src/runtime/extensions/custom.c), `dream_string_copy`, creates an independent `dr_alloc` base or returns null. Proven factory returns preserve that local allocation. |
| Release | [memory.c](../../src/runtime/memory/memory.c), `dr_release`, accepts null, otherwise reads the allocation header and decrements its reference. It cannot safely accept arbitrary literal or borrowed backing merely because the result has string type. |
| Loop headers | [stmt.c](../../src/codegen/stmt.c), `ND_FOR`, emits nondeclaration init, condition and update through `cg_emit_expr`. A statement-only change does not select those calls. |

For `owned();`, the callee transfers its local's reference, but the caller
discards the pointer. No binding records it for block, jump or return cleanup.
The allocation stays registered until global shutdown. Top-level discard also
has no surviving consumer and can release immediately; this differs from a
top-level declaration whose stored result has program lifetime.

The existing [audit](../../codex/python/owned_result_audit.py) `other_consumers`
case includes one discarded `owned()` alongside append, Console and argument
consumption. P0052 measured **three nonnull releases and four survivors per mode**
on 2026-11-17. Those are aggregate historical measurements, not an isolated
discard or allocation-identity experiment. The source path explains one survivor;
an implementation must reproduce isolated retention before claiming its removal.

## Selected implementation boundary

Select only a complete `ND_EXPR_STMT` whose entire expression is an `ND_CALL`
accepted by the unchanged `adopts_string_call`. Parentheses erased by the parser
do not create an extra consumer. Keep the proof static in `stmt.c`; do not widen
its two-statement body, primitive scalar parameter, local identity, synchronous
result or actual-definition requirements. No interprocedural metadata is needed.

Emit the conceptual C statement:

```c
dr_release(factory(arguments));
```

Use `cg_emit_expr` once for the whole selected call. The outer release runs only
after the call and its arguments finish. No caller copy, retain, new binding,
temporary, helper name or cleanup metadata is needed. The factory returns an
independent allocation at reference count one, or null; releasing it consumes
exactly the transferred reference. Null adds no allocation or nonnull release.
Do not remove the call as dead code: its arguments may have observable effects.
This adds no ordering between multiple arguments beyond existing C emission.

Parameter snapshots must finish copying before the result is released, and the
caller backing and independent snapshots must survive. A nested argument may
replace caller storage before the factory runs; release only the returned
independent pointer. Nested owned argument results still have their existing
unresolved lifetime: this slice consumes the outer result only. Tests for bounded
storage must isolate that outer result rather than hide nested accumulation.

All other calls and expression contexts retain current emission or diagnostics:
borrowed literal/parameter/instance/static results, mixed or conditional returns,
methods, async factories, extra/effectful/recursive bodies, unsupported parameter
shapes, unresolved/shadowed names, console/append/argument consumers, forwarding
returns, assignments, conditional expressions and for headers. Do not release
each `ND_CALL` recursively or infer ownership from managed-looking bytes.
Existing P0050/P0052 declaration and assignment behavior remains intact.

This release describes normal completion. If argument evaluation exits through
an exception or asynchronous operation before returning, it reaches no outer
release; no new unwinding guarantee follows. General exception/async cleanup,
aliases/containers/destructors/fields and pointer/class transfer remain separate.

## Required implementation evidence

1. Before changing code, add ordinary and module-entry/imported-definition
   regressions that fail the exact discard-release C gate. Separately measure an
   isolated repeated discard with the checked allocator at the unchanged parent;
   preserve this result even if the C gate prevents the main harness from running.
2. After implementation require exactly one call and immediate release of its
   original allocation, reference count one, with no copy/retain. Repeat at least
   10,000 isolated calls per mode: peak live count one, zero survivors after each
   call and no second release during scope exit or shutdown. Track allocation and
   release identities, not just final counts. Null has zero nonnull releases.
3. Cover literal, empty, escaped, const-local, absent and typed-null parameter
   factories, scalar parameter snapshots, forward/imported definitions and
   same-spelled borrowed versus owned lowered functions. Keep existing grammar
   rejection of direct null string declarations; do not broaden syntax for tests.
4. Check primitive argument effects occur once, backing/snapshots survive, and
   nested argument replacement finishes before outer-result release. Cover
   top-level, scoped, unbraced conditional, zero/multiple-iteration loop bodies,
   break/continue, switch and early-return paths. A discarded result must not
   remain in a scope's cleanup list.
5. Require unchanged C for every failed factory proof and excluded context,
   including for-header calls and nested owned arguments. Borrowed runtime
   controls must survive without an added release; unsafe dangling, alias and
   exception controls remain emission-only. Preserve existing semantic rejection
   diagnostics and absence of generated artifacts.
6. Preserve prior identity/refcount assertions. Explain each eligible discarded
   statement migration in existing fixtures. For the aggregate audit, source
   accounting predicts **four releases and three survivors per mode**: only its
   discarded original is newly released. This is future acceptance, not measured
   P0053 evidence; append/Console/argument survivors remain unresolved.
7. Run focused and required full Dream, supported Go quick (`-count=1`), Python
   runner and Linux cross-build checks; inspect native CI. No new exclusions or
   weakened assertions. Cross-linking alone does not prove native Linux execution.

## Implementation and measured behavior

P0054 emits `dr_release((void *)factory(arguments));` in the expression-statement
emitter, reusing the existing factory proof. The cast adapts the const-qualified
C return type to the allocator API. The call is emitted once, including its
arguments. There is no caller copy, retain or scoped binding.

The isolated eight-call audit changed from zero releases and eight live originals
to eight releases and zero live originals before shutdown in each mode. The
aggregate audit measured four releases and three survivors per mode. The checked
native harness independently verifies 10,000 calls per mode, one live allocation
at peak and zero after each call. Each release uses the original pointer at
reference count one. See the [pass record](../../tasks/passes/P0054-owned-call-discard.md)
for commands and remaining limits.

P0066 applies the same release to standalone calls that return a proven join.
See [standalone calls that discard returned joins](string-ownership.md#standalone-calls-that-discard-returned-joins-p0066).
P0076 extends the same release to
[two qualifying returned appends](string-ownership.md#standalone-calls-that-discard-two-returned-appends-p0076).
P0086 extends it to
[three qualifying returned appends](string-ownership.md#standalone-calls-that-discard-three-returned-appends-p0086).
