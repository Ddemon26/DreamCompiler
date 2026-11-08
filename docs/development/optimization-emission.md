# First optimization-to-emission slice

Status: **implemented in P0009, 2026-11-08**, following the P0008 design.
At `-O1`/`-O2`/`-O3`, checked literal arithmetic now reaches the AST C emitter
through isolated SCCP IR. Default emission retains the original expressions.
General CFG emission and SSA remain later work.

## Implementation checkpoint

[literal_fold.c](../../src/opt/literal_fold.c) implements the bridge below.
Its temporary instructions, pointer table, block, and CFG are stack-owned and
bounded; it does not call the unchecked general IR/CFG allocators. SCCP uses
`sccp_checked` to distinguish allocation failure from convergence. Replacement
text uses `arena_try_alloc`; existing callers of `arena_alloc` retain their prior
abort-on-failure contract. Replacement is atomic per expression and retains source
position, filename, and text. Bridge-internal errors return before the driver
opens output files. The legacy whole-program pipeline is no longer invoked.

The limits are **256 binary operations and depth 64** (root depth zero), with
canonical decimal leaves and every intermediate in `0..INT_MAX-1`. A maximal
binary expression is considered as a whole, including unsupported operators;
fallback never partially folds its children. Unary expressions are also left whole.
Known statement/container children, function/method bodies, call arguments, and
conditional arms are visited explicitly. Unknown containers remain untouched.

The shared C emitter also serves object emission. Object temporary C now uses
runtime include directories instead of paths relative to `build/bin`; emission,
write, and host-compiler failures propagate nonzero to the driver. Both CLI modes
still expect the project working directory and its runtime sources.

[Go regressions](../../codex/go/literal_emission_test.go) inspect generated C
before host compilation and compare exact runtime output at all four settings.
They cover nested trees, zero, boundaries, free functions, methods, imported
functions/source mapping, side effects, fallback, accepted/rejected resource
limits, object success, and host-compilation failure. The
[C harness](../../codex/c/literal_fold_harness.c) uses the real implementation
with controlled SCCP/arena allocation failures, a transformed-root mismatch, and
nonconvergence; it checks diagnostics, nonzero status, unchanged candidates, and
source metadata. Fault injection exists only in the harness. It does not qualify
the general optimizer for arbitrary IR. See [P0009](../../tasks/passes/P0009-literal-emission.md)
for measured validation and remaining limits.

## Audited boundary

The following historical findings come from P0008 source review at parent
`c162e0826d21a329ce3ca58a615c58f542cab92f`, not new runtime experiments.

| Source | Current behavior | Consequence for emission |
| --- | --- | --- |
| [driver/main.c](../../src/driver/main.c), `main` | Lowers the semantic AST, runs the pipeline, frees the CFG, then passes the AST to codegen; SSA construction is disabled | No optimized representation reaches emitted C |
| [ir/lower.c](../../src/ir/lower.c), `emit_expr` | Handles integer literals, identifiers, and binary operations; other expression kinds return constant zero | Calls, unary expressions, and other unsupported expressions cannot be silently emitted from this IR |
| Same file, `binop_from_token` / `emit_stmt` | Unknown binary operators become addition; unhandled statements are ignored, including function declarations | Logical/compound operators and function bodies need explicit support before general lowering |
| Same file, `get_var_id` | Variables are keyed by spelling in a global linked list, with mutable destination IDs | Scope shadowing and reaching definitions are not represented adequately for general propagation |
| [ir/ir.h](../../src/ir/ir.h), `ir_const` / `ir_const_value` | Constants use negative IDs via `-v-1`; nonnegative IDs are variables | Negative integers collide with variables; decoding `INT_MAX` negates `INT_MIN`, overflowing signed `int` |
| [opt/sccp.c](../../src/opt/sccp.c), `fold_bin` | Uses host signed arithmetic; division/remainder by zero leave result zero; shifts are unchecked | Arithmetic must be qualified before SCCP can supply emitted values |
| Same file, `sccp` | A traversal records values by destination ID, without a general SSA/reaching-definition proof | A constant assigned later can incorrectly influence earlier uses if mutable IDs are admitted |
| [opt/pipeline.c](../../src/opt/pipeline.c), `run_pipeline` | Combines SCCP, CFG pruning, DCE, propagation, numbering, CSE, loops, LICM, and peepholes | P0007 convergence/ownership fixes do not qualify all these transformations for emission |
| [codegen/expr.c](../../src/codegen/expr.c), `cg_emit_expr` | Prints literal slices and recursively prints AST arithmetic | Replacing an expression with a validated literal can affect C without replacing runtime/control-flow emission |
| [driver/main.c](../../src/driver/main.c), host compiler command | Passes `-O2`/`-O3` to the host C compiler; default and `-O1` pass no host optimization flag | Runtime equivalence alone cannot establish a Dream transformation; inspect C before host compilation |

The working backend is [codegen/codegen.c](../../src/codegen/codegen.c).
`codegen_emit_obj` and `codegen_emit_c` now share its internal emitter; the experimental
[backend.c](../../src/codegen/backend.c) is not the driver integration point.
Module expansion precedes semantic analysis and supplies a combined AST through
[driver/modules.c](../../src/driver/modules.c).

## Chosen behavior

At the default setting, preserve AST emission. At `-O1`, `-O2`, and `-O3`, fold
pure integer-literal arithmetic through an isolated, single-block IR, then replace
only the accepted expression node with the resulting integer literal in the AST.
Use the same restricted pass at all three settings. This is an IR-to-AST bridge,
not a general IR backend, and makes no performance claim.

For example, `int result = (6 * 7) + (10 - 3); Console.WriteLine(result);`
must keep the arithmetic in default generated C and emit `int result = 49;` at
each supported optimization setting. Both executables must print exactly `49`.
The replacement value must come from the transformed IR, not a separate AST
evaluator that bypasses the optimizer.

Supported candidates are complete expression trees containing only `ND_INT` and
`ND_BINOP` with `+`, `-`, or `*`. Every leaf and every intermediate value must lie
in `0..INT_MAX-1`. Parse decimal slices with checked accumulation; accept only
canonical decimal spelling (`0` or a nonzero digit followed by digits), so a
leading-zero C literal cannot change meaning. Bound expression size/depth before
recursive lowering. A proposed initial limit is 256 binary operations and depth
64; exceeding either uses fallback, not a language rejection.

Exclude identifiers, calls, assignments, increment/decrement, unary operations,
division/remainder, shifts, comparisons, logical/conditional operations, and all
non-integer literals. Exclude negative intermediate values even if the final
answer would be nonnegative. This is an optimization eligibility rule, not a new
restriction on valid Dream programs or a definition of overflow semantics.

## Lower, optimize, and commit atomically

The original P0008 implementation requirements are retained below. P0009 uses
stack-owned bounded IR instead of allocating a general CFG, as described above.

1. Add a restricted helper under `src/opt/`, registered in `build.zig`. Invoke it
   after successful semantic analysis and module expansion, before output files
   are opened. Replace the driver's discarded whole-program lower/run/free phase
   with this qualified path. Do not run the unqualified pipeline on fallback ASTs:
   even discarded IR can crash or execute undefined arithmetic. Keep the legacy
   optimizer implementations for later qualification; do not advertise them as
   emitted-code optimizations.
2. Traverse explicit child slots of known AST containers, including free function
   and method bodies, return expressions, variable initializers, console/call
   arguments, and control-flow bodies/conditions. Preserve evaluation order and
   AST ownership. Unknown containers remain untouched. Treat a maximal arithmetic
   tree as one candidate: if it fails preflight, leave that whole tree unchanged
   rather than partially rewriting its children.
3. Preflight the whole candidate without mutation. Check addition with
   `lhs <= limit-rhs`, subtraction with `lhs >= rhs`, and multiplication with
   `rhs == 0 || lhs <= limit/rhs`, where `limit = INT_MAX-1`. Thus neither
   preflight nor accepted SCCP arithmetic overflows. Retain the checked expected
   result only as a verifier; the IR produces the replacement.
4. Lower postorder into a fresh one-block CFG with fresh, unique nonnegative
   destinations, no named variables, no edges, and only `IR_ADD`, `IR_SUB`, and
   `IR_MUL`. Keep the root result ID separately. Use explicit eligibility/status
   results, never the zero/addition defaults of `ir_lower_program`. Each
   instruction has one owner. Check allocations, including helper paths used by
   CFG/instruction creation and SCCP; current unchecked allocators need handling
   before this bridge can promise an error result.
5. Run only `sccp` on this acyclic expression graph. No DCE, CFG rewrites, LICM,
   loop passes, or SSA are needed. Repeat until unchanged with a bound of the
   binary-operation count plus one calls; exceeding it is an internal error.
   Qualify this exact use of SCCP with nested-tree regressions before relying on
   its propagation. Do not infer safety for mutable variables from these tests.
6. Find the instruction defining the saved root ID. Require `IR_MOV` with a
   constant operand, validate its representable range, and compare its decoded
   value with the checked preflight result. An unexpected operation, missing
   producer, or mismatch is an internal compiler error, not fallback.
7. Only after verification, allocate an arena-owned decimal slice, replace the
   expression with `ND_INT`, and retain `pos`, `source_file`, and `source_text`.
   Never retain pointers into freed CFG storage or a temporary formatting buffer.
   Free all temporary IR on success, fallback, and error. The existing AST emitter
   then consumes the actual transformed result in C and object emission modes.

## Fallback and error contract

| Outcome | Required action |
| --- | --- |
| Default optimization setting | Skip the bridge; emit the original AST |
| Unsupported node/operator/spelling, out-of-range leaf/intermediate, or size limit | Leave the candidate intact and continue through the existing AST emitter; no fabricated constant and no new language error |
| Accepted candidate with validated transformed root | Emit its replacement literal, preserving surrounding code and source metadata |
| Allocation failure, malformed restricted IR, nonconvergence, or verification mismatch | Emit an explicit compiler error, return nonzero, and do not create new C/object/executable artifacts |

Fallback must avoid evaluating unsupported arithmetic in the compiler. It preserves
existing behavior; it does not promise valid execution for source with undefined
overflow or divide-by-zero behavior. Existing semantic diagnostics still run first.
Do not hide internal failures by presenting a successful unoptimized compilation.
Existing files from earlier invocations are not proof that a failed invocation
emitted anything; failure tests must use an artifact-free test setup.

## Required implementation evidence

These acceptance checks were designed in P0008 and exercised in P0009's focused
matrix and fault harness. They were **not tests run by P0008**.

| Case | Generated-C evidence | Execution evidence |
| --- | --- | --- |
| `(6 * 7) + (10 - 3)` | Default initializer contains arithmetic; `-O1`/`-O2`/`-O3` initializer is literal 49 | Exact output 49 at all four settings |
| Deeper nested trees, zero multiply, zero subtraction, and `INT_MAX-1` boundary | Corresponding literal results; nested cases exercise propagation/convergence | Same independently computed outputs at all settings |
| Same pure expression in a free function, method, and imported function | Fold occurs in the actual emitted body; module symbol/source mapping retained | Call each body and compare output at all settings |
| Identifier/call/assignment/increment inside an arithmetic candidate | Whole candidate retains its operations and side effects | Counter/variable results and call counts match default; no dropped or reordered calls |
| Negative intermediate followed by positive result, unary minus, leading-zero literal, division/remainder, shifts, mixed float/string operations | Unsupported candidate retains AST emission | Execute defined examples and compare exact output |
| `INT_MAX`, overflow, zero divisor, excessive depth/size | No fabricated zero or wrapped literal; compiler preflight does not evaluate invalid arithmetic | Use `--dev` C inspection for undefined source; do not require its runtime to succeed |
| Malformed restricted IR and controlled allocation failure, where practical in a focused C harness | No emitted artifacts after internal error | Nonzero status and diagnostic; no success fallback |

Use CLI `--dev` to inspect `build/bin/dream.c` immediately after each setting,
before another compilation overwrites it. Match the relevant initializer/body,
ignoring whitespace and debug comments; do not snapshot the entire generated file.
Then compile and execute the same valid sources with `CC=zig cc`. No test may
share output artifacts concurrently. Include C emission and the common object
emission path; fix status propagation if an object-path error would report success.

Add the focused Go matrix to both quick protocols and CI, preserving existing
P0007 loop coverage. Run it with `-count=1`, the full annotated Dream suite, Python
runner tests, Windows Go quick protocol, and Linux cross-build as required by
[AGENTS.md](../../AGENTS.md). Inspect native Linux/Windows CI after publication.
No changes to expected outputs, fixture exclusions, or unsupported flags such as
`-O0` are needed: the current CLI uses the default invocation for level zero.

## Later qualification

Signed/tagged constants and checked arithmetic must precede broader numeric
lowering. Binding IDs and scoped per-function lowering must precede propagation
through variables. Complete call/control-flow lowering, CFG invariants, SSA or a
sound non-SSA analysis, and independent optimizer regressions must precede general
IR emission. Preserve the existing runtime, cleanup, exception, class, and module
paths until each has an explicit replacement and behavioral evidence.
