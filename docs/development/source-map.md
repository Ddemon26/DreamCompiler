# Compiler source and workflow map

| Area | Entry points and responsibility |
| --- | --- |
| Build | `build.zig`; Zig 0.17.1, re2c generation, compiler tools, runtime, test steps |
| Driver | `src/driver/main.c`; flags, diagnostics, compilation and native linking |
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
| Scalar string ownership | `src/codegen/stmt.c`, `src/runtime/extensions/custom.c`; bounded [storage contract](string-ownership.md), literal/indexed/identifier/member scalar copies, scoped for-header cleanup and checked lifetime evidence in `codex/go/indexed_strings_test.go` |
| CI | `.github/workflows/ci.yml`; Linux/Windows checks and pinned Zig version |
| Feature truth | `codex/FEATURES.md`, current source, and `tasks/MostRecentTestResults.md` |
| Grammar | `docs/reference/grammar/Grammar.md`; behavior changes need matching docs/tests |
| Module guide | `docs/advanced-topics/modules.md` |
| Type / inheritance guide | `docs/fundamentals/type-system.md` |
| Daily workflow | root `AGENTS.md`, `docs/development/agent-workflow.md`, `codex/prompts/daily-progress.md` |
| Handoff / backlog | `tasks/STATE.md`, `tasks/TODO.md`, `tasks/passes/` |
| Editor plugins | `assets/vscode/`, `assets/jetbrains/`; JetBrains has its own `AGENTS.md` |

Update this map when a major component is added, moved, or removed. Individual
regressions belong with their implementation area and do not need a complete file list.
