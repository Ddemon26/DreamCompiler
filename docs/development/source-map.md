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
| Optimization | `src/opt/`; passes run on an IR that is not yet used for emitted C |
| Optimization emission design | `docs/development/optimization-emission.md`; P0008 audited boundary, restricted first slice, fallback/errors, and acceptance checks (not implemented) |
| Main C backend | `src/codegen/codegen.c`, `stmt.c`, `expr.c`, `context.c`, `c_emit.c` |
| Experimental backend | `src/codegen/backend.c`, `module.c`; not the working multi-file driver path |
| Runtime | `src/runtime/memory/`, `io/`, `exceptions/`, `extensions/`, `system/`; graphics remains experimental |
| Dream regressions | `tests/`; `// Expected:`, `Expected error:`, `Expected exit:`, `Options:` metadata |
| Deferred examples | `not_implemented_tests/`; excluded graphics/async experiments |
| Test runner | `codex/test_cli.py`, `codex/python/test_manager.py`, `codex/test_config.json` |
| Runner regressions | `codex/python/test_test_manager.py` |
| Go quick checks | `codex/go/driver_stability_test.go`, `module_integration_test.go`, Makefile and PowerShell protocol |
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
