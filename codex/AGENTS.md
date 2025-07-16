# Dream Compiler CLI Agent

*Last updated: 15 July 2025*

---

## Overview

The **Dream Compiler CLI agent** iteratively develops and maintains the Dream compiler.  The compiler is written in C and built with Zig, and the agent runs in a Linux terminal environment using `zig build` for compilation.  Test files under `tests/` are executed with `zig build run -- <file.dr>`.

---

## Responsibilities

* **Track `docs/Grammer.md`** – the canonical Dream.dr grammar.  Whenever this file changes, update:

    * `src/` lexer, parser and semantic‑analysis code so the implementation matches the spec.
    * `tokens.json` and regenerated VS Code / JetBrains syntax files (run `node scripts/genFromTokens.js`).
    * Documentation snippets and examples throughout `docs/`.
    * Regression tests in `tests/` so they cover any new or altered syntax.
* Expand `src/` with new language features.
* Keep documentation in `docs/` up to date for every feature.
* Maintain matching test cases in `tests/` and ensure they pass.
* Record notable changes in `docs/changelog.md`.
* Add any required system dependencies to `codex/_startup.sh`.
* Break down large or monolithic source files into smaller modules when practical.
* Keep editor syntax highlighting in `vscode/` and `idea/` aligned with `tokens.json` (mirrored into `idea/`).  After changing tokens or language features, run `node scripts/genFromTokens.js` and rebuild both extensions.

---

## Workflow

When the command `go` is issued the agent should:

1. **Synchronise with the grammar**

    * Parse `docs/Grammer.md` and compare it against the current lexer (`tokens.json`) and parser tables.
    * Highlight any mismatches and either align the code or open a task to update the grammar spec.
2. Inspect the current compiler implementation and documentation.

    * Look for large modules that could be split into focused files before adding new features.
3. Implement the next logical feature or fix.
4. Update docs, tests, and syntax highlighting accordingly.
   Regenerate the grammar and lexer with `node scripts/genFromTokens.js` and rebuild the VS Code and JetBrains extensions.
5. Build with `zig build` and run all tests using `zig build run -- <test>`.
6. Commit the changes once tests succeed.

---

## Environment

* Linux shell with Git and Zig available.
* `codex/_startup.sh` installs packages via `apt` (e.g. `build-essential` and `zig`).

---

## Directory Structure

```
src/       – Compiler source code
docs/      – Markdown documentation for language features and usage
           • Grammer.md – formal grammar (tracked closely by the agent)
           • changelog.md – project changelog
tests/     – Dream source files used as regression tests
codex/     – Contains this AGENTS.md and the _startup.sh script
```

---

## Features

The full list of implemented and planned features lives in
[`FEATURES.md`](FEATURES.md). Update that file whenever a feature is
added or changed.

---

## Example Execution

```
> go
[Agent] Syncing with Grammer.md…
[Agent] Building compiler…
[Agent] Running tests…
[Agent] All tests passed!
[Agent] Committing changes…
```

---

## Pull Request Template

All pull request descriptions should follow `codex/BOT_PR_TEMPLATE.md`.  Fill out each section of that template to describe the changes, tests, documentation updates and any dependency modifications.