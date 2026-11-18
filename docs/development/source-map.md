# Compiler source and workflow map

| Area | Entry points and responsibility |
| --- | --- |
| Build | `build.zig`; Zig 0.17.1, re2c generation, compiler tools, runtime, test steps |
| Driver | `src/driver/main.c`; flags, diagnostics, compilation and native linking |
| Compiler dogfooding | `src/driver/source_text.dr`, `source_text.h`, `source_text_bootstrap.c`; Dream production entry-file normalization, C-only host seed, function-library emission and staged builds; [contract](compiler-dogfooding.md), `codex/go/compiler_dogfood_test.go`, `codex/c/source_text_harness.c` |
| Modules | `src/driver/modules.c`, `modules.h`; dependency loading, exports, symbol lowering |
| Lexer | `src/lexer/lexer.re`, `tokens.def`, `lexer.h`; generated `lexer.c` is ignored |
| Parser / AST | `src/parser/parser.c`, `expr.inc`, `stmt.inc`, `ast.h`, `ast.c` |
| Diagnostics | `src/parser/diagnostic.c`, `diag.inc`, `warnings.c` |
| Semantics | `src/sem/analysis.c`; names, signatures, class hierarchy; `scope.c`, `symbol.c`, `type.c`, `infer.c` |
| IR / CFG / SSA | `src/ir/`, `src/cfg/`, `src/ssa/`; SSA construction is disabled in the driver |
| Optimization | `src/opt/literal_fold.c`; bounded literal-only SCCP IR feeds the AST emitter; general passes remain unqualified and are not invoked by the driver |
| Optimization emission contract | `docs/development/optimization-emission.md`; P0009 implementation, P0008 audited boundary/design, fallback/errors, and acceptance checks |
| Main C backend | `src/codegen/codegen.c`, `stmt.c`, `expr.c`, `context.c`, `c_emit.c` |
| Experimental backend | `src/codegen/backend.c`, `module.c`; not the working multi-file driver path |
| Runtime | `src/runtime/memory/`, `io/`, `exceptions/`, `extensions/`, `system/`; graphics remains experimental |
| Dream regressions | `tests/`; `// Expected:`, `Expected error:`, `Expected exit:`, `Options:` metadata |
| Deferred examples | `not_implemented_tests/`; excluded graphics/async experiments |
| Test runner | `codex/test_cli.py`, `codex/python/test_manager.py`, `codex/test_config.json` |
| Runner regressions | `codex/python/test_test_manager.py` |
| Go quick checks | `codex/go/driver_stability_test.go`, `parser_recovery_test.go`, `module_integration_test.go`, `method_arguments_test.go`, `constructor_arguments_test.go`, `pointer_parameters_test.go`, `indexed_strings_test.go`, `indexed_console_test.go`, `literal_emission_test.go`, `codex/c/literal_fold_harness.c`, `codex/c/indexed_string_harness.c`, Makefile and PowerShell protocol |
| Storage ownership | `src/codegen/stmt.c`, `expr.c`, `context.h`, `context.c`, `codegen.c`, `src/runtime/extensions/custom.c`; bounded [storage contract](string-ownership.md), literal/indexed/identifier/member/call/conditional/assignment scalar copies, scoped for-header cleanup, owned scalar replacement/string append, borrowed array/pointer block cleanup, string/class loop/switch jump cleanup, owned strings/scalar class locals on bare void and primitive scalar synchronous returns, direct owned-local result preservation, direct/conditional literal and selected owned-local string-result cleanup, direct/conditional unchanged borrowed scalar parameter return cleanup, and checked lifetime evidence in `codex/go/indexed_strings_test.go` |
| CI | `.github/workflows/ci.yml`; Linux/Windows checks and pinned Zig version |
| Member-return cleanup | `src/codegen/stmt.c`: `member_body_readonly`, `borrowed_member_result`, `is_scoped_string_result`; [P0043 audit/P0044-P0045 implementation](member-return-cleanup.md), direct and conditional scalar `this.field` returns with whole-body write/call/class-alias barriers; `codex/go/member_return_test.go`, `conditional_member_return_test.go` and corresponding `tests/basics/memory/` fixtures |
| Static member-return cleanup | [P0046 audit/P0047-P0048 implementation contract](static-member-return-cleanup.md); `borrowed_static_member_result` in `src/codegen/stmt.c`, shared guarded-name read-only traversal, owning-global expression emission; `codex/go/static_member_return_test.go`, `conditional_static_return_test.go` and their Dream fixtures qualify direct/conditional class type-qualified scalar static returns in both modes |
| Caller string-result ownership | [P0049 audit/P0050 adoption](owned-call-result-consumption.md); `adopts_string_call` in `src/codegen/stmt.c` proves direct scalar factory initializers; `codex/go/owned_call_initializer_test.go` and `codex/c/owned_call_initializer_harness.c` check original allocation identity/peak lifetime; `codex/python/owned_result_audit.py` preserves borrowed/mixed/other-consumer probes with superseded eligible counts; module lowering in `src/driver/modules.c` |
| Owned factory assignment | [P0051 audit and implementation contract](owned-call-assignment.md); `ND_BINOP` in `src/codegen/expr.c`, `ND_EXPR_STMT` in `stmt.c`, replacement helper in `codegen.c`; P0052 complete-statement adoption via a fresh RHS temporary; `codex/go/owned_call_assignment_test.go` and `codex/c/owned_call_assignment_harness.c` check identity/order/peak lifetime |
| Discarded owned factory results | [P0053 audit/P0054 implementation](owned-call-discard.md); complete `ND_EXPR_STMT` calls in `src/codegen/stmt.c`, unchanged `adopts_string_call`, and `dr_release` in `src/runtime/memory/memory.c`; `codex/go/owned_call_discard_test.go` and `codex/c/owned_call_discard_harness.c` check emitted C and lifetime |
| Owned factory arguments | [P0055 argument audit and proposed release rule](owned-call-argument.md); `ND_CALL` in `src/codegen/expr.c`, borrowed parameter bindings in `src/codegen/stmt.c`, and retention and escape probes in `codex/python/owned_result_audit.py` |
| Documentation website | `docs/index.html`, `index.css`, `site.js`, `site-data.js`, `vendor/`; [maintenance and design rules](docs-site.md), static homepage and Markdown reader |
| Feature truth | `codex/FEATURES.md`, current source, and `tasks/MostRecentTestResults.md` |
| Grammar | `docs/reference/grammar/Grammar.md`; behavior changes need matching docs/tests |
| Module guide | `docs/advanced-topics/modules.md` |
| Type / inheritance guide | `docs/fundamentals/type-system.md` |
| Daily workflow | root `AGENTS.md`, `docs/development/agent-workflow.md`, `codex/prompts/daily-progress.md` |
| Handoff / backlog | `tasks/STATE.md`, `tasks/TODO.md`, `tasks/passes/` |
| Editor plugins | `assets/vscode/`, `assets/jetbrains/`; JetBrains has its own `AGENTS.md` |

Update this map when a major component is added, moved, or removed. Individual
regressions belong with their implementation area and do not need a complete file list.
