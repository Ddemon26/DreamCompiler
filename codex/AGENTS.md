# Dream Compiler CLI Agent

*Last updated: 18 July 2025*

---

## Cross-Platform Compatibility

* All code, scripts, and build configurations **must** support both Linux and Windows environments.
* Use preprocessor defines in C/Zig and build scripts to select OS-specific code paths:

  ```c
  #ifdef _WIN32
    // Windows-specific implementation
  #elif defined(__linux__)
    // Linux-specific implementation
  #endif
  ```
* In build and CI scripts, detect the OS and set appropriate commands and flags. Example in Bash:

  ```bash
  if [[ "$(uname -s)" == "Linux" ]]; then
    zig build
  elif [[ "$OS" == "Windows_NT" ]]; then
    zig build
  fi
  ```
* Ensure Python helper scripts adapt at runtime:

  ```python
  import platform
  if platform.system() == "Windows":
      # Windows paths or commands
  elif platform.system() == "Linux":
      # Linux paths or commands
  ```
* Validate cross-platform compatibility by running CI jobs on both Linux and Windows matrices.

---

## Overview

The **Dream Compiler CLI agent** bootstraps, grows, and maintains the DreamCompiler code‑base.  The compiler is pure **C11**, built via **Zig** and tested using the **GNU toolchain**.  Source files end in `.dr`; the build pipeline is:

```
.dream  →  DreamCompiler (C frontend)  →  portable C  →  zig cc  →  native exe
```

Everything lives in a terminal environment where you invoke `zig build` for building, and use GNU tools (e.g., `make test`, `gcc`, `gdb`) for testing and debugging.  All steps must work on both Linux and Windows, using OS defines where necessary.

* **Python Helper Scripts** – if the agent needs to perform any heavy tasks (e.g., code generation, analysis, scaffolding, or test automation), it should generate a Python file in the `python/` directory to implement helper functionality.

---

## Dev Environment Tips  *(adapted from the Codex contributor guide)*

* **Jump quickly between modules** – use `grep -R "module "` or `find . -name '*.c'` instead of scanning with `ls`.
* **Prime the incremental cache** – run `zig build` once; subsequent builds re‑compile only what changed.
* **Run the full test suite** – `make test` (or `./test_runner`) executes every unit and regression test and prints a concise summary.
* **Focus on one test** – use the test runner with a `--filter="<pattern>"` option (pattern matches file or function).
* **Spin up a scratch playground** – `zig run tools/scratch.zig -- <snippet.dr>` to experiment without touching the main code‑base.
* **Add new external tools responsibly** – when you introduce something like **re2c** or **clang‑format**, record it in `codex/_startup.sh` so CI can install it.
* **Keep target names consistent** – double‑check any new `build.zig` target’s `name` field to avoid CI mis‑routing.

---

## Debug Console Helpers

The compiler relies on helper functions in `src/util/console_debug.h` for
printing diagnostics during development. `Console.Write`, `Console.WriteLine`,
`Console.ReadLine` and `Console.Read` map to these helpers. When the
`DREAM_RELEASE` macro is defined they become no-ops, so ensure any debugging
calls are removed or guarded before release builds.

---

## Testing Instructions

* The CI plan lives at `.github/workflows/ci.yml`.
* **Run everything** – from the repo root, use `make test` (or `./test_runner`).
* **Narrow scope** – use `make test FILTER="<pattern>"` (or `./test_runner --filter "<pattern>"`) to run only matching tests.
* Fix *all* test or type errors until the whole suite is green.
* After moving files or changing `#include` paths, run `zig fmt --check` and `zig build` to ensure formatting and compilation still pass.
* *Always* add or update tests for the code you change, even if nobody asked.

---

## PR Instructions

Title format: `[dreamc] <Short, imperative summary>`

Every pull request **must** fill out `codex/BOT_PR_TEMPLATE.md`, covering:

1. **What changed** – features, refactors, or bug fixes.
2. **How it was tested** – commands, new test files, CI results.
3. **Docs updated** – grammar, examples, changelog entries.
4. **Dependencies** – any additions in `codex/_startup.sh`.

---

## Responsibilities

* **Track `docs/grammar/Grammar.md`** – the authoritative Dream grammar.  Whenever the spec changes, update:

    * **`lexer/tokens.def`** and regenerate `lexer/lexer.c` via **re2c**.
    * **`parser/`**, **`sem/`** modules so the implementation matches the spec (new precedence levels, keywords, etc.).
    * **Examples and snippets** throughout `docs/`.
    * **Regression tests** under `tests/`.
* Grow functionality of the compiler incrementally, starting from a minimal working set.
* Keep documentation (`docs/`) fully in‑sync with behaviour.
* Maintain green test suite (`make test`).  Add tests for every new feature and bug‑fix.
* Record notable changes in `docs/changelog.md`.
* Add required system dependencies to `codex/_startup.sh` (e.g., `re2c`).
* Split large modules into focused files when practical.

---

## Workflow

When the command `go` is issued the agent should:

1. **Synchronise with the grammar**

    * Parse `docs/grammar/Grammar.md` and compare with `lexer/tokens.def` and precedence tables in `parser/parser.c`.
    * Report mismatches; update code or open a task to correct the spec.
2. **Review project state**

    * Identify monolithic areas that need extraction before adding new code.
3. **Implement the feature**

    * Add new functionality in the appropriate module (lexer, parser, semantic analysis, IR, etc.).
    * Ensure the implementation matches the spec and is documented.
    * Write tests for the new feature under `tests/`.
    * If a bug is fixed, document it in `docs/changelog.md` and add a regression test.
    * For any substantial or repetitive operations, scaffold or automate using Python helper scripts placed under `python/`, ensuring they detect and adapt to the host OS via preprocessor or runtime checks.
4. **Update docs & tests**

    * Regenerate lexer (`re2c`), rebuild compiler, and run all tests.
5. **Build & run**

    * Run `zig build` (using OS defines if needed to adjust flags).
    * If any Zig test files (`*.zig`) are present in the `tests/` directory, run `zig build test`; otherwise, skip it.
    * Run `make test` (or `./test_runner`).
    * Optionally, run `zig build run -- <file.dr>`.
6. **Commit** once tests succeed.

---

## Environment

* Ubuntu-based or Windows shell with Git, Zig (`>=0.13.0`), **GCC**/**MSVC**, **Make**/**nmake**, **GDB**/**WinDbg**, and **re2c** available.
* Additional packages installed via `codex/_startup.sh` (Linux) or `codex/_startup.ps1` (Windows).

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
    grammar/Grammar.md          – BNF grammar (source of truth)
    changelog.md        – Chronological changes
tests/      – Regression test suite (.dr → expected)
codex/      – Agent docs & startup scripts
build.zig   – Zig build pipeline (dr → c → exe)
python/     – Generated Python helper scripts
```

---

## Features & Roadmap

A living list of implemented and planned features is kept in [`FEATURES.md`](FEATURES.md).  Update this file whenever features are added, removed, or altered.

---

## Example Session

```
> go
[Agent] Building compiler…
[Agent] No Zig test files found; skipping `zig build test`.
[Agent] Running tests via make test…
[Agent] Committing changes…
```