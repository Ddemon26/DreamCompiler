# Dream Compiler CLI Agent

*Last updated: 17 July 2025*

---

## Overview

The **Dream Compiler CLI agent** bootstraps, grows, and maintains the DreamCompiler code‑base.  The compiler is pure **C11**, built and tested via **Zig**.  Source files end in `.dr`; the build pipeline is:

```
.dream  →  DreamCompiler (C frontend)  →  portable C  →  zig cc  →  native exe
```

Everything lives in a Linux terminal environment where you invoke `zig build`, `zig build run -- <file.dr>`, and `zig build test`.

---

## Responsibilities

* **Track `docs/Grammar.md`** – the authoritative Dream grammar.  Whenever the spec changes, update:

    * **`lexer/tokens.def`** and regenerate `lexer/lexer.c` via **re2c**.
    * **`parser/`**, **`sem/`** modules so the implementation matches the spec (new precedence levels, keywords, etc.).
    * **Examples and snippets** throughout `docs/`.
    * **Regression tests** under `tests/`.
* Grow functionality of the compiler incrementally, starting from a minimal working set.
* Keep documentation (`docs/`) fully in‑sync with behaviour.
* Maintain green test suite (`zig build test`).  Add tests for every new feature and bug‑fix.
* Record notable changes in `docs/changelog.md`.
* Add required system dependencies to `codex/_startup.sh` (e.g., `re2c`).
* Split large modules into focused files when practical.

---

## Workflow

When the command `go` is issued the agent should:

1. **Synchronise with the grammar**

    * Parse `docs/Grammar.md` and compare with `lexer/tokens.def` and precedence tables in `parser/parser.c`.
    * Report mismatches; update code or open a task to correct the spec.
2. **Review project state**

    * Identify monolithic areas that need extraction before adding new code.
3. **Implement the feature**

    * Add new functionality in the appropriate module (lexer, parser, semantic analysis, IR, etc.).
    * Ensure the implementation matches the spec and is documented.
    * Write tests for the new feature under `tests/`.
    * If a bug is fixed, document it in `docs/changelog.md` and add a regression test.
4. **Update docs & tests**

    * Regenerate lexer (`re2c`), rebuild compiler, and run all tests.
5. **Build & run**

    * `zig build` → `zig build test` → optionally `zig build run -- <file.dr>`.
6. **Commit** once tests succeed.

---

## Environment

* Ubuntu‑based shell with Git, Zig (`>=0.13.0`), and **re2c** available.
* Additional packages installed via `codex/_startup.sh`.

---

## Directory Structure

```
lexer/      – re2c spec & generated lexer
parser/     – Pratt / precedence‑climbing parser & AST
sem/        – Symbol table, type system, inference
ir/         – Three‑address SSA IR & CFG
opt/        – Optimisation passes (SCCP, DCE, LICM, …)
codegen/    – C emitter (topological, generics)
docs/       – Language spec, change‑log, design docs
    Grammar.md          – BNF grammar (source of truth)
    changelog.md        – Chronological changes
tests/      – Regression test suite (.dr → expected)
codex/      – Agent docs & startup scripts
build.zig   – Zig build pipeline (dr → c → exe)
```

---

## Features & Roadmap

A living list of implemented and planned features is kept in [`FEATURES.md`](FEATURES.md).  Update this file whenever features are added, removed, or altered.

---

## Example Session

```
> go
[Agent] Syncing lexer/tokens.def with docs/Grammar.md…
[Agent] Re‑generating lexer.c via re2c…
[Agent] Building compiler…
[Agent] Running tests…
[Agent] All tests passed!
[Agent] Committing changes…
```

---

## Pull Request Template

All pull requests must follow `codex/BOT_PR_TEMPLATE.md`.  Fill out each section describing code changes, tests, documentation updates, and dependency modifications.

---

> **Tip:** For quick access to the agent's functionality, use the `go` command in your terminal. This will automatically sync the grammar, build the compiler, run tests, and commit changes if everything passes.
```