# Proven factory results passed to string parameters

P0055 audited direct argument consumption at parent
`04daec85aeee2d7c2a352e7d498bd5c24f7ac914`. The compiler still leaves
these results registered until shutdown. The selected implementation scope is a
complete call statement to a synchronous, single-parameter console wrapper.

## Source audit

| Stage | Finding |
| --- | --- |
| Call shape | [expr.inc](../../src/parser/expr.inc) builds `ND_CALL` with its callee and argument expressions. [stmt.inc](../../src/parser/stmt.inc) records a complete call as an `ND_EXPR_STMT`. The `Console.Write` and `WriteLine` intrinsics have a distinct `ND_CONSOLE_CALL` shape. |
| Resolution | [analysis.c](../../src/sem/analysis.c), `analyze_call`, checks the resolved function's argument count and declared types. This check establishes neither parameter ownership nor whether its value escapes. [modules.c](../../src/driver/modules.c), `rewrite_node`, recursively lowers both callee identifiers to their actual local or imported definitions. |
| Caller | [expr.c](../../src/codegen/expr.c), `ND_CALL`, emits nested calls as ordinary C arguments. [stmt.c](../../src/codegen/stmt.c) only consumes a proven outer result on direct discard. A call like `view(owned());` has a void outer result and creates no ownership binding for the inner result. |
| Consumer | [stmt.c](../../src/codegen/stmt.c), `emit_func_impl`, gives synchronous parameters depth-one borrowed bindings. The scalar cleanup helpers exclude these bindings. `parameter_unchanged` is used for borrowed parameter return analysis and allows calls that may forward or store the pointer. It cannot prove the parameter stays local. |
| Lifetime | [custom.c](../../src/runtime/extensions/custom.c), `dream_string_copy`, returns independent managed storage or null. A factory accepted by `adopts_string_call` transfers that reference. For the audited wrapper, [console.c](../../src/runtime/io/console.c) uses the parameter in a synchronous `printf` and keeps no copy of the pointer. |

The [checked audit](../../codex/python/owned_result_audit.py) verifies generated
C, exact checked and normally linked output, and allocation counts just before
shutdown. Results match in ordinary and module-entry compilation.

| Probe | Nonnull releases | Live originals | Observed behavior |
| --- | ---: | ---: | --- |
| Eight calls to `view(owned())` | 0 | 8 | Eight lines of `owned` |
| Same-signature consumer with `Vault.saved = source` | 0 | 1 | C assigns the original pointer to the static field. A second print reads it after the consumer returns. |
| `view(owned(), sibling())` | 0 | 1 | Generated C preserves both argument expressions in one call. The sibling prints once, then the wrapper prints `owned` and `7`. |
| Existing aggregate consumer probe | 4 | 3 | The append, Console and argument results account for the three survivors. |

The static store establishes a concrete escape with a synchronous void signature
and an unchanged parameter slot. A borrowed string return can also propagate the
value. An async wrapper stores parameters in its task arguments, and a throwing
callee can leave by `longjmp` before reaching any caller cleanup. These shapes
need different lifetime reasoning.

The sibling probe checks execution once and call shape. The C call syntax does
not establish a relative order between sibling argument evaluations. Pulling
one argument out into a preceding declaration would impose such an order. A
single outer argument avoids this problem and leaves evaluation inside the
factory call governed by the same C expression as before.

## Selected implementation boundary

Select a complete `ND_EXPR_STMT` containing a direct identifier call to an
actual synchronous free-function definition returning `void`. Require exactly
one scalar string parameter and one argument. Reject pointer/array shapes and
caller names shadowed by a variable. Require the complete callee body to be a
block containing exactly one statement, the intrinsic `Console.Write` or
`Console.WriteLine` of the unmodified parameter identifier. ReadLine, computed
operands, and all additional body statements are outside this proof. Match the
parameter in the resolved definition, including when the function was imported.

Require the sole argument to pass the existing `adopts_string_call` proof with
its current shape and provenance limits. Implement the selection at statement
emission. The conceptual output is one C block:

```c
{
    const char *temporary = factory(arguments);
    view(temporary);
    dr_release((void *)temporary);
}
```

Choose a fresh temporary that cannot shadow any C name used by the original
expressions. Emit the factory expression once, pass the original pointer, and
release it once after the consumer returns. No caller copy, retain, or scope
cleanup entry is needed. The block remains one statement for unbraced control
flow. There is no new ordering within the factory's own arguments and no
recursive processing of their results.

The release operation accepts null. Preserve the consumer call and its input
even when the factory yields null. The current Console implementation passes
that pointer to `printf("%s", ...)`, so a null factory is an emission-only
control for this wrapper boundary.

Sibling arguments, assignments, declaration initializers, for headers, method
receivers, nested/conditional expression consumers, forwarded results, direct
Console and append, unproven factories, and callees outside the exact body
shape retain their existing behavior. General parameter escapes, exception and
async unwinding, and alias/container ownership remain separate work.

## Evidence required for implementation

1. Reuse the isolated audit as the before measurement. Add an emitted-C check
   that fails at the unchanged parent and then requires one factory call, one
   consumer call with the captured pointer, and immediate release in both
   ordinary and module-entry modes. Check actual imported definitions and
   same-spelled local/imported consumers whose bodies differ.
2. Instrument the original allocation. Verify it is readable in Console,
   released once on return at reference count one, and absent after every
   iteration of at least 10,000 isolated calls per mode. Verify peak live count
   one, no caller copy/retain, and no second scope or shutdown release. Keep
   output checks on normal linking and checked execution.
3. Cover `Write` and `WriteLine`, a primitive argument effect in the factory,
   a borrowed input snapshot, unbraced and scoped calls, and temporary name
   collisions. Check null emission without passing null to the Console runtime.
4. Keep the static-store and sibling controls unchanged. Verify fallback for
   pointer/array parameters, async, side effects or forwarding in the callee
   body, non-void returns, non-statement contexts and failed factory proofs.
   Run emission-only cases when the existing runtime path is unsafe. Preserve
   existing diagnostics for rejected programs.
5. The expected per-mode result after implementation is eight releases and
   zero survivors in the isolated probe, and five releases and two survivors in
   the aggregate probe. Those numbers are acceptance criteria. Run the focused
   suite, full Dream tests, Go quick protocol, Python runner checks and Linux
   cross-build, and inspect native CI.
