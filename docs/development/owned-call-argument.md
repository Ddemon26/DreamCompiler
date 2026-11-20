# Proven factory results passed to string parameters

P0055 audited direct argument consumption at parent
`04daec85aeee2d7c2a352e7d498bd5c24f7ac914`. P0056 implements release after
normal return for a complete call statement to a synchronous, single-parameter
Console wrapper. It preserves the existing factory proof.
P0057 uses the same statement release sequence for
[direct Console output](string-ownership.md#direct-console-output-of-proven-factory-results-p0057).
P0068 also qualifies [proven returned joins](string-ownership.md#returned-joins-passed-to-console-wrappers-p0068)
as arguments to these wrappers.

## P0055 source audit and before measurements

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

## P0056 implementation boundary

The emitter selects a complete `ND_EXPR_STMT` containing a direct identifier
call to an actual synchronous free-function definition returning `void`. It
requires exactly one scalar string parameter and one argument. The complete
callee body must be a block containing exactly one statement, the intrinsic
`Console.Write` or `Console.WriteLine` of the parameter identifier. The proof
matches the parameter in the resolved definition, including when the function
was imported. Caller names shadowed by a variable, pointer and array parameters,
ReadLine, computed operands and additional body statements fall outside this rule.

The sole argument must pass the existing `adopts_string_call` proof with its
current shape and provenance limits. Statement emission produces one C block:

```c
{
    const char *temporary = factory(arguments);
    view(temporary);
    dr_release((void *)temporary);
}
```

The temporary uses a name without underscores and avoids visible bindings,
function names and unqualified C enum constants. Lowered member names contain
an underscore separating the type from the member. The block evaluates the
factory expression once, passes the original pointer and releases it once
after the consumer returns. It remains one statement for unbraced control
flow. The factory's own arguments keep their original C evaluation order and
lifetime rules.

The release operation accepts null. Preserve the consumer call and its input
even when the factory yields null. The current Console implementation passes
that pointer to `printf("%s", ...)`, so a null factory is an emission-only
control for this wrapper boundary.

At the P0056 checkpoint, sibling arguments, assignments, declaration initializers,
for headers, method receivers, nested/conditional expression consumers, forwarded
results, direct Console and append, unproven factories, and callees outside the
exact body shape retained their existing behavior. General parameter escapes,
exception and async unwinding, and alias/container ownership remain separate work.

## Implementation checks

1. The generated-C regression fails at the unchanged parent and requires one
   factory call, one consumer call with the captured pointer, and immediate
   release in both modes. Same-spelled local and imported consumers exercise
   actual definition lookup.
2. The checked harness verifies the original pointer is readable in Console
   and released once at reference count one. It verifies peak live count one
   and an empty allocator list after each of 10,000 calls per mode. Checked
   and ordinarily linked fixture runs compare exact output.
3. The fixture covers `Write` and `WriteLine`, a primitive argument effect in the
   factory, a borrowed input snapshot, unbraced and scoped calls, and temporary
   name collisions. The null result is tested at emission only.
4. Exclusion checks cover pointer parameters, async, forwarding, extra
   statements, computed operands, non-void returns, non-statement contexts,
   unproven factories and direct Console output. Array parameter syntax still
   produces its parser diagnostic.
5. P0056 measures eight releases and zero survivors in the isolated probe,
   and five releases and two survivors in the aggregate probe, in each mode.
   The escape and sibling controls each still measure zero releases and one
   survivor. See the [pass record](../../tasks/passes/P0056-owned-call-argument.md)
   for regression and platform results.
